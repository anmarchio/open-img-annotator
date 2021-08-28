#pragma once
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>

#include <opencv2/ximgproc.hpp>

#include <ctype.h>
#include <stdio.h>
#include <iostream>

class SuperPixelMask
{
public:
	SuperPixelMask();
	~SuperPixelMask();

	int CreateSLICMask(bool use_video_capture_flag, std::string img_file_path);

	void trackbarChanged(int pos, void* data);
	int CreateSEEDSorLSCMask(bool IS_SEEDS, std::string img_file_path);
};