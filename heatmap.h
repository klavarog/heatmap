#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "common.h"

const std::string save_file = "heatmap_file.txt"; /// Файл, куда сохраняется статистика
const int tap_count_to_save = 3000; /// Сколько должно пройти нажатий, чтобы текущая статистика сбросилась в файл на диск
const int tap_count_to_update_date = 100; /// Сколько должно пройти нажатий, чтобы обновилась дата, вдруг день уже не сегодняшний, или вдруг человек не выключает компьютер

template<typename T>
class Vector3D
{
public:
	Vector3D() : l(0, Rows(0, Cols(0, T()))) {}
	Vector3D(int layersN, int rowsN, int colsN, const T& obj) : l(layersN, Rows(rowsN, Cols(colsN, obj))) {}

	T& operator[](const Tap& tap) {
		if (tap.layer >= layersCount() || tap.row >= rowsCount() || tap.col >= colsCount()) {
			resize(std::max(tap.layer+1, layersCount()), 
			       std::max(tap.row+1, rowsCount()),
			       std::max(tap.col+1, colsCount()));
		}
		
		return l[tap.layer][tap.row][tap.col];
	}
	const T& operator[](const Tap& tap) const {
		if (tap.layer >= layersCount() || tap.row >= rowsCount() || tap.col >= colsCount()) {
			/*std::cerr << "Error when trying access to tap: " << tap << std::endl;
			throw std::exception();*/
			return T();
		}
		
		return l[tap.layer][tap.row][tap.col];	
	}

	int layersCount(void) const {
		return l.size();
	}
	int rowsCount(void) const {
		if (layersCount() > 0)
			return l[0].size();
		else
			return 0;
	}
	int colsCount(void) const {
		if (rowsCount() > 0)
			return l[0][0].size();
		else
			return 0;
	}

	void save(std::ostream& out) const {
		out << layersCount() << " " << rowsCount() << " " << colsCount() << std::endl;
		for (int i = 0; i < layersCount(); ++i) {
			for (int j = 0; j < rowsCount(); ++j) {
				for (int k = 0; k < colsCount(); ++k) {
					if constexpr (has_save<T>::value) {
						l[i][j][k].save(out);
						out << std::endl;
					} else {
						out << l[i][j][k] << " ";
					}
				}
				if constexpr (!has_save<T>::value) {
					out << std::endl;
				}
			}
			if constexpr (!has_save<T>::value) {
				out << std::endl;
			}
		}
	}
	void load(std::istream& in) {
		int layersN, rowsN, colsN;
		in >> layersN >> rowsN >> colsN; 
		if constexpr (has_load<T>::value) {
			std::string line;
			std::getline(in, line);
		}
		l.clear();
		l.resize(layersN, Rows(rowsN, Cols(colsN, T())));
		for (int i = 0; i < layersN; ++i) {
			for (int j = 0; j < rowsN; ++j) {
				for (int k = 0; k < colsN; ++k) {
					if constexpr (has_load<T>::value) {
						l[i][j][k].load(in);
					} else {
						in >> l[i][j][k];
					}
				}
			}
		}
	}
private:
	// Шаблонная магия, которая проверяет, есть ли у класса метод save. Ох уж эти шаблоны, их срочно надо сделать по-нормальному, чтобы такой херни не было. Большое спасибо https://stackoverflow.com/questions/257288/is-it-possible-to-write-a-template-to-check-for-a-functions-existence
	template <typename D>
	class has_save
	{
	    typedef char one;
	    typedef long two;

	    template <typename C> static one test( decltype(&C::save) ) ;
	    template <typename C> static two test(...);    

	public:
	    enum { value = sizeof(test<D>(0)) == sizeof(char) };
	};

	// Аналогичная шаблонная магия, но только на метод load
	template <typename D>
	class has_load
	{
	    typedef char one;
	    typedef long two;

	    template <typename C> static one test( decltype(&C::load) ) ;
	    template <typename C> static two test(...);    

	public:
	    enum { value = sizeof(test<D>(0)) == sizeof(char) };
	};

	typedef std::vector<T> Cols;
	typedef std::vector<Cols> Rows;
	typedef std::vector<Rows> Layers;

	void resize(int layersN, int rowsN, int colsN) {
		Layers result(layersN, Rows(rowsN, Cols(colsN, T())));
		for (int i = 0; i < l.size(); ++i) {
			for (int j = 0; j < l[i].size(); ++j) {
				for (int k = 0; k < l[i][j].size(); ++k) {
					result[i][j][k] = l[i][j][k];
				}
			}
		}

		swap(result, l);
	}

	Layers l;
};

class Heatmap
{
public:
	virtual void onTap(Tap tap) = 0;

	virtual void save(std::ostream& out) const = 0;
	virtual void load(std::istream& in) = 0;	
};

/** Просто статистика числа нажатий по каждой клавише. */
class OneTapHeatmap : public Heatmap
{
public:
	OneTapHeatmap() : OneTapHeatmap(0, 0, 0) {}
	OneTapHeatmap(int layersN, int rowsN, int colsN);

	void onTap(Tap tap);

	void save(std::ostream& out) const;
	void load(std::istream& in);

	int layersCount(void) const;
	int rowsCount(void) const;
	int colsCount(void) const;
	int getTapCount(Tap tap) const;
private:
	int counter;
	Vector3D<int32_t> v;
};

/** Статистика двух нажатий, когда одно идет за другим. Как часто та или иная комбинация встречается. */
class TwoTapHeatmap : public Heatmap
{
public:
	TwoTapHeatmap(int layersN, int rowsN, int colsN);

	void onTap(Tap tap);

	void save(std::ostream& out) const;
	void load(std::istream& in);

	int layersCount(void) const;
	int rowsCount(void) const;
	int colsCount(void) const;
	int getTapCount(Tap first, Tap second) const; // У второго слой считается таким же как и у первого
private:
	Vector3D<OneTapHeatmap> v;
	Tap lastTap;
};

/** Количество нажатий за определенный день(просто всех клавиш). */
class DayHeatmap : public Heatmap
{
public:
	DayHeatmap();
	void onTap(Tap tap);

	void save(std::ostream& out) const;
	void load(std::istream& in);

	int getToday(void) const;

	int getYear(int day) const;
	int getMonth(int day) const;
	int getDay(int day) const;

	std::map<int, int32_t> getStatistics() const;
private:
	void updateToday();

	int counter;
	int today;
	std::map<int, int32_t> statistics;
};

class AllStatistics
{
public:
	AllStatistics(const std::string& file);

	void load(void);
	void save(void) const;

	OneTapHeatmap onetap;
	TwoTapHeatmap twotap;
	DayHeatmap    daytap;	
	std::string   file;
};

//-----------------------------------------------------------------------------
class Fingers
{
public:
	void save(std::ostream& out) const;
	void load(std::istream& in);

	int getFinger(Tap tap) const;
	int getRow(Tap tap) const;

	std::string getHandName(int finger);
	std::string getFingerName(int finger);
	std::string getRowName(int row);

	void set(Tap tap, int finger, int row);
private:
	class LoadablePair : public std::pair<int, int>
	{
	public:
		LoadablePair() :  std::pair<int, int>() {}
		LoadablePair(const std::pair<int, int>& p) :  std::pair<int, int>(p) {}

		void save(std::ostream& out) const {
			out << this->first << " " << this->second;
		}
		void load(std::istream& in) {
			in >> this->first >> this->second;
		}
	};

	Vector3D<LoadablePair> v;
};

class Names
{
public:
	void save(std::ostream& out) const;
	void load(std::istream& in);

	std::string getName(Tap tap);

	void setName(Tap tap, const std::string& name);
private:
	class LoadableString : public std::string
	{
	public:
		LoadableString() : std::string() {}
		LoadableString(const std::string& str) : std::string(str) {}

		void save(std::ostream& out) const {
			out << *this;
		}
		void load(std::istream& in) {
			std::getline(in, *this);
		}
	};

	Vector3D<LoadableString> v;
};