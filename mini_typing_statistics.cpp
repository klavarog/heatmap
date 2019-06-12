#include <iostream>
#include <iomanip>

#include "keyboard_listen.h"

//-----------------------------------------------------------------------------
bool onStart(int argc, char** argv) {
	return true;
}

//-----------------------------------------------------------------------------
void onTap(Tap tap, long time_duration, long time_offset) {
	using namespace std;
	cout << "col=" << tap.col << ", "
	     << "row=" << tap.row << ", " 
	     << "layer=" << tap.layer << ", "
	     << "offset=" << time_offset << "ms, "
	     << "duration=" << time_duration << "ms"
	     << endl;
}

//-----------------------------------------------------------------------------
void onExit(void) {
	using namespace std;
	cerr << endl;
	cerr << "Terminating..." << endl;
}