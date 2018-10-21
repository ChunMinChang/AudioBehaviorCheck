CXX=g++
CFLAGS=-Wall -std=c++14
LIBRARIES=-framework CoreAudio -framework AudioUnit -framework CoreFoundation

SOURCES=AudioDeviceListener.cpp\
        AudioObject.cpp\
        AudioObjectUtils.cpp\
        AudioStream.cpp
OBJECTS=$(SOURCES:.cpp=.o)

TESTS=test_audio.cpp\
      test_cfstring.cpp\
      test_deadlock.cpp\
      test_listener.cpp\
      test_utils.cpp
EXECUTABLES=$(TESTS:.cpp=)

all: $(OBJECTS) build

build:
	$(foreach src, $(TESTS), $(CXX) $(CFLAGS) -lc++ $(LIBRARIES) $(OBJECTS) $(src) -o $(src:.cpp=);)

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLES) *.o
