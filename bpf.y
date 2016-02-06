%{
/*
 * Copyright (c) 2003, John T Criswell
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the contributors may be used to
 *       endorse or promote products derived from this software without
 *       specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * RCS_ID: $Header: /home/criswell/cvs/bpfa/bpf.y,v 1.14 2003/12/31 02:59:07 criswell Exp $
 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>

#include "ins.h"
#include "symtable.h"

/* Line number */
extern int lineno;

/* Head of the linked list of instructions */
struct instruction * inslist = NULL;

/* The name of the program being compiled */
char * progname = "program";

/* Function Prototypes */
struct symbol * create_symbol_table (struct instruction * code);
int resolve_symbols (struct instruction * program, struct symtable * symtable);
struct symbol * symlookup (struct symtable * symtable, char * name);
extern int yylex();
extern int yyerror();
%}

%union
{
	/* Used for operand values which we will not validate */
	char * value;

	/* Marks which opcode we're using */
	struct instruction * opcode;
}

%token <value> Label XIndex
%token <value> Value
%token <opcode> LDAW LDAH LDAB LDAI LDAL LDA
%token <opcode> LDX LDXI LDXL LDXIPV4LEN
%token <opcode> STA STX
%token <opcode> ADD SUB MUL DIV AND ORA LSH RSH NEG
%token <opcode> JMP BGT BGE BLT BLE BEQ BNE BSET BNSET
%token <opcode> RET
%token <opcode> TAX TXA
%token <opcode> STRUCT
%token <opcode> ENDSTRUCT
%token <opcode> VAR

%type <opcode> Load Store Branch Alu Instruction Statement StatementList Program

%token Endl;

%%
Program:	STRUCT Value Endl StatementList	ENDSTRUCT {$$ = $4; progname = $2;}
		| STRUCT Value Endl StatementList	ENDSTRUCT Endl {$$ = $4; progname = $2;}
		;

StatementList:	Statement			{inslist = $$ = $1;}
		| StatementList Statement	{$1->next = $2; $$ = $2;}
		;

Statement:	Instruction Endl		{$$ = $1;}
		| Label Instruction Endl	{$$ = $2; $$->label = $1;}
		| VAR Label Endl	{MKINS($$,"VAR",$2);}
		;

Instruction:	Load		{$$ = $1;}
		| Store		{$$ = $1;}
		| Branch	{$$ = $1;}
		| Alu		{$$ = $1;}
		;

Load:		LDA Label		{MKINS($$,"BPF_LD+BPF_MEM",$2);}
		| LDAW Value		{MKINS($$,"BPF_LD+BPF_W+BPF_ABS",$2);}
		| LDAH Value		{MKINS($$,"BPF_LD+BPF_H+BPF_ABS",$2);}
		| LDAB Value		{MKINS($$,"BPF_LD+BPF_B+BPF_ABS",$2);}
		| LDAW Value XIndex	{MKINS($$,"BPF_LD+BPF_W+BPF_IND",$2);}
		| LDAH Value XIndex	{MKINS($$,"BPF_LD+BPF_H+BPF_IND",$2);}
		| LDAB Value XIndex	{MKINS($$,"BPF_LD+BPF_B+BPF_IND",$2);}
		| LDAI Value		{MKINS($$,"BPF_LD+BPF_IMM",$2);}
		| LDAL			{MKINS($$,"BPF_LD+BPF_W+BPF_LEN",NULL);}
		| LDX Label		{MKINS($$,"BPF_LDX+BPF_W_BPF_MEM",$2);}
		| LDXI Value		{MKINS($$,"BPF_LDX+BPF_W_BPF_IMM",$2);}
		| LDXL 			{MKINS($$,"BPF_LDX+BPF_W+BPF_LEN",NULL);}
		| LDXIPV4LEN 		{MKINS($$,"BPF_LDX+BPF_B+BPF_MSH","14");}
		| TAX 			{MKINS($$,"BPF_MISC+BPF_TAX",NULL);}
		| TXA 			{MKINS($$,"BPF_MISC+BPF_TXA",NULL);}
		;

Store:		STA Label		{MKINS($$,"BPF_ST",$2);}
		| STX Label		{MKINS($$,"BPF_STX",$2);}
		;

Branch:		JMP Label		{MKINS($$,"BPF_JMP+BPF_JA",$2);}
		| BGT Value Label Label	{MKBR($$,"BPF_JMP+BPF_JGT+BPF_K",$2, $3, $4);}
		| BGE Value Label Label	{MKBR($$,"BPF_JMP+BPF_JGE+BPF_K",$2, $3, $4);}
		| BLT Value Label Label	{MKBR($$,"BPF_JMP+BPF_JGE+BPF_K",$2, $4, $3);}
		| BLE Value Label Label	{MKBR($$,"BPF_JMP+BPF_JGT+BPF_K",$2, $4, $3);}
		| BEQ Value Label Label	{MKBR($$,"BPF_JMP+BPF_JEQ+BPF_K",$2, $3, $4);}
		| BNE Value Label Label	{MKBR($$,"BPF_JMP+BPF_JEQ+BPF_K",$2, $4, $3);}
		| BSET Value Label Label	{MKBR($$,"BPF_JMP+BPF_JSET+BPF_K",$2, $3, $4);}
		| BNSET Value Label Label	{MKBR($$,"BPF_JMP+BPF_JSET+BPF_K",$2, $4, $3);}
		| BGT Label Label	{MKBR($$,"BPF_JMP+BPF_JGT+BPF_X",0, $2, $3);}
		| BGE Label Label	{MKBR($$,"BPF_JMP+BPF_JGE+BPF_X",0, $2, $3);}
		| BLT Label Label	{MKBR($$,"BPF_JMP+BPF_JGE+BPF_X",0, $3, $2);}
		| BLE Label Label	{MKBR($$,"BPF_JMP+BPF_JGT+BPF_X",0, $3, $2);}
		| BEQ Label Label	{MKBR($$,"BPF_JMP+BPF_JEQ+BPF_X",0, $2, $3);}
		| BNE Label Label	{MKBR($$,"BPF_JMP+BPF_JEQ+BPF_X",0, $3, $2);}
		| BSET Label Label	{MKBR($$,"BPF_JMP+BPF_JSET+BPF_X",0, $2, $3);}
		| BNSET Label Label	{MKBR($$,"BPF_JMP+BPF_JSET+BPF_X",0, $3, $2);}
		| RET Value		{MKINS($$,"BPF_RET+BPF_K",$2);}
		| RET 			{MKINS($$,"BPF_RET+BPF_A",NULL);}
		;

Alu:		ADD		{MKINS($$,"BPF_ALU+BPF_ADD+BPF_X",NULL);}
		| SUB		{MKINS($$,"BPF_ALU+BPF_SUB+BPF_X",NULL);}
		| MUL		{MKINS($$,"BPF_ALU+BPF_MUL+BPF_X",NULL);}
		| DIV		{MKINS($$,"BPF_ALU+BPF_DIV+BPF_X",NULL);}
		| AND		{MKINS($$,"BPF_ALU+BPF_AND+BPF_X",NULL);}
		| ORA		{MKINS($$,"BPF_ALU+BPF_ORA+BPF_X",NULL);}
		| LSH		{MKINS($$,"BPF_ALU+BPF_LSH+BPF_X",NULL);}
		| RSH		{MKINS($$,"BPF_ALU+BPF_RSH+BPF_X",NULL);}
		| NEG		{MKINS($$,"BPF_ALU+BPF_NEG",NULL);}
		| ADD Value	{MKINS($$,"BPF_ALU+BPF_ADD+BPF_K",$2);}
		| SUB Value	{MKINS($$,"BPF_ALU+BPF_SUB+BPF_K",$2);}
		| MUL Value	{MKINS($$,"BPF_ALU+BPF_MUL+BPF_K",$2);}
		| DIV Value	{MKINS($$,"BPF_ALU+BPF_DIV+BPF_K",$2);}
		| AND Value	{MKINS($$,"BPF_ALU+BPF_AND+BPF_K",$2);}
		| ORA Value	{MKINS($$,"BPF_ALU+BPF_ORA+BPF_K",$2);}
		| LSH Value	{MKINS($$,"BPF_ALU+BPF_LSH+BPF_K",$2);}
		| RSH Value	{MKINS($$,"BPF_ALU+BPF_RSH+BPF_K",$2);}
		;

%%

int
yywrap ()
{
	return 1;
}

int
yyerror ()
{
	fprintf (stderr, "Error: Line number %d\n", lineno);
	return 0;
}

