
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW
EXT =
CPPFLAGS = `pkg-config --cflags glfw3` -g -O0

CC = g++
EXE = assign3_part1
OBJS = modelViewer.o Shader.o Viewer.o Object.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

modelViewer.o: modelViewer.cpp InputState.h
	$(CC) $(CPPFLAGS) -c modelViewer.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Object.o : Object.cpp Object.h
	$(CC) $(CPPFLAGS) -c Object.cpp

Viewer.o: Viewer.h Viewer.cpp InputState.h
	$(CC) $(CPPFLAGS) -c Viewer.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
