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

AUTOMAKE_OPTIONS = dist-zip
EXTRA_DIST = asn1test.dsp asn1test.vcproj test.asn

AM_CPPFLAGS =  @PIPEFLAG@ @DEBUGFLAG@ -I$(top_builddir)/asn1 -I$(top_srcdir)/asn1 -I$(top_builddir)/asnh323

TESTS = asn1test

check_PROGRAMS = asn1test

asn1test_LDADD =$(top_builddir)/asn1/libasn1.a @LIBSTL@

BUILT_SOURCES = cxx/myhttp.cxx cxx/test1.cxx cxx/test1.h cxx/test1.inl cxx/myhttp.h cxx/myhttp.inl
	
asn1test_SOURCES = $(BUILT_SOURCES) asn1test.cpp 	
	

$(BUILT_SOURCES) : test.asn
	@ASNPARSER@ -v -o cxx -c $(srcdir)/test.asn

DISTFILES = $(DIST_COMMON) $(EXTRA_DIST) asn1test.h asn1test.cpp

CLEANFILES = $(BUILT_SOURCES)

asn1test_LDFLAGS = @LDFLAGS@
