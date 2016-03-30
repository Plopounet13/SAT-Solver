%{
#include <iostream>
#include <cstdlib>
using namespace std;
#define YY_DECL extern "C" int yylex()

#include "expr.tab.hpp"
%}
%%
[ \t]         ;
[1-9][0-9]*           { yylval.ival = atoi(yytext); return TK_VAR; }
"("         			    { return TK_LPAREN; }
")"             { return TK_RPAREN; }
"\\/"           { return TK_OU; }
"/\\"           { return TK_ET; }
[TF]			{ yylval.ival = (strcmp(yytext,"F")); return TK_BOOL; }
~				{ return TK_TIL; }
-			{ return TK_MNS; }
X				{ return TK_XOR; }
"=>"			{ return TK_IMP; }
"<=>"			{ return TK_EQIV; }
\n            { return TK_BN; }
0				{ return TK_END; }
c[a-zA-Z0-9 ]*\n				{ return TK_COM; }
\nc[a-zA-Z0-9 ]*				{ return TK_COM; }
.               ;
%%
