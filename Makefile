CXX = g++

CXXFLAGS += -D_WIN32 # Если компиляция происходит под Windows
#CXXFLAGS += -Dlinux # Если компиляция происходит под Linux
CXXFLAGS += -U__STRICT_ANSI__

# Общие флаги
CXXFLAGS += -O3
CXXFLAGS += -std=c++17
CXXFLAGS += -ID:/MinGW/MinGW/include

# Сильно увеличивает размер, но делает проги независимыми на чужом компе
LINKFLAGS += -static-libgcc -static-libstdc++

# Если эту опцию поместить перед mwindows, то получится консоль с окном
#LINKFLAGS += -mconsole 

# Оптимизация размера
LINKFLAGS += -s 
LINKFLAGS += -lboost_program_options
SIZEFLAGS += -Wl,--gc-sections -Wl,--strip-all 
SIZEFLAGS += -fdata-sections -ffunction-sections
SIZEFLAGS += -ffast-math
SIZEFLAGS += -Wunused
SIZEFLAGS += -flto

INCLUDES += keyboard_listen.h
INCLUDES += heatmap.h
INCLUDES += common.h

all: mini_typing_statistics heatmap_recorder heatmap_displayer make_names_fingers

mini_typing_statistics: mini_typing_statistics.o keyboard_listen.o
	$(CXX) $(SIZEFLAGS) $(CXXFLAGS) mini_typing_statistics.o keyboard_listen.o -o mini_typing_statistics $(LINKFLAGS)

heatmap_recorder: heatmap_recorder.o keyboard_listen.o heatmap.o
	$(CXX) $(SIZEFLAGS) $(CXXFLAGS) heatmap.o heatmap_recorder.o keyboard_listen.o -o heatmap_recorder $(LINKFLAGS)

heatmap_displayer: heatmap_displayer.o heatmap.o
	$(CXX) $(SIZEFLAGS) $(CXXFLAGS) heatmap_displayer.o heatmap.o -o heatmap_displayer $(LINKFLAGS)

make_names_fingers: make_names_fingers.o heatmap.o
	$(CXX) $(SIZEFLAGS) $(CXXFLAGS) make_names_fingers.o heatmap.o -o make_names_fingers $(LINKFLAGS)

%.o: %.cpp $(INCLUDES)
	$(CXX) $(SIZEFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm mini_typing_statistics.o 
	rm keyboard_listen.o 
	rm heatmap_recorder.o 
	rm heatmap_displayer.o 
	rm heatmap.o
	rm mini_typing_statistics
	rm heatmap_recorder
	rm heatmap_displayer