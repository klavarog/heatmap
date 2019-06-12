#include <iostream>

#include "keyboard_listen.h"
#include "heatmap.h"

AllStatistics all(save_file);

//-----------------------------------------------------------------------------
bool onStart(int argc, char** argv) {
	all.load();
	return true;
}

//-----------------------------------------------------------------------------
void onTap(Tap tap, long time_duration, long time_offset) {
	using namespace std;
	static int counter = 0;
	counter++;
	all.onetap.onTap(tap);
	all.twotap.onTap(tap);
	all.daytap.onTap(tap);
	if (counter > tap_count_to_save) {
		counter = 0;
		all.save();
	}
}

//-----------------------------------------------------------------------------
void onExit(void) {
	using namespace std;

	cerr << endl;
	cerr << "Saving information..." << endl;
	all.save();
	cerr << "Terminating..." << endl;
}