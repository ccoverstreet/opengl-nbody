driver: *.cpp
	g++ -Wall -fopenmp -O3 `sdl2-config --cflags --libs` -lGL -lGLEW *.cpp -o driver

run: driver
	./driver
