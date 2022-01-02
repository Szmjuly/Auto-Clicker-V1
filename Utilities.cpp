void SetSpeed();

struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_0,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
	BUTTON_9,
	BUTTON_ENTER,
	BUTTON_COUNT, // Should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};

/*		if (pressed(BUTTON_ENTER))
		{
			SetSpeed();
		}
		*/

void simulate(Input* input, INPUT ip)
{
	if (pressed(BUTTON_ENTER))
	{
		SetSpeed();
	}
	
}

void SetSpeed()
{
	wchar_t speed_t[100], text[50];
	GetWindowTextW(hSpeed, speed_t, 100);
	speed = (int)speed_t;
	wcscpy_s(text, L"Speed is set to: ");
	wcscat_s(text, speed_t);
	SetWindowTextW(hOut, text);
}