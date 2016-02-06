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
 * RCS_ID: $Header: /home/criswell/cvs/bpfa/ins.h,v 1.4 2003/12/19 03:09:04 criswell Exp $
 */

#ifndef _INS_H
#define _INS_H

#include <stdlib.h>

#ifdef _cplusplus
extern C {
#endif

/*
 * Structure: Instruction
 *
 * Description:
 *	This structure describes a single BPF instruction.  A list of these
 *	will describe an entrie program, and will be scanned (i.e. a pass)
 *	to generate the desired BPF header file.
 */
struct instruction
{
	/* Flags whether the instruction is a branch or statement */
	unsigned char statement;

	/* A pointer to the label */
	char * label;

	/* The opcode as a string value */
	char * opcode;

	/* A pointer to the operand. */
	char * operand;

	/* True branch location */
	char * true_branch;

	/* False branch location */
	char * false_branch;

	/* The next instruction in the linked list */
	struct instruction * next;
};

/*
 * Function: mkins()
 *
 * Description:
 *	This function allocates and initializes an instruction structure.  It is
 *	mainly used to save screen real-estate in the Yacc source code.
 *
 * Inputs:
 *  op    - A pointer to a string representing the opcode of the instruction.
 *  value - A pointer to a string representing the operand of the instruction.
 *
 * Return value:
 *  NULL - No new instruction could be allocated.
 *  Otherwise, a pointer to the newly allocated instruction is returned.
 */
static inline struct instruction *
mkins (char * op, char * value)
{
	/* Pointer to the new instruction */
	struct instruction * p;

	/*
	 * Allocate a new instruction.
	 */
	p = malloc (sizeof (struct instruction));
	if (p)
	{
		p->statement = (1);
		p->opcode = op;
		p->operand = value;
		p->next = NULL;
		p->label = NULL;
		p->true_branch = p->false_branch = NULL;
	}

	return p;
}

/*
 * Function: mkbr()
 *
 * Description:
 *	This function creates a branch instruction.  It takes the two locations
 *	for branching and the operand and generates the proper branch
 *	instruction.
 *
 * Inputs:
 *  op    - A pointer to a string representing the opcode of the instruction.
 *  value - A pointer to a string representing the operand of the instruction.
 *
 * Return value:
 *  NULL - No new instruction could be allocated.
 *  Otherwise, a pointer to the newly allocated instruction is returned.
 */
static inline struct instruction *
mkbr (char * op, char * value, char * true_branch, char * false_branch)
{
	/* Pointer to the new instruction */
	struct instruction * p;

	/*
	 * Allocate a new instruction.
	 */
	p = malloc (sizeof (struct instruction));
	if (p)
	{
		p = malloc (sizeof (struct instruction));
		p->statement = 0;
		p->opcode = op;
		p->operand = value;
		p->next = NULL;
		p->label = NULL;
		p->true_branch = true_branch;
		p->false_branch = false_branch;
	}

	return p;
}

#ifdef _cplusplus
};
#endif

#endif
