
%{
    #include <string.h>
    #include "parser.h"
%}

%option noyywrap

WHITESPACE          [ \n\t]+

SEMICOLON           ;
COMMA               ,

OPEN_BLOCK          "{"
CLOSE_BLOCK         "}"
OPEN_PAREN          "("
CLOSE_PAREN         ")"

OPEN_ARR            "["
CLOSE_ARR           "]"
MEMBER_SELECTOR     "."

ASSIGN              ==
ADD                 "+"
SUBTRACT            -
MULTIPLY            "*"
DIVIDE              "/"
MOD                 "%"

INCREMENT           "+""+"
DECREMENT           --

SHIFT_LEFT          <<
SHIFT_RIGHT         >>
BIT_AND             &
BIT_OR              "|"
BIT_XOR             "^"

ADD_ASSIGN          "+"=
SUBTRACT_ASSIGN     -=
MULTIPLY_ASSIGN     "*"=
DIVIDE_ASSIGN       "/"=
MOD_ASSIGN          "%"=
SHIFT_LEFT_ASSIGN   <<=
SHIFT_RIGHT_ASSIGN  >>=
BIT_AND_ASSIGN      &=
BIT_OR_ASSIGN       "|"=
BIT_XOR_ASSIGN      "^"=

EQUAL               ==
LESS_THAN           <
GREATER_THAN        >
LESS_THAN_EQUAL     <=
GREATER_THAN_EQUAL  >=
NOT_EQUAL           !=

AND                 &&
OR                  "|""|"
NOT                 !
QUESTION            "?"
COLON               :

TYPE            void|byte|short|int|long|float|double
IDENTIFIER      [_a-zA-Z][_a-zA-Z0-9]*

%%

{SEMICOLON} { printf ("Found SEMICOLON \"%s\"\n", yytext); }
{COMMA} { printf ("Found COMMA \"%s\"\n", yytext); }

{OPEN_BLOCK} { printf ("Found OPEN_BLOCK \"%s\"\n", yytext); }
{CLOSE_BLOCK} { printf ("Found CLOSE_BLOCK \"%s\"\n", yytext); }
{OPEN_PAREN} { printf ("Found OPEN_PAREN \"%s\"\n", yytext); }
{CLOSE_PAREN} { printf ("Found CLOSE_PAREN \"%s\"\n", yytext); }

{OPEN_ARR} { printf ("Found OPEN_ARR \"%s\"\n", yytext); }
{CLOSE_ARR} { printf ("Found CLOSE_ARR \"%s\"\n", yytext); }
{MEMBER_SELECTOR} { printf ("Found MEMBER_SELECTOR \"%s\"\n", yytext); }

{ASSIGN} { printf ("Found ASSIGN \"%s\"\n", yytext); }
{ADD} { printf ("Found ADD \"%s\"\n", yytext); }
{SUBTRACT} { printf ("Found SUBTRACT \"%s\"\n", yytext); }
{MULTIPLY} { printf ("Found MULTIPLY \"%s\"\n", yytext); }
{DIVIDE} { printf ("Found DIVIDE \"%s\"\n", yytext); }
{MOD} { printf ("Found MOD \"%s\"\n", yytext); }

{INCREMENT} { printf ("Found INCREMENT \"%s\"\n", yytext); }
{DECREMENT} { printf ("Found DECREMENT \"%s\"\n", yytext); }

{SHIFT_LEFT} { printf ("Found SHIFT_LEFT \"%s\"\n", yytext); }
{SHIFT_RIGHT} { printf ("Found SHIFT_RIGHT \"%s\"\n", yytext); }
{BIT_AND} { printf ("Found BIT_AND \"%s\"\n", yytext); }
{BIT_OR} { printf ("Found BIT_OR \"%s\"\n", yytext); }
{BIT_XOR} { printf ("Found BIT_XOR \"%s\"\n", yytext); }

{ADD_ASSIGN} { printf ("Found ADD_ASSIGN \"%s\"\n", yytext); }
{SUBTRACT_ASSIGN} { printf ("Found SUBTRACT_ASSIGN \"%s\"\n", yytext); }
{MULTIPLY_ASSIGN} { printf ("Found MULTIPLY_ASSIGN \"%s\"\n", yytext); }
{DIVIDE_ASSIGN} { printf ("Found DIVIDE_ASSIGN \"%s\"\n", yytext); }
{MOD_ASSIGN} { printf ("Found MOD_ASSIGN \"%s\"\n", yytext); }
{SHIFT_LEFT_ASSIGN} { printf ("Found SHIFT_LEFT_ASSIGN \"%s\"\n", yytext); }
{SHIFT_RIGHT_ASSIGN} { printf ("Found SHIFT_RIGHT_ASSIGN \"%s\"\n", yytext); }
{BIT_AND_ASSIGN} { printf ("Found BIT_AND_ASSIGN \"%s\"\n", yytext); }
{BIT_OR_ASSIGN} { printf ("Found BIT_OR_ASSIGN \"%s\"\n", yytext); }
{BIT_XOR_ASSIGN} { printf ("Found BIT_XOR_ASSIGN \"%s\"\n", yytext); }

{EQUAL} { printf ("Found EQUAL \"%s\"\n", yytext); }
{LESS_THAN} { printf ("Found LESS_THAN \"%s\"\n", yytext); }
{GREATER_THAN} { printf ("Found GREATER_THAN \"%s\"\n", yytext); }
{LESS_THAN_EQUAL} { printf ("Found LESS_THAN_EQUAL \"%s\"\n", yytext); }
{GREATER_THAN_EQUAL} { printf ("Found GREATER_THAN_EQUAL \"%s\"\n", yytext); }
{NOT_EQUAL} { printf ("Found NOT_EQUAL \"%s\"\n", yytext); }

{AND} { printf ("Found AND \"%s\"\n", yytext); }
{OR} { printf ("Found OR \"%s\"\n", yytext); }
{NOT} { printf ("Found NOT \"%s\"\n", yytext); }
{QUESTION} { printf ("Found QUESTION \"%s\"\n", yytext); }
{COLON} { printf ("Found COLON \"%s\"\n", yytext); }

{TYPE}          { printf ("Found type \'%s\'\n", yytext); }
{IDENTIFIER}    { printf ("Found identifier \'%s\'\n", yytext); }

{WHITESPACE}    /* eat up whitespace */
.               { printf ("Unrecognized character \'%s\'\n", yytext); }

<<EOF>>         { yyterminate(); }

%%