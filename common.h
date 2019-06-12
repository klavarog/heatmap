#pragma once

#include <iostream>

/** Структура положения нажатия на клавиатуре. */
struct Tap
{
	int col, row, layer;
};

inline std::ostream& operator<<(std::ostream& out, const Tap& tap) {
	out << "(layer=" << tap.layer << ", row=" << tap.row << ", col=" << tap.col << ")";
	return out;
}

/** Строка str имеет формат: "value=150, another=10, something=0", возвращает чему равно то или иное значение по его названию "value", "another", "something". */
int parseStrValue(const std::string& str, const std::string& name);