%{
	// Defined to use STRPTIME on Windows Machine
	#ifndef _XOPEN_SOURCE
		#define _XOPEN_SOURCE
	#endif
	
	// Standard Includes
	#include <ctime>
	#include <string>
	#include <cstdio>
	#include <fstream>
	#include <iostream>
	
	// Custom Includes
	#include "Building.h"
	
	// Declared for parser
	extern FILE *yyin;
	extern int line_num;
	
	extern "C" { 
		int yylex();
		int yyparse();
		void yyerror(const char *s);
	}
	
	// Variables to store data
	time_t start_t, stop_t;
	std::string start_str, stop_str; 
	
	int MIN2SEC = 60;
	int time_step, control_type, horizon, start, stop;	

	// Load defined simulator
	using namespace SimpleBuildingSimulator;

	// Initialize Building
	Building dc;
	
	std::string in_dir = "input/";
	std::string out_dir = "output/";
	
	std::string file_type = ".csv";
%}

%union {
	int ival;
	float fval;
	char *sval;
}

%token ENDL
%token <sval> FILEPATH

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

%token INWFILE
%token INOFILE
%token OUTTFILE
%token OUTFILE

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
	ZONES ':' INT							{ dc.ParamsIn.CommonBuilding.num_zones_ = $3; }
	| ROOMS ':' INT							{ dc.ParamsIn.CommonBuilding.num_rooms_ = $3; }
	| START ':' STRING						{ start_str = std::string($3); }
	| STOP ':' STRING						{ stop_str = std::string($3); }
	| HORIZON ':' INT						{ horizon = $3; }
	| TIMESTEP ':' INT						{ time_step = $3; }
	| CONTROL ':' INT						{ control_type = $3; }
	| HEFF ':' FLOAT						{ dc.ParamsIn.CommonAHU.HeatingEfficiency = $3; }
	| CEFF ':' FLOAT						{ dc.ParamsIn.CommonAHU.CoolingEfficiency = $3; }
	| C ':' INT								{ dc.ParamsIn.CommonRoom.C = $3; }
	| CSPOT ':' INT							{ dc.ParamsIn.CommonRoom.C_ = $3; }
	| ALPHAO ':' FLOAT						{ dc.ParamsIn.CommonRoom.alpha_o = $3; }
	| ALPHAR ':' FLOAT						{ dc.ParamsIn.CommonRoom.alpha_r = $3; }
	| QL ':' FLOAT							{ dc.ParamsIn.CommonRoom.Q_l = $3; }
	| QH ':' FLOAT							{ dc.ParamsIn.CommonRoom.Q_h = $3; }
	| QS ':' FLOAT							{ dc.ParamsIn.CommonRoom.Q_s = $3; }
	| FANCOEF ':' FLOAT						{ dc.ParamsIn.CommonRoom.fan_coef = $3; }
	| DENSITY ':' FLOAT						{ dc.ParamsIn.CommonAir.density = $3; }
	| SPHEAT ':' FLOAT						{ dc.ParamsIn.CommonAir.specific_heat = $3; }
	| P1 ':' FLOAT							{ dc.ParamsIn.PMV_Params.P1 = $3; }
	| P2 ':' FLOAT							{ dc.ParamsIn.PMV_Params.P2 = $3; }
	| P3 ':' FLOAT							{ dc.ParamsIn.PMV_Params.P3 = $3; }
	| P4 ':' FLOAT							{ dc.ParamsIn.PMV_Params.P4 = $3; }
	| BPARAMS ':' FLOAT						{ dc.ParamsIn.CommonErrors.err_bparams = $3; }
	| TEXT ':' FLOAT						{ dc.ParamsIn.CommonErrors.err_text = $3; }
	| INWFILE ':' STRING					{ dc.ParamsIn.Files.weather_file = in_dir + std::string($3) + file_type; }
	| INOFILE ':' STRING					{ dc.ParamsIn.Files.occupancy_file = in_dir + std::string($3) + file_type; }
	| OUTTFILE ':' STRING					{ dc.ParamsIn.Files.merged_data_file = out_dir + std::string($3) + file_type; }
	| OUTFILE ':' STRING					{ dc.ParamsIn.Files.output_file = out_dir + std::string($3) + file_type; }
	| STRING ':' STRING						{ std::cout << $3 << std::endl; }
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
    else			// If correct
    {
		// Open a file handler to open a particular file
		FILE *myfile = fopen(argv[1], "r");
		
		// Make sure file is valid
		if (!myfile) {
			std::cout << "Can't open in.jason!" << std::endl;
			return -1;
		}
		
		// Set flex to read from file instead of STDIN
		yyin = myfile;
		
		// Parse through the input
		do {
			yyparse();
		} while (!feof(yyin));
		
		// Convert string to datetime mentioned by the user
		struct tm tm = {0};
		
		std::cout << start_str << "\t" << stop_str << std::endl;
		if (strptime(start_str.c_str(), "%Y%m%dT%H%M", &tm)) {
			start_t = mktime(&tm);
		}
		
		if (strptime(stop_str.c_str(), "%Y%m%dT%H%M", &tm)) {
			stop_t = mktime(&tm);
		}
		dc.Simulate(start_t, stop_t, time_step, control_type, horizon);

	    return 0;
	}
}

void yyerror(const char *s) {
	std::cout << "EEK, parse error on line " << line_num << "! Message: " << s << std::endl;
	
	// Halt the program
	exit(-1);
}