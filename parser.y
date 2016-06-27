%{
	#include <cstdio>
	#include <time.h>
	#include <iostream>
	#include "Building.h"

	using namespace SimpleBuildingSimulator;
	using namespace Eigen;
	using namespace std;
	
	extern "C" int yylex();
	extern "C" int yyparse();
	extern "C" FILE *yyin;
	extern int line_num;
	
	void yyerror(const char *s);
	
	/* Initialize Building */
	Building dc;
	
	int time_step, control_type;
	long int duration; 
	int MIN2SEC = 60;
%}

%union {
	int ival;
	float fval;
	char *sval;
}

%token ENDL

%token ZONES
%token ROOMS

%token DURATION
%token TIMESTEP
%token CONTROL

%token HEFF
%token CEFF
%token C
%token CSPOT
%token ALPHAO
%token ALPHAR
%token QL
%token QH
%token QS
%token FANCOEF
%token DENSITY
%token SPHEAT
%token P1
%token P2
%token P3
%token P4

%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%

simulator: template;

template: start parameters stop;

parameters: 
	parameters ',' ENDLS body_line
	| parameters ',' ENDLS bracket
	| bracket
	| body_line
	;
	
bracket: start body_lines stop;

start: STRING ':' '{' ENDLS;

stop: ENDLS '}';

body_lines: 
	body_lines ',' ENDLS body_line
	| body_line
	;
	
body_line:
	ZONES ':' INT							{ dc.num_zones_ = $3; }
	| ROOMS ':' INT							{ dc.num_rooms_ = $3; }
	| DURATION ':' INT						{ duration = $3; }
	| TIMESTEP ':' INT						{ time_step = $3; }
	| CONTROL ':' INT						{ control_type = $3; }
	| HEFF ':' FLOAT						{ dc.CommonAHU.HeatingEfficiency = $3; }
	| CEFF ':' FLOAT						{ dc.CommonAHU.CoolingEfficiency = $3; }
	| C ':' INT								{ dc.CommonRoom.C = $3; }
	| CSPOT ':' INT							{ dc.CommonRoom.C_ = $3; }
	| ALPHAO ':' FLOAT						{ dc.CommonRoom.alpha_o = $3; }
	| ALPHAR ':' FLOAT						{ dc.CommonRoom.alpha_r = $3; }
	| QL ':' FLOAT							{ dc.CommonRoom.Q_l = $3; }
	| QH ':' FLOAT							{ dc.CommonRoom.Q_h = $3; }
	| QS ':' FLOAT							{ dc.CommonRoom.Q_s = $3; }
	| FANCOEF ':' FLOAT						{ dc.CommonRoom.fan_coef = $3; }
	| DENSITY ':' FLOAT						{ dc.CommonAir.density = $3; }
	| SPHEAT ':' FLOAT						{ dc.CommonAir.specific_heat = $3; }
	| P1 ':' FLOAT							{ dc.PMV_Params.P1 = $3; }
	| P2 ':' FLOAT							{ dc.PMV_Params.P2 = $3; }
	| P3 ':' FLOAT							{ dc.PMV_Params.P3 = $3; }
	| P4 ':' FLOAT							{ dc.PMV_Params.P4 = $3; }
	| STRING ':' STRING						{ cout << $1 << ": " << $3 << endl; }
	;
	
ENDLS: 
	ENDLS ENDL
	| ENDL
	;

%%

int main( int argc, char *argv[] ) {
	
	if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        /* We print argv[0] assuming it is the program name */
        printf( "usage: %s filename", argv[0] );
        return -1;
    }
    else
    {
		// Open a file handler to open a particular file
		FILE *myfile = fopen(argv[1], "r");
		
		// Make sure file is valid
		if (!myfile) {
			cout << "Can't open in.jason!" << endl;
			return -1;
		}
		
		// Set flex to read from file instead of STDIN
		yyin = myfile;
		
		// Parse through the input
		do {
			yyparse();
		} while (!feof(yyin));
		
		time_t t = time(0);   // get time now
     	struct tm * now = localtime( & t );
	
     	char buffer[80];
     	strftime(buffer,80,"%y%m%d%H%M%S",now);
		
		char csvfile[80];
		
		strcpy(csvfile, "output/simulation_");
		strcat(csvfile, buffer);
		strcat(csvfile, ".csv");
     	
     	cout << filename << endl;
		dc.Simulate(duration, time_step, control_type, csvfile);
	}
}

void yyerror(const char *s) {
	cout << "EEK, parse error on line " << line_num << "! Message: " << s << endl;
	
	// Halt the program
	exit(-1);
}