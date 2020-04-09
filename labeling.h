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

#ifndef LABELING_H
#define LABELING_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>

class CLabelClassProperties
{
  public:
    std::string className;
    cv::Vec3b rgbClassColor;
  public:
    CLabelClassProperties() {}
};

class Labeling : public cv::Mat
{
  // Static members
  public:
    static std::vector<CLabelClassProperties> vectClassesProperties;

  // Static member functions
  public:

    // Initialize class properties (names and colors) by parsing file 'classes_config.txt'
    static void InitClassesProperties();

  public:
	Labeling() {}

    // Create an empty labeling with given size
    // Labels are stored on unsigned chars (0 = unlabeled)
    void Create(const cv::Size &);

    // Get or set the label of a given pixel
	void SetLabel(const cv::Point &, unsigned int);
	unsigned int GetLabel(const cv::Point &) const;

    // Change labels in a disk
    // Params : center, radius, base label, target label
    // boolean indicating if target label should replace all current labels (base label is ignored)
    void SetLabelDisk(const cv::Point &, unsigned int, unsigned int, unsigned int, bool bReplaceAll=false);

    // Unlabel all pixels of a given class
    virtual void EmptyClass(unsigned int);

    // Unlabel all pixels
	void Empty();

    bool IsEmpty() const;

    void MakeLabelImage(cv::Mat &) const;
    void MakeLabelImageRGB(cv::Mat &) const;

    void InitFromLabelImage(const cv::Mat &);
};

#endif
