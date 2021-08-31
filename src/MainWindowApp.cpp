#include "pch.h"
#include "wx/wxprec.h"
#include "MainWindowApp.h"
#include "SuperPixelMask.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>w
#include <opencv2/core/utility.hpp>

#include <opencv2/ximgproc.hpp>

#include <iostream>
#include <ctype.h>
#include <stdio.h>

using namespace cv;
using namespace std;

static const char* window_name = "SLIC Superpixels";

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.


// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
	// call the base class initialization method, currently it only parses a
	// few common command-line options but it could be do more in the future
	
	if (!wxApp::OnInit())
		return false;

	// create the main application window
	
	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	//frame->Show(true);
	OnTestImageLabeller();

	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned false here, the
	// application would exit immediately.
		
	//char* arg = new char();
	//cv::ximgproc::mainSLIC(0, &arg);
	//SuperPixelMask *spm;
	//spm = new SuperPixelMask();
	//spm->CreateSLICMask(false, "C:\\dev\\applets\\acquisition\\paramTuner\\samples\\white_fabric_tiny.jpg");
	//spm->CreateSEEDSorLSCMask(1, "C:\\dev\\applets\\acquisition\\paramTuner\\samples\\white_fabric_tiny.jpg");
	//spm->CreateSEEDSorLSCMask(0, "C:\\dev\\applets\\acquisition\\paramTuner\\samples\\road_tiny.jpg");
	
	return true;
}
/*
OnTestImageLabeller is only used for testing purposes.
It will immediately create the ImageLabelWindow frame for faster coding.
*/
void MyApp::OnTestImageLabeller()
{
	const std::string path = "C:\\dev\\applets\\acquisition\\paramTuner\\samples\\white_fabric_tiny.jpg";

	// Image Labeller Panel
	wxDialog *frame;
	ImageLabelWindow * drawPane;

	// make sure to call this first
	wxInitAllImageHandlers();

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	frame = new wxDialog(NULL, wxID_ANY, wxT("Label Image"), wxPoint(50, 50), wxSize(800, 600));

	// then simply create like this
	drawPane = new ImageLabelWindow(frame, path, wxBITMAP_TYPE_ANY);
	sizer->Add(drawPane, 1, wxEXPAND);


	frame->SetSizer(sizer);

	frame->ShowModal();
}
