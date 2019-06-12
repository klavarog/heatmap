#include <algorithm>
#include <chrono>
#include <fstream>
#include "heatmap.h"

//-----------------------------------------------------------------------------
OneTapHeatmap::OneTapHeatmap(int layersN, int rowsN, int colsN) : v(layersN, rowsN, colsN, 0) {}

//-----------------------------------------------------------------------------
void OneTapHeatmap::onTap(Tap tap) {
	v[tap]++;
}

//-----------------------------------------------------------------------------
void OneTapHeatmap::save(std::ostream& out) const {
	v.save(out);
}

//-----------------------------------------------------------------------------
void OneTapHeatmap::load(std::istream& in) {
	v.load(in);
}

//-----------------------------------------------------------------------------
int OneTapHeatmap::layersCount(void) const {
	return v.layersCount();
}

//-----------------------------------------------------------------------------
int OneTapHeatmap::rowsCount(void) const {
	return v.rowsCount();
}

//-----------------------------------------------------------------------------
int OneTapHeatmap::colsCount(void) const {
	return v.colsCount();
}

//-----------------------------------------------------------------------------
int OneTapHeatmap::getTapCount(Tap tap) const {
	return v[tap];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TwoTapHeatmap::TwoTapHeatmap(int layersN, int rowsN, int colsN) : v(layersN, rowsN, colsN, OneTapHeatmap(1, rowsN, colsN)) {}

//-----------------------------------------------------------------------------
void TwoTapHeatmap::onTap(Tap tap) {	
	if (lastTap.layer != -1) {
		auto tap_copy = tap;
		tap_copy.layer = 0;
		v[lastTap].onTap(tap_copy);
	}
	lastTap = tap;
}

//-----------------------------------------------------------------------------
void TwoTapHeatmap::save(std::ostream& out) const {
	v.save(out);
}

//-----------------------------------------------------------------------------
void TwoTapHeatmap::load(std::istream& in) {
	v.load(in);
}

//-----------------------------------------------------------------------------
int TwoTapHeatmap::layersCount(void) const {
	return v.layersCount();
}

//-----------------------------------------------------------------------------
int TwoTapHeatmap::rowsCount(void) const {
	return v.rowsCount();
}

//-----------------------------------------------------------------------------
int TwoTapHeatmap::colsCount(void) const {
	return v.colsCount();
}

//-----------------------------------------------------------------------------
int TwoTapHeatmap::getTapCount(Tap first, Tap second) const {
	second.layer = 0;
	return v[first].getTapCount(second);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DayHeatmap::DayHeatmap() : counter(0) {
	updateToday();
}

//-----------------------------------------------------------------------------
void DayHeatmap::onTap(Tap tap) {
	counter++;

	statistics[today]++;

	if (counter > tap_count_to_update_date) {
		counter = 0;
		updateToday();
	}
}

//-----------------------------------------------------------------------------
void DayHeatmap::save(std::ostream& out) const {
	out << statistics.size() << std::endl;
	for (auto& i : statistics)
		out << i.first << " " << i.second << std::endl;
}

//-----------------------------------------------------------------------------
void DayHeatmap::load(std::istream& in) {
	int size;
	in >> size;
	for (int i = 0; i < size; ++i) {
		int first, second;
		in >> first >> second;
		statistics[first] = second;
	}
}

//-----------------------------------------------------------------------------
int DayHeatmap::getToday(void) const {
	using namespace std::chrono;
	int today1;
	auto now = system_clock::now();
	today1 = duration_cast<hours>(now.time_since_epoch()).count();
	today1 /= 24;
	return today1;
}

//-----------------------------------------------------------------------------
int DayHeatmap::getYear(int day) const {
	using namespace std::chrono;
	int today1 = getToday();
	day = today1 - day;
	auto now = system_clock::now();
	now -= hours(24*day);

	time_t tt = system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);

	return local_tm.tm_year + 1900;
}

//-----------------------------------------------------------------------------
int DayHeatmap::getMonth(int day) const {
	using namespace std::chrono;
	int today1 = getToday();
	day = today1 - day;
	auto now = system_clock::now();
	now -= hours(24*day);

	time_t tt = system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);

	return local_tm.tm_mon + 1;
}

//-----------------------------------------------------------------------------
int DayHeatmap::getDay(int day) const {
	using namespace std::chrono;
	int today1 = getToday();
	day = today1 - day;
	auto now = system_clock::now();
	now -= hours(24*day);

	time_t tt = system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);

	return local_tm.tm_mday;
}

//-----------------------------------------------------------------------------
std::map<int, int32_t> DayHeatmap::getStatistics() const {
	return statistics;
}

//-----------------------------------------------------------------------------
void DayHeatmap::updateToday() {
	using namespace std::chrono;
	auto now = system_clock::now();
	today = duration_cast<hours>(now.time_since_epoch()).count();
	today /= 24;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
AllStatistics::AllStatistics(const std::string& file) : onetap(1, 1, 1), twotap(1, 1, 1), daytap(), file(file) {}

//-----------------------------------------------------------------------------
void AllStatistics::load(void) {
	std::ifstream fin(file);
	if (fin) {
		onetap.load(fin);
		twotap.load(fin);
		daytap.load(fin);
		fin.close();
	}
}

//-----------------------------------------------------------------------------
void AllStatistics::save(void) const {
	std::ofstream fout(file);
	onetap.save(fout);
	twotap.save(fout);
	daytap.save(fout);
	fout.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void Fingers::save(std::ostream& out) const {
	v.save(out);
}

//-----------------------------------------------------------------------------
void Fingers::load(std::istream& in) {
	v.load(in);
} 

//-----------------------------------------------------------------------------
int Fingers::getFinger(Tap tap) const {
	tap.layer = 0;
	return v[tap].first;
}

//-----------------------------------------------------------------------------
int Fingers::getRow(Tap tap) const {
	tap.layer = 0;
	return v[tap].second;
}

//-----------------------------------------------------------------------------
std::string Fingers::getHandName(int finger) {
	if (finger < 5)
		return "left";
	else if (finger < 10)
		return "right";
	else
		return "any";
}

//-----------------------------------------------------------------------------
std::string Fingers::getFingerName(int finger) {
	const std::vector<std::string> mas = {
		"pinkie",
		"ring",
		"middle",
		"index",
		"thumb",

		"thumb",
		"index",
		"middle",
		"ring",
		"pinkie",

		"any",
	};

	return mas[finger];
}

//-----------------------------------------------------------------------------
std::string Fingers::getRowName(int row) {
	const std::vector<std::string> mas = {
		"any",

		"2-low",
		"1-low",
		"home",
		"1-up",
		"2-up", 
	};

	return mas[row];	
}

//-----------------------------------------------------------------------------
void Fingers::set(Tap tap, int finger, int row) {
	tap.layer = 0;
	v[tap] =  std::pair<int, int>(finger, row);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void Names::save(std::ostream& out) const {
	v.save(out);
}

//-----------------------------------------------------------------------------
void Names::load(std::istream& in) {
	v.load(in);
}

//-----------------------------------------------------------------------------
std::string Names::getName(Tap tap) {
	return v[tap];
}

//-----------------------------------------------------------------------------
void Names::setName(Tap tap, const std::string& name) {
	v[tap] = name;
}