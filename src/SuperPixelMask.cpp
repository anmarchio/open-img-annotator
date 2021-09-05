#include "pch.h"
#include "SuperPixelMask.h"

using namespace cv;
using namespace std;

wxImage SuperPixelMask::ConvertMatToWxImage(Mat &img) {
	Mat im2;

	if (img.channels() == 1) {
		cvtColor(img, im2, COLOR_GRAY2RGB);
	}
	else if (img.channels() == 4)
	{
		cvtColor(img, im2, COLOR_BGRA2RGB);
	}
	else
	{
		cvtColor(img, im2, COLOR_BGR2RGB);
	}

	long imsize = im2.rows*im2.cols*im2.channels();

	wxImage wx(im2.cols, im2.rows, (unsigned char*)malloc(imsize), false);

	unsigned char* s = im2.data;
	unsigned char* d = wx.GetData();

	for (long i = 0; i < imsize; i++) {
		d[i] = s[i];
	}
	return wx;
}

cv::Mat SuperPixelMask::ConvertWxImageToMat(wxImage &wx) {
	Mat im2(Size(wx.GetWidth(), wx.GetHeight()), CV_8UC3, wx.GetData());

	cvtColor(im2, im2, COLOR_RGB2BGR);

	return im2;
}

static const char* window_name = "SLIC Superpixels";

static const char* keys =
"{h help      | | help menu}"
"{c camera    |0| camera id}"
"{i image     | | image file}"
"{a algorithm |1| SLIC(0),SLICO(1),MSLIC(2)}"
;

SuperPixelMask::SuperPixelMask() {
}

std::string window_name_str = "SLIC";


void SuperPixelMask::getSuperpixelSLICContours(int min_element_size, wxImage &input_image, std::vector<PolygonShape>* superpixelLabels, int* testvar)
{	
	const int algorithm = 0;
	const int region_size = 50;
	const int ruler = 30;
	const int num_iterations = 5;
	
	/*
		Missing: int* to Mat conversion
	*/
	Mat matInputImage = ConvertWxImageToMat(input_image);
	Mat result = Mat::zeros(matInputImage.rows, matInputImage.cols, CV_8UC3);
	Mat mask;

	Mat converted;
	cvtColor(matInputImage, converted, COLOR_BGR2HSV);

	Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(converted, algorithm + cv::ximgproc::SLIC, region_size, float(ruler));
	slic->iterate(num_iterations);
	slic->enforceLabelConnectivity(min_element_size);
	int numberOfSuperpixels = slic->getNumberOfSuperpixels();
	
	
	// get the contours for displaying
	slic->getLabelContourMask(mask, true);
	Mat labels = Mat(matInputImage.rows, matInputImage.cols, CV_8U);
	slic->getLabels(labels);
	/*
	------------------------------------------------------------------
		NOTE: At this point, we may want to think about
		converting contours to an 'array of segments of wxPoints'.

		(!) For performance pursposes, we also may want to limit
		the maximum number of segments & wxPoints to keep the application
		responsive at all times.
	------------------------------------------------------------------
	*/    
	Mat hierarchy;
	vector<vector<Point>> contours;
	Mat superpixel_mask = labels == 1;
	Mat dst;
	threshold(superpixel_mask, dst, 128.0f, 255.0, CV_8U);
	findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//input_image = ConvertMatToWxImage(result);
	
	/*
	for (;;)
	{
		// Show OpenCV mask in window
		Scalar color(0, 0, 255);
		drawContours(result, contours, 0, color, LINE_8, 8, hierarchy);
		//prepare a grey mask
		//cv::cvtColor(contours[0], maskMat, COLOR_BGR2GRAY);
		//threshold(maskMat, maskMat, 0, 255, THRESH_BINARY);
		// use mask to crop original image
		//result.copyTo(result, maskMat);
		namedWindow(window_name, 0);
		resizeWindow(window_name, cv::Size(matInputImage.cols, matInputImage.rows));
		imshow(window_name, result);
		int c = waitKey(1) & 0xff;
		if (c == 'q' || c == 'Q' || c == 27)
			break;
	}
	*/

	// Fill superpixelLabels
	for (int i = 0; i < contours.size(); i++)
	{
		PolygonShape* shp = new PolygonShape();
		for (int j = 0; j < contours[i].size(); j++)
		{
			wxPoint* pt = new wxPoint();
			pt->x = contours[i][j].x;
			pt->y = contours[i][j].y;
			
			if (pt->x < 0) pt->x = 0;
			else if(pt->x > matInputImage.cols)
			{
				pt->x = matInputImage.cols;
			}
			if (pt->y < 0) pt->y = 0;
			else if (pt->y > matInputImage.rows)
			{
				pt->y = matInputImage.rows;
			}			
			shp->insertPoint(pt);
		}
		superpixelLabels->push_back(*shp);
	}

	*testvar = 230;
}

void SuperPixelMask::DisplayImageWindow(int use_video_capture, cv::VideoCapture cap, cv::Mat input_image, int algorithm, int region_size, int ruler, int min_element_size, int num_iterations)
{
	namedWindow(window_name, 0);
	cv::createTrackbar("Algorithm", window_name, &algorithm, 1, 0);
	cv::createTrackbar("Region size", window_name, &region_size, 200, 0);
	cv::createTrackbar("Ruler", window_name, &ruler, 100, 0);
	cv::createTrackbar("Connectivity", window_name, &min_element_size, 100, 0);
	cv::createTrackbar("Iterations", window_name, &num_iterations, 12, 0);

	Mat result, mask;
	int display_mode = 0;

	for (;;)
	{
		Mat frame;
		if (use_video_capture)
			cap >> frame;
		else
			input_image.copyTo(frame);

		if (frame.empty())
			break;

		result = frame;
		Mat converted;
		cvtColor(frame, converted, COLOR_BGR2HSV);

		double t = (double)getTickCount();

		Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(converted, algorithm + cv::ximgproc::SLIC, region_size, float(ruler));
		slic->iterate(num_iterations);
		if (min_element_size > 0)
			slic->enforceLabelConnectivity(min_element_size);

		t = ((double)getTickCount() - t) / getTickFrequency();
		std::cout << "SLIC" << (algorithm ? 'O' : ' ')
			<< " segmentation took " << (int)(t * 1000)
			<< " ms with " << slic->getNumberOfSuperpixels() << " superpixels" << endl;

		// get the contours for displaying
		slic->getLabelContourMask(mask, true);
		result.setTo(Scalar(0, 0, 255), mask);

		// display output
		switch (display_mode)
		{
		case 0: //superpixel contours
			imshow(window_name, result);
			break;
		case 1: //mask
			imshow(window_name, mask);
			break;
		case 2: //labels array
		{
			// use the last x bit to determine the color. Note that this does not
			// guarantee that 2 neighboring superpixels have different colors.
			// retrieve the segmentation result
			Mat labels;
			slic->getLabels(labels);
			const int num_label_bits = 2;
			labels &= (1 << num_label_bits) - 1;
			labels *= 1 << (16 - num_label_bits);
			imshow(window_name, labels);
			break;
		}
		}

		int c = waitKey(1) & 0xff;
		if (c == 'q' || c == 'Q' || c == 27)
			break;
		else if (c == ' ')
			display_mode = (display_mode + 1) % 3;
	}
}

int SuperPixelMask::CreateSLICMask(bool use_video_capture_flag, std::string img_file_path)
{
	//int capture = cmd.get<int>("camera");
	int capture = 0;
	//string img_file = cmd.get<string>("image");
	string img_file = img_file_path;
	//int algorithm = cmd.get<int>("algorithm");
	int algorithm = 0;
	int region_size = 100;
	int ruler = 30;
	int min_element_size = 100;
	int num_iterations = 1;
	//bool use_video_capture = img_file.empty();
	bool use_video_capture = use_video_capture_flag;

	VideoCapture cap;
	Mat input_image;

	if (use_video_capture)
	{
		if (!cap.open(capture))
		{
			std::cout << "Could not initialize capturing..." << capture << "\n";
			return -1;
		}
	}
	else
	{
		input_image = imread(img_file);
		if (input_image.empty())
		{
			std::cout << "Could not open image..." << img_file << "\n";
			return -1;
		}
	}
	
	DisplayImageWindow(use_video_capture, cap, input_image, algorithm, region_size, ruler, min_element_size, num_iterations);

	return 0;
}

static bool init = false;

void SuperPixelMask::trackbarChanged(int, void*)
{
	init = false;
}

int SuperPixelMask::CreateSEEDSorLSCMask(bool IS_SEEDS, std::string img_file_path)
{
	Mat input_image;

	input_image = imread(img_file_path);

	if (input_image.empty())
	{
		std::cout << "Could not open image...\n";
		return -1;
	}

	namedWindow(window_name, 0);
	int num_iterations = 4;
	int prior = 2;
	bool double_step = false;
	int num_superpixels = 400;
	int num_levels = 4;
	int num_histogram_bins = 5;
	cv::createTrackbar("Number of Superpixels", window_name, &num_superpixels, 1000, false);
	cv::createTrackbar("Smoothing Prior", window_name, &prior, 5, false);
	cv::createTrackbar("Number of Levels", window_name, &num_levels, 10, false);
	cv::createTrackbar("Iterations", window_name, &num_iterations, 12, 0);

	Mat result, mask;
	Ptr<cv::ximgproc::SuperpixelSEEDS> seeds;
	Ptr<cv::ximgproc::SuperpixelLSC> lsc_seeds;
	int width, height;
	int display_mode = 0;

	for (;;)
	{
		Mat frame;
		/*
		if (use_video_capture)
			cap >> frame;
		else
		*/
		input_image.copyTo(frame);

		if (frame.empty())
			break;

		if (!init)
		{
			width = frame.size().width;
			height = frame.size().height;

			if (IS_SEEDS)
			{
				seeds = cv::ximgproc::createSuperpixelSEEDS(width, height, frame.channels(), num_superpixels,
					num_levels, prior, num_histogram_bins, double_step);
			}
			else {
				lsc_seeds = cv::ximgproc::createSuperpixelLSC(frame, (int)10, (float)0.07500000298F);
			}

			init = true;
		}
		Mat converted;
		cvtColor(frame, converted, COLOR_BGR2HSV);

		double t = (double)getTickCount();

		Mat labels;
		if (IS_SEEDS)
		{
			seeds->iterate(converted, num_iterations);
			result = frame;

			t = ((double)getTickCount() - t) / getTickFrequency();
			printf("SEEDS segmentation took %i ms with %3i superpixels\n",
				(int)(t * 1000), seeds->getNumberOfSuperpixels());

			/* retrieve the segmentation result */
			seeds->getLabels(labels);

			/* get the contours for displaying */
			seeds->getLabelContourMask(mask, false);
		}
		else
		{
			lsc_seeds->iterate(num_iterations);
			result = frame;

			t = ((double)getTickCount() - t) / getTickFrequency();
			printf("LSC segmentation took %i ms with %3i superpixels\n",
				(int)(t * 1000), lsc_seeds->getNumberOfSuperpixels());

			/* retrieve the segmentation result */
			lsc_seeds->getLabels(labels);

			/* get the contours for displaying */
			lsc_seeds->getLabelContourMask(mask, false);
		}
		result.setTo(Scalar(0, 0, 255), mask);

		/* display output */
		switch (display_mode)
		{
		case 0: //superpixel contours
			imshow(window_name, result);
			break;
		case 1: //mask
			imshow(window_name, mask);
			break;
		case 2: //labels array
		{
			// use the last x bit to determine the color. Note that this does not
			// guarantee that 2 neighboring superpixels have different colors.
			const int num_label_bits = 2;
			labels &= (1 << num_label_bits) - 1;
			labels *= 1 << (16 - num_label_bits);
			imshow(window_name, labels);
		}
		break;
		}


		int c = waitKey(1);
		if ((c & 255) == 'q' || c == 'Q' || (c & 255) == 27)
			break;
		else if ((c & 255) == ' ')
			display_mode = (display_mode + 1) % 3;
	}

	return 0;
}