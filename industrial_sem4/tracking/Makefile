CXX=clang++
CXXFLAGS=-O0

CXX_FILES = main.cpp TrackedInt.cpp BaseLogger.cpp
HEADERS = FunctionCallHandler.hpp ConsoleLogger.hpp HtmlLogger.hpp DotLogger.hpp BaseLogger.hpp TrackedInt.hpp 

trackedint: $(CXX_FILES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o trackedint $(CXX_FILES)

trackedint-noelide: $(CXX_FILES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -fno-elide-constructors -o trackedint-noelide $(CXX_FILES)

noelide: trackedint-noelide

clean:
	rm -f trackedint trackedint-noelide log.*