CXX=clang++
CXXFLAGS=-O0

CXX_FILES = main.cpp TrackedInt.cpp BaseLogger.cpp

trackedint: main.cpp
	$(CXX) $(CXXFLAGS) -o trackedint $(CXX_FILES)

trackedint-noelide: main.cpp
	$(CXX) $(CXXFLAGS) -fno-elide-constructors -o trackedint-noelide $(CXX_FILES)

noelide: trackedint-noelide

main.cpp: FunctionCallHandler.hpp TrackedInt.cpp BaseLogger.cpp ConsoleLogger.hpp
TrackedInt.cpp: TrackedInt.hpp
BaseLogger.cpp: BaseLogger.hpp

clean:
	rm -f trackedint trackedint-noelide