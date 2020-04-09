/*
Copyright 2015-2020 Julien Mille

This file is part of the PixelLabeling source code package.

PixelLabeling is free software: you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

PixelLabeling is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU General Public License,
and a copy of the GNU Lesser General Public License, along with
PixelLabeling. If not, see <http://www.gnu.org/licenses/>.
*/

#include "labeling.h"
// #include <string.h>
// #include <float.h>
// #include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>

using namespace std;

/**********************************************************************
*                             Labeling                               *
**********************************************************************/

vector<CLabelClassProperties> Labeling::vectClassesProperties;

void Labeling::InitClassesProperties()
{
    int idxClassMax, idxClass, idxLine;
    stringstream streamLine;
    string currentLine;

    const string labelField = "label=";
    const string nameField = "name=";
    const string rgbField = "rgb=";

    size_t pos;

    ifstream fileConfig("classes_config.txt");
    if (!fileConfig.is_open())
    {
        cout<<"Failed to open classes_config.txt. Setting default class configuration"<<endl;

        vectClassesProperties.resize(2);

        vectClassesProperties[0].className="Unlabeled";
        vectClassesProperties[0].rgbClassColor = cv::Vec3b(0,0,0);

        vectClassesProperties[1].className="Object";
        vectClassesProperties[1].rgbClassColor = cv::Vec3b(255,0,0);

        cout<<"O: Unlabeled"<<endl;
        cout<<"1: Object rgb=(255,0,0)"<<endl;
    }
    else
        cout<<"Parsing file classes_config.txt..."<<endl;

    // iLine = 1;
    idxClassMax = 0;

    while (getline(fileConfig, currentLine))
    {
        pos = currentLine.find(labelField);
        if (pos!=string::npos)
        {
            idxClass = stoi(currentLine.substr(pos + labelField.size()));
            if (idxClass>idxClassMax)
                idxClassMax = idxClass;
        }
    }

    if (idxClassMax<=0)
    {
        cerr<<"ERROR: invalid maximum class label"<<endl;
        fileConfig.close();
        return;
    }

    vectClassesProperties.resize(idxClassMax+1);

    vectClassesProperties[0].className="Unlabeled";
    vectClassesProperties[0].rgbClassColor = cv::Vec3b(0,0,0);

    for (idxClass=1; idxClass<=idxClassMax; idxClass++)
    {
        vectClassesProperties[idxClass].className="Unknown";
        vectClassesProperties[idxClass].rgbClassColor = cv::Vec3b(0,0,0);
    }

    vector<bool> vectClassFound(idxClassMax+1);
    fill(vectClassFound.begin(), vectClassFound.end(), false);
    vectClassFound[0] = true;

    // Seek to beginning of file
    fileConfig.clear();
    fileConfig.seekg(0, fileConfig.beg);

    idxLine = 1;
    while (getline(fileConfig, currentLine))
    {
        // Search for label field
        pos = currentLine.find(labelField);
        if (pos!=string::npos)
        {
            idxClass = stoi(currentLine.substr(pos + labelField.size()));
            if ((idxClass>0 && idxClass<=idxClassMax) && !vectClassFound[idxClass])
            {
                CLabelClassProperties *pProp = &(vectClassesProperties[idxClass]);

                vectClassFound[idxClass] = true;

                // Label is ok, search for name field
                pos = currentLine.find(nameField);
                if (pos!=string::npos)
                {
                    size_t posQuoteStart, posQuoteEnd;
                    posQuoteStart = currentLine.find("\"", pos+nameField.size());

                    if (posQuoteStart!=string::npos)
                    {
                        posQuoteEnd = currentLine.find("\"", posQuoteStart+1);
                        if (posQuoteEnd!=string::npos)
                            pProp->className = currentLine.substr(posQuoteStart+1, posQuoteEnd-posQuoteStart-1);
                        else
                            cout<<"WARNING: line "<<idxLine<<": cannot find ending '\"' for 'name' field."<<endl;
                    }
                    else
                        cout<<"WARNING: line "<<idxLine<<": expected '\"' after 'name='"<<endl;
                }
                else
                    cout<<"WARNING: line "<<idxLine<<": cannot find 'name' field"<<endl;

                // Label is ok, search for rgb field
                pos = currentLine.find(rgbField);
                if (pos!=string::npos)
                {
                    size_t posBracketStart, posBracketEnd;
                    posBracketStart = currentLine.find("(", pos+rgbField.size());

                    if (posBracketStart!=string::npos)
                    {
                        posBracketEnd = currentLine.find(")", posBracketStart+1);
                        if (posBracketEnd!=string::npos)
                        {
                            string rgbTriplet = currentLine.substr(posBracketStart+1, posBracketEnd-posBracketStart-1);
                            rgbTriplet.erase(remove(rgbTriplet.begin(),rgbTriplet.end(),' '), rgbTriplet.end());

                            size_t posComma1, posComma2;

                            posComma1 = rgbTriplet.find(",");
                            if (posComma1!=string::npos)
                            {
                                posComma2 = rgbTriplet.find(",", posComma1+1);
                                if (posComma2!=string::npos)
                                {
                                    // Read red, green and blue values
                                    pProp->rgbClassColor[2] = (unsigned char)stoi(rgbTriplet.substr(0, posComma1));
                                    pProp->rgbClassColor[1] = (unsigned char)stoi(rgbTriplet.substr(posComma1+1, posComma2-posComma1-1));
                                    pProp->rgbClassColor[0] = (unsigned char)stoi(rgbTriplet.substr(posComma2+1));
                                }
                                else
                                    cout<<"WARNING: line "<<idxLine<<": cannot find second ',' in RGB triplet."<<endl;
                            }
                            else
                                cout<<"WARNING: line "<<idxLine<<": cannot find first ',' in RGB triplet"<<endl;
                        }
                        else
                            cout<<"WARNING: line "<<idxLine<<": cannot find ending ')' for 'rgb' field"<<endl;
                    }
                    else
                        cout<<"WARNING: line "<<idxLine<<": expected '(' after 'rgb='"<<endl;
                }
                else
                    cout<<"WARNING: line "<<idxLine<<": cannot find 'rgb' field"<<endl;
            }
            else if (vectClassFound[idxClass])
                cout<<"WARNING: line "<<idxLine<<": label="<<idxClass<<" was met previously. Line is ignored"<<endl;
            else
                cout<<"WARNING: line "<<idxLine<<": invalid label="<<idxClass<<". Line is ignored"<<endl;
        }
        else {
            cout<<"WARNING: line "<<idxLine<<": cannot find 'label' field. Line is ignored."<<endl;
        }
        idxLine++;
    }

    fileConfig.close();

    for (idxClass=1; idxClass<=idxClassMax; idxClass++)
        if (vectClassFound[idxClass])
            cout<<"Label="<<idxClass<<", "<<vectClassesProperties[idxClass].className<<", "
                <<"RGB="<<vectClassesProperties[idxClass].rgbClassColor<<endl;
        else
            cout<<"Label="<<idxClass<<" was not found"<<endl;

    cout<<"Class configuration file parsed"<<endl;
}

void Labeling::Create(const cv::Size &size)
{
    create(size, CV_8U);
    setTo(0);
}

void Labeling::SetLabelDisk(const cv::Point &center, unsigned int radius,
    unsigned int baseLabel, unsigned int targetLabel, bool replaceAll)
{
	cv::Point p, pmin, pmax;
	int y2;

	if (center.y<(int)radius)
		pmin.y = -center.y;
	else pmin.y = -(int)radius;

	if (center.y+(int)radius>=rows)
		pmax.y = rows-center.y-1;
	else pmax.y = (int)radius;

	// Fill disk
	for (p.y=pmin.y; p.y<=pmax.y; p.y++)
	{
		y2 = (int)sqrt((float)(radius*radius-p.y*p.y));
		if (center.x<y2)
			pmin.x = -center.x;
		else pmin.x = -y2;

		if (center.x+y2>=cols)
			pmax.x = cols-center.x-1;
		else pmax.x = y2;

		for (p.x=pmin.x; p.x<=pmax.x; p.x++)
            if ((replaceAll==false && GetLabel(center+p)==baseLabel) || replaceAll==true)
                SetLabel(center + p, targetLabel);
	}
}

void Labeling::EmptyClass(unsigned int label)
{
	cv::Point p;

	for (p.y=0; p.y<rows; p.y++)
	    for (p.x=0; p.x<cols; p.x++)
            if (GetLabel(p)==label)
                SetLabel(p,0);
}

void Labeling::Empty()
{
	setTo(0);
}

bool Labeling::IsEmpty() const
{
	cv::Point p;

    if (cols==0 || rows==0)
        return true;

    // Check all pixels
	for (p.y=0; p.y<rows; p.y++)
	    for (p.x=0; p.x<cols; p.x++)
            if (GetLabel(p)!=0)
                return false;

    return true;
}

void Labeling::MakeLabelImage(cv::Mat &imgOutput) const
{
    copyTo(imgOutput);
}

void Labeling::MakeLabelImageRGB(cv::Mat &imgOutput) const
{
    cv::Point p;

	imgOutput.create(size(), CV_8UC3);

	for (p.y=0; p.y<rows; p.y++)
		for (p.x=0; p.x<cols; p.x++)
			imgOutput.at<cv::Vec3b>(p) = vectClassesProperties[at<uchar>(p)].rgbClassColor;
}

void Labeling::InitFromLabelImage(const cv::Mat &imgLabel)
{
	cv::Point p;

    Empty();

	if (imgLabel.type()!=CV_8U)
	{
		cerr<<"ERROR in Labeling::InitFromLabelImage(...): selected label file is not 8-bit."<<endl;
		return;
	}

	if (imgLabel.size()!=size())
	{
		cerr<<"ERROR in Labeling::InitFromLabelImage(...): the size of the selected label file does not match the image size."<<endl;
		return;
	}

	for (p.y=0; p.y<rows; p.y++)
		for (p.x=0; p.x<cols; p.x++)
            at<uchar>(p) = imgLabel.at<uchar>(p);
}

void Labeling::SetLabel(const cv::Point &p, unsigned int uiLabelNew)
{
    at<uchar>(p) = (uchar)uiLabelNew;
}

unsigned int Labeling::GetLabel(const cv::Point &p) const
{
    return (unsigned int)at<uchar>(p);
}
