appname := fix-copc-srs

CXX := clang++
CXXFLAGS := -std=c++11 -I${CONDA_PREFIX}/include
LDFLAGS := -L${CONDA_PREFIX}/lib -lpdalcpp

srcfiles := $(shell find . -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)
	install_name_tool -change @rpath/libpdalcpp.13.dylib ${CONDA_PREFIX}/lib/libpdalcpp.13.dylib $(appname)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend
