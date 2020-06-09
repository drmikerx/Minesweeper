#include "cMain.h"

enum
{
	ID_New = 1,
	ID_Options = 2
};

// We didn't use the event table for the event handlers of buttons because we wanted to create them DYNAMICALLY
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_MENU(ID_New, cMain::OnNewClicked)
	EVT_MENU(wxID_EXIT, cMain::OnExit)
	EVT_MENU(wxID_ABOUT, cMain::OnAbout)
	EVT_MENU(ID_Options, cMain::OnOptionsClicked)
wxEND_EVENT_TABLE()

// We are utilizing the wxFrame constructor and giving arguments, and then also running extra code we specify below
// nullptr cuz no parent of this window. This window is the parent of everything else
// wxPoint is number of pixels relative to the screen that we want the window to appear when it launches

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Minesweeper", wxPoint(30, 30), wxSize(798, 593))
{
	// Initialize top menu
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_New, "&New Game\tCtrl-N", "Start a new game - erases current progress");
	menuFile->Append(ID_Options, "&Options\tCtrl-O", "Set how many mines you want on the board");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to Minesweeper");
	

	btn = new wxButton * [nFieldWidth * nFieldHeight];	// Initialize our array of buttons when the window is created

	// A sizer manages the size of a child relative to its parent window. So by putting the buttons in a sizer, they will adapt to the window size
	// There are different versions of sizers available, we're just using the grid one here
	// wxGridSizer(# of columns, # of rows, padding up/down, padding left/right)
	wxGridSizer* grid = new wxGridSizer(nFieldWidth, nFieldHeight, 0, 0);

	// Create array of mines

	nField = new int[nFieldWidth * nFieldHeight];

	enableStatus = new int[nFieldWidth * nFieldHeight];		// array to keep track of which buttons have been enabled

	// Set all buttons to unpressed status
	resetButtonPressedStatus();


	// Make the font of the numbers easier to read

	wxFont font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);

	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			// Add buttons to the array of buttons each with a parent of this window and a unique updating ID number
			btn[y * nFieldWidth + x] = new wxButton(this, 10000 + (y * nFieldWidth + x));

			// Set the font of each button to the more easy to read setup from above
			btn[y * nFieldWidth + x]->SetFont(font);

			// Add the new button to the sizer object for placement in the window
			// wxEXPAND | wxALL says we want the button to occupy the whole space of its square in the grid
			grid->Add(btn[y * nFieldWidth + x], 1, wxEXPAND | wxALL);

			// Dynamically add an event handler to each of the buttons. Bind() is the alternative to the event table that lets us dynamically add these handlers
			btn[y * nFieldWidth + x]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnButtonClicked, this);

			// Dynamically add the right button click event handler to each button as well
			btn[y * nFieldWidth + x]->Bind(wxEVT_RIGHT_DOWN, &cMain::OnRightClick, this);

			// Set the defaults of the mine status to 0 (no mine in that space to start)
			nField[y * nFieldWidth + x] = 0;
		}
	}

	// Tell the parent window what sizer to use
	this->SetSizer(grid);

	// Tell the grid to restructure itself based on what is stored within
	grid->Layout();
}


cMain::~cMain()
{
	// The array of buttons we created need to be deleted or we'll have memory leaks
	// The array of pointers was the one thing we created with 'new' that was not a wxWidgets item
	// wxWidget items are freed by themselves
	delete[]btn;
	delete[]nField;
	delete[]enableStatus;
}

void cMain::OnButtonClicked(wxCommandEvent& evt)
{
	// Get coordinate of button in field array
	// Each button was assigned a unique ID so we can do this
	int x = (evt.GetId() - 10000) % nFieldWidth;
	int y = (evt.GetId() - 10000) / nFieldWidth;

	// On the very first click of the game, generate the mine field

	if (bFirstClick)
	{
		//int mines = 30;
		int mines = this->getNumMinesToUse();

		// Generate random locations for the mines in the grid
		while (mines)
		{
			int rx = rand() % nFieldWidth;
			int ry = rand() % nFieldHeight;

			// If no mine already in that space, place one there. Also check that the mine is not placed at the location of first button click
			if (nField[ry * nFieldWidth + rx] == 0 && rx != x && ry != y)
			{
				nField[ry * nFieldWidth + rx] = -1;
				mines--;
			}
		}

		bFirstClick = false;
	}

	// Disable button from being able to be pressed again

	btn[y * nFieldWidth + x]->Enable(false);
	enableStatus [y * nFieldWidth + x] = 1;		// Button has been pressed

	// Check if player hit mine
	if (nField[y * nFieldWidth + x] == -1)
	{
		// Game over - mine hit
		wxMessageBox("BOOOOOOM !! - Game Over :(");

		// Show where the mines were
		revealMines();

	}

	else
	{
		// Still going so count neighboring mines
		int mine_count = 0;
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (x + i >= 0 && x + i < nFieldWidth && y + j >= 0 && y + j < nFieldHeight)
				{
					if (nField[(y + j) * nFieldWidth + (x + i)] == -1)
					{
						mine_count++;
					}
				}
			}
		}

		// Update button's label to show mine count if > 0
		if (mine_count > 0)
		{
			btn[y * nFieldWidth + x]->SetLabel(std::to_string(mine_count));

			/*
			switch (wxAtoi(btn[y * nFieldWidth + x]->GetLabel()))
			{
			case 1 :
				btn[y * nFieldWidth + x]->SetBackgroundColour(wxColor(*wxGREEN));
				break;

			case 2:
				btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(*wxBLUE));
				break;

			case 3:
				btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(*wxCYAN));
				break;

			case 4:
				btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(*wxYELLOW));
				break;

			case 5:
				btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(*wxRED));
				break;

			default:
				btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(*wxBLACK));
			}

			*/

		}

		// Check for Win
		if (checkForWin())	// Player won, so display winning dialog and reveal the mines
		{
			revealMines();
			wxMessageBox("Congratulations! You won!");
		}

	}

	evt.Skip();	// The event is handled
}

void cMain::OnNewClicked(wxCommandEvent& evt)
{
	// New pressed, so start a new game
	// Reset the game
	startNewGameBoard();
}

void cMain::OnExit(wxCommandEvent& evt)
{
	Close(true);
}

void cMain::OnAbout(wxCommandEvent& evt)
{
	wxMessageBox("Minesweeper Recreation Test By Michael Childress\n\nClick the squares and try to avoid the mines - numbers tell you how many mines are directly adjacent to that square. Right click a square to place a marker if you think a mine might be there.", 
		"About Minesweeper", wxOK | wxICON_INFORMATION);
}


void cMain::OnRightClick(wxMouseEvent& evt)
{
	// Place a marker on the button when right clicked
	// Get coordinate of button in field array
	// Each button was assigned a unique ID so we can do this
	int x = (evt.GetId() - 10000) % nFieldWidth;
	int y = (evt.GetId() - 10000) / nFieldWidth;


	// Label already exists so remove it
	if (btn[y * nFieldWidth + x]->GetLabel() == "X")
	{
		btn[y * nFieldWidth + x]->SetLabel("");
		//btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(wxNullColour));
	}

	else 
	{
		btn[y * nFieldWidth + x]->SetLabel("X");	// Set that button text to "X" for possible mine. Button itself is not activated
		//btn[y * nFieldWidth + x]->SetForegroundColour(wxColor(*wxRED));
	}
	

	evt.Skip();
}


void cMain::OnOptionsClicked(wxCommandEvent& evt)
{
	//options_frame = new cOptions();
	//options_frame->Show();
	wxNumberEntryDialog* numOfMines = new wxNumberEntryDialog(this, "How many mines would you like?", "&Number of mines (1-80)", "Options", this->getNumMinesToUse(), 1, 80, wxPoint(30, 30));
	numOfMines->ShowModal();


	int numMinesWanted = numOfMines->GetValue();

	if (numMinesWanted != this->getNumMinesToUse())	// If they entered a new number of mines, set new value and start a new game
	{
		this->setNumMinesToUse(numMinesWanted);
		startNewGameBoard();
	}

	//wxString mystring = wxString::Format(wxT("%i"), this->getNumMinesToUse());

	//wxMessageBox("New Number of Mines is " + mystring);
}


int cMain::getNumMinesToUse()
{
	return this->numMinesToUse;
}


void cMain::setNumMinesToUse(int mines)
{
	this->numMinesToUse = mines;
}


void cMain::startNewGameBoard()
{
	bFirstClick = true;
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			nField[y * nFieldWidth + x] = 0;
			btn[y * nFieldWidth + x]->SetLabel("");
			btn[y * nFieldWidth + x]->Enable(true);
		}
	}

	resetButtonPressedStatus();
}


void cMain::resetButtonPressedStatus()
{
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			enableStatus[y * nFieldWidth + x] = 0;
		}
	}
}


void cMain::revealMines()
{
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			if (nField[y * nFieldWidth + x] == -1)
			{
				btn[y * nFieldWidth + x]->SetLabel("M");
			}
		}
	}
}

bool cMain::checkForWin()
{
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			if (nField[y * nFieldWidth + x] == 0 && enableStatus[y * nFieldWidth + x] == 0)	// If a button has no mine and hasn't been enabled yet, they haven't won
			{
				return false;
			}
		}
	}

	// If this point reached, all buttons without mines have been pressed

	return true;
}