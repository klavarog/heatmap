#include <iostream>
#include <iomanip>
#include <fstream> 

#include "keyboard_listen.h"
#include "heatmap.h"

Fingers fingers;
Names names;

//-----------------------------------------------------------------------------
bool onStart(int argc, char** argv) {
	std::ifstream fin("layout_fingers.txt");
	fingers.load(fin);
	fin.close();
	fin.open("layout_names.txt");
	names.load(fin);
	fin.close();
	return true;
}

//-----------------------------------------------------------------------------
void onTap(Tap tap, long time_duration, long time_offset) {
	using namespace std;
	cout << "col=" << tap.col << ", "
	     << "row=" << tap.row << ", " 
	     << "layer=" << tap.layer << ", "
	     << "offset=" << time_offset << "ms, "
	     << "duration=" << time_duration << "ms, " 
	     << "finger=" << fingers.getFinger(tap) << ", "
	     << "row=" << fingers.getRow(tap) << ", "
	     << "name=\"" << names.getName(tap) << "\""
	     << endl;
}

//-----------------------------------------------------------------------------
void onExit(void) {
	using namespace std;
	cerr << endl;
	cerr << "Terminating..." << endl;
}