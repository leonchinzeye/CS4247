FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL

COMPILERFLAGS = -Wall
CC = g++ 
CFLAGS = $(COMPILERFLAGS) 

all: quadsviewer

quadsviewer: common.cpp quadmodel.cpp quadsviewer.cpp trackball.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp quadmodel.cpp quadsviewer.cpp trackball.cpp -o quadsviewer.o

clean:
	rm *.o