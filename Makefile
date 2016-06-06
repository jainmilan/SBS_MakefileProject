# the compiler: gcc for C program, define as g++ for C++
CC = g++

CFLAGS =	-O2 -g -Wall -fmessage-length=0

# define any directories containing header files other than /usr/include
INCLUDES = -I "./include/EIGENDIR" -I "./include/amplapi32/include"

SRCS = SimpleBuildingSimulator.cpp Building.cpp Weather.cpp ControlBox.cpp Occupancy.cpp

OBJS = SimpleBuildingSimulator.o Building.o Weather.o ControlBox.o Occupancy.o

LFLAGS = -L "./include/amplapi32/lib" 

LIBS = -lampl1.2.2

TARGET = SimpleBuildingSimulator

ALL = $(TARGET)

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) -Wl,-rpath="./include/amplapi32/bin"

SimpleBuildingSimulator.o: SimpleBuildingSimulator.cpp defs.h Building.h
	$(CC) $(CFLAGS) $(INCLUDES) -c SimpleBuildingSimulator.cpp
	
Building.o: Building.cpp Building.h defs.h Weather.h ControlBox.h Occupancy.h
	$(CC) $(CFLAGS) $(INCLUDES) -c Building.cpp

Weather.o: Weather.cpp Weather.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c Weather.cpp
	
ControlBox.o: ControlBox.cpp ControlBox.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ControlBox.cpp

Occupancy.o: Occupancy.cpp Occupancy.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c Occupancy.cpp

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
