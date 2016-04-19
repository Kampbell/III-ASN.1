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

# enable make dist-zip

AUTOMAKE_OPTIONS 	= dist-zip

EXTRA_DIST		= ASN1.dsp ASN1.vcproj asn1_dll.dsp asn1_dll.vcproj dllmain.cxx asn1.dtf

AM_CPPFLAGS		= @PIPEFLAG@ @DEBUGFLAG@

# specify the header files that should be installed
include_HEADERS 	= asn1.h AssocVector.h Platform.h Platform_POSIX.h Platform_WIN32.h asn1_useful.h 

#specify the header files used only when building the library
noinst_HEADERS 		= ios_helper.h

# specify the name of the library
lib_LIBRARIES 		= libasn1.a

# specify the source files of the library
libasn1_a_SOURCES 	= asn1.cxx BEREncoder.cxx BERDecoder.cxx PEREncoder.cxx PERDecoder.cxx AVNEncoder.cxx AVNDecoder.cxx ValidChecker.cxx CoderEnv.cxx InvalidTracer.cxx ios_helper.cxx

SUBDIRS = . testsuite
