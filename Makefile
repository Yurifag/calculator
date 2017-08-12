rwildcard         = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

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

CXXSOURCES       := $(call rwildcard,$(SRC),*.cpp)
CXXOBJECTS       := $(CXXSOURCES:$(CPP_SOURCE)=$(OBJECT))
CXXFLAGS         := $(INCLUDEDIRS) -std=gnu++14 $(DEBUG) -MMD
CXX              := g++-7

DEPENDENCIES     := $(CXXSOURCES:.cpp=.d)
EXECUTABLE       := $(BIN)/calc

.DEFAULT_GOAL    := all

all: resources $(EXECUTABLE)

$(OBJECT): $(CPP_SOURCE)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(BIN)/resources.o $(CXXFLAGS) $(LDFLAGS)

resources:
	cd $(RES) && glib-compile-resources --target=resources.c --generate-source $(R_SOURCE)
	$(CXX) $(CXXFLAGS) -c $(RES)/resources.c -o $(BIN)/resources.o

.PHONY: clean
clean:
	$(RM) $(CXXOBJECTS) $(ROBJECTS) $(EXECUTABLE) $(RES)/resources.c

.PHONY: run
run:
	./$(EXECUTABLE)

-include $(DEPENDENCIES)
