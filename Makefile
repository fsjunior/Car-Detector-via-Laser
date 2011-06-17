all:
		gcc -c laserselector.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c utils.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c main.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -o car_detector main.o laserselector.o utils.o `pkg-config --libs playerc gsl opencv` -lm -Wall
		

clean:
		rm *.o car_detector
