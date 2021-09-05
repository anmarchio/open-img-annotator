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
#include "PolygonShape.h"

class SuperPixelMask
{
public:
	SuperPixelMask();

	wxImage ConvertMatToWxImage(cv::Mat &img);
	cv::Mat ConvertWxImageToMat(wxImage &wx);

	void DisplayImageWindow(int use_video_capture, cv::VideoCapture cap, cv::Mat input_image, int algorithm, int region_size, int ruler, int min_element_size, int num_iterations);
	void getSuperpixelSLICContours(int min_element_size, wxImage &input_image, std::vector<PolygonShape>* superpixelLabels, int* testvar);
	int CreateSLICMask(bool use_video_capture_flag, std::string img_file_path);

	void trackbarChanged(int pos, void* data);
	int CreateSEEDSorLSCMask(bool IS_SEEDS, std::string img_file_path);
};