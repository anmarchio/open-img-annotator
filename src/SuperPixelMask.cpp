#include "pch.h"
#include "SuperPixelMask.h"

using namespace cv;
using namespace std;

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


Mat SuperPixelMask::getSuperpixelSLICContours(int min_element_size, Mat input_image)
{	
	const int algorithm = 0;
	const int region_size = 100;
	const int ruler = 30;
	const int num_iterations = 5;
	
	Mat result, mask;

	Mat converted;
	cvtColor(input_image, converted, COLOR_BGR2HSV);

	Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(converted, algorithm + cv::ximgproc::SLIC, region_size, float(ruler));
	slic->iterate(num_iterations);
	slic->enforceLabelConnectivity(min_element_size);

	int numberOfSuperpixels = slic->getNumberOfSuperpixels();
	
	// get the contours for displaying
	slic->getLabelContourMask(mask, true);
	result.setTo(Scalar(0, 0, 255), mask);	
	/*
	------------------------------------------------------------------
		NOTE: At this point, we may want to think about
		converting contours to an 'array of segments of wxPoints'.
		
		(!) For performance pursposes, we also may want to limit 
		the maximum number of segments & wxPoints to keep the application
		responsive at all times.
	------------------------------------------------------------------
	*/
	// return contour points from polygon mask
	return result;
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
			cout << "Could not initialize capturing..." << capture << "\n";
			return -1;
		}
	}
	else
	{
		input_image = imread(img_file);
		if (input_image.empty())
		{
			cout << "Could not open image..." << img_file << "\n";
			return -1;
		}
	}
	namedWindow(window_name, 0);
	createTrackbar("Algorithm", window_name, &algorithm, 1, 0);
	createTrackbar("Region size", window_name, &region_size, 200, 0);
	createTrackbar("Ruler", window_name, &ruler, 100, 0);
	createTrackbar("Connectivity", window_name, &min_element_size, 100, 0);
	createTrackbar("Iterations", window_name, &num_iterations, 12, 0);

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
		cout << "SLIC" << (algorithm ? 'O' : ' ')
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
		cout << "Could not open image...\n";
		return -1;
	}

	namedWindow(window_name, 0);
	int num_iterations = 4;
	int prior = 2;
	bool double_step = false;
	int num_superpixels = 400;
	int num_levels = 4;
	int num_histogram_bins = 5;
	createTrackbar("Number of Superpixels", window_name, &num_superpixels, 1000, false);
	createTrackbar("Smoothing Prior", window_name, &prior, 5, false);
	createTrackbar("Number of Levels", window_name, &num_levels, 10, false);
	createTrackbar("Iterations", window_name, &num_iterations, 12, 0);

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