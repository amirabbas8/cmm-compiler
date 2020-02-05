%option noyywrap
%option nounput
%{

#include "parser.tab.hpp"

%}

KEYWORD int|else|return|for|void|if|while

IDENTIFIER [_a-zA-Z][_a-zA-Z0-9]*

OPERATOR [.][.][.]|[<>][<>][=]|[-][-]|[+][+]|[|][|]|[#][#]|[&][&]|[+\-*\/<>=!%^|&][=]|[<][<]|[->][>]|[<>&=+\/\-*(){}\[\]\.,%~!?:|^;]


DECIMAL ([1-9][0-9]*)
ZERO ([0])



WHITESPACE [ \t\r\n]+


ALL .

%%

void|int      {yylval.string = new std::string(yytext);  return TOKEN_TYPE; }

[;]		{yylval.string = new std::string(yytext);  return TOKEN_SC; }
[,]		{yylval.string = new std::string(yytext);  return TOKEN_CMA; }
[(]		{yylval.string = new std::string(yytext);  return TOKEN_LP; }
[)]		{yylval.string = new std::string(yytext);  return TOKEN_RP; }
[{]             {yylval.string = new std::string(yytext);  return TOKEN_LCB; }
[}]             {yylval.string = new std::string(yytext);  return TOKEN_RCB; }
[|][|]		{yylval.string = new std::string(yytext);  return TOKEN_LOG_OR; }
[&][&]		{yylval.string = new std::string(yytext);  return TOKEN_LOG_AND; }
[|]		{yylval.string = new std::string(yytext);  return TOKEN_OR; }
[&]		{yylval.string = new std::string(yytext);  return TOKEN_AND; }
[=][=]		{yylval.string = new std::string(yytext);  return TOKEN_EQUALITY_OP; }
[!][=]		{yylval.string = new std::string(yytext);  return TOKEN_EQUALITY_OP; }
([<>][=])|[<>]	{yylval.string = new std::string(yytext);  return TOKEN_REL_OP; }
[*] 		{ yylval.string = new std::string(yytext); return TOKEN_MULT; }
[\/]		{ yylval.string = new std::string(yytext); return TOKEN_DIV; }
[~]		{ yylval.string = new std::string(yytext); return TOKEN_TILDE; }
[!]		{ yylval.string = new std::string(yytext); return TOKEN_NOT; }
[+-]		{ yylval.string = new std::string(yytext);return TOKEN_ADDSUB_OP; }
[=]		{ yylval.string = new std::string(yytext); return TOKEN_EQ; }


if		{yylval.string = new std::string(yytext);  return TOKEN_IF; }
else		{yylval.string = new std::string(yytext);  return TOKEN_ELSE; }
return		{yylval.string = new std::string(yytext);  return TOKEN_RETURN; }
while		{yylval.string = new std::string(yytext);  return TOKEN_WHILE; }
for		{yylval.string = new std::string(yytext);  return TOKEN_FOR; }

{IDENTIFIER}	{ yylval.string = new std::string(yytext); return TOKEN_IDENTIFIER; }

({ZERO}|{DECIMAL})	{ yylval.number = strtol(yytext, NULL, 0); return TOKEN_INT_CONST; }

{WHITESPACE}		{ ; }

.                   { fprintf(stderr, "Invalid token\n"); exit(1); }

%%

void yyerror(char const *s) {
     fprintf (stderr, "Parse error : %s\n", s);
     exit(1);
}
