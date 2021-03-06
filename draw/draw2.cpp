// draw.cpp : Defines the entry point for the application.
//Techniki programowania - Projekt 3 - Jan Mikicki Pawel Wilgan
#include "stdafx.h"
#include "draw2.h"
#include <vector>
#include <cstdio>
#include <fstream>
#include <cmath>

#define MAX_LOADSTRING 100
#define TMR_1 1
#define M_PI 3.14159265358979323846

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

INT value;

// buttons
HWND hwndButton;
HWND hText;

// sent data
int col = 0;
int linia = 1;
double skala_x = 1;
double skala_y = 1;
std::vector<Point> data;
std::vector<int> wektor_wysokosci;
std::vector<double> roll;
std::vector<double> pitch;

RECT drawArea1 = { 0, 0, 950, 400 };
RECT drawArea2 = { 50, 400, 650, 622};

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);
double get_radians(double);
double kat_wychylenia(double, double);
void wysokosc(std::vector<Point>);


void MyOnPaint(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 255));
	Pen pen2(Color(255, 25 * col, 0, 255));
	
	for (size_t i = 1; i < data.size(); i++)
		graphics.DrawLine(&pen2, static_cast<int>(skala_x * data[i - 1].X), static_cast<int>(200 - skala_y * data[i - 1].Y), static_cast<int>(skala_x * data[i].X), static_cast<int>(200 - skala_y * data[i].Y));

	for (int i = 1; i < value && i < wektor_wysokosci.size(); i++)
		graphics.DrawLine(&pen, 50 + 10 * (i - 1), 620 - 10 * wektor_wysokosci[i - 1],50 + 10 * i, 620 - 10 * wektor_wysokosci[i]);
}

void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea)
{
	if (drawArea==NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	MyOnPaint(hdc);
	EndPaint(hWnd, &ps);
}

void inputData()
{	
	roll.clear();
	pitch.clear();
	data.clear();

	std::ifstream plik("outputPendulum01.log"); 
	
	if (!plik) {                    		
		system("pause");
		exit(0);
	}

	double liczba;
	
	while (plik.peek() != EOF) {
		while (linia) {
			plik.ignore(256, '\n');
			linia--;
		}
		plik >> liczba;
		roll.push_back(liczba);
		plik >> liczba;
		pitch.push_back(liczba);		
		plik.ignore(256, '\n');
	}
	
	plik.close();
			
	for (size_t i = 1; i < roll.size(); i++){
		data.push_back(Point(i + 1, static_cast<int>(200 * kat_wychylenia(roll[i], pitch[i]))));
	}
	
	wysokosc(data);
}

int oblicz_wysokosc(int kat)
{	
	double x = cos(kat / 200.0) / 2;
	return static_cast<int>(500 * (0.5 - x));
}

void wysokosc(std::vector<Point> data)
{
	wektor_wysokosci.clear();
	
	for (size_t i = 2; i < data.size() - 2; i++) {
		if (data[i - 2].Y <= data[i].Y && data[i + 2].Y < data[i].Y) {		
			wektor_wysokosci.push_back(oblicz_wysokosc(data[i].Y));
		}
	}
}

double kat_wychylenia(double a, double b)
{
	return atan(sqrt(pow(tan(get_radians(a)), 2) + pow(tan(get_radians(b)), 2)));
}

double get_radians(double degrees)
{
	return  degrees * (M_PI / 180.0);
}

int OnCreate(HWND window)
{
	inputData();
	return 0;
}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	value = 1;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;


	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	// create button and store the handle                                                       
	
	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Draw"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		960, 60,                                  // the left and top co-ordinates
		170, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need
	
	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("< x >"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		960, 120,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON3,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("> x <"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		960, 180,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON4,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("< y >"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1050, 120,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON5,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("> y <"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1050, 180,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON6,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("Wczytaj"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		970, 330,
		150, 50,
		hWnd,
		(HMENU)ID_BUTTON7,
		hInstance,
		NULL);

	hText = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
		970, 280,
		150, 45,
		hWnd,
		(HMENU)ID_EDITBOX,
		hInstance,
		NULL);

	// create button and store the handle                                                       

	hwndButton = CreateWindow(TEXT("button"), TEXT("Rysuj"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		700, 400, 100, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Stop"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		700, 455, 100, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{		
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON1 :
			col++;
			if (col > 10)
				col = 0;
			skala_x = 1;
			skala_y = 1;
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;	
		case ID_BUTTON3 :
			skala_x *= 2;
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_BUTTON4:
			skala_x /= 2;
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_BUTTON5:
			skala_y *= 2;
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_BUTTON6:
			skala_y /= 2;
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_BUTTON7:
		{
			
			linia = GetDlgItemInt(hWnd, ID_EDITBOX, NULL, TRUE);
			if (linia > 839)
				linia = 1;
			
			value = 1;
			inputData();
			break;
		}
		case ID_RBUTTON1:
			SetTimer(hWnd, TMR_1, 25, 0);
			break;
		case ID_RBUTTON2:
			KillTimer(hWnd, TMR_1);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			//force window to repaint
			repaintWindow(hWnd, hdc, ps, &drawArea2);
			value++;
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
