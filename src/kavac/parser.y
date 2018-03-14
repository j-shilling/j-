%{
    #include <stdio.h>

    int yylex (void);
    void yyerror (const char *s);
%}

%define parse.error verbose

%union
{
    char *string;
}

%token TOK_FUNCTION_DEFINITION

%%

function_definition:
    TOK_FUNCTION_DEFINITION     { printf ("Found function: %s\n", $<string>1); }
;

%%

void
yyerror (const char *s)
{
  fprintf (stderr, "%s\n", s);
}