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

static const char* window_name = "Open Image Annotator";

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

const std::string path = "samples/road_tiny.jpg";

bool MyApp::OnInit()
{	
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();

	// Create the main frame window
	ImageLabelWindow* frame = new ImageLabelWindow((wxFrame *)NULL,
		path,
		wxBITMAP_TYPE_ANY,
		wxID_ANY,
		window_name,
		wxPoint(100, 100), 
		wxDefaultSize);

	frame->Show(true);	

	return true;
}