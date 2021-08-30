#pragma once
#include <wx/wx.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>

#include <opencv2/ximgproc.hpp>

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include "utils.h"

class SuperPixelMask
{
private:
	wxImage ConvertMatToWxImage(Mat &img);
	Mat ConvertWxImageToMat(wxImage &wx);
public:
	SuperPixelMask();
	~SuperPixelMask();

	void getSuperpixelSLICContours(int min_element_size, wxImage &input_image);
	int CreateSLICMask(bool use_video_capture_flag, std::string img_file_path);

	void trackbarChanged(int pos, void* data);
	int CreateSEEDSorLSCMask(bool IS_SEEDS, std::string img_file_path);
};