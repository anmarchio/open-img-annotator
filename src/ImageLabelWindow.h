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
#endif 

class ImageLabelWindow : public wxPanel
{
	wxImage image;
	wxBitmap resized;
	int w, h;

	bool trackMouseMovement;
	int maxDistanceToStartPoint;

public:
	wxPoint startPoint;
	wxPoint edgePoint;
	std::vector<DrawLine> lines;
	std::vector<PolygonShape> polygons;	

	ImageLabelWindow(wxDialog * parent, wxString file, wxBitmapType format);

	void RecreateToolbar(wxPanel * parent);	
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
	void OnAbout(wxCommandEvent & event);
	void OnOpen(wxCommandEvent & event);
	void OnSave(wxCommandEvent & event);

	DECLARE_EVENT_TABLE();
};