#include "pch.h"
#include "ImageLabelWindow.h"

BEGIN_EVENT_TABLE(ImageLabelWindow, wxPanel)
EVT_MOTION(ImageLabelWindow::OnMouseMove)
EVT_LEFT_DOWN(ImageLabelWindow::OnLeftMouseDown)
EVT_MENU(wxID_OPEN, ImageLabelWindow::OnOpen)
EVT_MENU(wxID_SAVE, ImageLabelWindow::OnSave)
EVT_MENU(wxID_HELP, ImageLabelWindow::OnAbout)
EVT_MENU(wxID_FILE1, ImageLabelWindow::OnComputeSuperpixels)
EVT_MENU(wxID_EXIT, ImageLabelWindow::OnQuit)
EVT_PAINT(ImageLabelWindow::paintEvent)
//Size event
//EVT_SIZE(ImageLabelWindow::OnSize)
END_EVENT_TABLE()

ImageLabelWindow::ImageLabelWindow(wxFrame* parent, 
	wxString file, 
	wxBitmapType format,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	displaySuperpixels = false;

	image.LoadFile(file, format);
	w = -1;
	h = -1;	

	//create FlexGridSizer
	wxFlexGridSizer* sizer = new wxFlexGridSizer(1);
	sizer->SetRows(11);

	//create sliders
	regionSizeSlider = new wxSlider(this, ID_SLIDER0, 0, 0, 100, wxPoint(50, 30), wxSize(400, 30), wxSL_HORIZONTAL);
	regionSizeSlider->SetAutoLayout(true);

	//bind eventhandlers
	regionSizeSlider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, wxScrollEventHandler(ImageLabelWindow::OnScroll), this);
	regionSizeSlider->Enable(false);

	//create textlines
	valueRegionSizeSlider = new wxStaticText(this, wxID_ANY, wxT("0"));
	valueRegionSizeSlider->SetAutoLayout(true);

	toolBarPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 50), wxSL_HORIZONTAL);
	toolBarPanel->SetAutoLayout(true);
	RecreateToolbar(toolBarPanel);
	//parent->Bind(wxEVT_CHAR_HOOK, &ImageLabelWindow::OnKeyDown, this);

	//imagePanel = new ImageLabelWindow(parent, file, format);
	//imagePanel->SetSize(image.GetWidth(), image.GetHeight());	

	//add elements to Sizer
	sizer->Insert(0, toolBarPanel);
	sizer->Insert(1, regionSizeSlider);
	sizer->Insert(2, valueRegionSizeSlider);
	//sizer->Insert(3, imagePanel);

	this->SetSize((image.GetWidth() > 400) ? image.GetWidth() : 400, image.GetHeight() + 100);
	SetSizer(sizer);
	
	trackMouseMovement = false;
	maxDistanceToStartPoint = 5;
	Show();
}

int ImageLabelWindow::getHeaderPanelHeight()
{
	int toolBarPanelX, toolBarPanelY;
	toolBarPanel->GetSize(&toolBarPanelX, &toolBarPanelY);
	int regionSizeSliderX, regionSizeSliderY;
	regionSizeSlider->GetSize(&regionSizeSliderX, &regionSizeSliderY);
	return toolBarPanelY + regionSizeSliderY;
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
	wxToolBar *toolBar = new wxToolBar(parent, wxID_FILE1, wxDefaultPosition, wxDefaultSize, wxTB_TEXT | wxTB_FLAT | wxTB_TOP, wxString("Toolbar"));

	toolBar->SetToolBitmapSize(wxSize(w, h));
	
	toolBar->AddTool(wxID_OPEN, "Open", toolBarBitmaps[Tool_open], "Open", wxITEM_NORMAL);
	toolBar->AddTool(wxID_SAVE, "Save", toolBarBitmaps[Tool_save], "Save As", wxITEM_NORMAL);
	toolBar->AddTool(wxID_HELP, "About", toolBarBitmaps[Tool_help], "About", wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_FILE1, "Superpixels", toolBarBitmaps[Tool_help], "Show Superpixels", wxITEM_CHECK);
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_EXIT, "Quit", toolBarBitmaps[Tool_exit], "Quit application", wxITEM_NORMAL);
	toolBar->Realize();
}

//////////////////////////
// Bitmap Draw Actions
//////////////////////////
void ImageLabelWindow::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	//wxPaintDC dc(this);
	//render(dc);
	wxBufferedPaintDC dc(this);
	//wxClientDC dc(this);

	render(dc);
	dc.SetFont(GetFont());
	dc.SetTextForeground(GetForegroundColour());
	dc.SetPen(wxPen(wxColor(255, 0, 0), 2));
	wxPoint pos = ScreenToClient(wxGetMousePosition());
	dc.DrawLabel(_T("[X,Y] : ") + wxString::Format(wxT("%i,%i"), 0, pos.y), wxNullBitmap, GetClientRect(), wxALIGN_BOTTOM | wxALIGN_RIGHT);

	// Draw current lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i].Draw(dc);
	}

	// draw superpixel labels
	for (size_t i = 0; i < superpixelLabels.size(); i++)
	{
		dc.SetBrush(*wxGREEN_BRUSH); // green filling
		//dc.DrawPolygon(superpixelLabels[i].points, 0, 0, wxWINDING_RULE);
		for (size_t j = 0; j < superpixelLabels[0].size; j++)
		{
			//wxPoint* pt = superpixelLabels[0].points->Item(j)->GetData();
			//dc.DrawPoint((wxCoord)pt->x, (wxCoord)pt->y);
			dc.SetBrush(*wxGREEN_BRUSH); // green filling
			//dc.DrawPolygon(superpixelLabels[i].points, 0, 0, wxWINDING_RULE);
			dc.DrawLines(superpixelLabels[i].points, 0, 0);
		}
	}
	// draw all polygons
	for (size_t i = 0; i < polygons.size(); i++)
	{
		dc.SetBrush(*wxGREEN_BRUSH); // green filling
		dc.DrawPolygon(polygons[i].points, 0, 0, wxWINDING_RULE);
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
	dc.DrawBitmap(image, 0, getHeaderPanelHeight(), false);
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
	if (trackMouseMovement)
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

/////////////////////////////////////
// Panel Icon Events
/////////////////////////////////////
void ImageLabelWindow::OnOpen(wxCommandEvent& event)
{
	// Open File Dialog, select image
	wxFileDialog
		openFileDialog(this, _("Open image file"), "", "",
			"Image files (*.bmp;*.gif;*.png;*.jpg)|*.bmp;*.gif;*.jpg;*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() != wxID_CANCEL)
	{
		imgFilePath = openFileDialog.GetPath();
	}
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
		for (int i = 0; i < imagePanel->polygons.size(); i++)
		{
			for (size_t j = 0; j < imagePanel->polygons[i].points->size(); j++)
			{
				wxPoint* pt = imagePanel->polygons[i].points->operator[](j);
				outputFile << pt->x << ";" << pt->y << ";";
			}
			outputFile << "\n";
		}
	}
	outputFile.close();
}

void ImageLabelWindow::OnAbout(wxCommandEvent& event)
{
	(void)wxMessageBox("Open Img Annotator is a tiny label editor for efficient and simple image annotation using polygons and superpixels", "About Open Img Annotator");
}

void ImageLabelWindow::OnQuit(wxCommandEvent& event)
{
	quick_exit(0);
}

void ImageLabelWindow::OnScroll(wxScrollEvent& WXUNUSED(event))
{
	regionSizeSlider->GetValue();
	//Updating slidervalue in textline
	valueRegionSizeSlider->SetLabel(wxString(std::to_string(regionSizeSlider->GetValue())));
	
	if (displaySuperpixels && regionSizeSlider->GetValue() > 0)
	{
		SuperPixelMask *spm = new SuperPixelMask();		
		std::vector<PolygonShape> tmp_superpixelLabels;
		spm->getSuperpixelSLICContours(regionSizeSlider->GetValue(), image, &tmp_superpixelLabels/*&imagePanel->superpixelLabels*/);
	}
	Refresh();
}

void ImageLabelWindow::OnComputeSuperpixels(wxCommandEvent& event)
{
	displaySuperpixels = !displaySuperpixels;
	regionSizeSlider->Enable(!regionSizeSlider->IsEnabled());
}