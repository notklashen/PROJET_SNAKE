CC = g++
CPPFLAGS = -std=c++11 -fPIC -c -g
LD = g++
LDFLAGS = -std=c++11
FINAL_TARGET = snake

INCLUDE_DIR = -I/usr/include/x86_64-linux-gnu/qt5
LIBS = -lQt5Widgets -lQt5Gui -lQt5Core

default: $(FINAL_TARGET)

$(FINAL_TARGET): jeu.o main.o snakewindow.o
	$(LD) $+ -o $@ $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE_DIR) $< -o $@

clean:
	rm -f *.o $(FINAL_TARGET)