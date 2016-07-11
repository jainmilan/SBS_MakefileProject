%option noyywrap

%{
	#include "parser.tab.h"
	
	#define YY_DECL extern "C" int yylex()
	
	int line_num = 1;
%}

%%

[\t]           		;
[\{\}:;=,]			{ return yytext[0]; }
[0-9]+\.[0-9]+ 		{ yylval.fval = atof(yytext); return FLOAT; }
[0-9]+            	{ yylval.ival = atoi(yytext); return INT; }

"zones"				{ return ZONES; }
"rooms"				{ return ROOMS; }

"start"				{ return START; }
"stop"	     		{ return STOP; }
"horizon"			{ return HORIZON; }
"timestep" 			{ return TIMESTEP; }
"control" 			{ return CONTROL; }

"heff" 				{ return HEFF; }
"ceff" 				{ return CEFF; }

"C"					{ return C; }
"CSPOT"				{ return CSPOT; }
"alphao"			{ return ALPHAO; }
"alphar"			{ return ALPHAR; }
"Ql"				{ return QL; }
"Qh"				{ return QH; }
"Qs"				{ return QS; }
"fancoef" 			{ return FANCOEF; }
"density"			{ return DENSITY; }
"spheat"			{ return SPHEAT; }
"p1"				{ return P1; }
"p2" 				{ return P2; }
"p3" 				{ return P3; }
"p4" 				{ return P4; }

"bparams" 			{ return BPARAMS; }
"text" 				{ return TEXT; }

[a-zA-Z0-9]+    	{ yylval.sval = strdup(yytext); return STRING; }
[\n]				{ ++line_num; return ENDL; }

%%

