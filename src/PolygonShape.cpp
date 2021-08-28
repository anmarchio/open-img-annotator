#include "pch.h"
#include "PolygonShape.h"

PolygonShape::PolygonShape(wxPointList* pts, int number)
{
	points = new wxPointList();
	points = pts;
	size = number;
};

PolygonShape::PolygonShape(std::vector<DrawLine> lines)
{
	points = new wxPointList();
	for (int i = 0; i < lines.size(); i++)
	{
		points->Insert(new wxPoint(lines[i].start.x, lines[i].start.y));
	}
	size = lines.size();
};

void PolygonShape::insertPoint(wxPoint pt)
{
	points->Append(&pt);
	size++;
}


PolygonShape::~PolygonShape()
{
}

void PolygonShape::Draw(wxBufferedPaintDC& dc)
{
	dc.SetPen(wxPen(wxColor(255, 0, 0), 2));
	dc.SetBrush(*wxGREEN_BRUSH); // green filling
	dc.DrawPolygon(points, 0, 0, wxODDEVEN_RULE);
}
