# the compiler: gcc for C program, define as g++ for C++
CC = g++

CFLAGS = -O2 -g -Wall -fmessage-length=0

# define any directories containing header files other than /usr/include
INCLUDES = -I "./include/EIGENDIR" -I "./include/amplapi64_linux/include"

SRCS = SimpleBuildingSimulator.cpp Building.cpp Weather.cpp Occupancy.cpp ReadInput.cpp ThermalModel.cpp ControlBox.cpp# WriteOutput.cpp

OBJS = Building.o Weather.o Occupancy.o ReadInput.o ThermalModel.o ControlBox.o# WriteOutput.o 

LFLAGS = -L "./include/amplapi64_linux/lib" 

LIBS = -lampl

TARGET = Simulate

ALL = $(TARGET)

$(TARGET):	lex.yy.c parser.tab.c parser.tab.h  $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) parser.tab.c lex.yy.c -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) -Wl,-rpath="./include/amplapi64_linux/bin"

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l
		
SimpleBuildingSimulator.o: SimpleBuildingSimulator.cpp defs.h Building.h
	$(CC) $(CFLAGS) $(INCLUDES) -c SimpleBuildingSimulator.cpp
	
Building.o: Building.cpp Building.h defs.h Weather.h Occupancy.h ThermalModel.h
	$(CC) $(CFLAGS) $(INCLUDES) -c Building.cpp

Weather.o: Weather.cpp Weather.h ReadInput.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c Weather.cpp
	
Occupancy.o: Occupancy.cpp Occupancy.h ReadInput.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c Occupancy.cpp

ReadInput.o: ReadInput.cpp ReadInput.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ReadInput.cpp

ThermalModel.o: ThermalModel.cpp ThermalModel.h ControlBox.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ThermalModel.cpp

ControlBox.o: ControlBox.cpp ControlBox.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ControlBox.cpp

#WriteOutput.o: WriteOutput.cpp WriteOutput.h defs.h
#	$(CC) $(CFLAGS) $(INCLUDES) -c WriteOutput.cpp

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) lex.yy.c simulator parser.tab.c parser.tab.h 
	
version:
	$(CC) --version
