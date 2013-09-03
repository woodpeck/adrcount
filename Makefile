
CXX = g++
#CXX = clang++

#CXXFLAGS = -g
CXXFLAGS = -O3 

CXXFLAGS += -Wall -Wextra -Wdisabled-optimization -pedantic -Wctor-dtor-privacy -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wno-long-long

CXXFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CXXFLAGS += -I../include

# remove this if you do not want debugging to be compiled in

LIB_EXPAT  = -lexpat
LIB_PBF    = -lz -lpthread -lprotobuf-lite -losmpbf

LDFLAGS = $(LIB_EXPAT) $(LIB_PBF)

PROGRAMS = \
    adr \
    adrdump \
    adrquery \
    adrdiff 

.PHONY: all clean

all: $(PROGRAMS)

adr: adr.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
adrdump: adrdump.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) 
adrdiff: adrdiff.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
adrquery: adrquery.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o core $(PROGRAMS)

