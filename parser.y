%{
	#ifndef _XOPEN_SOURCE
	#define _XOPEN_SOURCE
	#endif
	
	#include <cstdio>
	#include <ctime>
	#include <iostream>
	#include <fstream>
	#include <vector>
	#include <string>
	#include <stdlib.h>
	#include <sstream>
	
	#include "defs.h"
	#include "Building.h"
	#include "Weather.h"
	#include "Occupancy.h"
	
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
	Weather weather;
	Occupants occupancy;

	int time_step, control_type, horizon, start, stop;
	time_t start_t, stop_t;
	string start_str, stop_str; 
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

%token START
%token STOP
%token HORIZON
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

%token BPARAMS
%token TEXT

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
	| START ':' STRING						{ start_str = string($3); }
	| STOP ':' STRING						{ stop_str = string($3); }
	| HORIZON ':' INT						{ horizon = $3; }
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
	| BPARAMS ':' FLOAT						{ dc.CommonErrors.err_bparams = $3; }
	| TEXT ':' FLOAT						{ dc.CommonErrors.err_text = $3; }
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
     	
     	cout << csvfile << endl;
		
		struct tm tm = {0};
		
		cout << start_str << "\t" << stop_str << "\n";
		if (strptime(start_str.c_str(), "%Y%m%dT%H%M", &tm)) {
			start_t = mktime(&tm);
		}
		
		if (strptime(stop_str.c_str(), "%Y%m%dT%H%M", &tm)) {
			stop_t = mktime(&tm);
		}
		dc.Simulate(start_t, stop_t, time_step, control_type, csvfile);

	    return 0;
	}
}

void yyerror(const char *s) {
	cout << "EEK, parse error on line " << line_num << "! Message: " << s << endl;
	
	// Halt the program
	exit(-1);
}