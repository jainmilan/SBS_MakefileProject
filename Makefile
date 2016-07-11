# the compiler: gcc for C program, define as g++ for C++
CC = g++

CFLAGS = -O2 -g -Wall -fmessage-length=0

# define any directories containing header files other than /usr/include
INCLUDES = -I "./include/EIGENDIR" -I "./include/amplapi32/include"

SRCS = SimpleBuildingSimulator.cpp Building.cpp Weather.cpp ControlBox.cpp Occupancy.cpp WriteOutput.cpp ReadInput.cpp

OBJS = Building.o Weather.o ControlBox.o Occupancy.o WriteOutput.o ReadInput.o

LFLAGS = -L "./include/amplapi32/lib" 

LIBS = -lampl1.2.2

TARGET = Simulate

ALL = $(TARGET)

$(TARGET):	lex.yy.c parser.tab.c parser.tab.h  $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) parser.tab.c lex.yy.c -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) -Wl,-rpath="./include/amplapi32/bin"

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l
		
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

ReadInput.o: ReadInput.cpp ReadInput.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ReadInput.cpp

WriteOutput.o: WriteOutput.cpp WriteOutput.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c WriteOutput.cpp

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) lex.yy.c simulator parser.tab.c parser.tab.h 
	
version:
	$(CC) --version
