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
 * RCS_ID: $Header: /home/criswell/cvs/bpfa/bpfa.c,v 1.15 2003/12/31 01:48:10 criswell Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include "bpf.tab.h"
#include "ins.h"
#include "symtable.h"
#include "bpf.h"

/* Function Prototypes */
struct symtable * create_symbol_table (struct instruction * code);
static int resolve_symbols (struct instruction * p, struct symtable * symtable);
static struct symbol * symlookup (struct symtable * symtable, char * name);
static int genprogram (FILE * file, char * pname, struct instruction * inslist);
static int fprintins (FILE * out, struct instruction * inp);

extern int yyparse ();
extern struct instruction * inslist;
extern char * progname;

extern FILE * yyin;

/*
 * Function: main()
 *
 * Description:
 *	This is where program execution begins.
 */
int
main (int argc, char ** argv)
{
	/* Pointer for traversing the parsed BPF program */
	struct instruction * p;

	/* Index for traversing the symbol table */
	int index;

	/* The symbol table */
	struct symtable * symtable;

	/* Output file */
	FILE * output;

	/* Option name */
	char opt;

	/*
	 * Get our input from standard input.
	 */
	yyin = stdin;
	output = stdout;

	/*
	 * Parse our command line inputs.
	 */
	while ((opt = (char) getopt (argc, argv, "Vo:")) != (char) EOF)
	{
		switch (opt)
		{
			case 'V':
				printf ("BPF Assembler Version %s\n", VERSION);
				printf ("%s\n", COPYRIGHT);
				exit (0);

			case 'o':
				output = fopen (optarg, "w+");
				if (output == NULL)
				{
					fprintf (stderr, "Failed to open %s for writing\n", optarg);
					exit (1);
				}
				break;

			case '?':
        fprintf (stderr, "Usage:\n");
        fprintf (stderr, "\t%s [options] <input>\n\n", argv[0]);
        fprintf (stderr, "\tOptions:\n\n");
        fprintf (stderr, "\t-V: Print version and exit\n");
        fprintf (stderr, "\t-o <file>: Write output to file\n");
        exit (1);
				break;
		}
	}

	/*
	 * All of the command line options have been parsed.  Determine if
	 * we've got an input filename to use.
	 */
	if (optind < argc)
	{
		if ((yyin = fopen (argv[optind], "r")) == NULL)
		{
			fprintf (stderr, "Failed to open input file %s\n", argv[optind]);
			exit (2);
		}
	}

	/*
	 * While we can keep reading, parse the input.
	 */
	while (!feof (yyin))
	{
		yyparse ();
	}

	/*
	 * Next, create the symbol table.
	 */
	if ((symtable = create_symbol_table (inslist)) == NULL)
	{
		exit (1);
	}

	/*
	 * Resolve all of the labels within the program.
	 */
	if (resolve_symbols (inslist, symtable))
	{
		/*
		 * Generate the output for this program if all symbols were
		 * resolved.
		 */
		genprogram (output, progname, inslist);
	}

	return 0;
}

/*
 * Function: genprogram()
 *
 * Description:
 *	Generates an output C structure containing the BPF program.
 *
 *
 * Inputs:
 *	file     - A C file pointer to the output file.
 *	progname - The name of the structure to generate.
 *	inslist  - A linked list of instructions composing the BPF program.
 *
 * Outputs:
 *	None.
 *
 * Return value:
 *	0  = Success
 *	!0 = Error
 */
static int
genprogram (FILE * file, char * progname, struct instruction * inslist)
{
	/* Pointer for scanning the program */
	register struct instruction * p;

	/* The number of instructions in the program */
	register int length = 0;

	/*
	 * Determine the number of instructions in the program.
	 */
	for (p = inslist; p != NULL; p=p->next)
	{
		/*
		 * Do not count variables as statements.
		 */
		if ((strcmp (p->opcode, "VAR")))
			length++;
	}

	/*
	 * First, create all of the necessary header stuff that we usually
	 * need.
	 */
	fprintf (file, "/*\n");
	fprintf (file, " * This file was generated by the BPF Assember.\n");
	fprintf (file, " * Avoid hand editing this file if possible.\n");
	fprintf (file, " */\n\n");
	fprintf (file, "#include <pcap.h>\n\n");
	fprintf (file, "#ifndef %s_H\n", progname);
	fprintf (file, "#define %s_H\n\n", progname);
	fprintf (file, "static struct bpf_insn %s_array [] = \n{\n", progname);

	/*
	 * Second, create the array elements for each BPF instruction.
	 */
	for (p = inslist; p != NULL; p=p->next)
	{
    /*
     * Do not generate instructions for memory variable declarations.
     */
    if ((strcmp (p->opcode, "VAR")) == 0)
    {
      continue;
    }

    /*
     * Generate an array element for this one instruction.
     */
    switch (p->instType)
    {
      case statement:
      case jump:
				fprintf (file, "\tBPF_STMT(%s, %s),\n", p->opcode,
				         (p->operand ? p->operand : "0"));
				break;

      case branch:
				fprintf (file, "\tBPF_JUMP(%s, %s, %s, %s),\n", p->opcode, p->operand,
				         p->true_branch, p->false_branch);
				break;
		}
	}
	fprintf (file, "};\n\n");

	/*
	 * Third, create a struct bpf_program which points to the program.
	 */
	fprintf (file, "static struct bpf_program %s = ", progname);
	fprintf (file, "{%d, &(%s_array[0])};\n\n", length, progname);
	fprintf (file, "#endif\n");
	return 0;
}

/*
 * Function: create_symbol_table()
 *
 * Description:
 *	This function takes a BPF program generated from the parser and
 *	creates a symbol table that defines where all of the labels are.
 *
 * Inputs:
 *	code = A pointer to a symbol table structure.
 *
 * Outputs:
 *	None.
 *
 * Return value:
 *	NULL = An error occured or there are no symbols.
 *	Otherwise, a pointer to an array of symbols is returned.
 *
 */
struct symtable *
create_symbol_table (struct instruction * code)
{
	/* Index variable */
	unsigned int index = 0;

	/* The offset of a symbol from the beginng of the program */
	unsigned int offset;

	/* Next memory location available in the memory */
	unsigned int nextMemory = 0;

	/* The number of labels in the program */
	unsigned int labelcount = 0;

	/* A pointer to scan over the program */
	struct instruction * p;

	/* The new symbol table */
	struct symtable * symtable;

	/*
	 * Allocate and initialize a new symbol table.
	 */
	if ((symtable = malloc (sizeof (struct symtable))) == NULL)
	{
		return NULL;
	}

	symtable->count = 0;
	symtable->symbols = NULL;

	/*
	 * Determine the number of symbols created by this BPF program.
	 */
	for (p=inslist; p != NULL; p=p->next)
	{
    /*
     * A symbol is either a label or a newly declared variable.
     */
		if ((p->label != NULL) || ((strcmp (p->opcode, "VAR")) == 0))
		{
			labelcount++;
		}
	}

	/*
	 * If there are no symbols, return an empty symbol table.
	 */
	if (!labelcount)
	{
		return symtable;
	}

	/*
	 * Allocate a symbol table of the proper size.
	 */
	symtable->count = labelcount;
	if ((symtable->symbols=calloc (labelcount,sizeof(struct symbol)))==NULL)
	{
		free (symtable);
		return NULL;
	}

	/*
	 * Scan through the program again and grab each label and put it in
	 * the symbol table.
	 */
	for (offset=0, p=inslist; p != NULL; offset++, p=p->next)
	{
    /*
     * If this instruction has a label, then add that label to the symbol
     * table.
     */
		if (p->label != NULL)
		{
			/*
			 * Check to see if this symbol is already in the symbol table.  If it
			 * is, then there is a duplicate symbol error that we should report to
			 * the user.
			 */
			for (unsigned checkIndex = 0; checkIndex < index; ++checkIndex)
			{
				if ((strcmp (p->label, symtable->symbols[checkIndex].symname)) == 0)
				{
					fprintf (stderr, "Duplicate symbol %s\n", p->label);
					return NULL;
				}
			}

			/*
			 * Add the symbol to the symbol table.
			 */
			symtable->symbols[index].symname = p->label;
			symtable->symbols[index].type = symInst;
			symtable->symbols[index].symoffset = offset;

			/*
			 * Advance the index to the next available symbol in the array.
			 */
			index++;
		}

		/*
     * If this instruction declares a label for a memory location, create
     * a memory label for the location.
     */
		if ((strcmp (p->opcode, "VAR")) == 0)
		{
			/*
			 * Check to see if this symbol is already in the symbol table.  If it
			 * is, then there is a duplicate symbol error that we should report to
			 * the user.
			 */
			for (unsigned checkIndex = 0; checkIndex < index; ++checkIndex)
			{
				if ((strcmp (p->operand, symtable->symbols[checkIndex].symname)) == 0)
				{
					fprintf (stderr, "Duplicate symbol %s\n", p->operand);
					return NULL;
				}
			}

			/*
			 * Add the symbol to the symbol table.
			 */
			symtable->symbols[index].symname   = p->operand;
			symtable->symbols[index].type      = symMemory;
			symtable->symbols[index].symoffset = (nextMemory++);
			index++;
		}
	}

	/*
	 * Return the pointer to the allocated symbol table.
	 */
	return symtable;
}

/*
 * Function: needToResolve()
 *
 * Description:
 *  Determine if the specified string is a symbol that needs to be resolved.
 *
 * Inputs:
 *	label - The label to check.  It can be NULL.
 *
 * Return values:
 *  0 - The symbol does not need to be resolved.
 *  1 - The symbol needs to be resolved.
 */
static inline unsigned char
needToResolve (char * label)
{
	/*
	 * If the label is NULL, has a zero length, or isn't a symbol,
	 * then it does not need resolution.  Symbols start with a percent sign.
	 */
	if ((label == NULL) || ((strlen (label)) == 0) || (label[0] != '%'))
	{
		return 0;
	}
	return 1;
}

/*
 * Function: resolve_symbols()
 *
 * Description:
 *	This function takes a program and resolves all of the symbols within it.
 *
 * Inputs:
 *	program  - A pointer to a linked list of instructions.
 *	symtable - A pointer to the symbol table.
 *
 * Outputs:
 *	program - The list of instructions is modified so that all of the
 *	          symbols are resolved.
 *
 * Return value:
 *	0 - Not all symbols were resolved.
 *  1 - All symbols were resolved.
 */
static int
resolve_symbols (struct instruction * program, struct symtable * symtable)
{
	/* Pointer for scanning through the program */
	struct instruction * inp;

	/* Index variable */
	unsigned int index = 1;

	/* Pointer to the symbol */
	struct symbol * sym;

	/* Offset of the branch */
	unsigned int offset;

	/* Flags whether all symbols have been resolved */
	unsigned int allResolved = 1;

	/*
	 * Scan through the entire program and resolve the symbols in each
	 * instruction.
	 */
	for (inp = program; inp != NULL; index++, inp=inp->next)
	{
		/*
		 * Resolve the symbol in the instruction's operand if necessary.
		 */
		if ((needToResolve (inp->operand)) && (strcmp (inp->opcode, "VAR")))
		{
			/*
			 * Find the offset of the symbol from the beginning of
			 * the program.
			 */
			if ((sym = symlookup (symtable, inp->operand)) == NULL)
			{
				fprintf (stderr, "Invalid label %s\n",
				         inp->operand);
				fprintins (stderr, inp);
				allResolved = 0;
				continue;
			}

			/*
			 * Ensure that the correct type of symbol is used for this
			 * instruction.  Unconditional jumps should use
			 * instruction labels while loads and stores to memory
			 * should use memory labels defined by the VAR directive.
			 */
			if ((inp->instType == statement) && (sym->type == symInst))
			{
				fprintf (stderr,
				         "Error: Memory access using instruction label %s\n",
								 inp->operand);
				allResolved = 0;
				continue;
			}

			if ((inp->instType == jump) && (sym->type == symMemory))
			{
				fprintf (stderr,
				         "Error: Jump access using memory label %s\n",
								 inp->operand);
				allResolved = 0;
				continue;
			}

			/*
			 * Determine the distance to be covered and place it
			 * into the instruction if it is a label.  Otherwise,
       * use the memory location within the symbol.
			 */
      if (sym->type == symInst)
        sprintf (inp->operand,"%d",sym->symoffset - index);
      else
        sprintf (inp->operand,"%d",sym->symoffset);
		}

		/*
		 * Resolve the symbol in a branch's true branch.
		 */
		if (needToResolve (inp->true_branch))
		{
			/*
			 * Find the offset of the symbol from the beginning of
			 * the program.
			 */
			if ((sym = symlookup (symtable, inp->true_branch)) == NULL)
			{
				fprintf (stderr, "Invalid true branch label %s\n",
				         inp->true_branch);
				fprintins (stderr, inp);
				allResolved = 0;
				continue;
			}

			/*
			 * Ensure that the symbol refers to an instruction label.
			 */
      if (sym->type == symMemory)
			{
				fprintf (stderr,
				         "Error: Conditional branch using memory label %s\n",
								 inp->true_branch);
				allResolved = 0;
				continue;
			}

			/*
			 * Determine the distance to be covered and place it
			 * into the instruction.
			 */
			sprintf (inp->true_branch,"%d",sym->symoffset - index);
		}

		/*
		 * Resolve the symbol in a branch's false branch.
		 */
		if (needToResolve (inp->false_branch))
		{
			/*
			 * Find the offset of the symbol from the beginning of
			 * the program.
			 */
			if ((sym = symlookup (symtable, inp->false_branch)) == NULL)
			{
				fprintf (stderr, "Invalid false branch label %s\n",
				         inp->false_branch);
				fprintins (stderr, inp);
				allResolved = 0;
				continue;
			}

			/*
			 * Ensure that the symbol refers to an instruction label.
			 */
      if (sym->type == symMemory)
			{
				fprintf (stderr,
				         "Error: Conditional branch using memory label %s\n",
								 inp->false_branch);
				allResolved = 0;
				continue;
			}

			/*
			 * Determine the distance to be covered and place it
			 * into the instruction.
			 */
			sprintf (inp->false_branch,"%d",sym->symoffset - index);
		}
	}

	return allResolved;
}

/*
 * Function: symlookup()
 *
 * Description:
 *	This function finds the specified symbol within the symbol table.
 *
 */
static struct symbol *
symlookup (struct symtable * symtable, char * name)
{
	/* Index variable */
	unsigned int index;

	for (index = 0; index < symtable->count; index++)
	{
		if (!(strcmp (symtable->symbols[index].symname, name)))
		{
			return (&(symtable->symbols[index]));
		}
	}

	return NULL;
}

static int
fprintins (FILE * out, struct instruction * inp)
{
	fprintf (out, "%s\t%s\t%s\n", inp->label, inp->opcode, inp->operand);
	return 0;
}
