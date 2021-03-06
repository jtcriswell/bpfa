#
# Copyright (c) 2003, 2016 John T Criswell
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the author nor the contributors may be used to
#       endorse or promote products derived from this software without
#       specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# RCS_ID: $Header: /home/criswell/cvs/bpfa/Makefile,v 1.3 2003/12/31 02:24:48 criswell Exp $
#

# Compilation flags
CFLAGS= -g

# Header files
HEADERS=bpf.h ins.h symtable.h bpf.tab.h

# List of object files
OBJS=lex.yy.o bpf.tab.o bpfa.o

bpfa: $(OBJS)
	$(CC) $(CFLAGS) -o bpfa $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

lex.yy.c: bpf.tab.h bpf.l
	flex bpf.l

#
# Make bpf.tab.c depend on bpf.tab.h.  bpf.tab.h will, in turn, depend
# upon bpf.y.  This will ensure that parallel builds work correctly.
#
bpf.tab.c: bpf.tab.h

bpf.tab.h: bpf.y
	bison -d bpf.y

clean:
	rm -f $(OBJS) lex.yy.c bpf.tab.c bpf.tab.h

clobber: clean
	rm -f bpfa

