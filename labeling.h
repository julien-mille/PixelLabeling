/*
	labeling.h

	Copyright 2019 Julien Mille (julien.mille@insa-cvl.fr)

	Header file of library implementing the multiple region labeling
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

// Class CLabeling
class CLabeling : public cv::Mat
{
  // Static members
  public:
    static std::vector<CLabelClassProperties> vectClassesProperties;

  // Static member functions
  public:
    static bool InitClassesProperties();

  public:
	// Default constructor
	CLabeling() {}

    // Get or set the label of a given pixel
	virtual void SetLabel(const cv::Point &, unsigned int);
	virtual unsigned int GetLabel(const cv::Point &) const;

    // Change labels in a disk
    // Params : center, radius, base label, target label
    // boolean indicating if target label should replace all current labels (base label is ignored)
    virtual void SetLabelDisk(const cv::Point &, unsigned int, unsigned int, unsigned int, bool bReplaceAll=false);

    // Unlabel all pixels of a given class
    virtual void EmptyClass(unsigned int);

    // Unlabel all pixels
	virtual void Empty();

    virtual bool IsEmpty() const;

    virtual void MakeLabelImage(cv::Mat &) const;
    virtual void MakeLabelImageRGB(cv::Mat &) const;

    virtual void InitFromLabelImage(const cv::Mat &);

    // void GetConnectedComponents(cv::Mat &, std::vector<std::pair<cv::Point, cv::Point > > &) const;
};

#endif
