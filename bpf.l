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
 * RCS_ID: $Header: /home/criswell/cvs/bpfa/bpf.l,v 1.14 2003/12/31 03:17:01 criswell Exp $
 */

#include <string.h>

#include "ins.h"
#include "bpf.tab.h"

int lineno=0;
%}

%s QUOTE

%%
#.*\n		{lineno++;}
[ \t]+		{;}
^\n		{lineno++;}
^[ \t]+\n	{lineno++;}
\n		{lineno++; return Endl;}

,X		{return XIndex;}
[,]		{;}

struct|STRUCT	{return STRUCT;}
ldaw|LDAW	{return LDAW;}
ldah|LDAH	{return LDAH;}
ldab|LDAB	{return LDAB;}
ldai|LDAI	{return LDAI;}
ldal|LDAL	{return LDAL;}
lda|LDA		{return LDA;}

ldx|LDX		{return LDX;}
ldxi|LDXI	{return LDXI;}
ldxl|LDXL	{return LDXL;}
ldxipv4len|LDXIPV4LEN	{return LDXIPV4LEN;}

sta|STA	{return STA;}
stx|STX	{return STX;}

add|ADD	{return ADD;}
sub|SUB	{return SUB;}
mul|MUL	{return MUL;}
div|DIV	{return DIV;}
and|AND	{return AND;}
ora|ORA	{return ORA;}
lsh|LSH	{return LSH;}
rsh|RSH	{return RSH;}
neg|NEG	{return NEG;}

jmp|JMP		{return JMP;}
bgt|BGT		{return BGT;}
bge|BGE		{return BGE;}
blt|BLT		{return BLT;}
ble|BLE		{return BLE;}
beq|BEQ		{return BEQ;}
bne|BNE		{return BNE;}
bset|BSET	{return BSET;}
bnset|BNSET	{return BNSET;}

ret|RET	{return RET;}

txa|TXA	{return TXA;}
tax|TAX	{return TAX;}

%[a-zA-Z_0-9]+	{yylval.value = strdup (yytext); return Label;}
[A-Za-z_]+	{yylval.value = strdup (yytext); return Value;}
-?[0-9]+	{yylval.value = strdup (yytext); return Value;}
(0x)?[a-fA-F0-9]+	{yylval.value = strdup (yytext); return Value;}

\[		{BEGIN QUOTE;}
<QUOTE>\]	{BEGIN 0;}
<QUOTE>[^\]]+ 	{yylval.value = strdup (yytext); return Value;}

%%

