FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL

COMPILERFLAGS = -Wall
CC = g++ 
CFLAGS = $(COMPILERFLAGS) 

all: quadsviewer radiositysolver radiosityviewer

quadsviewer: common.cpp quadmodel.cpp quadsviewer.cpp trackball.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp quadmodel.cpp quadsviewer.cpp trackball.cpp -o quadsviewer.o

radiositysolver: common.cpp quadmodel.cpp radiositysolver.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp quadmodel.cpp radiositysolver.cpp -o solver.o

radiosityviewer: common.cpp trackball.cpp radiosityviewer.cpp
	$(CC) $(FRAMEWORK) $(CFLAGS) common.cpp trackball.cpp radiosityviewer.cpp -o radiosityviewer.o
	
clean:
	rm *.o