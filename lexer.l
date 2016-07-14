%option noyywrap

%{
	#include "parser.tab.h"
	#define YY_DECL extern "C" int yylex()
	
	int line_num = 1;
%}

%x COMMENTS

%%

[\t]           			;
[\{\}:;=,]				{ return yytext[0]; }
[0-9]+\.[0-9]+ 			{ yylval.fval = atof(yytext); return FLOAT; }
[0-9]+            		{ yylval.ival = atoi(yytext); return INT; }

"zones"					{ return ZONES; }
"rooms"					{ return ROOMS; }
	
"start"					{ return START; }
"stop"	     			{ return STOP; }
"horizon"				{ return HORIZON; }
"time_step"				{ return TIMESTEP; }
"control"	 			{ return CONTROL; }
	
"h_eff" 				{ return HEFF; }
"c_eff" 				{ return CEFF; }
	
"C"						{ return C; }
"C_SPOT"				{ return CSPOT; }
"alpha_o"				{ return ALPHAO; }
"alpha_r"				{ return ALPHAR; }
"Q_l"					{ return QL; }
"Q_h"					{ return QH; }
"Q_s"					{ return QS; }
"fan_coef" 				{ return FANCOEF; }
"density"				{ return DENSITY; }
"sp_heat"				{ return SPHEAT; }
"p1"					{ return P1; }
"p2" 					{ return P2; }
"p3" 					{ return P3; }
"p4"	 				{ return P4; }
	
"input_wthr"			{ return INWFILE; }
"input_occ"				{ return INOFILE; }
"output"				{ return OUTFILE; } 

"b_params" 				{ return BPARAMS; }
"t_ext" 				{ return TEXT; }

[a-zA-Z0-9]+    		{ yylval.sval = strdup(yytext); return STRING; }
[\n]					{ ++line_num; return ENDL; }

\/\*                    { // start of a comment: go to a 'COMMENTS' state.
	BEGIN(COMMENTS);
	}
<COMMENTS>\*\/    {   // end of a comment: go back to normal parsing.
	BEGIN(INITIAL);
	}
<COMMENTS>\n      { ++line_num; }   // still have to increment line numbers inside of comments!
<COMMENTS>.       ;   // ignore every other character while we're in this state

%%

