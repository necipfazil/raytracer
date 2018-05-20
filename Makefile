files = source/utility/*.cpp source/utility/tinyply-2.0/source/*.cpp source/image/*.cpp source/image/reinhard/*.cpp source/filemanip/*.cpp source/geometry/impl/*.cpp source/scene/*.cpp source/main.cpp
flags = -std=c++11 -O3 -ljpeg -lpng -pthread
debugflags = -std=c++11 -pg -ljpeg -lpng -pthread
gdbflags = -std=c++11 -O0 -g -ljpeg -lpng -pthread
opencv_cflags = $(shell pkg-config --libs --cflags opencv)
compiler = g++
output = raytracer

all:
	$(compiler) $(files) -o $(output) $(flags) $(opencv_cflags)
debug:
	$(compiler) $(files) -o $(output) $(debugflags) $(opencv_cflags)
gdb:
	$(compiler) $(files) -o $(output) $(gdbflags) $(opencv_cflags)
clean:
	rm $(output)
