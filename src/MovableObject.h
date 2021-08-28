#pragma once
#include "ImageLabelWindow.h"
#include <wx/wx.h>
#include "wx/utils.h"


class MovableObject : public wxButton
{
	bool dragging;
	int x, y;
	wxPanel* parent;

public:
	MovableObject(wxPanel* parent);

	void onMouseDown(wxMouseEvent& evt);
	void onMouseUp(wxMouseEvent& evt);
	void onMove(wxMouseEvent& evt);

private:
	DECLARE_EVENT_TABLE()
};
