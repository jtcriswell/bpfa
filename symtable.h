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
 * RCS_ID: $Header: /home/criswell/cvs/bpfa/symtable.h,v 1.1 2003/07/23 03:11:35 criswell Exp $
 */

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

/*
 * Structure: symbol
 *
 * Description:
 *	This structure defines everything that we need to know about an
 *	individual symbol.  It can be used in an array to create a symbol
 *	table.
 */
struct symbol
{
	/* The name of the symbol */
	char * symname;

	/* The index of the symbol into the BPF program */
	int symoffset;
};

/*
 * Structure: symtable
 *
 * Description:
 *	This structure represents a symbol table.  It is basically a collection
 *	of symbol structures, but it also includes additional information
 *	for handling the size of the symbol table.
 */
struct symtable
{
	/* The number of elements in the symbol table */
	int count;

	/* A variable sized array of symbols */
	struct symbol * symbols;
};
#endif /* _SYMTABLE_H */

