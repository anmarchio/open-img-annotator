#include "pch.h"
#include "MovableObject.h"

BEGIN_EVENT_TABLE(MovableObject, wxButton)
EVT_LEFT_DOWN(MovableObject::onMouseDown)
EVT_LEFT_UP(MovableObject::onMouseUp)
EVT_MOTION(MovableObject::onMove)
END_EVENT_TABLE()

MovableObject::MovableObject(wxPanel* parent) : wxButton(parent, wxID_ANY, wxT("Drag me around"))
{
	MovableObject::parent = parent;
	dragging = false;
}


void MovableObject::onMouseDown(wxMouseEvent& evt)
{
	CaptureMouse();
	x = evt.GetX();
	y = evt.GetY();
	dragging = true;
}


void MovableObject::onMouseUp(wxMouseEvent& evt)
{
	ReleaseMouse();
	dragging = false;
}


void MovableObject::onMove(wxMouseEvent& evt)
{
	if (dragging)
	{
		wxPoint mouseOnScreen = wxGetMousePosition();
		int newx = mouseOnScreen.x - x;
		int newy = mouseOnScreen.y - y;
		this->Move(parent->ScreenToClient(wxPoint(newx, newy)));
	}
}
