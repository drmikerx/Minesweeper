#pragma once

#include "wx/wx.h"
#include "cMain.h"

class cApp : public wxApp
{
private:
	cMain* m_frame1 = nullptr;

public:
	cApp();
	~cApp();
	virtual bool OnInit();		// You have to override the OnInit function in your main application class
};



