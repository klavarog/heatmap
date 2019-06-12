# Введение

На самом деле программируемая клавиатура способна не только передавать нажатые клавиши, но ещё и писать в консоль при помощи `print.h`. Для того, чтобы посмотреть что она пишет, необходима программа `hid_listener`, бинарники и исходники которой можно скачать на сайте: [https://www.pjrc.com/teensy/hid_listen.html](https://www.pjrc.com/teensy/hid_listen.html). 

В последующих инструментах общение с клавиатурой будет реализовано через эту программу, так что необходимо поместить её в папку с программой-инструментом.

Все инструменты здесь будут так или иначе связаны с этой фичей QMK.

# Настройки своей раскладки

Для работы инструментов необходимо, чтобы клавиатура писала текущую нажатую клавишу при помощи `uprint`. Необходимо добавить следующий код в `keymap.c` в функцию `process_record_user`:
```c++
#include <print.h>

// ...

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  #ifdef CONSOLE_ENABLE
  uprintf("KL: col=%d, row=%d, pressed=%d, layer=%d\n", record->event.key.col, record->event.key.row, record->event.pressed, biton32(layer_state));
  #endif

// ...
```

И включить консоль в `rules.mk`:
```c++
CONSOLE_ENABLE = yes
```

# Mini Typing Statistics

Инструмент для отображения нажатой клавиши в реальном времени. Показывает положение в матрице, а так же слой, время нажатия (duration) и временное смещение относительно начала предыдущего нажатия (offset). Требует `hid_listener`. Так же является примером работы с кодом.

Компиляция:
```
cd tools
make mini_typing_statistics
./mini_typing_statistics
```

Пример работы:
```
col=2, row=9, layer=0, offset=0ms, duration=86ms
col=2, row=3, layer=0, offset=100ms, duration=100ms
col=1, row=12, layer=0, offset=149ms, duration=50ms
col=1, row=12, layer=0, offset=150ms, duration=49ms
col=2, row=2, layer=0, offset=99ms, duration=89ms
col=5, row=10, layer=0, offset=100ms, duration=148ms
col=3, row=10, layer=0, offset=199ms, duration=32ms
col=2, row=2, layer=0, offset=50ms, duration=50ms
col=1, row=11, layer=0, offset=99ms, duration=99ms
col=1, row=12, layer=0, offset=199ms, duration=100ms
col=2, row=8, layer=0, offset=49ms, duration=100ms
col=5, row=10, layer=0, offset=200ms, duration=150ms
col=2, row=0, layer=0, offset=450ms, duration=2449ms # Это клавиша включения слоя, и она зажата на протяжении всех следующих клавиш, поэтому и duration такое большое 
col=2, row=9, layer=4, offset=299ms, duration=50ms
col=2, row=9, layer=4, offset=150ms, duration=49ms
col=2, row=9, layer=4, offset=99ms, duration=50ms
col=2, row=9, layer=4, offset=150ms, duration=50ms
col=2, row=9, layer=4, offset=100ms, duration=49ms
col=2, row=9, layer=4, offset=99ms, duration=50ms
col=2, row=9, layer=4, offset=149ms, duration=50ms
col=2, row=9, layer=4, offset=81ms, duration=19ms
col=2, row=9, layer=4, offset=119ms, duration=49ms
col=2, row=9, layer=4, offset=99ms, duration=50ms
col=2, row=9, layer=4, offset=100ms, duration=99ms
col=2, row=10, layer=4, offset=550ms, duration=99ms
```

# Heatmap Recorder

Требует `hid_listener`.

Инструмент для записи статистики в файл. Записывает:
* Нажатия по одному символу для каждой клавиши на каждом слое.
* Нажатия по два последовательных символа.
* Число нажатий в день.

Так же обрабатывает закрытие программы различными способами и сохраняет всю статистику в перед выходом.

Эту программу лучше всего поставить в автозагрузку.

Компиляция:

```
cd tools
make heatmap_recorder
./heatmap_recorder
```

## Запуск этой программы при старте системы

На Windows: нажать Win+R, ввести `shell:startup`, поместить туда ярлык этой программы.

# Heatmap Displayer

Программа, которая выводит статистику по файлу `heatmap_file.txt`, в который записывает предыдущая утилита. Не требует `hid_listener`. Всю статистику выводит в табличном формате.

Возможности вывода статистики:
* Подробная справка с использованием `boost::program_options`. `-h`
	```
	General options:
	  -h [ --help ]                 Show help
	  -s [ --statfile ] arg         File with statistics. For default it is
	                                `heatmap_file.txt`.
	  -t [ --type ] arg             Select statistics type: `sum`, `onetap`,
	                                `daily`.
	                                  `sum` - sum keys count in different ways.
	                                  `onetap` - statistics of one tap.
	                                  `daily` - statistics by each day.

	`sum` options:
	  -r [ --regime ] arg           Select sum regime: `all`, `fingers`, `rows`.
	                                  `all` - write count of all taps.
	                                  `fingers` - write count of taps by fingers.
	                                  `rows` - write count of taps by rows.
	  -f [ --fingerfile ] arg       File with fingers and rows specification
	  -S [ --separate-by-layers ]   Separate any statistics by layers.

	`onetap` options:
	  -z [ --show-zeros ]           Show keys with zero taps.
	  -S [ --separate-by-layers ]   Separate any statistics by layers.
	  -r [ --sort ] arg             Can be: `frequent`, `no`, `rare`. Sorted
	                                statistics by taps count. If frequent is
	                                selected, then most frequent keys will be on
	                                the top.
	  -f [ --namefile ] arg         File with key names. If this file is specified,
	                                then position of key isn't displayed. To
	                                display it, use next option.
	  -p [ --show-pos-with-name ]   When file with keys names is specified, prints
	                                position in format: layer,row,col,"Name".
	                                Example: 0,1,1,"A".
	  -u [ --substr ] arg           Integer value N. Shows only N first results.

	`daily` options:
	  -p [ --period ] arg           Can be: `today`, `yesterday`, `week`, `month`,
	                                `year`, `all`.
	  -S [ --sort-by-taps ]         For default sorted by day. With this option
	                                stats will be sorted by taps count.
	```
* Число всех нажатий, что зафиксировала программа. `-tsum -rall`
	```
	Count of all taps: 51045
	```
* Число нажатий для каждого слоя. `-tsum -rall -S`
	```
	Layer: 0, taps: 17385, percent: 34.06%
	Layer: 1, taps:  1108, percent:  2.17%
	Layer: 2, taps: 23789, percent: 46.60%
	Layer: 3, taps:   335, percent:  0.66%
	Layer: 4, taps:  8355, percent: 16.37%
	Layer: 5, taps:    71, percent:  0.14%
	Layer: 6, taps:     2, percent:  0.00%
	```
* Число нажатий по каждому пальцу. `-tsum -rfingers -f"layout_fingers.txt"`
	```
	Hand:  left, finger: pinkie, taps: 5283, percent: 10.35%
	Hand:  left, finger:   ring, taps: 1864, percent:  3.65%
	Hand:  left, finger: middle, taps: 3622, percent:  7.10%
	Hand:  left, finger:  index, taps: 7620, percent: 14.93%
	Hand:  left, finger:  thumb, taps: 4043, percent:  7.92%
	Hand: right, finger:  thumb, taps: 5285, percent: 10.35%
	Hand: right, finger:  index, taps: 8268, percent: 16.20%
	Hand: right, finger: middle, taps: 6432, percent: 12.60%
	Hand: right, finger:   ring, taps: 5291, percent: 10.37%
	Hand: right, finger: pinkie, taps: 3206, percent:  6.28%
	Hand:   any, finger:    any, taps:  131, percent:  0.26%
	```
* Число нажатий по каждому пальцу, для каждого слоя. `-tsum -rfingers -S -f"layout_fingers.txt"`
	```
	For layer: 0
	Hand:  left, finger: pinkie, taps: 3092, percent: 17.79%
	Hand:  left, finger:   ring, taps:  689, percent:  3.96%
	Hand:  left, finger: middle, taps: 1158, percent:  6.66%
	Hand:  left, finger:  index, taps: 2008, percent: 11.55%
	Hand:  left, finger:  thumb, taps: 2777, percent: 15.97%
	Hand: right, finger:  thumb, taps: 1543, percent:  8.88%
	Hand: right, finger:  index, taps: 1946, percent: 11.19%
	Hand: right, finger: middle, taps: 1389, percent:  7.99%
	Hand: right, finger:   ring, taps: 1353, percent:  7.78%
	Hand: right, finger: pinkie, taps: 1328, percent:  7.64%
	Hand:   any, finger:    any, taps:  102, percent:  0.59%

	For layer: 1
	Hand:  left, finger: pinkie, taps: 368, percent: 33.21%
	Hand:  left, finger:   ring, taps: 111, percent: 10.02%
	Hand:  left, finger: middle, taps:  45, percent:  4.06%
	Hand:  left, finger:  index, taps: 108, percent:  9.75%
	Hand:  left, finger:  thumb, taps:  15, percent:  1.35%
	Hand: right, finger:  thumb, taps: 162, percent: 14.62%
	Hand: right, finger:  index, taps:  99, percent:  8.94%
	Hand: right, finger: middle, taps:  56, percent:  5.05%
	Hand: right, finger:   ring, taps:  53, percent:  4.78%
	Hand: right, finger: pinkie, taps:  88, percent:  7.94%
	Hand:   any, finger:    any, taps:   3, percent:  0.27%

	...
	```
* Число нажатий по каждому ряду. `-tsum -rrows -f"layout_fingers.txt"`
	```
	Row:  2-up, taps:  1791, percent:  3.51%
	Row:  1-up, taps: 12134, percent: 23.77%
	Row:  home, taps: 29666, percent: 58.12%
	Row: 1-low, taps:  7304, percent: 14.31%
	Row: 2-low, taps:    19, percent:  0.04%
	Row:   any, taps:   131, percent:  0.26%
	```
* Число нажатий по каждому ряду, для каждого слоя. `-tsum -rrows -S -f"layout_fingers.txt"`
	```
	For layer: 0
	Row:  2-up, taps:  1292, percent:  7.43%
	Row:  1-up, taps:  4157, percent: 23.91%
	Row:  home, taps: 10065, percent: 57.89%
	Row: 1-low, taps:  1760, percent: 10.12%
	Row: 2-low, taps:     9, percent:  0.05%
	Row:   any, taps:   102, percent:  0.59%

	For layer: 1
	Row:  2-up, taps: 184, percent: 16.61%
	Row:  1-up, taps: 519, percent: 46.84%
	Row:  home, taps: 216, percent: 19.49%
	Row: 1-low, taps: 185, percent: 16.70%
	Row: 2-low, taps:   1, percent:  0.09%
	Row:   any, taps:   3, percent:  0.27%

	...
	```
* Количество нажатий каждой клавиши. `-tonetap -u10`
	```
	2,10,5 taps: 2986, percent: 5.85%
	 2,9,2 taps: 2080, percent: 4.07%
	 0,0,2 taps: 2009, percent: 3.94%
	4,10,2 taps: 1968, percent: 3.86%
	4,11,2 taps: 1954, percent: 3.83%
	 2,4,2 taps: 1536, percent: 3.01%
	 2,4,1 taps: 1496, percent: 2.93%
	 0,2,5 taps: 1361, percent: 2.67%
	2,10,1 taps: 1288, percent: 2.52%
	0,10,5 taps: 1263, percent: 2.47%
	```
* Количество нажатий каждой клавиши с настраиваемым именем каждой клавиши. `-tonetap -u10 -f"layout_names.txt"`
	```
	    Space taps: 2986, percent: 5.85%
	        о taps: 2080, percent: 4.07%
	    MO(4) taps: 2009, percent: 3.94%
	    KC_UP taps: 1968, percent: 3.86%
	  KC_DOWN taps: 1954, percent: 3.83%
	        а taps: 1536, percent: 3.01%
	        е taps: 1496, percent: 2.93%
	Backspace taps: 1361, percent: 2.67%
	        и taps: 1288, percent: 2.52%
	    Space taps: 1263, percent: 2.47%
	```
* Количество нажатий каждой клавиши, разделенные по слою. `-tonetap -u10 -S -f"layout_names.txt"`
	```
	Layer: 0
	    MO(4) taps: 2009, percent: 11.56%
	Backspace taps: 1361, percent:  7.83%
	    Space taps: 1263, percent:  7.26%
	        e taps: 1039, percent:  5.98%
	        t taps:  846, percent:  4.87%
	    Shift taps:  773, percent:  4.45%
	        s taps:  734, percent:  4.22%
	        a taps:  712, percent:  4.10%
	    Enter taps:  596, percent:  3.43%
	        i taps:  594, percent:  3.42%

	Layer: 1
	    : taps: 184, percent: 16.61%
	    ' taps: 136, percent: 12.27%
	    _ taps:  99, percent:  8.94%
	    < taps:  99, percent:  8.94%
	    S taps:  47, percent:  4.24%
	    1 taps:  44, percent:  3.97%
	    0 taps:  38, percent:  3.43%
	    T taps:  35, percent:  3.16%
	    > taps:  34, percent:  3.07%
	Space taps:  30, percent:  2.71%

	...
	```
* Количество напечатанных символов за каждый день записи статистики. `-tdaily`
	```
	13.4.2019: 18034 taps
	14.4.2019: 33011 taps
	```

Компиляция:

```
cd tools
make heatmap_displayer
./heatmap_displayer
```

# Make Fingers Names

Это код для создания файлов `layout_names.txt` `layout_fingers.txt` конкретно для вашей раскладки. Эти файлы содержат названия, палец и ряд каждой клавиши. Для создания этого можно легко адаптировать вашу раскладку из `keymap.c`, более подробно смотрите в самом файле. Его потом придется скомпилировать и запустить, потому что там используется препроцессор, при помощи которого задана ваша раскладка.

Эти файлы далее используются для вывода статистики по пальцам, рядам, а так же для удобного обозначения клавиш, например в режиме `onetap`.