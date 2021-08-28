#pragma once
#include <wx/wx.h>

class DrawLine
{
public:
	wxPoint start;
	wxPoint end;
	DrawLine(wxPoint s, wxPoint e);

	void Draw(wxDC& dc);
};