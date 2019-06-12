#ifdef _WIN32
#define __WINDOWS
#else
	#ifdef linux
		#undef __WINDOWS
	#else
		#error "Unknown system!"
	#endif
#endif

#include <chrono>
#include <iostream>
#include <deque>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>

#ifdef __WINDOWS
#include <windows.h>
#include <csignal>
#else
#include <unistd.h>
#include <signal.h> 
#endif

#include "keyboard_listen.h"

#define BUF_SIZE 500

namespace {

typedef std::chrono::time_point<std::chrono::system_clock> Time;

struct Key
{
	Time start_press;
	int col, row, layer;

	bool isFinished;
	long offset, duration;
};

//-----------------------------------------------------------------------------
long calcDuration(Time a, Time b) {
	return std::abs(std::chrono::duration_cast<std::chrono::milliseconds>(a - b).count());
}

//-----------------------------------------------------------------------------
FILE* listener;
bool isTerminate = false;
bool isExit = false;
void handle_sig(void) {
	if (!isExit) {
		isExit = true;
		isTerminate = true;
		onExit();
		#ifdef __WINDOWS
		_pclose(listener);
		#else 
		pclose(listener);
		#endif
	}
}

void handle_sig_int(int) {
	handle_sig();
}

};

//-----------------------------------------------------------------------------
int parseStrValue(const std::string& str, const std::string& name) {
	using namespace std;

	auto pos = str.find(name + "=");
	if (pos == string::npos) return -1;
	pos += name.size();
	pos++;
	string number;
	while (pos < str.size() && isdigit(str[pos])) {
		number += str[pos];
		pos++;
	}
	return stoi(number);
}

//-----------------------------------------------------------------------------
std::deque<Key> keys;
void processKey(int col, int row, int layer, int pressed) {
	using namespace std;

	static auto last_press_time = chrono::system_clock::now(); // Время последнего нажатия, чтобы считать offset
	auto current_time = chrono::system_clock::now();

	if (!pressed) {
		// Если клавишу отжимают, то ищем такую клавишу в очереди уже нажатых, и если нашли, говорит, что она отпущена
		for (auto& i : keys) {
			if (!i.isFinished && i.col == col && i.row == row) {
				i.isFinished = true;
				i.duration = calcDuration(current_time, i.start_press);
				break;
			}
		}
	} else {
		// Если клавишу нажимают, то помещаем её в очередь нажимаемых
		keys.push_back({
			current_time, 
			col, row, layer, 
			false,
			calcDuration(last_press_time, current_time), 0
		});
		last_press_time = current_time;
	}

	// Затем убираем из очереди первые клавиши, которые уже получили событие отжатия
	while ((keys.size() > 0) && ((keys.front().isFinished) ||
	       (calcDuration(keys.front().start_press, current_time) > max_hold_time))) {
		auto key = keys.front();
		if (!key.isFinished)
			key.duration = calcDuration(keys.front().start_press, current_time);

		// Сообщаем, что клавиша была отжата из-за ошибки
		if (key.duration > max_hold_time) {
			cerr << "Unpress key "
			     << "col=" << key.col
			     << ", row=" << key.row
			     << ", layer=" << key.layer
			     << ", because out of time." << endl;
		}
		onTap({key.col, key.row, key.layer}, key.duration, key.offset);
		keys.pop_front();
	}
}

#ifdef __WINDOWS
//-----------------------------------------------------------------------------
static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
		case CTRL_C_EVENT: // Ctrl+C
			handle_sig();
			return TRUE;
			break;
		case CTRL_BREAK_EVENT: // Ctrl+Break
			handle_sig();
			return TRUE;
			break;
		case CTRL_CLOSE_EVENT: // Closing the console window
			handle_sig();
			return TRUE;
			break;
		case CTRL_LOGOFF_EVENT: // User logs off. Passed only to services!
			handle_sig();
			return TRUE;
			break;
		case CTRL_SHUTDOWN_EVENT: // System is shutting down. Passed only to services!
			handle_sig();
			return TRUE;
			break;
	}

	// Return TRUE if handled this message, further handler functions won't be called.
	// Return FALSE to pass this message to further handlers until default handler calls ExitProcess().
	return FALSE;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
	using namespace std;

	if (!onStart(argc, argv))
		return 0;

	atexit(handle_sig);
	//at_quick_exit(handle_sig);
	#ifdef __WINDOWS
	SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
	listener = _popen("hid_listen.exe", "r");
	if (listener == NULL)
		cout << "Error when running `hid_listen.exe`." << endl;
	#else 
	signal(SIGINT, handle_sig_int);  // ^C
	signal(SIGABRT, handle_sig_int); // abort()
	signal(SIGTERM, handle_sig_int); // sent by "kill" command
	signal(SIGTSTP, handle_sig_int); // ^Z
	listener = popen("sudo ./hid_listen", "r");
	if (listener == NULL)
		cout << "Error when running `sudo ./hid_listen.exe`." << endl;
	#endif

	char buf[BUF_SIZE] = {};
	while (!isTerminate) {
		// Считываем строку из вывода клавиатуры
		if (fgets(buf, BUF_SIZE, listener) != NULL) {
			string str(buf);
			// Если это строка, в которой содержится необходимая нам информация
			if (str.size() > 3 && str.substr(0, 3) == "KL:") {
				// Получаем параметры текущего нажатия
				int col = parseStrValue(str, "col");
				int row = parseStrValue(str, "row");
				int pressed = parseStrValue(str, "pressed");
				int layer = parseStrValue(str, "layer");

				// Проверяем их на корректность
				#define check(a) if (a == -1) {\
					cerr << "Error when parsing `" << #a << "` in string `" << str << "`." << endl;\
					continue;\
				}

				check(col);
				check(row);
				check(pressed);
				check(layer);

				processKey(col, row, layer, pressed);
			}
		}
	}
}