%code requires{

#include "mipser.hpp"
extern MipserRoot *mipserRoot;

int yylex(void);
void yyerror(const char *);

}


%union{
    const Base *stmnt;
    int number;
    std::string *string;
}
                        
%token TOKEN_INT_CONST
%token TOKEN_IF TOKEN_WHILE TOKEN_FOR TOKEN_RETURN
%token TOKEN_TYPE TOKEN_IDENTIFIER
%token TOKEN_SC TOKEN_CMA TOKEN_LP TOKEN_LCB TOKEN_RCB
%token TOKEN_LOG_OR TOKEN_LOG_AND TOKEN_OR TOKEN_AND
%token TOKEN_EQUALITY_OP TOKEN_REL_OP TOKEN_MULT
%token TOKEN_DIV TOKEN_TILDE TOKEN_NOT TOKEN_ADDSUB_OP
%token TOKEN_EQ
%nonassoc TOKEN_RP
%nonassoc TOKEN_ELSE
			
                        
%type <stmnt> Start StartDeclaration
			
%type <stmnt> FuncDef ParameterList Parameter
			
%type <stmnt> DeclarationList Declaration  StartDeclarator StartDeclaratorList
			
%type <stmnt> StatementList Statement ScopeStatement ScopeStatementEnd IfStatement ExpressionStatement ReturnStatement LoopStatement
			
%type <stmnt> Expression AssignmentExpression LogicalOrExpression LogicalAndExpression InclusiveOrExpression
%type <stmnt> AndExpression EqualityExpression RelationalExpression
%type <stmnt> AdditiveExpression MultiplicativeExpression PostfixExpression PostfixExpressionEnd ArgumentExpressionList
%type <stmnt> PrimaryExpression UnaryExpression

			
%type <number> Constant TOKEN_INT_CONST

			
%type <string> Declarator TOKEN_IDENTIFIER MultDivRemOP TOKEN_EQ TOKEN_AND TOKEN_ADDSUB_OP TOKEN_TILDE TOKEN_NOT TOKEN_MULT TOKEN_DIV TOKEN_LOG_OR TOKEN_LOG_AND TOKEN_OR TOKEN_EQUALITY_OP TOKEN_REL_OP TOKEN_TYPE
                        
%start ROOT
                        
%%

ROOT:
	        Start { ; }
		;


Start:
		StartDeclaration { mipserRoot->push($1); }
        |       Start StartDeclaration { mipserRoot->push($2); }
		;

StartDeclaration:
		Declaration { $$ = $1; }
        |       FuncDef { $$ = $1; }
		;

// FUNCTION

FuncDef:
		TOKEN_TYPE TOKEN_IDENTIFIER TOKEN_LP ParameterList TOKEN_RP ScopeStatement { $$ = new Function(*$2, $4, $6,*$1); }
		;

ParameterList:
		/* empty */  { $$ = new ParamList(); }
	| 	Parameter { $$ = new ParamList($1); }
	|       ParameterList TOKEN_CMA Parameter { $$->push($3); }
		;

Parameter:
		TOKEN_TYPE TOKEN_IDENTIFIER { $$ = new Parameter(*$2,*$1);}
		;


// Declaration

DeclarationList:
		Declaration { $$ = new DeclarationList($1); }
	|	DeclarationList Declaration { $$->push($2); }
		;

Declaration:
		TOKEN_TYPE StartDeclaratorList TOKEN_SC { $$ = $2; }
		;


StartDeclaratorList:
		StartDeclarator { $$ = new VariableDeclaration($1); }
	|       StartDeclaratorList TOKEN_CMA StartDeclarator { $$->push($3); }
		;

StartDeclarator:
		Declarator { $$ = new Variable(*$1); }
	|	Declarator TOKEN_EQ AssignmentExpression { $$ = new Variable(*$1,$3); }
		;

Declarator:
		TOKEN_IDENTIFIER {$$=$1; }
		;

// Statement

StatementList:
		Statement { $$ = new Statement($1); }
	|	StatementList Statement { $$->push($2); }
		;

Statement:
		ScopeStatement { $$ = $1; }
	|	IfStatement { $$ = $1; }
	|	ExpressionStatement { $$ = $1; }
	|       ReturnStatement { $$ = $1; }
	|	LoopStatement { $$ = $1; }
		;

ScopeStatement:
		TOKEN_LCB ScopeStatementEnd { $$ = $2; }
		;

ScopeStatementEnd:
		TOKEN_RCB { $$ = new ScopeStatement; }
	|	DeclarationList TOKEN_RCB { $$ = new ScopeStatement($1); }
	|	DeclarationList StatementList TOKEN_RCB { $$ = new ScopeStatement($1, $2); }
	|	StatementList TOKEN_RCB { $$ = new ScopeStatement($1); }
		;

IfStatement:
		TOKEN_IF TOKEN_LP Expression TOKEN_RP Statement { $$ = new IfStatement($3, $5); }
	|	TOKEN_IF TOKEN_LP Expression TOKEN_RP Statement TOKEN_ELSE Statement { $$ = new IfStatement($3, $5, $7); }
		;

ExpressionStatement:
		TOKEN_SC { $$ = new Statement(); }
	|	Expression TOKEN_SC { $$ = $1; }
		;

ReturnStatement:
		TOKEN_RETURN ExpressionStatement { $$ = new ReturnStatement($2); }
		;

LoopStatement:
		TOKEN_WHILE TOKEN_LP Expression TOKEN_RP Statement { $$ = new WhileStatement($3,$5); }
	|	TOKEN_FOR TOKEN_LP Expression TOKEN_SC Expression TOKEN_SC Expression TOKEN_RP Statement { $$ =new ForStatement($3, $5, $7, $9); }
		;

// Expressions

Expression:
		AssignmentExpression { $$ = $1; }
		;

AssignmentExpression:
		LogicalOrExpression { $$ = $1; }
	|	PostfixExpression TOKEN_EQ AssignmentExpression { $$ = new Expression($1, $3,*$2); }
		;


LogicalOrExpression:
		LogicalAndExpression { $$ = $1; }
	|	LogicalOrExpression TOKEN_LOG_OR LogicalAndExpression { $$ = new Expression($1, $3,*$2); }
		;

LogicalAndExpression:
		InclusiveOrExpression { $$ = $1; }
	|	LogicalAndExpression TOKEN_LOG_AND InclusiveOrExpression { $$ = new Expression($1, $3,*$2); }
		;

InclusiveOrExpression:
		AndExpression { $$ = $1; }
	|	InclusiveOrExpression TOKEN_OR AndExpression { $$ = new Expression($1, $3,*$2); }
		;


AndExpression:
		EqualityExpression { $$ = $1; }
	|	AndExpression TOKEN_AND EqualityExpression { $$ = new Expression($1, $3,*$2); }
		;

EqualityExpression:
	        RelationalExpression { $$ = $1; }
	|	EqualityExpression TOKEN_EQUALITY_OP RelationalExpression { $$ = new Expression($1, $3,*$2); }
		;

RelationalExpression:
		AdditiveExpression { $$ = $1; }
	|       RelationalExpression TOKEN_REL_OP AdditiveExpression { $$ = new Expression($1, $3,*$2); }
		;


AdditiveExpression:
		MultiplicativeExpression { $$ = $1; }
	|	AdditiveExpression TOKEN_ADDSUB_OP MultiplicativeExpression { $$ = new Expression($1, $3,*$2); }
		;

MultiplicativeExpression:
		UnaryExpression { $$ = $1; }
	|	MultiplicativeExpression MultDivRemOP UnaryExpression { $$ = new Expression($1, $3,*$2); }
		;

MultDivRemOP:
		TOKEN_MULT { $$ = $1; }
	|	TOKEN_DIV { $$ = $1; }
		;

UnaryExpression:
		PostfixExpression { $$ = $1; }
	|	TOKEN_TILDE PostfixExpression { $$ = new Expression(0, $2, *$1); }
	|	TOKEN_NOT PostfixExpression { $$ = new Expression(0, $2, *$1); }

PostfixExpression:
		PrimaryExpression { $$ = $1; }
	|	PostfixExpression TOKEN_LP PostfixExpressionEnd { const ParamList *pl=(const ParamList *)$3;pl->funcName=$1->key;$$ = new Expression($1, pl,"func"); }
		;

PostfixExpressionEnd:
		TOKEN_RP { $$ = new ParamList(true); }
	|	ArgumentExpressionList TOKEN_RP { $$ = $1; }
		;

ArgumentExpressionList:
		AssignmentExpression { $$ = new ParamList($1,true); }
	|	ArgumentExpressionList TOKEN_CMA AssignmentExpression { $$->push($3); }
		;

PrimaryExpression:
		TOKEN_IDENTIFIER { $$ = new Expression(*$1); }
	|       Constant { $$ = new Expression($1); }
	|	TOKEN_LP Expression TOKEN_RP { $$ = $2; }
		;

Constant:
		TOKEN_INT_CONST { $$ = $1; }
		;

%%

MipserRoot *mipserRoot;

MipserRoot *parseMIPSER(bool isShow) {
    mipserRoot = new MipserRoot(isShow);
    yyparse();
    return mipserRoot;
}
