#pragma once
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "ImageLabelWindow.h"

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
	// override base class virtuals
	// ----------------------------

	// this one is called on application startup and is a good place for the app
	// initialization (doing it here and not in the ctor allows to have an error
	// return: if OnInit() returns false, the application terminates)
	virtual bool OnInit() wxOVERRIDE;
	void OnTestImageLabeller();
	int CreateSLICMask(std::string img_file_path);

	void trackbarChanged(int pos, void* data);
	int CreateSEEDSorLSCMask(bool IS_SEEDS, std::string img_file_path);
};
