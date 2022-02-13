CXX=clang++
CXXFLAGS=-O0

CXX_FILES = main.cpp FunctionCallHandler.cpp TrackedInt.cpp

trackedint: main.cpp
	$(CXX) $(CXXFLAGS) -o trackedint $(CXX_FILES)

trackedint-noelide: main.cpp
	$(CXX) $(CXXFLAGS) -fno-elide-constructors -o trackedint-noelide $(CXX_FILES)

noelide: trackedint-noelide

main.cpp: FunctionCallHandler.cpp TrackedInt.cpp
FunctionCallHandler.cpp: FunctionCallHandler.hpp
TrackedInt.cpp: TrackedInt.hpp

clean:
	rm -f trackedint trackedint-noelide