dnl
dnl Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China 
dnl Written by mangelo@users.sourceforge.net
dnl 
dnl This file is part of III ASN.1 Tool.
dnl 
dnl The contents of this file are subject to the Mozilla Public License
dnl Version 1.0 (the "License"); you may not use this file except in
dnl compliance with the License. You may obtain a copy of the License at
dnl http://www.mozilla.org/MPL/
dnl
dnl Software distributed under the License is distributed on an "AS IS"
dnl basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
dnl the License for the specific language governing rights and limitations
dnl under the License.
dnl 
dnl III ASN.1 Tool is the ASN.1 to C++ compiler which transforms ASN.1 Module
dnl to C++ codes along with the C++ run time library for encoding and decoding 
dnl ASN.1 types. For details, see the III ASN.1 Tool World-Wide-Web page, 
dnl `http://iiiasn1.sourceforge.net'.
dnl

AC_INIT

AC_CONFIG_AUX_DIR(config)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(iiiasn1, 2_12)

dnl 
dnl Choose the correct C++ compiler tool setting.
dnl
dnl If the subdirecotry "config" contains a file with the pattern 
dnl mh-$host_alias, then use the definition of CC CXX CPP CXXCPP 
dnl RANLIB LDFLAGS CFLAGS CXXFLAGS CPPFLAGS from that file, otherwise
dnl use AC_PROG_CXX and AC_PROG_RANLIB to find a suitable setting.
dnl

AC_PROG_RANLIB

III_CROSS_TOOLS

dnl 
dnl Generate config.h
dnl 

AM_CONFIG_HEADER(config.h)

AC_CHECK_HEADERS([unistd.h]) 

dnl
dnl Checks whether the environment variable CPLUS_INCLUDE_PATH work correctly
dnl This is bug specific to cygwin 1.3.x
dnl

III_CHECK_CXX_INCLUDE_PATH

dnl
dnl Check if the compiler support namespace 
dnl

AC_CXX_NAMESPACES

dnl
dnl Check if STL is present
dnl

AC_CXX_HAVE_STL

if test "$ac_cv_cxx_have_stl" = no; then
	AC_MSG_ERROR(This compiler does not have STL)
fi

if test  "$ac_cv_cxx_namespaces" = no; then
	AC_MSG_ERROR(This compiler does not support namespace)
fi

dnl
dnl check whether the STL is stlport
dnl

III_CHECK_STLPORT

dnl
dnl Enable multithread support or not
dnl

AC_ARG_ENABLE(thread, [
  --enable-thread         enable thread support (default if using stlport)
  --disable-thread        disable thread support (default otherwise)], [
  iii_cv_enable_thread=$enableval
], [
        if test x"$iii_cv_have_stlport" = xno; then
                iii_cv_enable_thread=no
        else
                iii_cv_enable_thread=yes
        fi
])

if test x"$iii_cv_enable_thread" = xyes && test "$host_os" != cygwin; then
    dnl check the existence of Pthread Library
	ACX_PTHREAD([ 
		LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	],[
		AC_MSG_WARN([Pthread not found, no compiler and linker flags for multithread are specified])
	])
fi 

if test x"$iii_cv_enable_thread" = xyes ; then
  III_ENABLE_STL_THREAD
else
  AC_DEFINE(BOOST_NO_MT, , [disable multithreading for boost/pool])
fi

dnl
dnl Enable custom memory manager or not
dnl

AC_ARG_ENABLE(allocator, [
  --enable-allocator         enable custom memory allocator (default)
  --disable-allocator        disable custom memory allocator], [
  iii_cv_enable_allocator=$enableval
], [
  if test "$host_os" != cygwin; then
  	iii_cv_enable_allocator=yes
  else
    iii_cv_enable_allocator=no
  fi
])

if test x"$iii_cv_enable_allocator" = xyes; then

  AC_CACHE_CHECK([whether the compiler supports boost/pool Library],
    iii_cv_boost_pool, [
		AC_LANG_SAVE
		AC_LANG_CPLUSPLUS
		AC_TRY_COMPILE([
			#include <boost/pool/singleton_pool.hpp>
			struct Tag {};
		],[
			void* p = boost::singleton_pool<Tag,sizeof(int)>::malloc();
			boost::singleton_pool<Tag,sizeof(int)>::free(p);
			return 0;
		], [
			iii_cv_boost_pool=yes	
		], [
			iii_cv_boost_pool=no	
		])
		AC_LANG_RESTORE
	])

	if test x"$iii_cv_boost_pool" = xyes; then
		AC_DEFINE(ASN1_ALLOCATOR, ,[enable custom memory manager])
	else
		AC_MSG_WARN([This compiler does not support boost/pool Library, custom memory manager disabled])
		iii_cv_enable_allocator=no
	fi
fi



dnl
dnl 
dnl 

AC_ARG_WITH(asnh323, [
  --with-asnh323       build libasnh323.a (default)
  --without-asnh323    do not build libasnh323.a
], [
  iii_cv_with_asnh323=$enableval
], [
  iii_cv_with_asnh323=yes
])

if test x"$iii_cv_with_asnh323" = xyes; then
  asnh323dir=asnh323
fi

dnl
dnl If using GNU stdc++ library, without defineing __THROW_BAD_ALLOC, it would
dnl link "cerr", which is undesirable when iostream is not to be used.
dnl

if test x"$ac_cv_prog_gxx" = xyes && test x"$iii_cv_have_stlport" = xno ; then
# assume using libstdc++, avoid linking cerr
		AC_TRY_RUN([
			#include <vector>
			int main(){
			#ifndef __THROW_BAD_ALLOC
			return 0;
			#else
			return 1;
			#endif
			}
		], [
			AC_DEFINE(__THROW_BAD_ALLOC, ,[throw std::bad_alloc when memory is exhausted])	
		])
fi


dnl
dnl Test if the new style iostream is present
dnl

AC_CXX_HAVE_NEW_IOSTREAM

dnl
dnl Test if the wstring is present
dnl
dnl In GNU stdc++ library, the wstring is not turn on by default,
dnl you have to modify <string> to uncommnet the line 
dnl "typedef basic_string<wchar_t> wstring;"  
dnl

AC_CXX_HAVE_WSTRING

if test "$ac_cv_cxx_have_wstring" = no; then
	AC_MSG_ERROR(This compiler does not have a working wstring)
fi


dnl 
dnl Let user choose if AVN should be supported
dnl

AC_ARG_ENABLE(avn, [
  --enable-avn            enable AVN support (default)
  --disable-avn           disable AVN support], [
	if test "$enableval" = yes && test "$ac_cv_cxx_have_new_iostream" = no; then
		AC_MSG_WARN([The compiler does not support new iostream, disable AVN])
		enable_avn=no
	fi
],[ enable_avn=$ac_cv_cxx_have_new_iostream ] )

if test x"$ac_cv_cxx_have_new_iostream" = xyes; then
	AC_DEFINE(ASN1_HAS_IOSTREAM, , [enable AVN support])
fi


dnl
dnl Specify the directory where Boost Libraray is located  
dnl 

AC_ARG_WITH(boost, [
  --with-boost=[boost-dir]  specify the directory where BOOST library is located  
], [ CPPFLAGS = "$CPPFLAGS -I$withval"])


dnl
dnl If GNU C is used, append -pipe compiler option
dnl

if test "$ac_cv_prog_gxx" = yes; then
	PIPEFLAG=-pipe
fi

AC_SUBST(LIBSTL)
AC_SUBST(PIPEFLAG)
AC_SUBST(asnh323dir)

builddir=`pwd`
ASNPARSER=$builddir/asnparser/asnparser
AC_SUBST(ASNPARSER)

dnl
dnl 
dnl 
AC_ARG_ENABLE(shared,
[  --enable-shared     Turn on -fPIC],
[shared="${enableval}"],[shared=no])


if test "x${shared}" = xyes ; then
CXXFLAGS="${CXXFLAGS} -fPIC"
else
CXXFLAGS="${CXXFLAGS}"
fi

AC_ARG_ENABLE(optimizations,
[  --enable-optimizations     Turn on optimizations],
[optimizations="${enableval}"],[optimizations=no])


if test "x${optimizations}" = xyes ; then
CXXFLAGS="${CXXFLAGS} -O2"
else
CXXFLAGS="${CXXFLAGS} -O0"
fi

AC_ARG_ENABLE(debug,
[  --enable-debug     Turn on debugging],
[debug="${enableval}"],[debug=no])


if test "x${debug}" = xyes ; then
CXXFLAGS="${CXXFLAGS} -g -ggdb"
DEBUGFLAG="-D_DEBUG"
else
DEBUGFLAG="-DNDEBUG"
fi

AC_SUBST(DEBUGFLAG)

dnl
dnl Output Makefile.
dnl

AC_OUTPUT(Makefile asnparser/Makefile asn1/Makefile asn1/testsuite/Makefile asnh323/Makefile asnh323/testsuite/Makefile)
