CXX=clang++
CXXFLAGS=-mavx2 -lsfml-window -lsfml-graphics -lsfml-system -O3

build:
	$(CXX) $(CXXFLAGS) main.cpp MandelbrotApp.cpp -o mandelbrot

clean:
	rm -f mandelbrot