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
int getInputMethod(void) {
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
bool isShiftPressed(void) {
	return (GetKeyState( VK_SHIFT ) & 0x8000) != 0;
}

//-----------------------------------------------------------------------------
std::map<int, int> langInputMap;
int getLanguageId(void) {
	return langInputMap[getInputMethod()] * 2 + isShiftPressed();
}

//-----------------------------------------------------------------------------
HHOOK hhook = 0;
LRESULT __stdcall keyproc( int code, WPARAM wParam, LPARAM lParam ) {
	/* Начальные присвоения. */
	int vkCode = ((KBDLLHOOKSTRUCT*) lParam)->vkCode;
	int scanCode = ((KBDLLHOOKSTRUCT*) lParam)->scanCode;
	int flags = ((KBDLLHOOKSTRUCT*) lParam)->flags;
	int extraInf = ((KBDLLHOOKSTRUCT*) lParam)->dwExtraInfo;

	std::cout << "KL: col=" << vkCode << ", row=" << isShiftPressed() << ", pressed=" << (wParam == WM_KEYDOWN) << ", layer=" << getLanguageId() << std::endl;

	return CallNextHookEx(hhook, code, wParam, lParam);
};

//-----------------------------------------------------------------------------
int main() {
	HKL hh[500];
	int count = GetKeyboardLayoutList(500, hh);
	for (int i = 0; i < count; i++)
		langInputMap[LOWORD(hh[i])] = i;
	
	/* Создание хука и вывод сообщения если он не может быть создан. */
	hhook = SetWindowsHookEx(WH_KEYBOARD_LL, keyproc, NULL, 0);
	if (NULL == hhook) {
	    MessageBox(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR);
	}

	/* Основной цикл работы программы. */
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
}