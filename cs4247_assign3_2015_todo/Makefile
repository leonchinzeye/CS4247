FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL

COMPILERFLAGS = -Wall
CC = g++ 
CFLAGS = $(COMPILERFLAGS) 

all: quadsviewer solver viewer

quadsviewer: common.cpp quadmodel.cpp quadsviewer.cpp trackball.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp quadmodel.cpp quadsviewer.cpp trackball.cpp -o quadsviewer.o

solver: common.cpp quadmodel.cpp radiositysolver.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp quadmodel.cpp radiositysolver.cpp -o solver.o

viewer: common.cpp trackball.cpp radiosityviewer.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp trackball.cpp radiosityviewer.cpp -o viewer.o

remove:
	rm viewer.o
	
clean:
	rm *.o