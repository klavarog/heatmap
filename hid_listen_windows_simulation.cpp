#ifdef WINVER
#undef WINVER
#endif 
#define WINVER 0x500

#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <map>

//-----------------------------------------------------------------------------
/** Возвращает айди текущего языка для текущего окна. */
int getLanguageId(void) {
	HWND hwnd = GetForegroundWindow();
	if (hwnd) {
		DWORD threadID = GetWindowThreadProcessId(hwnd, NULL);
		HKL currentLayout = GetKeyboardLayout(threadID);
		unsigned int x = (unsigned int)currentLayout & 0x0000FFFF;
		return ((int)x);
	}
	return 0;
}

//-----------------------------------------------------------------------------
/** Определяет, нажата ли клавиша шифт в данный момент. */
bool isShiftPressed(void) {
	return (GetKeyState(VK_SHIFT) & 0x8000) != 0;
}

//-----------------------------------------------------------------------------
std::map<int, int> langInputMap;
/** Возвращает номер текущего "слоя", основываясь на нажатом шифте и выбранном языке. Считается, что один язык - это два слоя, первый слой - без шифта, а второй - с шифтом. */
int getLayer(void) {
	return langInputMap[getLanguageId()] * 2 + isShiftPressed();
}

//-----------------------------------------------------------------------------
/** Инициализирует map с языками и их айди от 0 до количества языков. */
void initLanguageMap(void) {
	HKL hh[10];
	int count = GetKeyboardLayoutList(10, hh);
	for (int i = 0; i < count; i++)
		langInputMap[LOWORD(hh[i])] = i;
}

//-----------------------------------------------------------------------------
std::map<std::pair<int, int>, bool> pressedMap;
HHOOK hhook = 0;
LRESULT __stdcall keyproc( int code, WPARAM wParam, LPARAM lParam ) {
	// Определяем переданные нам данные
	int vkCode   = ((KBDLLHOOKSTRUCT*) lParam)->vkCode;
	int scanCode = ((KBDLLHOOKSTRUCT*) lParam)->scanCode;
	int flags    = ((KBDLLHOOKSTRUCT*) lParam)->flags;
	int extraInf = ((KBDLLHOOKSTRUCT*) lParam)->dwExtraInfo;

	bool isPressed = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
	int layer = getLayer();

	// Костыль. В момент, когда нажимается шифт, нам не надо считать что он нажат, то есть шифт нажимается из четного слоя. Плюс там есть особенности с тем, когда первый раз получается сообщение о нажатом шифте, а функция isShiftPressed() говорит что он не нажат, а потом говорит что нажат.
	bool isShift = vkCode == VK_RSHIFT || vkCode == VK_LSHIFT || vkCode == VK_SHIFT;
	if (isShift && isShiftPressed()) {
		layer--;
	}

	// Наблюдается такой баг, что при переключении раскладки на Alt+Shift случается так, что откуда-то вылезает клавиша Ctrl с кодом 162, и затем не отпускается. Я не знаю что с этим делать.

	struct Message
	{
		int vkCode;
		int layer;
		bool isPressed;
	};

	// Проверяет, отправлялось ли это же сообщение раньше. Функция нужна, чтобы по триста раз не отправлять сообщение "Нажата клавиша N", когда пользователь её зажал и не отпускает.
	auto isThisMessageAlreadySent = [] (Message msg) {
		return (msg.isPressed && !pressedMap[{msg.layer, msg.vkCode}]) || !msg.isPressed;
	};

	// Изменяет свое внутреннее состояние с учетом того, что данное сообщение отправилось. Суть такая, что отправлять подряд несколько сообщений о нажатии клавиши N, нам не надо, а вот когда её отпустили, сообщение о её нажатии можно и отправить.
	auto notifyThatMessageSent = [] (Message msg) {
		pressedMap[{msg.layer, msg.vkCode}] = msg.isPressed;
	};

	// Печатает сообщение на экран.
	auto printMessage = [] (Message msg) {
		std::cout << "KL: col=" << msg.vkCode << ", row=" << 0 << ", pressed=" << msg.isPressed << ", layer=" << msg.layer << std::endl;
	};

	Message msg{vkCode, layer, isPressed};
	if (isThisMessageAlreadySent(msg)) {
		printMessage(msg);
		notifyThatMessageSent(msg);
	}

	// Так надо делать по документации.
	return CallNextHookEx(hhook, code, wParam, lParam);
};

//-----------------------------------------------------------------------------
int main() {
	initLanguageMap();
	
	// Создание хука или вывод сообщения если он не может быть создан.
	hhook = SetWindowsHookEx(WH_KEYBOARD_LL, keyproc, NULL, 0);
	if (NULL == hhook) {
	    MessageBox(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR);
	}

	// Не знаю почему, но почему-то это работает... А простой вечный цикл нет, хотя по факту тут никакие сообщения не обрабатываются.
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
}