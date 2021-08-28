#include "pch.h"
#include "DrawLine.h"

DrawLine::DrawLine(wxPoint s, wxPoint e) : start(s), end(e)
{
}

void DrawLine::Draw(wxDC& dc) {
	dc.DrawLine(start, end);
}