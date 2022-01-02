#include <windows.h>
#include <stdlib.h>
#include <string>

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)


#define FILE_MENU_NEW 1
#define FILE_MENU_OPEN 2
#define FILE_MENU_EXIT 3

#define SET_SPEED 4

#define LEFT_CLICK_START 5
#define LEFT_CLICK_STOP 6
#define RIGHT_CLICK_START 7
#define RIGHT_CLICK_STOP 8

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

void AddMenues(HWND hwnd);
void AddControls(HWND hWnd);
void LeftClick();
void RightClick();
void SetSpeedButton();
void ReSize(HWND hWnd);

struct Render_State {
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};

Render_State render_state;

HMENU hMenu;
HWND hSpeed, hSpeedOut, hOut;

bool leftClick, rightClick = false;
bool running = false;
int speed = 1000;	// 1000 = 1 second

#include "Utilities.cpp"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	// Just a Plain old Messeage box saying hello world
	//MessageBox(NULL, L"HELLO WORLD", L"My first GUI", MB_OK);

	WNDCLASSW wc = { 0 };

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc))
	{
		return -1;
	}

	HWND window = CreateWindowW(L"myWindowClass", L"My Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,		// Position of top left corner of window on screen in pixels
		480, 335,							// Width and Hieght of the window
		NULL, NULL, NULL, NULL);

	//HDC hdc = GetDC(window);

	Input input = {};

	MSG msg = { 0 };

	running = true;

	while (running)
	{

		while (GetMessage((&msg), NULL, NULL, NULL))
		{
			switch (msg.message)
			{
			case WM_KEYUP:

			case WM_KEYDOWN:
			{
				unsigned int vk_code = (unsigned int)msg.wParam;
				bool is_down = ((msg.lParam & (1 << 31)) == 0);

#define processButton(b,vk)\
case vk: {\
input.buttons[b].is_down = is_down;\
input.buttons[b].changed = true;\
}break;

				switch (vk_code)
				{
					processButton(BUTTON_ENTER, VK_RETURN);
					break;
				}
				break;
			}

			default:
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			}

		}

		INPUT ip;

		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		simulate(&input, ip);

	}
		
	return 0;

}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_COMMAND:
		// To check which item in the menu was pressed
		switch (wp)
		{
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case FILE_MENU_NEW:
			MessageBeep(MB_ICONASTERISK);
			break;
		case SET_SPEED:
			SetSpeedButton();
			break;
		case LEFT_CLICK_START:
			LeftClick();
			break;
		case RIGHT_CLICK_START:
			RightClick();
			break;
		}
		break;
	case WM_CREATE:
	{
		AddMenues(hWnd);
		AddControls(hWnd);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		ReSize(hWnd);
		break;
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
	
}


void AddMenues(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hSubMenu = CreateMenu();

	// SubMenu
	AppendMenu(hSubMenu, MF_STRING, NULL, L"SubMenu Item");

	// File Menu
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_NEW, L"New");
	AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Open");

	// Line Seperator
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);

	// Exit Application
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

	// Main Menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenu(hMenu, MF_STRING, NULL, L"Edit");

	SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd)
{
	CreateWindowW(L"button", L"Left Click Start",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		15, 10, 435, 35,
		hWnd, (HMENU)LEFT_CLICK_START, NULL, NULL
	);	
	
	CreateWindowW(L"button", L"Right Click Start",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		15, 50, 435, 35,
		hWnd, (HMENU)RIGHT_CLICK_START, NULL, NULL
	);

	CreateWindowW(L"static", L"Press X to stop running",
		WS_VISIBLE | WS_CHILD | WS_THICKFRAME | SS_CENTER,
		15, 90, 435, 35,
		hWnd, NULL, NULL, NULL);

	CreateWindowW(L"static", L"Speed: ",
		WS_VISIBLE | WS_CHILD | WS_BORDER| SS_CENTER,
		15, 130, 85, 20,
		hWnd, NULL, NULL, NULL
	);

	hSpeed = CreateWindowW(L"Edit", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
		105, 130, 265, 20,
		hWnd, NULL, NULL, NULL);

	CreateWindowW(L"button", L"Set",
		WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
		375, 130, 75, 20,
		hWnd, (HMENU)SET_SPEED, NULL, NULL
	);

	hOut = CreateWindowW(L"static", L"Output...",
		WS_VISIBLE | WS_CHILD | WS_THICKFRAME | SS_CENTER,
		15, 155, 435, 35,
		hWnd, NULL, NULL, NULL
	);

	hSpeedOut = CreateWindowW(L"static", L"The Default speed is 1000 (1 second)",
		WS_VISIBLE | WS_CHILD | WS_THICKFRAME | SS_CENTER,
		15, 195, 435, 65,
		hWnd, NULL, NULL, NULL
	);
}

void LeftClick()
{
	leftClick = true;
	SetWindowTextW(hOut, L"Left Click True");

	while (leftClick == true)
	{
		Sleep(speed);
		SetWindowTextW(hOut, L"Output...");
		SetWindowTextW(hOut, L"Left Clicking...");
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

		if (GetAsyncKeyState('X'))
		{
			speed = 1000;
			leftClick = false;
			SetWindowTextW(hOut, L"Left Click False");
			SetWindowTextW(hOut, L"Output...");
		}
	}
}

void RightClick()
{
	rightClick = true;
	SetWindowTextW(hOut, L"Right Click True");
	SetWindowTextW(hOut, L"Output...");

	while (rightClick == true)
	{
		Sleep(speed);
		SetWindowTextW(hOut, L"Right Clicking...");
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);

		if (GetAsyncKeyState('X'))
		{
			speed = 1000;
			rightClick = false;
			SetWindowTextW(hOut, L"Right Click False");
			SetWindowTextW(hOut, L"Output...");
		}
	}
}

void SetSpeedButton()
{
	wchar_t speed_t[100], text[50];
	GetWindowTextW(hSpeed, speed_t, 100);
	speed = (int)speed_t;
	wcscpy_s(text, L"Speed is set to: ");
	wcscat_s(text, speed_t);
	SetWindowTextW(hOut, text);
}

void ReSize(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	render_state.width = rect.right - rect.left;
	render_state.height = rect.bottom - rect.top;

	int size = render_state.width * render_state.height * sizeof(unsigned int);

	if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
	render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
	render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
	render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
	render_state.bitmap_info.bmiHeader.biPlanes = 1;
	render_state.bitmap_info.bmiHeader.biBitCount = 32;
	render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
}