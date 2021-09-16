#pragma once
#include <fstream>
#include <vector>
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/toolbar.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/frame.h>
#include <wx/wxprec.h>
#include <wx/tglbtn.h>
#include "DrawLine.h"
#include "PolygonShape.h"
#include "SuperPixelMask.h"

#define USE_XPM_BITMAPS 1

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

#if USE_XPM_BITMAPS
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/exit.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/superpixels.xpm"

#endif 

const int ID_SLIDER0 = 100;

class ImageLabelWindow : public wxFrame
{
	wxImage image;
	wxBitmap resized;
	wxString imgFilePath;

private:
	bool polygonFinished;
	bool trackMouseMovement;
	int maxDistanceToStartPoint;
	bool displaySuperpixels;
	int w, h;

	void initializeValues(wxString file, wxBitmapType format);

public:
	ImageLabelWindow(wxFrame *parent,
		wxString file, 
		wxBitmapType format,
		wxWindowID id = wxID_ANY,
		const wxString& title = "Image Label Editor",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN);

	wxPanel* toolBarPanel;
	wxSlider* regionSizeSlider;
	wxStaticText* valueRegionSizeSlider; 

	wxPoint startPoint;
	wxPoint edgePoint;

	std::vector<DrawLine> lines;
	std::vector<PolygonShape> polygons;	
	std::vector<PolygonShape> superpixelLabels;
		
	void RecreateToolbar(wxPanel * parent);
	int getHeaderPanelHeight();

	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);

	void finishPolygon(wxPoint currentPosition);
	bool isCloseToStartPoint(wxPoint startPosition, wxPoint currentPosition);
	void OnLeftMouseDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnEraseBackground(wxEraseEvent& Event);
	void OnKeyDown(wxKeyEvent & evt);
	void OnQuit(wxCommandEvent & event);
	void OnClose(wxCloseEvent & event);
	void OnAbout(wxCommandEvent & event);
	void OnOpen(wxCommandEvent & event);
	void OnSave(wxCommandEvent & event);
	void OnScroll(wxScrollEvent& WXUNUSED(event));

	void OnComputeSuperpixels(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();
};