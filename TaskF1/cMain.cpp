#include "cMain.h"
#define HEIGHT 450
#define WIDTH 800
#define SHIP 1
#define NOT_ACTIVE 2
#define SHOOTEN 4

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Battle Ships", wxPoint(30, 30), wxSize(WIDTH, HEIGHT))
{
	srand(time(0));
	colors = new wxColor[5];
	colors[0].Set(wxT("#aaaaaa"));
	colors[1].Set(wxT("#eeeeee"));
	colors[2].Set(wxT("#999900")); //kill button
	colors[3].Set(wxT("#990000")); //full kill ship
	colors[4].Set(wxT("#333333"));
	values = new int[fieldSize * fieldSize * 2];

	ships = new int[10]{ 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
	playerShips = 20;
	
	AIShips = 20;
	freeCells = fieldSize * fieldSize;

	for (size_t i = 0; i < fieldSize * fieldSize * 2; i++)
		values[i] = 0;
	NewGame();
}

cMain::~cMain()
{
}

void cMain::NewGame()
{
	InitMenu();
	InitButtons();

	while (!GenerateBoard()) 
		for (size_t i = 0; i < fieldSize * fieldSize; i++)
			values[fieldSize * fieldSize + i] = 0;
	for (size_t i = 0; i < fieldSize * fieldSize; i++)
		btn[fieldSize * fieldSize + i]->Enable(false);
	//StartGame();
}

void cMain::StartGame()
{
	for (size_t i = 0; i < fieldSize * fieldSize; i++)
	{
		btn[i]->Enable(false);
		btn[fieldSize * fieldSize + i]->Enable(true);
	}
}

bool cMain::GenerateBoard() {
	if (!GenerateShipWrapper(4)) return false;

	if (!GenerateShipWrapper(3)) return false;
	if (!GenerateShipWrapper(3)) return false;

	if (!GenerateShipWrapper(2)) return false;
	if (!GenerateShipWrapper(2)) return false;
	if (!GenerateShipWrapper(2)) return false;

	if (!GenerateShipWrapper(1)) return false;
	if (!GenerateShipWrapper(1)) return false;
	if (!GenerateShipWrapper(1)) return false;
	if (!GenerateShipWrapper(1)) return false;

	return true;
}

bool cMain::GenerateShipWrapper(int n) {
	bool ret = false;
	for (size_t i = 0; i < 50 && !ret; i++)
		ret = GenerateShip(n);
	return ret;
}

bool cMain::GenerateShip(int n) {
	int dir = rand() % 2; // 1 - horizontal, 0 - vertical

	if (dir) { // hor
		int y = rand() % fieldSize;
		int x = rand() % (fieldSize - n + 1);
		
		for (size_t i = 0; i < n; i++)
			if (values[fieldSize * fieldSize + fieldSize * y + x + i] != 0)
				return false;

		Fill(x, y, x + n - 1, y, fieldSize * fieldSize);
	} else { //vert
		int y = rand() % (fieldSize - n + 1);
		int x = rand() % fieldSize;

		for (size_t i = 0; i < n; i++)
			if (values[fieldSize * fieldSize + fieldSize * (y + i) + x] != 0)
				return false;

		Fill(x, y, x, y + n - 1, fieldSize * fieldSize);
	}

	return true;
}

void cMain::Fill(int x, int y, int fx, int fy, int c) {
	if (x == fx && y == fy) {
		for (size_t i = y - 1 < 0 ? 0 : y - 1; i <= (y + 1 == fieldSize ? fieldSize - 1 : y + 1); i++) {
			for (size_t j = x - 1 < 0 ? 0 : x - 1; j <= (x + 1 == fieldSize ? fieldSize - 1 : x + 1); j++) {
				values[c + i * fieldSize + j] = NOT_ACTIVE;
				//btn[c + i * fieldSize + j]->SetBackgroundColour(colors[0]);
			}
		}
		
		values[c + fieldSize * y + x] = SHIP;
		btn[c + fieldSize * y + x]->SetBackgroundColour(colors[4]);
		return;
	} // 1 palub ship

	if (x > fx) {
		int tmp = x;
		x = fx;
		fx = tmp;
	}
	if (y > fy) {
		int tmp = y;
		y = fy;
		fy = tmp;
	}
	for (size_t i = y - 1 < 0 ? 0 : y - 1; i <= (fy + 1 == fieldSize ? fieldSize - 1 : fy + 1); i++) {
		for (size_t j = x - 1 < 0 ? 0 : x - 1; j <= (fx + 1 == fieldSize ? fieldSize - 1 : fx + 1); j++) {
			values[c + i * fieldSize + j] = NOT_ACTIVE;
			//btn[c + i * fieldSize + j]->SetBackgroundColour(colors[0]);
		}
	}

	int dx = Sign(fx - x);
	int dy = Sign(fy - y);
	for (; x <= fx && y <= fy; x += dx, y += dy)
	{
		values[c + fieldSize * y + x] = SHIP;
		btn[c + fieldSize * y + x]->SetBackgroundColour(colors[4]);
	}
}

int cMain::Sign(int x) {
	return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

void cMain::buttonClicked(wxCommandEvent &evt)
{
	int y = (evt.GetId() - fieldSize * fieldSize - 10000) / fieldSize;
	int x = (evt.GetId() - fieldSize * fieldSize - 10000) % fieldSize;
	btn[fieldSize * fieldSize + fieldSize * y + x]->Enable(false);
	
	if (values[fieldSize * fieldSize + fieldSize * y + x] == SHIP) {
		btn[fieldSize * fieldSize + fieldSize * y + x]->SetBackgroundColour(colors[2]);
		btn[fieldSize * fieldSize + fieldSize * y + x]->SetLabel(wxT("X"));
		values[fieldSize * fieldSize + fieldSize * y + x] = SHOOTEN;

		if (!IsDead(x, y, new int[10])) {
			btn[fieldSize * fieldSize + fieldSize * y + x]->SetBackgroundColour(colors[2]);
		}
		AIShips--;
		if (AIShips == 0) {
			wxMessageBox(wxT("You are the winner!"));
		}
	} else {
		AIPlay();
	}


	evt.Skip();
}

bool cMain::IsDead(int x, int y, int* checked, int ptr)
{
	if (x < 0 || y < 0 || x > fieldSize - 1 || y > fieldSize - 1) return true;
	if (values[fieldSize * fieldSize + fieldSize * y + x] == SHIP) return false;
	if (values[fieldSize * fieldSize + fieldSize * y + x] == 0 || values[fieldSize * fieldSize + fieldSize * y + x] == NOT_ACTIVE) return true;
	for (size_t i = 0; i < ptr; i++) if (checked[i] == y * fieldSize + x) return true;

	checked[ptr] = y * fieldSize + x;
	ptr++;
	if (IsDead(x + 1, y, checked, ptr) && IsDead(x - 1, y, checked, ptr) &&
		IsDead(x, y + 1, checked, ptr) && IsDead(x, y - 1, checked, ptr) && 
		values[fieldSize * fieldSize + fieldSize * y + x] == SHOOTEN) {
		btn[fieldSize * fieldSize + fieldSize * y + x]->SetBackgroundColour(colors[3]);
		return true;
	} else {
		//btn[fieldSize * fieldSize + fieldSize * y + x]->SetBackgroundColour(colors[2]);
		return false;
	}
}

void cMain::initPlayerShipButton(wxCommandEvent& evt) {
	int y = (evt.GetId() - 10000) / fieldSize;
	int x = (evt.GetId() - 10000) % fieldSize;
	
	if (!secondPoint) {
		values[y * fieldSize + x] == 1;
		btn[fieldSize * y + x]->Enable(false);
		btn[fieldSize * y + x]->SetBackgroundColour(colors[4]);

		if (ships[ptr] > 1) {
			for (size_t i = 0; i < fieldSize * fieldSize; i++)
				btn[i]->Enable(false);
			lastPointX = x;
			lastPointY = y;
			secondPoint = true;
			OpenFinalPoint(y, x, ships[ptr] - 1);
		} else {
			Fill(x, y, x, y);
			for (size_t i = 0; i < fieldSize * fieldSize; i++)
				btn[i]->Enable(!values[i]);
			ptr++;
		}
	} else {
		Fill(lastPointX, lastPointY, x, y);
		secondPoint = false;
		ptr++;
		for (size_t i = 0; i < fieldSize * fieldSize; i++)
			btn[i]->Enable(!values[i]);
	}

	if (ptr == 10) {
		StartGame();
	}
	evt.Skip();
}

bool cMain::OpenFinalPoint(int y, int x, int n) {
	if (x - n >= 0) btn[fieldSize * y + x - n]->Enable(true);
	if (x + n < fieldSize) btn[fieldSize * y + x + n]->Enable(true);
	if (y - n >= 0) btn[fieldSize * (y - n) + x]->Enable(true);
	if (y + n < fieldSize) btn[fieldSize * (y + n) + x]->Enable(true);
	return true;
}

void cMain::InitMenu()
{
	menubar = new wxMenuBar;
	menubar->SetBackgroundColour(colors[0]);
	game = new wxMenu;
	help = new wxMenu;
	menubar->Append(game, wxT("&Game"));
	menubar->Append(help, wxT("&Help"));
	SetMenuBar(menubar);
}

void cMain::InitButtons()
{
	wxPanel* mainPanel = new wxPanel(this, -1);
	mainPanel->SetBackgroundColour(colors[1]);

	wxGridSizer* mainPanelGrid = new wxGridSizer(fieldSize + 2, fieldSize * 2 + 3, 0, 0);
	btn = new wxButton * [fieldSize * fieldSize * 2];
	for (size_t x = 0; x < fieldSize * 2 + 3; x++)
		mainPanelGrid->Add(new wxStaticText(mainPanel, wxID_ANY, wxT("")));

	for (size_t y = 0; y < fieldSize; y++)
	{
		mainPanelGrid->Add(new wxStaticText(mainPanel, wxID_ANY, wxT("")));
		for (size_t x = 0; x < fieldSize; x++)
		{
			btn[fieldSize * y + x] = new wxButton(mainPanel, 10000 + fieldSize * y + x);
			mainPanelGrid->Add(btn[fieldSize * y + x], 1, wxALL | wxEXPAND);
			btn[fieldSize * y + x]->Bind(wxEVT_BUTTON, &cMain::initPlayerShipButton, this);
		}

		mainPanelGrid->Add(new wxStaticText(mainPanel, wxID_ANY, wxT("")));
		for (size_t x = 0; x < fieldSize; x++)
		{
			btn[fieldSize * fieldSize + fieldSize * y + x] = new wxButton(mainPanel, fieldSize * fieldSize + 10000 + fieldSize * y + x);
			mainPanelGrid->Add(btn[fieldSize * fieldSize + fieldSize * y + x], 1, wxALL | wxEXPAND);
			btn[fieldSize * fieldSize + fieldSize * y + x]->Bind(wxEVT_BUTTON, &cMain::buttonClicked, this);
			//btn[fieldSize * fieldSize + fieldSize * y + x]->Enable(false);
		}

		mainPanelGrid->Add(new wxStaticText(mainPanel, wxID_ANY, wxT("")));
	}
	for (size_t x = 0; x < fieldSize * 2 + 3; x++)
		mainPanelGrid->Add(new wxStaticText(mainPanel, wxID_ANY, wxT("")));

	mainPanel->SetSizer(mainPanelGrid);
}

void cMain::AIPlay(int x, int y)
{
	if (x == -1 && y == -1) {
		int x = rand() % fieldSize;
		int y = rand() % fieldSize;

		values[y * fieldSize + x] = SHOOTEN;
		btn[y * fieldSize + x]->SetBackgroundColour(colors[1]);
		btn[y * fieldSize + x]->SetLabel(wxT("X"));

		if (values[y * fieldSize + x] == SHIP) {
			if (!IsDead(x, y, new int[10])) {
				AILastX = x;
				AILastY = y;
				AIPlay(x, y);
			} else {
				AIPlay();
			}
		}
	}
	
}
