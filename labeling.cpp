/*
	labeling.cpp

	Copyright 2018 Julien Mille (julien.mille@insa-cvl.fr)

*/

#include "labeling.h"
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

/**********************************************************************
*                             CLabeling                               *
**********************************************************************/

vector<CLabelClassProperties> CLabeling::vectClassesProperties;

bool CLabeling::InitClassesProperties()
{
    FILE *pFileConfig;
    int iIndexClassMax, iIndexClass, iLine;
    char strLine[500], *strId;

    pFileConfig = fopen("classes_config.txt", "rb");
    if (pFileConfig==NULL)
    {
        cout<<"ERROR: cannot open classes_config.txt"<<endl;
        return false;
    }
    else
        cout<<"Parsing file classes_config.txt..."<<endl;

    iLine = 1;
    iIndexClassMax = -1;
    while (fgets(strLine, 500, pFileConfig)!=NULL)
    {
        // iChar = 0;
        strId = strstr(strLine, "id=");
        if (strId!=NULL)
            sscanf(strId+3, "%d", &iIndexClass);

        if (iIndexClass>iIndexClassMax)
            iIndexClassMax = iIndexClass;
        iLine++;
    }
    fclose(pFileConfig);

    if (iIndexClassMax<=0)
    {
        cout<<"ERROR: invalid maximum class index"<<endl;
        return false;
    }

    vectClassesProperties.resize(iIndexClassMax+1);

    vectClassesProperties[0].className="Unlabeled";
    vectClassesProperties[0].rgbClassColor = cv::Vec3b(0,0,0);

    for (iIndexClass=1; iIndexClass<=iIndexClassMax; iIndexClass++)
    {
        vectClassesProperties[iIndexClass].className="Unknown";
        vectClassesProperties[iIndexClass].rgbClassColor = cv::Vec3b(0,0,0);
    }

    pFileConfig = fopen("classes_config.txt", "rb");
    if (pFileConfig==NULL)
    {
        cout<<"ERROR: cannot reopen classes_config.txt"<<endl;
        return false;
    }

    iLine = 1;
    while (fgets(strLine, 500, pFileConfig)!=NULL)
    {
        // iChar = 0;
        strId = strstr(strLine, "id=");
        if (strId!=NULL)
        {
            sscanf(strId+3, "%d", &iIndexClass);
            if (iIndexClass>0 && iIndexClass<=iIndexClassMax)
            {
                CLabelClassProperties *pProp = &(vectClassesProperties[iIndexClass]);
                char *strNameLoc;
                strNameLoc = strstr(strLine, "name=");
                if (strNameLoc!=NULL)
                {
                    if (strNameLoc[5]=='\"')
                    {
                        char *strEndQuote;

                        strNameLoc+=6;
                        strEndQuote = strstr(strNameLoc,"\"");
                        if (strEndQuote!=NULL)
                        {
                            char strName[500];

                            strEndQuote[0] = 0;
                            strcpy(strName, strNameLoc);
                            pProp->className = strName;
                            strEndQuote[0] = '\"';

                            char *strRgbLoc;
                            strRgbLoc = strstr(strLine, "rgb=");
                            if (strRgbLoc!=NULL)
                            {
                                int iRed, iGreen, iBlue;
                                sscanf(strRgbLoc+5, "%d,%d,%d", &iRed, &iGreen, &iBlue);
                                if (iRed>=0 && iRed<=255)
                                    pProp->rgbClassColor[2] = (unsigned char)iRed;
                                else
                                    cout<<"WARNING: line "<<iLine<<": invalid red value in 'rgb' field"<<endl;

                                if (iGreen>=0 && iGreen<=255)
                                    pProp->rgbClassColor[1] = (unsigned char)iGreen;
                                else
                                    cout<<"WARNING: line "<<iLine<<": invalid green value in 'rgb' field"<<endl;

                                if (iBlue>=0 && iBlue<=255)
                                    pProp->rgbClassColor[0] = (unsigned char)iBlue;
                                else
                                    cout<<"WARNING: line "<<iLine<<": invalid blue value in 'rgb' field"<<endl;
                                cout<<"Class id="<<iIndexClass<<" name="<<pProp->className<<" rgb=("<<iRed<<","<<iGreen<<","<<iBlue<<")"<<endl;
                            }
                            else
                                cout<<"WARNING: line "<<iLine<<": cannot find 'rgb' field"<<endl;
                        }
                        else
                            cout<<"WARNING: line "<<iLine<<": cannot find ending '\"' for 'name' field"<<endl;
                    }
                    else
                        cout<<"WARNING: line "<<iLine<<": expected '\"' after 'name='"<<endl;
                }
                else
                    cout<<"WARNING: line "<<iLine<<": cannot find 'name' field"<<endl;
            }
            else
                cout<<"WARNING: line "<<iLine<<": invalid id="<<iIndexClass<<endl;
        }
        else {
            cout<<"WARNING: line "<<iLine<<": cannot find 'id' field"<<endl;
        }
        iLine++;
    }
    fclose(pFileConfig);

    return true;
}

void CLabeling::SetLabelDisk(const cv::Point &center, unsigned int radius,
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

	// Fill circle
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

void CLabeling::EmptyClass(unsigned int label)
{
	cv::Point p;

	// Set all pixels out of region
	for (p.y=0; p.y<rows; p.y++)
	    for (p.x=0; p.x<cols; p.x++)
            if (GetLabel(p)==label)
                SetLabel(p,0);
}

void CLabeling::Empty()
{
	setTo(0);
}

bool CLabeling::IsEmpty() const
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

void CLabeling::MakeLabelImage(cv::Mat &imgOutput) const
{
    copyTo(imgOutput);
}

void CLabeling::MakeLabelImageRGB(cv::Mat &imgOutput) const
{
    cv::Point p;

	imgOutput.create(size(), CV_8UC3);

	for (p.y=0; p.y<rows; p.y++)
		for (p.x=0; p.x<cols; p.x++)
			imgOutput.at<cv::Vec3b>(p) = vectClassesProperties[at<uchar>(p)].rgbClassColor;
}

void CLabeling::InitFromLabelImage(const cv::Mat &imgLabel)
{
	cv::Point p;

    Empty();

	if (imgLabel.type()!=CV_8U)
	{
		cerr<<"ERROR in CLabeling::InitFromLabelImage(...): selected label file is not 8-bit."<<endl;
		return;
	}

	if (imgLabel.size()!=size())
	{
		cerr<<"ERROR in CLabeling::InitFromLabelImage(...): the size of the selected label file does not match the image size."<<endl;
		return;
	}

	for (p.y=0; p.y<rows; p.y++)
		for (p.x=0; p.x<cols; p.x++)
            at<uchar>(p) = imgLabel.at<uchar>(p);
}

void CLabeling::SetLabel(const cv::Point &p, unsigned int uiLabelNew)
{
    at<uchar>(p) = (uchar)uiLabelNew;
}

unsigned int CLabeling::GetLabel(const cv::Point &p) const
{
    return (unsigned int)at<uchar>(p);
}
