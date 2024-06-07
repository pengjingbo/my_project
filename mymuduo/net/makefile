CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g 

TARGET = libmymuduo.so
OBJS = ./Tcpserver/*.cc

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)  -fPIC -shared -o ./build/$(TARGET)  -pthread 

clean:
	rm -rf ../bin/$(OBJS) $(TARGET)
