#pragma once
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <vector>
#include "DrawLine.h"

class PolygonShape
{
private:

public:
	wxPointList* points;
	int size;

	PolygonShape();
	PolygonShape(wxPointList &points, int number);
	PolygonShape(std::vector<DrawLine> lines);
	void insertPoint(wxPoint* pt);
	~PolygonShape();

	void Draw(wxBufferedPaintDC & dc);
};