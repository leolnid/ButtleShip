#pragma once

#include "wx/wx.h"
#include <stdlib.h>
#include <stdio.h>

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();
	void NewGame();
	void StartGame();
	void initPlayerShipButton(wxCommandEvent& event);
	bool OpenFinalPoint(int x, int y, int n);
	void buttonClicked(wxCommandEvent& event);
	bool IsDead(int x, int y, int* checked, int ptr = 0);

private:
	void InitMenu();
	void InitButtons();
	void AIPlay(int x = -1, int y = -1);
	
	bool GenerateBoard();
	bool GenerateShipWrapper(int n);
	bool GenerateShip(int n);

	void Fill(int x, int y, int fx, int fy, int c = 0);

	int Sign(int x);


	wxMenuBar *menubar;
	wxMenu *game;
	wxMenu* help;

	int fieldSize = 10;
	wxButton** btn;
	wxColor* colors;

	int* values;
	int lastPointX;
	int lastPointY;
	bool secondPoint = false;
	int ptr = 0;
	int* ships;

	int playerShips;
	int AIShips;
	int freeCells;
	int AILastX;
	int AILastY;

};

