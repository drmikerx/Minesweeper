#include "cApp.h"

wxIMPLEMENT_APP(cApp);	// This line needs to appear once in the program code. This initializes the "main" method of the program. 
						//Since this is the application class definition file, this is the most convenient place

cApp::cApp()
{

}


cApp::~cApp()
{

}


bool cApp::OnInit()
{
	m_frame1 = new cMain();
	m_frame1->Show();

	return true;
}