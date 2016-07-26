# the compiler: gcc for C program, define as g++ for C++
CC = g++

CFLAGS = $(CCFLAGS) -O2 -g -Wall -fmessage-length=0

# define any directories containing header files other than /usr/include
INCLUDEAI = -I "./include/EIGENDIR"

ifeq ($(OS),Windows_NT)
	CCFLAGS += -D WIN32
	LIBS = -lampl1.2.2
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CCFLAGS += -D AMD64
		INCLUDES = $(INCLUDEAI) -I "./include/amplapi64/include"
		LFLAGS = -L "./include/amplapi64/lib"
		BINFILE = "./include/amplapi64/bin" 
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CCFLAGS += -D IA32
		INCLUDES = $(INCLUDEAI) -I "./include/amplapi32/include"
		LFLAGS = -L "./include/amplapi32/lib"
		BINFILE = "./include/amplapi32/bin" 
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CCFLAGS += -D LINUX
		LIBS = -lampl
	endif
	ifeq ($(UNAME_S),Darwin)
		CCFLAGS += -D OSX
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P), x86_64)
		CCFLAGS += -D AMD64
		INCLUDES = $(INCLUDEAI) -I "./include/amplapi64_linux/include"
		LFLAGS = -L "./include/amplapi64_linux/lib"
		BINFILE = "./include/amplapi64_linux/lib" 
	endif
	ifneq ($(filter %86, $(UNAME_P)),)
		CCFLAGS += -D IA32
		INCLUDES = $(INCLUDEAI) -I "./include/amplapi32_linux/include"
		LFLAGS = -L "./include/amplapi32_linux/lib"
		BINFILE = "./include/amplapi32_linux/lib" 
	endif
	ifneq ($(filter arm%, $(UNAME_P)),)
		CCFLAGS += -D ARM
	endif
endif


SRCS = SimpleBuildingSimulator.cpp Building.cpp Weather.cpp Occupancy.cpp ReadInput.cpp ThermalModel.cpp ControlBox.cpp IntroduceError.cpp# WriteOutput.cpp

OBJS = Building.o Weather.o Occupancy.o ReadInput.o ThermalModel.o ControlBox.o IntroduceError.o # WriteOutput.o 

TARGET = Simulate

ALL = $(TARGET)

$(TARGET):	lex.yy.c parser.tab.c parser.tab.h  $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) parser.tab.c lex.yy.c -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) -Wl,-rpath=$(BINFILE)

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l
		
SimpleBuildingSimulator.o: SimpleBuildingSimulator.cpp defs.h Building.h
	$(CC) $(CFLAGS) $(INCLUDES) -c SimpleBuildingSimulator.cpp
	
Building.o: Building.cpp Building.h defs.h IntroduceError.h Weather.h Occupancy.h ThermalModel.h
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

IntroduceError.o: IntroduceError.cpp IntroduceError.h defs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c IntroduceError.cpp

#WriteOutput.o: WriteOutput.cpp WriteOutput.h defs.h
#	$(CC) $(CFLAGS) $(INCLUDES) -c WriteOutput.cpp

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) lex.yy.c simulator parser.tab.c parser.tab.h 
	
version:
	$(CC) --version
