RM               := rm -f
DEBUG            := -g

SRC              := src
BIN              := bin
RES              := res

INCLUDEDIRS      := $(shell pkg-config gtkmm-3.0 gloox --cflags) -Iinclude/
LIBS             := $(shell pkg-config gtkmm-3.0 gloox --libs)
LDFLAGS          := $(LIBS)

R_SOURCE         := $(subst $(RES)/,,$(wildcard $(RES)/*.gresource.xml))

OBJECT           := $(BIN)/%.o
CPP_SOURCE       := $(SRC)/%.cpp

CXXSOURCES       := $(wildcard $(SRC)/*.cpp)
CXXOBJECTS       := $(CXXSOURCES:$(CPP_SOURCE)=$(OBJECT))
CXXFLAGS         := $(INCLUDEDIRS) -std=gnu++14 $(DEBUG) -MMD
CXX              := g++-7

DEPENDENCIES     := $(CXXSOURCES:.cpp=.d)

EXECUTABLE       := $(BIN)/calc

all: resources $(EXECUTABLE)

$(OBJECT): $(CPP_SOURCE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(BIN)/resources.o $(CXXFLAGS) $(LDFLAGS)

resources:
	cd $(RES) && glib-compile-resources --target=resources.c --generate-source $(R_SOURCE)
	$(CXX) $(CXXFLAGS) -c $(RES)/resources.c -o $(BIN)/resources.o

clean:
	$(RM) $(CXXOBJECTS) $(ROBJECTS) $(EXECUTABLE) $(RES)/resources.c

run:
	./$(EXECUTABLE)

-include $(DEPENDENCIES)
