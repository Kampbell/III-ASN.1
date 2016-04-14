#
# Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China 
# Written by mangelo@users.sourceforge.net
# 
# This file is part of III ASN.1 Tool.
# 
# The contents of this file are subject to the Mozilla Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
# the License for the specific language governing rights and limitations
# under the License.
# 
# III ASN.1 Tool is the ASN.1 to C++ compiler and C++ run time library
# for encoding and decoding ASN.1 types.
# For details, see the III ASN.1 Tool World-Wide-Web page, 
# `http://iiiasn1.sourceforge.net'.
#

AUTOMAKE_OPTIONS = nostdinc dist-zip

EXTRA_DIST	= asnparser.dsp asnparser.vcproj unistd.h getopt.c\
		  asn_grammar.y asn_lex.l asn_ref_grammar.y asn_ref_lex.l\
		  main.cxx IndentStream.cpp

bin_PROGRAMS 	= asnparser

noinst_HEADERS 	= main.h IndentStream.h

AM_CPPFLAGS 	= -std=c++11 -iquote -I. -I$(top_builddir) @PIPEFLAG@ @DEBUGFLAG@
  
asnparser_LDADD = asn_grammar.o asn_ref_grammar.o asn_lex.o asn_ref_lex.o main.o IndentStream.o @LIBSTL@
			
asnparser_SOURCES = 

CLEANFILES = asn_grammar.h asn_grammar.cxx asn_grammar.output \
             asn_ref_grammar.cxx asn_ref_grammar.h asn_ref_grammar.output\
	     asn_lex.cxx asn_ref_lex.cxx asn_lex.h asn_ref_lex.h 

	        
asn_grammar.h: asn_grammar.y
	bison -t -v -d -o asn_grammar $<
	mv asn_grammar asn_grammar.cxx
	
asn_grammar.cxx: asn_grammar.h

asn_grammar.o: asn_grammar.cxx
	$(COMPILE) -o asn_grammar.o -c $<

asn_lex.cxx: asn_lex.l
	flex -o $@ $< 
	
asn_lex.o: asn_lex.cxx
	$(COMPILE) -o asn_lex.o -c $<

asn_ref_grammar.cxx: asn_ref_grammar.y
	@if [ ! -d ref ] ; then mkdir ref ; fi
	bison -t -v -d -o asn_ref_grammar $<
	mv asn_ref_grammar asn_ref_grammar.cxx
	
asn_ref_grammar.o: asn_ref_grammar.cxx
	$(COMPILE) -o asn_ref_grammar.o -c $<	

asn_ref_lex.cxx: asn_ref_lex.l
	flex --header-file=asn_ref_lex.h -P id -o $@ $<
 
asn_ref_lex.o: asn_ref_lex.cxx
	$(COMPILE) -o asn_ref_lex.o -c $<    

main.o: main.cxx main.h asn_grammar.h
	$(COMPILE) -o $@ -c $<
				        
IndentStream.o: IndentStream.cpp IndentStream.h
	$(COMPILE) -o $@ -c $<
				        
asnparser_LDFLAGS = @LDFLAGS@
