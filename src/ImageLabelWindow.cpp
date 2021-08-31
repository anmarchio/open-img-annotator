#include "pch.h"
#include "ImageLabelWindow.h"

BEGIN_EVENT_TABLE(ImageLabelWindow, wxPanel)
	EVT_MOTION(ImageLabelWindow::OnMouseMove)
	EVT_LEFT_DOWN(ImageLabelWindow::OnLeftMouseDown)
	EVT_MENU(wxID_OPEN, ImageLabelWindow::OnOpen)
	EVT_MENU(wxID_SAVE, ImageLabelWindow::OnSave)
	EVT_MENU(wxID_HELP, ImageLabelWindow::OnAbout)
	EVT_MENU(wxID_EXIT, ImageLabelWindow::OnQuit)
	EVT_PAINT(ImageLabelWindow::paintEvent)
	//Size event
	//EVT_SIZE(ImageLabelWindow::OnSize)
END_EVENT_TABLE()

ImageLabelWindow::ImageLabelWindow(wxDialog* parent, wxString file, wxBitmapType format) : wxPanel(parent)
{

	wxPanel* m_panel = new wxPanel(this, wxID_ANY);
	RecreateToolbar(m_panel);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_panel->SetSizer(sizer); 
	m_panel->SetSize(wxSize(800, 40));

	parent->Bind(wxEVT_CHAR_HOOK, &ImageLabelWindow::OnKeyDown, this);
	
	image.LoadFile(file, format);
	w = -1;
	h = -1;

	trackMouseMovement = false;
	maxDistanceToStartPoint = 5;
	Show();
}

void ImageLabelWindow::RecreateToolbar(wxPanel* parent)
{
	enum
	{
		Tool_new,
		Tool_open,
		Tool_save,
		Tool_copy,
		Tool_cut,
		Tool_paste,
		Tool_print,
		Tool_help,
		Tool_exit,
		Tool_Max
	};

	wxBitmap toolBarBitmaps[Tool_Max];

	#if USE_XPM_BITMAPS
		#define INIT_TOOL_BMP(bmp) \
			toolBarBitmaps[Tool_##bmp] = wxBitmap(bmp##_xpm)
	#endif

	INIT_TOOL_BMP(open);
	INIT_TOOL_BMP(save);
	INIT_TOOL_BMP(help);
	INIT_TOOL_BMP(exit);

	int w = toolBarBitmaps[Tool_new].GetWidth(),
		h = toolBarBitmaps[Tool_new].GetHeight();


	const int ID_TOOLBAR = 500;
	static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;
	wxToolBar *toolBar = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_TEXT | wxTB_FLAT | wxTB_TOP, wxString("Toolbar"));

	toolBar->SetToolBitmapSize(wxSize(w, h));
	
	toolBar->AddTool(wxID_OPEN, "Open", toolBarBitmaps[Tool_open], "Open", wxITEM_NORMAL);
	toolBar->AddTool(wxID_SAVE, "Save", toolBarBitmaps[Tool_save], "Save As", wxITEM_NORMAL);
	toolBar->AddTool(wxID_HELP, "About", toolBarBitmaps[Tool_help], "About", wxITEM_NORMAL);
	toolBar->AddTool(wxID_EXIT, "Exit", toolBarBitmaps[Tool_exit], "Exit application", wxITEM_NORMAL);

	toolBar->Realize();
}

const int minElementSize = 100;

void ImageLabelWindow::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	//wxPaintDC dc(this);
	//render(dc);
	wxBufferedPaintDC dc(this);
	//dc.SetBackground(wxBrush(GetBackgroundColour()));
	//dc.Clear();

	render(dc);
	dc.SetFont(GetFont());
	dc.SetTextForeground(GetForegroundColour());
	dc.SetPen(wxPen(wxColor(255, 0, 0), 2));
	wxPoint pos = ScreenToClient(wxGetMousePosition());
	dc.DrawLabel(_T("[X,Y] : ") + wxString::Format(wxT("%i,%i"), pos.x, pos.y), wxNullBitmap, GetClientRect(), wxALIGN_BOTTOM | wxALIGN_RIGHT);
	
	// Draw current lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i].Draw(dc);
	}
	
	// draw all polygons
	for (size_t i = 0; i < polygons.size(); i++)
	{
		dc.SetBrush(*wxGREEN_BRUSH); // green filling
		dc.DrawPolygon(polygons[i].points, 0, 0, wxWINDING_RULE);
	}	

	// draw superpixel labels
	SuperPixelMask *spm = new SuperPixelMask();
	std::vector<PolygonShape> superpixelLabels;
	spm->getSuperpixelSLICContours(minElementSize, image, superpixelLabels);
	for (size_t i = 0; i < superpixelLabels.size(); i++)
	{
		dc.SetBrush(*wxGREEN_BRUSH); // green filling
		dc.DrawPolygon(superpixelLabels[i].points, 0, 0, wxWINDING_RULE);
	}

	if (GetCapture() == this) {
		DrawLine(edgePoint, pos).Draw(dc);
	}
}

void ImageLabelWindow::paintNow()
{
	wxClientDC dc(this);	
	render(dc);
}

void ImageLabelWindow::render(wxDC&  dc)
{
	int neww, newh;
	dc.GetSize(&neww, &newh);

	if (neww != w || newh != h)
	{
		resized = wxBitmap(image.Scale(neww, newh /*, wxIMAGE_QUALITY_HIGH */));
		w = neww;
		h = newh;

		dc.DrawBitmap(resized, 0, 0, false);
	}
	else {
		dc.DrawBitmap(resized, 0, 0, false);
	}
}

void ImageLabelWindow::OnSize(wxSizeEvent& event) {
	Refresh();
	event.Skip();
}

/////////////////////////////////////
// Mouse Events for Drawing
/////////////////////////////////////
bool polygonFinished = true;
bool trackMouseMovement = false;
int maxDistanceToStartPoint = 20;

void ImageLabelWindow::OnLeftMouseDown(wxMouseEvent& event)
{
	wxPoint currentPosition = event.GetPosition();
	if (polygonFinished)
	{
		edgePoint = currentPosition;
	}
	if (trackMouseMovement)
	{
		if (isCloseToStartPoint(startPoint, currentPosition))
		{
			trackMouseMovement = false;
			if (GetCapture() == this) {
				std::vector<DrawLine>::iterator it = lines.begin();
				it = lines.insert(it, DrawLine(edgePoint, startPoint));
			}
			finishPolygon(currentPosition);
			ReleaseMouse();
		}
		if (GetCapture() == this) {
			std::vector<DrawLine>::iterator it = lines.begin();
			it = lines.insert(it, DrawLine(edgePoint, currentPosition));
			edgePoint = currentPosition;
		}
		Refresh();
	}
	else
	{
		CaptureMouse();
		startPoint = event.GetPosition();
		trackMouseMovement = true;
		polygonFinished = false;
	}
}

void ImageLabelWindow::finishPolygon(wxPoint currentPosition)
{
	polygonFinished = true;
	std::vector<PolygonShape>::iterator it = polygons.begin();
	polygons.insert(it, PolygonShape(lines));
	lines.clear();
}

bool ImageLabelWindow::isCloseToStartPoint(wxPoint startPosition, wxPoint currentPosition)
{
	if (currentPosition.x >= startPosition.x - maxDistanceToStartPoint
		&& currentPosition.x <= startPosition.x + maxDistanceToStartPoint
		&& currentPosition.y >= startPosition.y - maxDistanceToStartPoint
		&& currentPosition.y <= startPosition.y + maxDistanceToStartPoint)
		return true;
	return false;
}

void ImageLabelWindow::OnMouseMove(wxMouseEvent& event)
{
	if(trackMouseMovement)
		Refresh();
}

void ImageLabelWindow::OnEraseBackground(wxEraseEvent& Event)
{

}

void ImageLabelWindow::OnKeyDown(wxKeyEvent& evt)
{
	// execute when ESC (code 27) is pressed
	if ((int)evt.GetKeyCode() == 27)
	{
		if (GetCapture() == this)
		{
			ReleaseMouse();
			trackMouseMovement = false;
		}
	}
	evt.Skip();
}

void ImageLabelWindow::OnOpen(wxCommandEvent& event)
{
	(void)wxMessageBox("Open JSON File", "Open File");
}

void ImageLabelWindow::OnSave(wxCommandEvent& event)
{
	wxFileDialog
		saveFileDialog(this, _("Save TXT file"), "", "",
			"TXT files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;

	std::ofstream outputFile;
	outputFile.open(saveFileDialog.GetPath().ToStdString());
	if (!outputFile.is_open())
	{
		wxLogError("Cannot save current contents in file '%s'.", saveFileDialog.GetPath());
		return;
	}
	else
	{
		for (int i = 0; i < polygons.size(); i++)
		{
			for (size_t j = 0; j < polygons[i].points->size(); j++)
			{
				wxPoint* pt = polygons[i].points->operator[](j);
				outputFile << pt->x << ";" << pt->y << ";";
			}
			outputFile << "\n";
		}
	}
	outputFile.close();
}

void ImageLabelWindow::OnAbout(wxCommandEvent& event)
{
	(void)wxMessageBox("Image label editor using polygons", "About Label Editor");
}

void ImageLabelWindow::OnQuit(wxCommandEvent& event)
{
	//exit(3); // hard exit
	Close(true);
}