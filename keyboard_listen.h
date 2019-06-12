#pragma once

#include <string>
#include <iostream>

#include "common.h"

const long max_hold_time = 60'000; // Одну клавишу максимум можно зажимать минуту, иначе считается, что событие о её отпускании потеряно, и она отпускается принудительно

//-----------------------------------------------------------------------------
/** Функции, которые пользователь должен переопределить. */

/** Функция вызывается при запуске программы. Возвращает значение - начинать ли процесс прослушивания клавиатуры или нет. */
bool onStart(int argc, char** argv);

/** Функция, вызываемая каждый раз, когда будет получен следующий завершенный тап по клавиатуре. Передается его положение на матрице клавиатуры, номер слоя, длительность нажатия, и смещение по временной оси относительно начала предыдущего нажатия. Корректность данных гарантируется, но не гарантируется, что функция будет вызвана в тот же момент, когда пришло сообщение об нажатии клавиши. Функция будет вызвана только когда пришло сообщение об отпускании клавиши или позже (зависит от предыдущих зажатых клавиш в очереди). */
void onTap(Tap tap, long time_duration, long time_offset);

/** Функция, вызываемая при закрытии программы. Может использоваться для сохранения статистики и прочего. */
void onExit(void);