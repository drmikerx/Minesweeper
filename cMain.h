#pragma once
// This class generates our main window of the application. Is created by our app class

#include "wx/wx.h"
#include "wx/numdlg.h"

class cMain : public wxFrame
{
public:

	//cOptions* options_frame = nullptr;

	int numMinesToUse = 30;

	int nFieldWidth = 10;
	int nFieldHeight = 10;
	wxButton** btn;		// This will be an array of pointers to buttons

	// Store the status of the mines
	int* nField = nullptr;	// array of integers which says whether mine exists there or not
	bool bFirstClick = true;	// When the first button is pressed, that's when the mine field is populated. Makes sure that first click doesn't auto lose.

	int* enableStatus = nullptr;	// array of ints used to track buttons pressed. will help with determining win state

	cMain();
	~cMain();

	void OnButtonClicked(wxCommandEvent& evt);
	void OnNewClicked(wxCommandEvent& evt);
	void OnExit(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnRightClick(wxMouseEvent& evt);
	void OnOptionsClicked(wxCommandEvent& evt);

	int getNumMinesToUse();
	void setNumMinesToUse(int mines);

	void startNewGameBoard();
	void resetButtonPressedStatus();
	void revealMines();

	bool checkForWin();

	wxDECLARE_EVENT_TABLE();
};

