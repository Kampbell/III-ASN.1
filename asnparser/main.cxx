/*
 * main.cxx
 *
 * PWLib application source file for asnparser
 *
 * ASN.1 compiler to produce C++ classes.
 *
 * Copyright (c) 1997-1999 Equivalence Pty. Ltd.
 *
 * Copyright (c) 2001 Institute for Information Industry
 * (http://www.iii.org.tw/iiia/ewelcome.htm)
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is ASN Parser.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors:
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 *
 * $Log: main.cxx,v $
 * Revision 1.27  2014/06/22 06:12:06  francisandre
 * c++11 updates: replace outdated strstream by sstream.add <cstring> for strlen. fix up method resolution
 *
 * Revision 1.26  2011/09/06 13:47:11  arunasr
 * Genesys fixes, includes contributions from Rodrigo Coimbra
 *
 * Revision 1.25  2011/08/09 18:12:43  arunasr
 * Genesys fixes: 3.0 release candidate
 *
 *  /main/20 2009/10/13 15:51:27 BST arunasr
 *     UNCTime added; compiler warnings cleanup
 * Revision 1.23  2006/05/12 20:53:00  arunasr
 * UTCTime, GeneralizedTime sorted
 * Fixes for generated code (dashes in class name)
 *
 * Revision 1.22  2005/09/14 18:30:16  arunasr
 * Generation of names list for Enum and BitString changed to used static member array
 *
 * Revision 1.21  2005/09/14 17:41:09  arunasr
 * Parsing of ImportedType field default values as names fixed
 *
 * Revision 1.20  2005/09/14 17:34:58  arunasr
 * Parsing of Enumerated and Integer field default values as names fixed
 *
 * Revision 1.19  2005/09/14 10:07:54  arunasr
 * Value parsing corrected in BIT STRING context
 * Generation of named bits corrected
 * Operators for ENUMERATED corrected
 *
 * Revision 1.18  2005/09/09 11:17:10  arunasr
 * Explicit tag in sequence type defined in-place not recognised:
 *   reset the flattened type's tag to back default
 *
 * Revision 1.17  2005/05/24 10:24:37  arunasr
 * Streams sorted
 *
 * Revision 1.16  2005/05/24 10:12:04  arunasr
 * Imported original changes from SF
 *
 * Revision 1.20  2003/07/21 03:26:02  mangelo
 * Fixed problem for parsing object using DefinedSyntax
 *
 * Revision 1.19  2003/06/20 03:49:49  mangelo
 * Fixed the generation of the constructor for IntegerWithNamedNumber in SEQUENCE
 *
 * Revision 1.18  2003/04/20 03:18:41  mangelo
 * Fixed OutputFile::Open() problem in VS.NET 2003
 *
 * Revision 1.17  2003/04/02 10:49:40  btrummer
 * Reimplemented the enum names output in EnumeratedType::GenerateInfo().
 * Now, ENUMERATED::getName() works correctly, even if the defined enums
 * don't start with 0 or contain holes in the value assignment.
 *
 * Revision 1.16  2003/02/17 12:12:30  btrummer
 * The last fix must be solved using an #ifdef.
 * Otherwise, it won't compile on Win32/VC6.
 *
 * Revision 1.15  2003/02/10 14:12:51  btrummer
 * ObjectClassDefn::GenerateCplusplus(): Added scope to const_iterator(i)
 * to make Solaris' Forte Developer 7 C++ 5.4 happy.
 *
 * Revision 1.14  2003/02/10 08:52:59  btrummer
 * Added a missing const in the BMPStringType::GenerateOperators() output.
 *
 * Revision 1.13  2003/01/27 12:41:29  btrummer
 * Added comparision operators for generated ENUMERATED classes.
 * When using g++ 3.2, no casts are necessary now,
 * when comparing an ENUMERATED class with one of its enum values.
 *
 * Revision 1.12  2003/01/10 13:57:42  btrummer
 * IntegerWithNamedNumber::getName() and setFromName(), ENUMERATED::getName(),
 * setFromName() and names(), CHOICE::getSelectionName() and
 * SEQUENCE::getFieldName() is now working without ASN1_HAS_IOSTREAM defined.
 *
 * Revision 1.11  2002/11/06 14:57:16  btrummer
 * If the new commandline option -C is given, the generated .cxx files
 * won't include the config.h header.
 *
 * Revision 1.10  2002/11/04 07:17:11  btrummer
 * Commited angelo's fix for the forward declaration generation
 * in TypeBase::BeginGenerateCplusplus().
 *
 * Revision 1.9  2002/10/31 07:36:15  btrummer
 * Aaargh. The new typedef must be defined protected rather than private.
 * (private is only working on g++ 2.95)
 *
 * Revision 1.8  2002/10/31 06:50:26  btrummer
 * Added a "typedef Inherited::InfoType InfoType;" to all generated classes
 * to prevent g++ compile errors, when the ASN.1 file contains tags.
 *
 * Revision 1.7  2002/10/03 07:12:14  btrummer
 * Fixed ObjectClassFieldType::GenerateDecoder():
 * if objSet.get() == NULL, true should be returned.
 *
 * Revision 1.6  2002/10/03 07:02:57  btrummer
 * Bugfix in InformationObjectSetDefn::GenerateTypeConstructor():
 * If module == NULL, objSet must be initialized to NULL.
 *
 * Revision 1.5  2002/09/14 01:56:32  mangelo
 * Removed the memory leak caused by strstream::str()
 *
 * Revision 1.4  2002/08/20 22:35:54  mangelo
 * Add MSVC DLL support
 *
 * Revision 1.3  2002/07/02 02:03:25  mangelo
 * Remove Pwlib dependency
 *
 * Revision 1.2  2001/09/07 22:39:28  mangelo
 * add Log keyword substitution
 *
 *
 * 2001/07/25 Huang-Ming Huang
 *   Added code to generate "#undef ERROR"
 *
 * 2001/07/18 Huang-Ming Huang
 *   Fixed the bug of generating non-static info for SEQUENCE_OF type.
 *
 * 2001/07/18 Huang-Ming Huang
 *   The includeOptionalField has been changed to take two parameter in
 *   accordance with the ASN1 library.
 *
 * 2001/06/26 Huang-Ming Huang
 *   Version 2.1 Reimplemented to minimize the generated code size.
 *
 * 2001/05/03 Huang-Ming Huang
 *   Fixed the problem with my wrong interpretation to varaible constraint.
 *
 * March, 2001  Huang-Ming Huang
 *   Add support for Information Object Class and generate code that follows
 *   X/Open ASN.1/C++ interface.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#else
extern "C" {
int getopt(int argc, char** argv, const char* optstring);
extern char* optarg;
extern int optind;
}
#endif

#include "main.h"
#include <boost/mem_fn.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/functional.hpp>
#include <typeinfo>
#include <algorithm>
#include <numeric>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <assert.h>
#include <limits.h>
#include "asn_grammar.h"

#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

//using namespace boost;
using std::endl;
using std::ends;
using std::string;

unsigned lineNumber;
std::string  fileName;

unsigned fatals, warnings;

extern void yyrestart(FILE*);
extern FILE * yyin;
extern FILE * idin;
extern int yydebug;
extern int iddebug;
int LexEcho;



static const char * const StandardClasses[] = {
  "ASN1::Null",
  "ASN1::BOOLEAN",
  "ASN1::INTEGER",
  "ASN1::ENUMERATED",
  "ASN1::BinaryReal",
  "ASN1::OBJECT_IDENGIFIER",
  "ASN1::BIT_STRING",
  "ASN1::OCTET_STRING",
  "ASN1::NumericString",
  "ASN1::PrintableString",
  "ASN1::VisibleString",
  "ASN1::GraphicString",
  "ASN1::ObjectDescriptor",
  "ASN1::IA5String",
  "ASN1::GeneralString",
  "ASN1::GeneralizedTime",
  "ASN1::UTCTime",
  "ASN1::BMPString",
  "ASN1::SEQUENCE_OF",
  "ASN1::EXTERNAL",
  "ASN1::ABSTRACT_SYNTAX"
};

static const char * CppReserveWords[] = {
  "and","and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char",
  "class", "compl", "const", "const_cast",
  "continue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
  "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", // "id",
  "long", "mutable", "namespace", "new", "not", "not_eq", "operator", "or", "or_eq",
  "private", "protected", "public", "register",
  "reinterpret_cast", "return", "short", "signed", "sizeof", "static", "static_cast", "struct",
  "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename",
  "union", "unsigned", "using", "virtual", "void", "volitile", "wchar_t", "while",
  "xor", "xor_eq"
};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

ModuleDefinition * Module;
ModuleList Modules;
ClassStack *classStack;
ParameterList * DummyParameters;
extern int IdentifierTokenContext ;
extern int BraceTokenContext ;
extern int InOIDContext ;
extern int NullTokenContext ;
extern int InObjectSetContext;
TypePtr ValueTypeContext;
std::vector<std::string> RemoveList;

class UsefulModuleDef : public ModuleDefinition
{
public:
  UsefulModuleDef();
  virtual void GenerateCplusplus(const std::string & modName, unsigned numFiles, bool verbose) {}
};

class CompareNamedNumber
{
public:
  CompareNamedNumber (int val) : m_val(val) {}
  bool operator()(NamedNumberPtr ptr) const { return ptr->GetNumber()==m_val; }
private:
  int m_val;
};


UsefulModuleDef * UsefulModule =0;
//
//  yyerror
//  required function for flex
//

void yyerror(char * str)
{
  extern char * yytext;
  std::cerr << StdError(Fatal) << str << " near token \"" << yytext <<"\"\n";
}

void iderror(char * str)
{
  extern char * idtext;
  std::cerr << StdError(Fatal) << str << " near token \"" << idtext <<"\"\n";
}

std::ostream & operator<<(std::ostream & out, const StdError & e)
{
  out << fileName << '(' << lineNumber << ") : ";
  if (e.e == Fatal) {
    fatals++;
    out << "error";
  }
  else {
    warnings++;
    out << "warning";
  }
  return out << ": ";
}


/////////////////////////////////////////////////////////
//
//  Utility
//

void str_replace(std::string& str, const char* src, const char* target, std::string::size_type pos = 0)
{
  const size_t l = strlen(src);

  while ( (pos = str.find(src,pos)) != std::string::npos)
    str.replace(pos, l, target);
}

struct str_less : std::binary_function<const char*, const char*, bool>
{
  bool operator () (const char* lhs, const char* rhs) const {
    return strcmp(lhs, rhs)<0;
  }
};

static std::string MakeIdentifierC(const std::string & identifier)
{
  std::string s = identifier;
  if (s != "")
  {
    str_replace(s, "-", "_");
    static const char** end = CppReserveWords+ARRAY_SIZE(CppReserveWords);
    if (std::binary_search(CppReserveWords,
        end,
        s.c_str(),
        str_less()))
      s += '_';
  }
  return s;
}

// creates a short name for module names from combining the first character
// of each '-' parted words, eg. H323-MESSAGES => HM,
// Multimedia-System-Control => MSC
static std::string ShortenName(const std::string& name)
{
  std::string s ;
  s += name[0];

  size_t i = 0;
  while ( (i = name.find('-', i+1)) != -1)
    s += name[i+1];

  return s;
}

inline std::string ToLower(const std::string& str)
{
  std::string result;
  result.resize(str.size());
  std::transform(str.begin(), str.end(), result.begin(), tolower);
  return result;
}

inline std::string ToUpper(const std::string& str)
{
  std::string result;
  result.resize(str.size());
  std::transform(str.begin(), str.end(), result.begin(), toupper);
  return result;
}

std::string GetFileName(const std::string& fullname)
{
  int i = fullname.find_last_of(DIR_SEPARATOR);
  return fullname.substr(i+1);
}

std::string GetFileDirectory(const std::string& fullname)
{
  std::string::size_type p = fullname.find_last_of(DIR_SEPARATOR);
  if (p!=std::string::npos)
    return fullname.substr(0, p);

  return std::string();
}

std::string GetFileTitle(const std::string& fullname)
{
  std::string result = GetFileName(fullname);
  int dotpos = result.find_last_of('.');
  if (dotpos == -1)
    return result.substr(0, dotpos-1);

  return result;
}


class OutputFile : public std::ofstream
{
public:
  OutputFile() {}
  ~OutputFile() { Close(); }

  bool Open(const std::string & path, const char* suffix, const char * extension);
  void Close();
  const std::string& GetFilePath() const  { return filename; }

private:
  OutputFile(const OutputFile&);
  OutputFile& operator = (const OutputFile&);
  std::string filename;
};


bool OutputFile::Open(const std::string & path,
                      const char * suffix,
                      const char * extension)
{
  filename = path + suffix + extension;
  open(filename.c_str());
  std::ostream& strm = *this;
  if (is_open()) {
    strm << "//\n"
             "// " << GetFileName(filename) << "\n"
             "//\n"
             "// Code automatically generated by asnparser.\n"
             "//\n"
             "\n";
    return true;
  }

  std::cerr << "asnparser : cannot create " << filename << endl;
  return false;
}


void OutputFile::Close()
{
  if (is_open()) {
    std::ostream& strm = *this;
    strm << "\n// End of " << GetFileName(filename) << endl;
  }
  using namespace std;

  ofstream::close();
}


class Indent {
public:
  Indent(std::streamsize i) : space(i){}
  Indent operator + (int i) const { return Indent(space +i); }
  Indent operator - (int i) const { return Indent(space -i); }
  Indent& operator += (int i) { space+= i; return *this;}
  Indent& operator -= (int i) { space-= i; return *this;}

  friend std::ostream& operator << (std::ostream& os, const Indent& indent)
  { return os << std::setw(indent.space) << ""; }

private:
  std::streamsize space;
};

void AddRemoveItem(const char* item)
{
    RemoveList.push_back(std::string(item));
}


template <class Cont, class Fun>
void for_all(Cont& cont, Fun fun)
{
  std::for_each(cont.begin(), cont.end(), fun);
}

ModuleDefinition* FindModule(const char* name)
{
  return FindWithName(Modules, name).get();
}


template <class T>
std::ostream& operator << (std::ostream& os, const std::vector<boost::shared_ptr<T> >& cont)
{
  typename std::vector<boost::shared_ptr<T> >::const_iterator it, last = cont.end();
  for (it = cont.begin(); it != last; ++it)
    os << **it;
  return os;
}

class Unfreezer
{
public:
  Unfreezer(std::stringstream& strm) : stream(strm) {}
  ~Unfreezer() { }

private:
  std::stringstream& stream;
};

/////////////////////////////////////////////////////////
//
//  Application
//

int verbose=0;
std::string dllMacroExport;
std::string dllMacroDLL;
bool includeConfigH = true;
std::string useReinterpretCast;

unsigned classesPerFile = 0;

const char *cppExt = ".cxx";
bool noInlineFiles = false;

int main(int argc, char** argv)
{
  extern int yydebug ;      //  nonzero means print parse trace

  const char* opt = "cdeno:s:vm:Cr:";

  int c;
  bool generateCpp = false;
  std::string path;

  while ((c=getopt(argc, argv, opt)) != -1)
  {
    switch (c) {
    case 'c':
      generateCpp = true;
      break;

    case 'd':
      yydebug = 1;
      iddebug = 1;
      break;

    case 'e':
      cppExt = ".cpp";
      break;

    case 'n':
      noInlineFiles = true;
      break;

    case 'o':
      path = optarg;
      break;

    case 's':
      classesPerFile = atoi(optarg);
      break;

    case 'v':
      ++verbose;
      break;

    case 'm':
      dllMacroExport = optarg;
      dllMacroExport += "_EXPORT ";
      dllMacroDLL    = optarg;
      dllMacroDLL    += "_DLL";
      break;

    case 'C':
      includeConfigH = false;
      break;

    case 'r':
      useReinterpretCast = optarg;
      break;
    }
  }

  size_t fileCount = argc-optind;

  std::cout << "asnparser version 2.3 by Institute for Information Industry\n" << endl;

  if (fileCount < 1 ) {
    std::cerr << "usage: asnparser [options] asnfile...\n"
                 "  -v          Verbose output (multiple times for more verbose)\n"
                 "  -d          Debug output (copious!)\n"
                 "  -c          Generate C++ files\n"
                 "  -e          Generated C++ files with .cpp extension\n"
                 "  -n          Use inline definitions rather than .inl files\n"
                 "  -s  n       Split output if it has more than n (default 1000) classes\n"
                 "  -o  dir     Output directory\n"
                 "  -m  name    Macro name for generating DLLs under windows with MergeSym\n"
                 "  -C          If given, the generated .cxx files won't include config.h\n"
                 "  -r  name    Use reinterpret_casts rather than static_casts in the\n"
                 "              generated .inl files, if -Dname is given to the compiler\n"
              << endl;
    return 1;
  }

  classStack = new ClassStack;

  std::vector<FILE*> fds(fileCount);

  size_t i;

  Modules.push_back(ModuleDefinitionPtr(UsefulModule = new UsefulModuleDef));

  for ( i = 0 ; i < fileCount; ++i)
  {
    idin = fds[i] = fopen(argv[i+optind],"r");
    if (!idin) {
      std::cerr << "asnparser: cannot open \""
                << argv[i+optind]  << '"'<<endl;
      return 1;
    }

    fileName   = argv[i+optind];
    lineNumber = 1;
    fatals     = 0;
    warnings   = 0;

    if (verbose)
      std::cout << "First Stage Parsing... " << fileName << endl;

    idparse(); // parse the identifier types
  }

  for (i = 0; i < fileCount; ++i)
  {
    fileName   = argv[i+optind];
    lineNumber = 1;
    fatals     = 0;
    warnings   = 0;

    yyin = fds[i];

    if (verbose)
      std::cout << "Second Stage Parsing... " << fileName << endl;

    rewind(yyin); // rewind the file
    yyrestart( yyin );
    yyparse(); // actually parse the content of the ASN.1
  }

  for (i = 0 ; i < Modules.size(); ++i) {
    Modules[i]->AdjustModuleName(path);
  }

  for (i = 0; i < RemoveList.size(); ++i)
  {
     int dotpos = RemoveList[i].find('.');
     std::string modulename = RemoveList[i].substr(0, dotpos);
     ModuleDefinition* module = FindModule(modulename.c_str());
     if (module)
         module->AddToRemoveList(RemoveList[i].substr(dotpos+1, RemoveList[i].size()-1));
  }

  for (i = 0 ; i < Modules.size(); ++i) {
    Modules[i]->RemoveReferences(verbose !=0 );
    Modules[i]->AdjustImportedModules();
  }

  for (i = 0; i < Modules.size(); ++i) {
    Module = Modules[i].get();
    if (verbose > 1)
      std::cerr << "Module " << *Module << endl;

    if (generateCpp)
      Module->GenerateCplusplus(path,
                                classesPerFile,
                                verbose!=0);
  }

  delete classStack;
  return 0;
}


/////////////////////////////////////////
//
//  miscellaneous
//

class indent : public std::string
{
  typedef std::string Base;

  public:
    indent() : Base(Module->GetIndentLevel()*3, ' ') { }
};


/////////////////////////////////////////
//
//  intermediate structures from parser
//

NamedNumber::NamedNumber(std::string * nam)
  : name(*nam)
{
  delete nam;
  number = 0;
  autonumber = true;
}


NamedNumber::NamedNumber(std::string * nam, int num)
  : name(*nam)
{
  delete nam;
  number = num;
  autonumber = false;
}


NamedNumber::NamedNumber(std::string * nam, const std::string & ref)
  : name(*nam), reference(ref)
{
  delete nam;
  number = 0;
  autonumber = false;
}


std::ostream& operator << (std::ostream & strm, const NamedNumber& obj)
{
  strm << obj.name << " (";
  if (obj.reference.size()==0)
    strm << obj.number;
  else
    strm << obj.reference;
  strm << ')';
  return strm;
}


void NamedNumber::SetAutoNumber(const NamedNumber & prev)
{
  if (autonumber) {
    number = prev.number + 1;
    autonumber = false;
  }
}


/////////////////////////////////////////////////////////

Tag::Tag(unsigned tagNum, Mode m)
{
  type = Universal;
  number = tagNum;
  mode = m;
}


const char * Tag::classNames[] = {
  "UNIVERSAL", "APPLICATION", "CONTEXTSPECIFIC", "PRIVATE"
};


const char * Tag::modeNames[] = {
  "IMPLICIT", "EXPLICIT", "AUTOMATIC"
};


void Tag::PrintOn(std::ostream & strm) const
{
  if (type != Universal || number != IllegalUniversalTag) {
    strm << '[';
    if (type != ContextSpecific)
      strm << classNames[type] << ' ';
    strm << number << "] " << modeNames[mode] << ' ';
  }
}


/////////////////////////////////////////////////////////

Constraint::Constraint(ConstraintElementPtr& elmt)
{
  standard.push_back(elmt);
  extendable = false;
}


Constraint::Constraint(std::auto_ptr<ConstraintElementVector> a_std,
             bool extend,
             std::auto_ptr<ConstraintElementVector> ext)
{
  if (a_std.get() != NULL) {
    standard.swap(*a_std);
  }
  extendable = extend;
  if (ext.get() != NULL) {
    extensions.swap(*ext);
  }
}

Constraint::Constraint(const Constraint& other)
: standard(other.standard)
, extendable(other.extendable)
, extensions(other.extensions)
{
}



void Constraint::PrintOn(std::ostream & strm) const
{
  strm << '(';
  PrintElements(strm);
  strm << ')';
}

void PrintVector(std::ostream& strm ,const ConstraintElementVector& elements, char delimiter)
{
  ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();

  if (i != e)
  {
    (*i)->PrintOn(strm);
       ++i;
  }

  for (; i != e; ++i)
  {
    strm << ' ' << delimiter << ' ';
    (*i)->PrintOn(strm);
  }
}

void Constraint::PrintElements(std::ostream & strm) const
{

  PrintVector(strm, standard, '|');
  if (extendable) {
    strm << indent();
    if (standard.size() > 0)
      strm << ", ";
    strm << "...";
    if (extensions.size() > 0)
      strm << ',';
    strm << ' ';

    PrintVector(strm, extensions, '|');
  }
}


void Constraint::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  switch (standard.size()) {
    case 0 :
      return;
    case 1 :
      break;
    default :
      std::cerr << StdError(Warning) << "unsupported UNION constraints, ignored." << endl;
  }

  if (extensions.size() > 0)
    std::cerr << StdError(Warning) << "unsupported extension constraints, ignored." << endl;

  std::string fn2 = fn;
  if (fn.find("ASN1::") == -1) {
    if (extendable)
      fn2 += "ASN1::ExtendableConstraint";
    else
      fn2 += "ASN1::FixedConstraint";
  }

  standard[0]->GenerateCplusplus(fn2, hdr, cxx, inl);
}

void Constraint::GetConstraint(std::string& str) const
{
  if (str.find("ConstrainedObject::, ") == -1 ) {
    if (extendable)
      str += "ASN1::ExtendableConstraint, ";
    else
      str += "ASN1::FixedConstraint, ";
  }
  standard[0]->GetConstraint(str);
}



bool Constraint::ReferencesType(const TypeBase & type) const
{
  ConstraintElementVector::size_type  i;

  for (i = 0; i < standard.size(); i++) {
    if (standard[i]->ReferencesType(type))
      return true;
  }

  for (i = 0; i < extensions.size(); i++) {
    if (extensions[i]->ReferencesType(type))
      return true;
  }

  return false;
}

ValueSetPtr Constraint::GetValueSetFromValueField(const std::string& field) const
{
  const ConstraintElementVector* vectors[] = {&standard,&extensions};
  ValueSetPtr result(new ValueSetDefn);

  for (int k = 0; k < 2; ++k)
  {
    ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
    for (; i != e ; ++i)
    {
      ValueSetPtr s = (*i)->GetValueSetFromValueField(field);
      result->Union(s);
    }
  }

  return result;
}

ValueSetPtr Constraint::GetValueSetFromValueSetField(const std::string& field) const
{
  const ConstraintElementVector* vectors[] = {&standard,&extensions};
  ValueSetPtr result(new ValueSetDefn);

  for (int k = 0; k < 2; ++k)
  {
    ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
    for (; i != e ; ++i)
    {
      ValueSetPtr s = (*i)->GetValueSetFromValueSetField(field);
      result->Union(s);
    }
  }
  return result;
}

ConstraintPtr Constraint::GetObjectSetFromObjectField(const std::string& field) const
{
  const ConstraintElementVector* vectors[] = {&standard,&extensions};
  ConstraintPtr result(new Constraint(extendable));

  for (int k = 0; k < 2; ++k)
  {
    ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
    for (; i != e; ++i)
    {
      ConstraintPtr j = (*i)->GetObjectSetFromObjectField(field);
      result->standard.insert(result->standard.end(), j->standard.begin(), j->standard.end());
      result->standard.insert(result->standard.end(), j->extensions.begin(), j->extensions.end());
      result->extendable |= j->extendable;
    }
  }
  return result;
}

ConstraintPtr Constraint::GetObjectSetFromObjectSetField(const std::string& field) const
{
  const ConstraintElementVector* vectors[] = {&standard,&extensions};
  ConstraintPtr result(new Constraint(extendable));

  for (int k = 0; k < 2; ++k)
  {
    ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
    for (; i != e; ++i)
    {
      ConstraintPtr j = (*i)->GetObjectSetFromObjectSetField(field);
      result->standard.insert(result->standard.end(), j->standard.begin(), j->standard.end());
      result->standard.insert(result->standard.end(), j->extensions.begin(), j->extensions.end());
      result->extendable |= j->extendable;
    }
  }
  return result;
}


bool Constraint::HasPERInvisibleConstraint(const Parameter& param) const
{
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    if ((*it)->HasPERInvisibleConstraint(param))
      return true;

    for (it = extensions.begin(),last = extensions.end();
         it != last;
         ++it)
      if ((*it)->HasPERInvisibleConstraint(param))
        return true;

  return false;
}

void Constraint::GenerateObjectSetInstanceCode(const std::string& prefix, std::ostream& cxx) const
{
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    (*it)->GenerateObjectSetInstanceCode(prefix, cxx);

  for (it = extensions.begin(), last = extensions.end();
       it != last;
       ++it)
    (*it)->GenerateObjectSetInstanceCode(prefix, cxx);
}


void Constraint::GenerateObjSetAccessCode(std::ostream& cxx)
{
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    (*it)->GenerateObjSetAccessCode(cxx);

  for (it = extensions.begin(), last = extensions.end();
       it != last;
       ++it)
    (*it)->GenerateObjSetAccessCode(cxx);
}

const SizeConstraintElement* Constraint::GetSizeConstraint() const
{
  const SizeConstraintElement* result= NULL;
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    if ((result = (*it)->GetSizeConstraint()) != NULL)
      break;
  return result;
}

const FromConstraintElement* Constraint::GetFromConstraint() const
{
  const FromConstraintElement* result=NULL;
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    if ((result = (*it)->GetFromConstraint()) != NULL)
      break;
  return result;
}

void Constraint::GetCharacterSet(std::string& characterSet) const
{
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    if ((*it)->GetCharacterSet(characterSet))
      return;
}

const SubTypeConstraintElement* Constraint::GetSubTypeConstraint() const
{
  const SubTypeConstraintElement* result=NULL;
  ConstraintElementVector::const_iterator it = standard.begin(),
                                          last = standard.end();

  for (; it != last; ++it)
    if ((result = (*it)->GetSubTypeConstraint()) != NULL)
      break;
  return result;
}

std::auto_ptr<Constraint> Constraint::Clone() const
{
  return std::auto_ptr<Constraint>(new Constraint(*this));
}


/////////////////////////////////////////////////////////

ConstraintElementBase::ConstraintElementBase()
{}

ConstraintElementBase::~ConstraintElementBase()
{}

void ConstraintElementBase::GenerateCplusplus(const std::string &, std::ostream &, std::ostream &, std::ostream &) const
{
  std::cerr << StdError(Warning) << "unsupported constraint, ignored." << endl;
}


bool ConstraintElementBase::ReferencesType(const TypeBase &) const
{
  return false;
}

ValueSetPtr ConstraintElementBase::GetValueSetFromValueField(const std::string& ) const
{
  std::cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
  return ValueSetPtr();
}

ValueSetPtr ConstraintElementBase::GetValueSetFromValueSetField(const std::string& ) const
{
  std::cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
  return ValueSetPtr();
}

ConstraintPtr ConstraintElementBase::GetObjectSetFromObjectField(const std::string& ) const
{
  std::cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
  return ConstraintPtr();
}

ConstraintPtr ConstraintElementBase::GetObjectSetFromObjectSetField(const std::string& ) const
{
  std::cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
  return ConstraintPtr();
}

const SizeConstraintElement* ConstraintElementBase::GetSizeConstraint() const
{
  return NULL;
}

const FromConstraintElement* ConstraintElementBase::GetFromConstraint() const
{
  return NULL;
}

bool ConstraintElementBase::GetCharacterSet(std::string& ) const
{
  return false;
}

const SubTypeConstraintElement* ConstraintElementBase::GetSubTypeConstraint() const
{
    return NULL;
}


/////////////////////////////////////////////////////////

ConstrainAllConstraintElement::ConstrainAllConstraintElement(ConstraintElementPtr excl)
{
  SetExclusions(excl);
}


/////////////////////////////////////////////////////////
ElementListConstraintElement::ElementListConstraintElement()
{
}

ElementListConstraintElement::ElementListConstraintElement(std::auto_ptr<ConstraintElementVector> list)
{
  elements.swap(*list);
}

void ElementListConstraintElement::PrintOn(std::ostream & strm) const
{
  PrintVector(strm, elements, '^');
}


void ElementListConstraintElement::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++)
    elements[i]->GenerateCplusplus(fn, hdr, cxx, inl);
}

void ElementListConstraintElement::GetConstraint(std::string& str) const
{
  for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++)
    elements[i]->GetConstraint(str);
}

bool ElementListConstraintElement::ReferencesType(const TypeBase & type) const
{
  for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++) {
    if (elements[i]->ReferencesType(type))
      return true;
  }
  return false;
}

ValueSetPtr ElementListConstraintElement::GetValueSetFromValueField(const std::string& field) const
{
  ValueSetPtr result = elements[0]->GetValueSetFromValueField(field);

  for (ConstraintElementVector::size_type i = 1; i < elements.size(); i++)
  {
    ValueSetPtr t = elements[i]->GetValueSetFromValueField(field);
    result->Intersect(t);
  }
  return result;
}

ValueSetPtr ElementListConstraintElement::GetValueSetFromValueSetField(const std::string& field) const
{
  ValueSetPtr result = elements[0]->GetValueSetFromValueField(field);

  for (ConstraintElementVector::size_type i = 1; i < elements.size(); i++)
  {
    ValueSetPtr t = elements[i]->GetValueSetFromValueSetField(field);
    result->Intersect(t);
  }
  return result;
}

void ElementListConstraintElement::AppendElements(
  ConstraintElementVector::const_iterator first,
  ConstraintElementVector::const_iterator last
  )
{
  elements.insert(elements.end(), first, last);
}

ConstraintPtr ElementListConstraintElement::GetObjectSetFromObjectField(const std::string& field) const
{
  boost::shared_ptr<ElementListConstraintElement>
     elem(new  ElementListConstraintElement);
  ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
  for (; i != e; ++i)
  {
    ConstraintPtr cons = (*i)->GetObjectSetFromObjectField(field);
    if (cons.get() != NULL)
    {
      elem->AppendElements(cons->GetStandardElements().begin(),
        cons->GetStandardElements().end());
      elem->AppendElements(cons->GetExtensionElements().end(),
        cons->GetExtensionElements().end());
    }
  }
  if (elem->elements.size()) {
    ConstraintElementPtr elm =
      boost::static_pointer_cast<ConstraintElementBase>(elem);
    return ConstraintPtr(new Constraint(elm));
  }

  return ConstraintPtr();
}

ConstraintPtr ElementListConstraintElement::GetObjectSetFromObjectSetField(const std::string& field) const
{
  boost::shared_ptr<ElementListConstraintElement>
    elem(new  ElementListConstraintElement);
  ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
  for (; i != e; ++i)
  {
    ConstraintPtr cons = (*i)->GetObjectSetFromObjectSetField(field);
    if (cons.get() != NULL)
    {
      elem->AppendElements(cons->GetStandardElements().begin(),
        cons->GetStandardElements().end());
      elem->AppendElements(cons->GetExtensionElements().end(),
        cons->GetExtensionElements().end());
    }
  }
  if (elem->elements.size()){
    ConstraintElementPtr elm =
      boost::static_pointer_cast<ConstraintElementBase>(elem);
    return ConstraintPtr(new Constraint(elm));
  }

  return ConstraintPtr();
}

bool ElementListConstraintElement::HasPERInvisibleConstraint(const Parameter& param) const
{
  ConstraintElementVector::const_iterator it = elements.begin(),
                                          last = elements.end();

  for (; it != last; ++it)
    if ((*it)->HasPERInvisibleConstraint(param))
      return true;

  return false;
}

void ElementListConstraintElement::GenerateObjectSetInstanceCode(const std::string& prefix, std::ostream& cxx) const
{
  ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
  for (; i != e; ++i)
    (*i)->GenerateObjectSetInstanceCode(prefix, cxx);
}

void ElementListConstraintElement::GenerateObjSetAccessCode(std::ostream& cxx)
{
  ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
  for (; i != e; ++i)
    (*i)->GenerateObjSetAccessCode(cxx);
}

const SizeConstraintElement* ElementListConstraintElement::GetSizeConstraint() const
{
  const SizeConstraintElement* result=NULL;
  ConstraintElementVector::const_iterator it = elements.begin(),
                                          last = elements.end();

  for (; it != last; ++it)
    if ((result = (*it)->GetSizeConstraint()) != NULL)
      break;
  return result;
}

const FromConstraintElement* ElementListConstraintElement::GetFromConstraint() const
{
  const FromConstraintElement* result=NULL;
  ConstraintElementVector::const_iterator it = elements.begin(),
                                          last = elements.end();

  for (; it != last; ++it)
    if ((result = (*it)->GetFromConstraint()) != NULL)
      break;
  return result;
}


/////////////////////////////////////////////////////////

SingleValueConstraintElement::SingleValueConstraintElement(const ValuePtr& val)
: value(val)
{
}

SingleValueConstraintElement::~SingleValueConstraintElement()
{
}


void SingleValueConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << *value;
}


void SingleValueConstraintElement::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  if (dynamic_cast<const IntegerValue*>(value.get())) {
    cxx << fn << ", ";
    value->GenerateCplusplus(hdr, cxx, inl);
    cxx << ", ";
    value->GenerateCplusplus(hdr, cxx, inl);
    cxx << ");\n";
    return;
  }

  if (dynamic_cast<const CharacterStringValue*>(value.get())) {
    cxx << fn << ", ";
    value->GenerateCplusplus(hdr, cxx, inl);
    cxx << ");\n";
    return;
  }

  std::cerr << StdError(Warning) << "Unsupported constraint type, ignoring." << endl;
}

void SingleValueConstraintElement::GetConstraint(std::string& str) const
{
  std::stringstream strm;
  Unfreezer unfreezer(strm);

  if (dynamic_cast<const IntegerValue*>(value.get())) {
    strm << *value << ", " << *value ;
    str += strm.str();
  }
  else  if (dynamic_cast<const CharacterStringValue*>(value.get())) {
    strm << *value;
    str += strm.str();
  }
}

bool SingleValueConstraintElement::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}

bool SingleValueConstraintElement::GetCharacterSet(std::string& characterSet) const
{
  const CharacterStringValue* val;
  if ((val = dynamic_cast<const CharacterStringValue*>(value.get())) != NULL)
  {
    val->GetValue(characterSet);
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////

ValueRangeConstraintElement::ValueRangeConstraintElement(ValuePtr lowerBound, ValuePtr upperBound)
{
  lower = lowerBound;
  upper = upperBound;
}

ValueRangeConstraintElement::~ValueRangeConstraintElement()
{
}


void ValueRangeConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << *lower << ".." << *upper;
}


void ValueRangeConstraintElement::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  cxx << fn << ", ";
  lower->GenerateCplusplus(hdr, cxx, inl);
  cxx << ", ";
  upper->GenerateCplusplus(hdr, cxx, inl);
  cxx << ");\n";
}



void ValueRangeConstraintElement::GetConstraint(std::string& str) const
{
  if (dynamic_cast<MinValue*>(lower.get()))
  {
    str_replace(str, "FixedConstraint", "Unconstrained");
  }
  else if (dynamic_cast<MaxValue*>(upper.get()))
  {
    str_replace(str,"FixedConstraint", "PartiallyConstrained");
  }
  else if (dynamic_cast<CharacterStringValue*>(lower.get()))
  {
    str_replace(str,"FromConstraint", "FromRangeConstraint");
  }

  std::stringstream strm;
  Unfreezer unfreezer(strm);
  strm << *lower << ", " << *upper ;
  str += strm.str();
}

bool ValueRangeConstraintElement::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}

bool ValueRangeConstraintElement::GetCharacterSet(std::string& characterSet) const
{
  const CharacterValue* l = dynamic_cast<const CharacterValue*>(lower.get());
  const CharacterValue* u = dynamic_cast<const CharacterValue*>(upper.get());
  if ( l && u)
  {
      for (char c = (char) l->GetValue();
           c <= (char) u->GetValue();
           ++c)
      {
          characterSet += c;
      }
      return true;
  }

  const CharacterStringValue* lv = dynamic_cast<const CharacterStringValue*>(lower.get());
  const CharacterStringValue* uv = dynamic_cast<const CharacterStringValue*>(upper.get());
  if (lv && uv)
  {
      std::string l, u;
      lv->GetValue(l);
      uv->GetValue(u);
      if (l.length()==1 && u.length() == 1)
      {
          for (char c = l[0]; c <= u[0]; ++c)
              characterSet += c;
          return true;
      }

  }

  return false;
}


/////////////////////////////////////////////////////////

SubTypeConstraintElement::SubTypeConstraintElement(TypePtr typ)
: subtype(typ)
{
}

SubTypeConstraintElement::~SubTypeConstraintElement()
{
}


void SubTypeConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << *subtype;
}


void SubTypeConstraintElement::GenerateCplusplus(const std::string & str, std::ostream & , std::ostream & cxx, std::ostream& ) const
{
  cxx << "    " << subtype->GetTypeName() << " typeConstraint;\n"
      << "    setConstraints(" ;
  if (str.find("ASN1::ExtendableConstraint") != -1)
      cxx << "ASN1::ExtendableConstraint,";
  else
      cxx << "ASN1::FixedConstraint,";
  cxx << " typeConstraint.getLowerLimit(), typeConstraint.getUpperLimit());\n";
}

void SubTypeConstraintElement::GetConstraint(std::string& str) const
{
  std::stringstream strm;
  Unfreezer unfreezer(strm);
  strm << str << subtype->GetTypeName() << "::LowerLimit, "
       << subtype->GetTypeName() << "::UpperLimit" ;
  str = strm.str();
}


bool SubTypeConstraintElement::ReferencesType(const TypeBase & type) const
{
  return subtype->ReferencesType(type);
}

bool SubTypeConstraintElement::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}

const SubTypeConstraintElement* SubTypeConstraintElement::GetSubTypeConstraint() const
{
    return this;
}

std::string SubTypeConstraintElement::GetSubTypeName() const
{
    return subtype->GetTypeName();
}


/////////////////////////////////////////////////////////

NestedConstraintConstraintElement::NestedConstraintConstraintElement(ConstraintPtr con)
{
  constraint = con;
}

NestedConstraintConstraintElement::~NestedConstraintConstraintElement()
{
}


bool NestedConstraintConstraintElement::ReferencesType(const TypeBase & type) const
{
  if (constraint.get() == NULL)
    return false;

  return constraint->ReferencesType(type);
}

bool NestedConstraintConstraintElement::HasPERInvisibleConstraint(const Parameter& param) const
{
  return constraint->HasPERInvisibleConstraint(param);
}

///////////////////////////////////////////////////////

SizeConstraintElement::SizeConstraintElement(ConstraintPtr constraint)
  : NestedConstraintConstraintElement(constraint)
{
}

void SizeConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << "SIZE" << *constraint;
}


void SizeConstraintElement::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  constraint->GenerateCplusplus(fn, hdr, cxx, inl);
}

void SizeConstraintElement::GetConstraint(std::string& str) const
{
  const char* cstr = "ASN1::FixedConstraint, ";
  const int len = strlen(cstr);
  std::string::iterator itr = std::find_end(str.begin(), str.end(), cstr, cstr+len);
  if (itr != str.end())
    str.replace(itr, itr+len, " ASN1::SizeConstraint<");
  constraint->GetConstraint(str);
  str += "> ";
}

const SizeConstraintElement* SizeConstraintElement::GetSizeConstraint() const
{
  return this;
}


/////////////////////////////////////////////////////////

FromConstraintElement::FromConstraintElement(ConstraintPtr constraint)
  : NestedConstraintConstraintElement(constraint)
{
}

void FromConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << "FROM" << *constraint;
}


void FromConstraintElement::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  std::string newfn = fn;
  str_replace(newfn,"setConstraints(", "setCharacterSet(");
  constraint->GenerateCplusplus(newfn, hdr, cxx, inl);
}

void FromConstraintElement::GetConstraint(std::string& ) const
{
}

const FromConstraintElement* FromConstraintElement::GetFromConstraint() const
{
  return this;
}

std::string FromConstraintElement::GetCharacterSet(const char* canonicalSet, int canonicalSetSize) const
{
    std::string characterSet;
    if (!constraint->IsExtendable())
    {
        std::string constrainedSet;
        constraint->GetCharacterSet(constrainedSet);

        int setSize = constrainedSet.size();
        const char* c = &constrainedSet[0];
        for (int i = 0; i < canonicalSetSize; i++) {
            if (memchr(c, canonicalSet[i], setSize) != NULL)
                characterSet += canonicalSet[i];
        }
    }
    return characterSet;
}

int FromConstraintElement::GetRange(std::ostream& cxx) const
{
  std::stringstream inl, hdr, tmpcxx;
  Unfreezer unfreezer1(inl),unfreezer2(hdr),unfreezer3(tmpcxx);
  std::string str;
  constraint->GenerateCplusplus(str,hdr, tmpcxx, inl);

  int min, max;
  char c;
  tmpcxx >> str >> min >> c >> max;
  cxx << min << ", " << max;
  return max-min;
}

/////////////////////////////////////////////////////////

WithComponentConstraintElement::WithComponentConstraintElement(std::string newName,
                                                               ConstraintPtr constraint,
                                                               int pres)
  : NestedConstraintConstraintElement(constraint)
{
  name = newName;
  presence = pres;
}

void WithComponentConstraintElement::PrintOn(std::ostream & strm) const
{
  if (name.empty())
    strm << "WITH COMPONENT";
  else
    strm << name;

  if (constraint.get() != NULL)
    strm << *constraint;

  switch (presence) {
    case Present :
      strm << " PRESENT";
      break;
    case Absent :
      strm << " ABSENT";
      break;
    case Optional :
      strm << " OPTIONAL";
      break;
  }
}


void WithComponentConstraintElement::GenerateCplusplus(const std::string &, std::ostream &, std::ostream & , std::ostream &) const
{
}

/////////////////////////////////////////////////////////

InnerTypeConstraintElement::InnerTypeConstraintElement(std::auto_ptr<ConstraintElementVector> list,
                                                       bool part)
  : ElementListConstraintElement(list)
{
  partial = part;
}


void InnerTypeConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << "WITH COMPONENTS { ";

  if (partial)
    strm << "..., ";

  for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++) {
    if (i > 0)
      strm << ", ";
    elements[i]->PrintOn(strm);
  }

  strm << " }";
}


void InnerTypeConstraintElement::GenerateCplusplus(const std::string & fn, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++)
    elements[i]->GenerateCplusplus(fn, hdr, cxx, inl);
}

/////////////////////////////////////////////////////////

UserDefinedConstraintElement::UserDefinedConstraintElement(ActualParameterListPtr list)
: parameters(list)
{
}

void UserDefinedConstraintElement::PrintOn(std::ostream & strm) const
{
  strm << "CONSTRAINED BY { ";
  for (size_t i = 0; i < parameters->size(); i++) {
    if (i > 0)
      strm << ", ";
    strm << * (*parameters)[i];
  }
  strm << " }";
}


void UserDefinedConstraintElement::GenerateCplusplus(const std::string &, std::ostream &, std::ostream &, std::ostream &) const
{
}

////////////////////////////////////////////////////////////

TableConstraint::TableConstraint(boost::shared_ptr<DefinedObjectSet> os,
                std::auto_ptr<StringList> as)
: objSet(os), atNotations(as)
{
}

TableConstraint::~TableConstraint()
{
}

std::string TableConstraint::GetObjectSetIdentifier() const
{
    return MakeIdentifierC(objSet->GetName());
}


bool TableConstraint::ReferenceType(const TypeBase& type)
{
  return type.GetTypeName() == GetObjectSetIdentifier();
}

/////////////////////////////////////////////////////////

// if tag type assigned from module - breaks BER encoding
TypeBase::TypeBase(unsigned tagNum, ModuleDefinition* md)
  : tag(tagNum, Tag::Implicit), defaultTag(tagNum, Tag::Implicit) // tag(tagNum, md->GetDefaultTagMode()), defaultTag(tagNum, md->GetDefaultTagMode())
{
  isOptional = false;
  isGenerated = false;
  isValueSetType = false;
  module = md;
}


TypeBase::TypeBase(TypeBase& copy)
  : name(copy.name),
    identifier(MakeIdentifierC(copy.name)),
    tag(copy.tag),
    defaultTag(copy.tag)
{
  isOptional = copy.isOptional;
  isGenerated = false;
  isValueSetType = false;
  module = Module;
}


void TypeBase::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  PrintFinish(strm);
}


void TypeBase::PrintStart(std::ostream & strm) const
{
  strm << indent();
  if (name.size()) {
    strm << name << parameters << ": ";
  }
  strm << tag << GetClass() << ' ';
  Module->SetIndentLevel(1);
}


void TypeBase::PrintFinish(std::ostream & strm) const
{
  Module->SetIndentLevel(-1);
  strm << ' ' << constraints;
  if (isOptional)
    strm << " OPTIONAL";
  if (defaultValue.get() != NULL)
    strm << " DEFAULT " << *defaultValue;
  strm << endl;
}


void TypeBase::SetName(const std::string& newName)
{
  name = newName;
  identifier = MakeIdentifierC(name);
}

void TypeBase::SetDefaultValue(ValuePtr value)
{
  defaultValue = value;
}


void TypeBase::AdjustIdentifier(bool)
{
  identifier = Module->GetPrefix() + MakeIdentifierC(name);
}


void TypeBase::SetTag(Tag::Type type, unsigned num, Tag::Mode mode)
{
  tag.type = type;
  tag.number = num;
  tag.mode = mode;
}


void TypeBase::SetParameters(ParameterList& list)
{
  parameters.swap(list);
}


void TypeBase::MoveConstraints(TypeBase& from)
{
  constraints.insert(constraints.end(),
                     from.constraints.begin(),
                     from.constraints.end());
  from.constraints.resize(0);
}

void TypeBase::CopyConstraints(const TypeBase& from)
{
  for (size_t i = 0; i < from.constraints.size(); ++i) {
    std::auto_ptr<Constraint> cons = from.constraints[i]->Clone();
    constraints.push_back(ConstraintPtr(cons));
  }
}

void TypeBase::FlattenUsedTypes()
{
}


TypePtr TypeBase::FlattenThisType(TypePtr& self, const TypeBase &)
{
  return self;
}


bool TypeBase::IsChoice() const
{
  return false;
}


bool TypeBase::IsParameterizedType() const
{
  return false;
}


bool TypeBase::IsPrimitiveType() const
{
  return true;
}



void TypeBase::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision());
  if (IsPrimitiveType() && !NeedGenInfo() )
  {
    hdr << indent << "typedef "<< GetTypeName() << ' '
        << GetIdentifier() << ";\n\n";
  }
  else
  {
    BeginGenerateCplusplus(hdr, cxx, inl);

    hdr << indent+4 << GetIdentifier() << "(const " << GetIdentifier() << "& that) : Inherited(that) {}\n";

    EndGenerateCplusplus(hdr, cxx, inl);
  }
}


void TypeBase::GenerateForwardDecls(std::ostream &)
{
}


void TypeBase::GenerateOperators(std::ostream &, std::ostream &, const TypeBase &)
{
}


std::string TypeBase::GetTypeName() const
{
  return GetAncestorClass();
}


bool TypeBase::CanReferenceType() const
{
  return false;
}


bool TypeBase::ReferencesType(const TypeBase &) const
{
  return false;
}


bool TypeBase::IsParameterisedImport() const
{
  return false;
}


void TypeBase::BeginGenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  shortClassNameString = GetIdentifier();

  parameters.GenerateCplusplus(templatePrefix, shortClassNameString);

  if (outerClassName.size())
  classNameString = outerClassName + "::" + shortClassNameString;
  else
  classNameString = shortClassNameString;

  // Output header file declaration of class
  Indent indent(hdr.precision()) ;

  if (hdr.precision() == 0)
  {
    hdr << "//\n"
           "// " << GetClassNameString() << "\n"
           "//\n"
           "\n";
  }

  cxx << "//\n"
         "// " << GetClassNameString() << "\n"
         "//\n"
         "\n";

  GenerateForwardDecls(hdr);

  if (outerClassName.size() == 0)
    hdr << templatePrefix;
  hdr << indent << "class " ;
  hdr << GetIdentifier() << " : public " << GetTypeName() << "\n"
      << indent << "{" << endl
      << indent << "    typedef " << GetTypeName() << " Inherited;\n";

  hdr << indent << "protected:\n"
      << indent << "    typedef Inherited::InfoType InfoType;\n"
      << indent << "    " << GetIdentifier() << "(const void* info) : Inherited(info) {}\n"
      << indent << "public:\n";

  GenerateConstructors(hdr, cxx, inl);
}


void TypeBase::EndGenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  GenerateOperators(hdr, cxx, *this);
  Indent indent(hdr.precision()+4);

  // Output header file declaration of class
  hdr << indent << shortClassNameString << " * clone() const";
  if(noInlineFiles)
  {
    hdr << endl
        << indent << "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }" << "\n";
  }
  else
  {
    hdr << ";\n";

    inl << GetTemplatePrefix()
        << "inline " << GetClassNameString() << "* " << GetClassNameString() << "::clone() const\n"
        << "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }" << "\n\n";
  }
  cxx << "\n";

  hdr << indent << "static bool equal_type(const ASN1::AbstractData& type)";
  if(noInlineFiles)
  {
    hdr << endl
        << indent << "{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }" << "\n";
  }
  else
  {
    hdr << ";\n";

    inl << GetTemplatePrefix()
        << "inline bool " << GetClassNameString() << "::equal_type(const ASN1::AbstractData& type)\n"
        << "{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }" << "\n\n";
  }

  GenerateInfo(this, hdr, cxx);

  hdr << indent-4 << "}; // end class " << shortClassNameString << "\n\n";

  isGenerated = true;
}

void TypeBase::GenerateInfo(const TypeBase* type, std::ostream & hdr, std::ostream& cxx)
{
  Indent indent(hdr.precision()) ;
  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
  cxx << GetTemplatePrefix()
      << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
      << "    " << GetAncestorClass() << "::create,\n"
      << "    ";
  type->GenerateTags(cxx);
  cxx << ",\n    &" << GetAncestorClass() << "::theInfo,\n"
      << "};\n\n";
}


void TypeBase::GenerateTags(std::ostream & cxx) const
{
  const int expl (tag.isDefined() && tag.mode==Tag::Explicit ? 1 : 0);

  cxx << "0x" << std::hex << std::setw(8) << std::setfill('0')
      << (tag.type << 30 | (expl<<29) | tag.number)
      << std::dec << std::setw(0) << std::setfill(' ');
}


void TypeBase::GenerateCplusplusConstraints(const std::string & prefix, std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  for (size_t i = 0; i < constraints.size(); i++)
    constraints[i]->GenerateCplusplus( "  "+ prefix + "setConstraints(", hdr, cxx, inl);
}

void TypeBase::BeginParseValue() const
{
  BeginParseThisTypeValue();
}

void TypeBase::EndParseValue() const
{
  BraceTokenContext = InObjectSetContext ? OBJECT_BRACE : '{';
  EndParseThisTypeValue();
  ValueTypeContext.reset();
}

void TypeBase::BeginParseValueSet() const
{
  BraceTokenContext = VALUESET_BRACE;
  BeginParseValue();
}

void TypeBase::EndParseValueSet() const
{
  EndParseValue();
}

bool TypeBase::FwdDeclareMe(std::ostream & hdr)
{
  if (CanBeFwdDeclared(true))
  {
    hdr << Indent(hdr.precision()) << "class " << GetTypeName()<< ";\n";
    return true;
  }
  return false;
}

std::string TypeBase::GetPrimitiveType(const std::string& myName) const
{
  if(!myName.empty())
    return myName + "::const_reference";

  return std::string();
}

bool TypeBase::CanBeFwdDeclared(bool ) const
{ return false; }

bool TypeBase::HasConstraints() const
{
  return constraints.size() > 0;
}

void TypeBase::RemovePERInvisibleConstraint(const ParameterPtr& vp)
{
  constraints.erase(
    std::remove_if(constraints.begin(), constraints.end(),
      boost::bind(&Constraint::HasPERInvisibleConstraint, _1, boost::ref(*vp))),
    constraints.end());
}


void TypeBase::RemovePERInvisibleConstraints()
{
  std::for_each(parameters.rep.begin(), parameters.rep.end(),
    boost::bind(&TypeBase::RemovePERInvisibleConstraint, this, _1));
}


const std::string& TypeBase::GetCModuleName() const
{
  return module->GetCModuleName();
}


void TypeBase::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision()) ;
  hdr  << indent +4 << GetIdentifier() << "() : Inherited(&theInfo) {}\n" ;
}


TypePtr TypeBase::SeqOfFlattenThisType(const TypeBase & parent, TypePtr result)
{
  if (!IsPrimitiveType() || NeedGenInfo() || HasNonStandardTag())
    result.reset(new DefinedType(result, parent));
  return result;;

}

bool TypeBase::NeedGenInfo() const
{
    if (HasNonStandardTag() && isupper(name[0]))
        return true;
    return false;
}


const char* TypeBase::GetClass() const
{
  return typeid(*this).name();
}
/////////////////////////////////////////////////////////

DefinedType::DefinedType(const std::string& name)
  : TypeBase(Tag::IllegalUniversalTag, Module),
    referenceName(name)
{
  unresolved = true;
}

DefinedType::DefinedType(TypePtr refType)
: TypeBase(*refType)
{
  CopyConstraints(*refType);
  baseType = refType;
  unresolved = false;
}

DefinedType::DefinedType(TypePtr refType, TypePtr& bType)
  : TypeBase(*refType),
    referenceName(bType->GetName())
{
  MoveConstraints(*refType);

  baseType = bType;
  unresolved = false;
}


DefinedType::DefinedType(TypePtr refType, const std::string & refName)
  : TypeBase(*refType)
{
  MoveConstraints(*refType);
  ConstructFromType(refType, refName);
}


DefinedType::DefinedType(TypePtr refType, const TypeBase & parent)
  : TypeBase(*refType)
{
  if (name.size())
    ConstructFromType(refType, parent.GetName() + '_' + name);
  else
    ConstructFromType(refType, parent.GetName() + "_subtype");
}

void DefinedType::ConstructFromType(TypePtr& refType, const std::string & refName)
{
  referenceName = refName;
  refType->SetName(refName);

  if (refName != "" || !refType->IsPrimitiveType() || refType->HasConstraints()){
    Module->AddType(refType);
  }

  baseType = refType;
  unresolved = false;
}


void DefinedType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << referenceName << ' ';
  PrintFinish(strm);
}


bool DefinedType::CanReferenceType() const
{
  return true;
}


bool DefinedType::IsChoice() const
{
  if (baseType.get())
    return baseType->IsChoice();
  return false;
}


bool DefinedType::IsParameterizedType() const
{
  if (baseType.get())
    return baseType->IsParameterizedType();
  return false;
}


bool DefinedType::ReferencesType(const TypeBase & type) const
{
  ResolveReference();
  return type.GetName() == referenceName;
}

bool DefinedType::UseType(const TypeBase & type) const
{
  return type.GetName() == referenceName;
}

void DefinedType::GenerateOperators(std::ostream & hdr, std::ostream & cxx, const TypeBase & actualType)
{
  if (baseType.get())
  {
    std::string basicTypeName = baseType->GetPrimitiveType(std::string());

    if (!basicTypeName.empty())
    {
//      if (basicTypeName.find("::value_type::") != -1)
  //      basicTypeName = basicTypeName.substr(basicTypeName.find_last_of(':')+1);
      Indent indent(hdr.precision() + 4);
      hdr << indent << GetIdentifier() << "(" << basicTypeName << " v, const void* info =&theInfo)"
          << " : Inherited(v, info) {}\n";
    }

    baseType->GenerateOperators(hdr, cxx, actualType);
  }
  else
    std::cout << "Cannot find type " << referenceName << " to generate operators\n";
}


const char * DefinedType::GetAncestorClass() const
{
  if (baseType.get())
    return baseType->GetAncestorClass();
  return NULL;
}


std::string DefinedType::GetTypeName() const
{
  ResolveReference();
  if (baseType.get() == NULL)
    return MakeIdentifierC(referenceName);

  std::string result = baseType->GetIdentifier();

  if (baseType->GetIdentifier().size() == 0 || result == GetIdentifier())
    return baseType->GetTypeName();

  if (GetCModuleName() != Module->GetCModuleName())
    result = GetCModuleName() + "::" + result;
  return result;
}

void DefinedType::BeginParseThisTypeValue() const
{
  ResolveReference();
  if (baseType.get())
    baseType->BeginParseThisTypeValue();
  else
  {// used when this type is an INTEGER and the subsequent value is a NamedNumber.
    IdentifierTokenContext = VALUEREFERENCE;
  }
}

void DefinedType::EndParseThisTypeValue() const
{
  if (baseType.get())
    baseType->EndParseThisTypeValue();
  else
    IdentifierTokenContext = IDENTIFIER;
}

void DefinedType::ResolveReference() const
{
  if (unresolved) {
    unresolved = false;

    if (Module == 0)
      Module = module;
    baseType = Module->FindType(referenceName);

    // AR Tag should not be fetched from base type
    // That only confuses SEQUENCE code generation
#if 0
    if (baseType.get() != NULL)
    {
      if (!HasNonStandardTag())
        ((Tag&)defaultTag) = ((Tag&)tag) = baseType->GetTag();
    }
#endif
  }
}

void DefinedType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  if (constraints.empty() && !HasNonStandardTag())
  {
    hdr << Indent(hdr.precision()) << "typedef " << GetTypeName()
        << ' ' << GetIdentifier() << ";\n";
  }
  else
  {
    TypeBase::GenerateCplusplus(hdr, cxx, inl);
  }
}


bool DefinedType::CanBeFwdDeclared(bool isComponent) const
{
  ResolveReference();
  if (isComponent && baseType.get())
    return baseType->CanBeFwdDeclared();
  else if (constraints.empty() && !HasNonStandardTag())
    return false;
  else
    return true;
}

const std::string& DefinedType::GetCModuleName() const
{
  if (GetName() == "" )
  {
    ResolveReference();
    if (baseType.get())
      baseType->GetCModuleName();
  }
  return module->GetCModuleName();
}

TypeBase::RemoveResult DefinedType::CanRemoveType(const TypeBase& type)
{
    return ReferencesType(type) ? MAY_NOT : OK;
}

bool DefinedType::RemoveThisType(const TypeBase& type)
{
    return ReferencesType(type);
}

std::string DefinedType::GetPrimitiveType(const std::string &idInContext) const
{
  if (baseType.get())
    return baseType->GetPrimitiveType(idInContext);
  else
    return TypeBase::GetPrimitiveType(idInContext);
}

bool DefinedType::NeedGenInfo() const
{
    return TypeBase::NeedGenInfo() || !constraints.empty();
}

void DefinedType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
  if (baseType.get())
    baseType->GenerateInfo(type, hdr, cxx);
}

TypePtr DefinedType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  TypePtr result = self;
  if (NeedGenInfo())
  {
      if (parent.HasParameters())
      {
          size_t i;
          const SubTypeConstraintElement*  subcons = NULL;
          for (i=0; i != constraints.size(); ++i)
          {
              if ((subcons = constraints[i]->GetSubTypeConstraint()) != NULL)
              {
                  const TypePtr subtype = subcons->GetSubType();
                  ParameterListRep paramList= parent.GetParameters().rep;
                  for (i = 0; i < paramList.size(); ++i)
                  {
                      if (paramList[i]->GetName() == subtype->GetTypeName())
                      {
                          parameters.rep.push_back(paramList[i]);
                          result.reset(
                              new ParameterizedType(self,
                                                    parent,
                                                    *parameters.MakeActualParameters()));
                          return result;
                      }
                  }
                  break;
              }
          }
      }
      result.reset(new DefinedType(self, parent));
  }

  return result;
}

/////////////////////////////////////////////////////////

ParameterizedType::ParameterizedType(const std::string& name, ActualParameterList& args)
  : DefinedType(name)
{
  arguments.swap(args);
}

ParameterizedType::ParameterizedType(TypePtr& refType,
                   const TypeBase& parent,
                   ActualParameterList& args)
  : DefinedType(refType, parent)
{
  arguments.swap(args);
}


void ParameterizedType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << referenceName << " { ";
  for (size_t i = 0; i < arguments.size(); i++) {
    if (i > 0)
      strm << ", ";
    strm << *arguments[i];
  }
  strm << " }";
  PrintFinish(strm);
}


bool ParameterizedType::IsParameterizedType() const
{
  return true;
}


bool ParameterizedType::ReferencesType(const TypeBase & type) const
{
  if (std::find_if(arguments.begin(), arguments.end(),
                   boost::bind(&ActualParameter::ReferencesType, _1, boost::cref(type))) != arguments.end())
    return true;

  return DefinedType::ReferencesType(type);
}

bool ParameterizedType::UseType(const TypeBase & type) const
{
  if (std::find_if(arguments.begin(), arguments.end(),
                   boost::bind(&ActualParameter::UseType, _1, boost::cref(type))) != arguments.end())
    return true;

  return DefinedType::UseType(type);
}

std::string ParameterizedType::GetTypeName() const
{
  std::string typeName = DefinedType::GetTypeName();
  if (IsParameterizedType()) {
    typeName += '<';
    for (size_t i = 0; i < arguments.size(); ++i) {
      if (arguments[i]->GenerateTemplateArgument(typeName))
        typeName += ", ";
    }
    typeName[typeName.size()-2] = '>';
  }

  return typeName;
}

TypeBase::RemoveResult ParameterizedType::CanRemoveType(const TypeBase& type)
{
  if (std::find_if(arguments.begin(), arguments.end(),
                   boost::bind(&ActualParameter::ReferencesType, _1, boost::cref(type))) != arguments.end())
    return FORBIDDEN;

  return DefinedType::CanRemoveType(type);
}


/////////////////////////////////////////////////////////

SelectionType::SelectionType(const std::string& name, TypePtr base)
  : TypeBase(Tag::IllegalUniversalTag, Module),
    selection(name)
{
  assert(base.get());
  baseType = base;
}


SelectionType::~SelectionType()
{
}


void SelectionType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << selection << '<' << *baseType;
  PrintFinish(strm);
}


void SelectionType::FlattenUsedTypes()
{
  baseType = baseType->FlattenThisType(baseType, *this);
}


TypePtr SelectionType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  return TypePtr(new DefinedType(self, parent));
}


void SelectionType::GenerateCplusplus(std::ostream &, std::ostream &, std::ostream &)
{
  std::cerr << StdError(Fatal) << "Cannot generate code for Selection type" << endl;
  isGenerated = true;
}


const char * SelectionType::GetAncestorClass() const
{
  return "";
}


bool SelectionType::CanReferenceType() const
{
  return true;
}


bool SelectionType::ReferencesType(const TypeBase & type) const
{
  return baseType->ReferencesType(type);
}

bool SelectionType::UseType(const TypeBase& type) const
{
  return baseType->UseType(type);
}


/////////////////////////////////////////////////////////

BooleanType::BooleanType()
  : TypeBase(Tag::UniversalBoolean, Module)
{
}


void BooleanType::GenerateOperators(std::ostream & hdr, std::ostream & , const TypeBase & actualType)
{
  Indent indent(hdr.precision()+4);
  hdr << indent << actualType.GetIdentifier() << " & operator=(bool v)"
      << " { BOOLEAN::operator=(v);  return *this; }\n";
}


const char * BooleanType::GetAncestorClass() const
{
  return "ASN1::BOOLEAN";
}


void BooleanType::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision()+4) ;
  hdr << indent << GetIdentifier() << "(bool b = false, const void* info =&theInfo) : Inherited(b, info) {}\n"
      << indent << GetIdentifier() << "(const void* info) : Inherited(info) {}\n";
}


/////////////////////////////////////////////////////////

IntegerType::IntegerType()
  : TypeBase(Tag::UniversalInteger, Module)
{
}


IntegerType::IntegerType(NamedNumberList& lst)
  : TypeBase(Tag::UniversalInteger, Module)
{
  allowedValues.swap(lst);
}

const char * IntegerType::GetAncestorClass() const
{
  return "ASN1::INTEGER";
}

void IntegerType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision()+4);
  if (!allowedValues.empty())
  {
    BeginGenerateCplusplus(hdr, cxx, inl);

    int maxEnumValue = 0;
    NamedNumberList::iterator first, last = allowedValues.end();
    for (first = allowedValues.begin() ; first != last ; ++first) {
      int num = (*first)->GetNumber();
      if (maxEnumValue < num)
        maxEnumValue = num;
    }

    // Generate enumerations and complete the constructor implementation
    hdr << indent << "enum NamedNumber {\n";


    int prevNum = -1;
    for (first = allowedValues.begin() ; first != last ; ++first) {
      if (first != allowedValues.begin()) {
        hdr << ",\n";
      }

      hdr << indent+4 << MakeIdentifierC((*first)->GetName());

      int num = (*first)->GetNumber();
      if (num != prevNum+1) {
        hdr << " = " << num;
      }
      prevNum = num;

    }

    hdr << "\n"
        << indent << "};\n"
           "\n";

    for (first = allowedValues.begin() ; first != last ; ++first) {
      std::string fname = MakeIdentifierC((*first)->GetName());
      hdr << indent << "bool is_" << fname << "() const { return value == " << fname << "; }\n"
          << indent << "void set_" << fname << "() { value = " << fname << "; }\n\n";
    }

    EndGenerateCplusplus(hdr, cxx, inl);
  }
  else
  {
    TypeBase::GenerateCplusplus(hdr, cxx, inl);
    /*
    if (type->GetConstraints().size())
    {
      hdr << indent << "enum {\n"
        << indent << "  LowerLimit = " <<  << ",\n"
        << indent << "  UpperLimit = " <<  << ",\n"
        << indent << "};\n";
    }
    */
  }
}

void IntegerType::GenerateInfo(const TypeBase* type, std::ostream& hdr , std::ostream& cxx)
{
  Indent indent(hdr.precision()+4);
  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
  if ( !allowedValues.empty() )
  {
    hdr << indent-4 << "private:\n"
        << indent << "static const NameEntry nameEntries[" << allowedValues.size() << "];\n";

    cxx << type->GetTemplatePrefix()
        << "const " << type->GetClassNameString() << "::NameEntry " << type->GetClassNameString()
        << "::nameEntries[" << allowedValues.size() << "] = {\n";

    NamedNumberList::iterator itr, last = allowedValues.end();
    for (itr = allowedValues.begin() ; itr != last ; ++itr ) {
      if (itr != allowedValues.begin())
        cxx << ",\n";

      cxx << "    { " << (*itr)->GetNumber() << ", \""
          << (*itr)->GetName() << "\" }";
    }

    cxx << "\n"
        << "};\n\n";
  }

  cxx << type->GetTemplatePrefix()
      << "const " ;

  if (type->GetTemplatePrefix().length())
    cxx << "typename ";

  cxx  << type->GetClassNameString() << "::InfoType " << type->GetClassNameString() << "::theInfo = {\n"
       << "    create,\n" ;

  cxx << "    ";

  type->GenerateTags(cxx);

  cxx << ",\n"
      << "    &" << GetAncestorClass() << "::theInfo,\n"
      << "    ";

  if (type->GetConstraints().size())
  {
    std::string strm;
    type->GetConstraints()[0]->GetConstraint(strm);
    cxx << strm;
  }
  else
    cxx << "ASN1::Unconstrained, 0, UINT_MAX";

  cxx << "\n";
  if ( allowedValues.size()  )
  {
     cxx << "    , nameEntries, " << allowedValues.size() << "\n";
  }

  cxx << "};\n\n";
}


std::string IntegerType::GetTypeName() const
{
  if (allowedValues.size())
    return "ASN1::IntegerWithNamedNumber";

  if (constraints.size())
  {
    std::string result("ASN1::Constrained_INTEGER<");
    constraints[0]->GetConstraint(result);
    result += "> ";
    return result;
  }
  else
    return GetAncestorClass();
}

void IntegerType::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision() + 4) ;
  hdr << indent
      << GetIdentifier() << "(int_type v =0, const void* info =&theInfo) : Inherited(v, info) {}\n" ;
}

void IntegerType::GenerateOperators(std::ostream & hdr, std::ostream & cxx, const TypeBase & actualType)
{
  Indent indent(hdr.precision()+4);

  if (!allowedValues.empty())
  {
    hdr << indent << GetIdentifier() << "(NamedNumber v, const void* info =&theInfo) : Inherited(v, info) {}\n";

    hdr << indent << actualType.GetIdentifier() << " & operator=(int_type v)";
    hdr << " { setValue(v); return *this; }\n";

    hdr << indent << "operator NamedNumber() const { return NamedNumber(getValue()); }\n\n";
  }
}


bool IntegerType::NeedGenInfo() const
{
    return TypeBase::NeedGenInfo() || allowedValues.size() > 0;
}

TypePtr IntegerType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  TypePtr result = self;
  if (NeedGenInfo() && parent.HasParameters())
  {
    size_t i;
    const SubTypeConstraintElement*  subcons = NULL;
    for (i=0; i != constraints.size(); ++i)
    {
      if ((subcons = constraints[i]->GetSubTypeConstraint())!= NULL)
      {
        const TypePtr subtype = subcons->GetSubType();
        ParameterListRep paramList= parent.GetParameters().rep;
        for (size_t i = 0; i < paramList.size(); ++i)
        {
          if (paramList[i]->GetName() == subtype->GetTypeName())
          {
            parameters.rep.push_back(paramList[i]);
            result.reset(new ParameterizedType(self,
                                               parent,
                                               *parameters.MakeActualParameters()));
            return result;
          }
        }
        break;
      }
    }
    result.reset(new DefinedType(self, parent));
  }
  return result;
}

bool IntegerType::CanReferenceType() const
{
  if (constraints.size())
    return true;

  return false;
}


bool IntegerType::ReferencesType(const TypeBase & type) const
{
  if (constraints.size())
    return constraints[0]->ReferencesType(type);
  else
    return false;
}

void IntegerType::BeginParseThisTypeValue() const
{
  if(!allowedValues.empty())
    IdentifierTokenContext = VALUEREFERENCE;
}

void IntegerType::EndParseThisTypeValue() const
{
  IdentifierTokenContext = IDENTIFIER;
}
/////////////////////////////////////////////////////////

EnumeratedType::EnumeratedType(NamedNumberList& enums, bool extend, NamedNumberList* ext)
  : TypeBase(Tag::UniversalEnumeration, Module),
  maxEnumValue(0)
{
  enumerations.swap(enums);
  numEnums = enumerations.size();
  extendable = extend;
  if (ext != NULL) {
    enumerations.splice( enumerations.end(), *ext);
    delete ext;
  }
}


void EnumeratedType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << endl;

  size_t i;
  NamedNumberList::const_iterator itr, last = enumerations.end();
  for (i = 0, itr = enumerations.begin() ; i < numEnums; i++, ++itr)
    strm << indent() << **itr << endl;

  if (extendable) {
    strm << "...\n";
    for (; itr != last; ++itr)
      strm << indent() << **itr << endl;
  }
  PrintFinish(strm);
}


TypePtr EnumeratedType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  return TypePtr(new DefinedType(self, parent));
}


void EnumeratedType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  NamedNumberList::iterator itr, last = enumerations.end();

  for (itr = enumerations.begin(); itr != last; ++itr) {
    int num = (*itr)->GetNumber();
    if (maxEnumValue < num)
      maxEnumValue = num;
  }

  Indent indent(hdr.precision()+4);

  BeginGenerateCplusplus(hdr, cxx, inl);

  // Generate enumerations and complete the constructor implementation
  hdr << indent << "enum NamedNumber {\n"
      << indent+4 << "unknownEnumeration_ = -1,\n";

  int prevNum = -1;
  for (itr = enumerations.begin(); itr != last; ++itr) {
    if (itr != enumerations.begin()) {
      hdr << ",\n";
    }

    hdr << indent+4 << MakeIdentifierC((*itr)->GetName());

    int num = (*itr)->GetNumber();
    if (num != prevNum+1) {
      hdr << " = " << num;
    }
    prevNum = num;
  }

  hdr << "\n"
      << indent <<   "};\n"
         "\n";

  GenerateCplusplusConstraints(std::string(), hdr, cxx, inl);

  for (itr = enumerations.begin(); itr != last; ++itr) {
    std::string fname = MakeIdentifierC((*itr)->GetName());
    hdr << indent << "bool is_" << fname << "() const { return value == " << fname << "; }\n"
        << indent << "void set_" << fname << "() { value = " << fname << "; }\n\n";
  }

  EndGenerateCplusplus(hdr, cxx, inl);
}


void EnumeratedType::GenerateOperators(std::ostream & hdr, std::ostream & , const TypeBase & actualType)
{
  Indent indent(hdr.precision()+4);

  hdr << indent << GetIdentifier() << "(const NamedNumber v) : Inherited(&theInfo)"
      << " { setFromInt(v); }\n";

  hdr << indent << actualType.GetIdentifier() << " & operator=(const NamedNumber v)";
  hdr << " { setFromInt(v);  return *this; }\n";

  hdr << indent << "operator NamedNumber() const { return NamedNumber(asInt()); }\n\n";

  hdr << indent << "bool operator == (NamedNumber rhs) const { return value == rhs; }\n"
      << indent << "bool operator != (NamedNumber rhs) const { return value != rhs; }\n"
      << indent << "bool operator <  (NamedNumber rhs) const { return value <  rhs; }\n"
      << indent << "bool operator >  (NamedNumber rhs) const { return value >  rhs; }\n"
      << indent << "bool operator <= (NamedNumber rhs) const { return value <= rhs; }\n"
      << indent << "bool operator >= (NamedNumber rhs) const { return value >= rhs; }\n"
      << indent << "bool operator == (const " << GetIdentifier() << " &rhs) const { return value == rhs.value; }\n"
      << indent << "bool operator != (const " << GetIdentifier() << " &rhs) const { return value != rhs.value; }\n"
      << "\n";

  hdr << indent << "void swap (" << GetIdentifier() << " & that) { Inherited::swap(that); }\n";
}


const char * EnumeratedType::GetAncestorClass() const
{
  return "ASN1::ENUMERATED";
}

void EnumeratedType::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
   Indent indent(hdr.precision()+4);
   hdr << indent << GetIdentifier() << "() : Inherited(&theInfo) {}\n";
}

bool EnumeratedType::IsPrimitiveType() const
{
    return false;
}

void EnumeratedType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
   Indent indent(hdr.precision()+4);
   hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";

   hdr << indent-4 << "private:\n"
       << indent << "static const char * nameList[];\n";

   cxx << type->GetTemplatePrefix()
       << "const char * " << type->GetClassNameString()
       << "::nameList[] = {\n"
          "    \"";

  NamedNumberList::iterator itr, last = enumerations.end();

  for(int i=0; i<=maxEnumValue; i++) {
    itr = std::find_if(enumerations.begin(), last, CompareNamedNumber (i));
    if (i>0)
      cxx << "\",\n    \"";

    if(itr!=last)
      cxx << (*itr)->GetName();
    else
      cxx << "<undefined>=" << i ;
  }

  cxx << "\"\n"
      << "  };\n\n";

  cxx << GetTemplatePrefix()
      << "const " << type->GetClassNameString() << "::InfoType " << type->GetClassNameString() << "::theInfo = {\n"
      << "    ASN1::ENUMERATED::create,\n"
      << "    ";

  type->GenerateTags(cxx);

  cxx << ",\n    0"
      << ",\n    " << extendable
      << ",\n    " << maxEnumValue << ", nameList"
         "\n};\n\n";
}

void EnumeratedType::BeginParseThisTypeValue() const
{
  IdentifierTokenContext = VALUEREFERENCE;
}

void EnumeratedType::EndParseThisTypeValue() const
{
  IdentifierTokenContext = IDENTIFIER;
}

/////////////////////////////////////////////////////////

RealType::RealType()
  : TypeBase(Tag::UniversalReal, Module)
{
}


const char * RealType::GetAncestorClass() const
{
  return "ASN1::REAL";
}

void RealType::GenerateOperators(std::ostream & hdr, std::ostream & , const TypeBase & actualType)
{
  Indent indent(hdr.precision());
  hdr << indent << "    " << actualType.GetIdentifier() << " & operator=(double v)";
  hdr << " { BinaryReal::operator=(v);  return *this; }\n";
}


void RealType::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision()) ;
  hdr  << indent << "    " << GetIdentifier() << "(double v = 0) : Inherited(v) {}\n" ;
}

/////////////////////////////////////////////////////////

BitStringType::BitStringType()
  : TypeBase(Tag::UniversalBitString, Module)
{
}


BitStringType::BitStringType(NamedNumberList& lst)
  : TypeBase(Tag::UniversalBitString, Module)
{
  allowedBits.swap(lst);
}


const char * BitStringType::GetAncestorClass() const
{
  return "ASN1::BIT_STRING";
}


std::string BitStringType::GetTypeName() const
{
  if (constraints.size())
  {
    std::string result("ASN1::Constrained_BIT_STRING<");
    constraints[0]->GetConstraint(result);
    result += "> ";
    return result;
  }
  else
    return GetAncestorClass();
}

bool BitStringType::NeedGenInfo() const
{
    return TypeBase::NeedGenInfo() || allowedBits.size() > 0;
}

void BitStringType::BeginParseThisTypeValue() const
{
  IdentifierTokenContext = BIT_IDENTIFIER;
}

void BitStringType::EndParseThisTypeValue() const
{
  IdentifierTokenContext = IDENTIFIER;
}

int BitStringType::GetToken() const
{
  return BIT_IDENTIFIER;
}

void BitStringType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision()+4);

  if (!allowedBits.empty())
  {
    BeginGenerateCplusplus(hdr, cxx, inl);

    // Generate enumerations and complete the constructor implementation
    hdr << endl << indent << "enum NamedBits {\n";

    int prevNum = -1;

    NamedNumberList::iterator itr, last = allowedBits.end();

    for (itr = allowedBits.begin() ; itr != last ; ++itr) {
      if (itr != allowedBits.begin()) {
        hdr << ",\n";
      }

      hdr << indent+4 << MakeIdentifierC((*itr)->GetName());

      int num = (*itr)->GetNumber();
      if (num != prevNum+1) {
        hdr << " = " << num;
      }
      prevNum = num;
    }

    hdr << "\n"
        << indent << "};\n"
           "\n";

    EndGenerateCplusplus(hdr, cxx, inl);
  }
  else
    TypeBase::GenerateCplusplus(hdr, cxx, inl);
}

void BitStringType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
  Indent indent(hdr.precision()+4);
  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";

  int maxNamedValue = 0;

  if(!allowedBits.empty()) {

    hdr << indent -4 << "private:\n"
        << indent << "static const char * nameList[];\n";

    cxx << type->GetTemplatePrefix()
        << "const char * " << type->GetClassNameString()
        << "::nameList[] = {\n"
           "    \"";

    NamedNumberList::iterator itr, last = allowedBits.end();

    for (itr = allowedBits.begin() ; itr != last ; ++itr) {
      int num = (*itr)->GetNumber();
      if (maxNamedValue < num)
        maxNamedValue = num;
    }

    for(int i=0; i<=maxNamedValue; i++) {
      itr = std::find_if(allowedBits.begin(), last, CompareNamedNumber (i));
      if (i>0)
        cxx << "\",\n    \"";

      if(itr!=last)
        cxx << (*itr)->GetName();
      else
        cxx << '0' ;
    }
    cxx << "\"\n"
           "};\n\n";
  }


  cxx << type->GetTemplatePrefix()
      << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n";

  cxx << "    ASN1::BIT_STRING::create,\n";

  cxx << "    ";

  type->GenerateTags(cxx);

  cxx << ",\n    &" << GetAncestorClass() << "::theInfo"
         ",\n    ";

  const SizeConstraintElement* sizeConstraint ;

  if (type->GetConstraints().size() &&
      ((sizeConstraint = type->GetConstraints()[0]->GetSizeConstraint()) != NULL))
  {
    std::string str;
    sizeConstraint->GetConstraint(str);
    cxx << str.substr(0, str.size()-2);
  }
  else
    cxx << "ASN1::Unconstrained, 0, UINT_MAX";

  if(!allowedBits.empty()) {
     cxx << ",\n    " << maxNamedValue+1
         << ", nameList";
  }
  else
    cxx << ",\n    " << "0, 0";

  cxx << "\n};\n\n";
}

/////////////////////////////////////////////////////////

OctetStringType::OctetStringType()
  : TypeBase(Tag::UniversalOctetString, Module)
{
}



const char * OctetStringType::GetAncestorClass() const
{
  return "ASN1::OCTET_STRING";
}


std::string OctetStringType::GetTypeName() const
{
  if (constraints.size())
  {
    std::string result(GetConstrainedType());
    result += '<';
    constraints[0]->GetConstraint(result);
    result += "> ";
    return result;
  }
  else
    return GetAncestorClass();
}

const char* OctetStringType::GetConstrainedType() const
{
  return "ASN1::Constrained_OCTET_STRING";
}

void OctetStringType::GenerateConstructors(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision()+4);
  TypeBase::GenerateConstructors(hdr, cxx, inl);

  hdr << indent   << GetIdentifier()  << "(size_type n, unsigned char v) : Inherited(n, v) {}\n"
      << indent   << "template <class Iterator>\n"
      << indent+2 << GetIdentifier() << "(Iterator first, Iterator last) : Inherited(first, last) {}\n"
      << indent   << GetIdentifier() << "(const ASN1_STD vector<char>& that) : Inherited(that) {}\n";

}

void OctetStringType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
  Indent indent(hdr.precision()+4);
  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
  cxx << type->GetTemplatePrefix()
      << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
      << "    " ;

  cxx << "ASN1::OCTET_STRING::create,\n";

  cxx << "    ";

  type->GenerateTags(cxx);

  cxx << ",\n    &" << GetAncestorClass() << "::theInfo,\n"
      << "    ";

  const SizeConstraintElement* sizeConstraint ;

  if (type->GetConstraints().size() &&
      (sizeConstraint = type->GetConstraints()[0]->GetSizeConstraint()) != NULL)
  {
    std::string str;
    sizeConstraint->GetConstraint(str);
    cxx << str.substr(0, str.size()-2);
  }
  else
    cxx << "ASN1::Unconstrained, 0, UINT_MAX";

  cxx << "\n"
      << "};\n\n";
}

/////////////////////////////////////////////////////////

NullType::NullType()
  : TypeBase(Tag::UniversalNull, Module)
{
}

void NullType::BeginParseThisTypeValue() const
{
  NullTokenContext = NULL_VALUE;
}

void NullType::EndParseThisTypeValue() const
{
  NullTokenContext = NULL_TYPE;
}

const char * NullType::GetAncestorClass() const
{
  return "ASN1::Null";
}


/////////////////////////////////////////////////////////

SequenceType::SequenceType(TypesVector* a_std,
                           bool extend,
                           TypesVector * ext,
                           unsigned tagNum)
  : TypeBase(tagNum, Module), detectingLoop(false)
{
  if (a_std != NULL) {
    numFields = a_std->size();
    a_std->swap(fields);
    delete a_std;
  }
  else
    numFields = 0;

  extendable = extend;
  if (ext != NULL) {
    fields.insert(fields.end(), ext->begin(), ext->end());
    delete ext;
  }
  needFwdDeclare.resize(fields.size());
}


void SequenceType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << endl;

  size_t i;
  for (i = 0; i < numFields; ++i)
    strm << *fields[i];

  if (extendable) {
    strm << indent() << "...\n";

    for (; i < fields.size(); ++i)
      strm << *fields[i];
  }

  PrintFinish(strm);
}


void SequenceType::FlattenUsedTypes()
{
  TypesVector::iterator itr, last = fields.end();

  for (itr = fields.begin(); itr != last; ++itr) {
    //[AR] Reset the base type's tag so explicit tags can be recognised
    TypePtr ptr(*itr);
    *itr = (*itr)->FlattenThisType(*itr, *this);
    if(ptr!=*itr)
      ptr->SetTag(ptr->GetDefaultTag());
  }
}


TypePtr SequenceType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  if (parent.HasParameters())
  {
    ParameterListPtr params = parent.GetParameters().GetReferencedParameters(*this);
    if (params.get())
    {
        SetParameters(*params);
        return TypePtr(new ParameterizedType(self,
                       parent,
                       *parameters.MakeActualParameters()));
    }
  }
  return TypePtr(new DefinedType(self, parent));
}


bool SequenceType::IsPrimitiveType() const
{
  return false;
}


void SequenceType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  size_t i;

  Indent indent(hdr.precision()+4);

  BeginGenerateCplusplus(hdr, cxx, inl);

  hdr << indent << GetIdentifier() << "(const " << shortClassNameString << " & that) : Inherited(that) {}\n\n";

  hdr << indent << shortClassNameString << "& operator=(const " << shortClassNameString << "& that)\n"
      << indent << "{ Inherited::operator=(that); return *this; }\n\n";
  // Output enum for optional parameters
  bool outputEnum = false;
  bool outputNumber = false;
  unsigned optionalId = 0;
  TypesVector::iterator itr, last = fields.end();
  for (i = 0, itr=fields.begin() ; itr != last ; ++i, ++itr) {
    TypeBase& field = **itr;
    if (i >= numFields || field.IsOptional()  || field.HasDefaultValue()) {
      if (outputEnum)
        hdr << ",\n";
      else {
        hdr << indent << "enum OptionalFields {\n";
        outputEnum = true;
      }

      if (!field.IsRemovedType())
      {
        hdr << indent << "  e_" << field.GetIdentifier();
        if (outputNumber)
          hdr << " = " << optionalId++;
      }
      else
      {
        optionalId++;
        outputNumber = true;
      }
    }
  }

  if (outputEnum)
    hdr << "\n"
        << indent <<   "};\n";

  // Output the Component scope classes and accessor/mutator functions
  std::stringstream tmpcxx;
  Unfreezer unfreezer(tmpcxx);
  for (i = 0, itr=fields.begin() ; itr != last ; ++i, ++itr) {
    GenerateComponent(**itr, hdr, tmpcxx, inl, i);
  }

  std::stringstream decoder;
  Unfreezer unfreezer1(decoder);
  for (itr=fields.begin() ; itr != last; ++itr) {
      (*itr)->GenerateDecoder(decoder);
  }

  hdr << indent << "void swap(" << GetIdentifier() << "& that)";
  if(noInlineFiles)
    hdr << " { Inherited::swap(that); }\n";
  else
  {
    hdr << ";\n";

    inl << GetTemplatePrefix()
        << "inline void " << GetClassNameString() << "::swap(" << GetIdentifier() << "& that)\n"
        << "{ Inherited::swap(that); }\n\n";
  }

  GenerateOperators(hdr, cxx, *this);

  // Output header file declaration of class
  hdr << indent << shortClassNameString << " * clone() const";
  if(noInlineFiles)
  {
    hdr << endl
        << indent << "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }" << "\n";
  }
  else
  {
    hdr << ";\n";

    inl << GetTemplatePrefix()
        << "inline " << GetClassNameString() << "* " << GetClassNameString() << "::clone() const\n"
        << "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }" << "\n\n";
  }

  cxx << "\n";

  GenerateInfo(this, hdr, cxx);

  if (!decoder.str().empty())
  {
	  hdr << indent << "static ASN1::AbstractData* create(const void*);\n"
          << indent << "bool do_accept(ASN1::Visitor& visitor);\n";

      cxx << GetTemplatePrefix()
          << "ASN1::AbstractData* "<< GetClassNameString() << "::create(const void* info)\n"
          << "{\n"
          << "    return new " << GetIdentifier() << "(info);\n"
          << "}\n"
          << "\n"
          << GetTemplatePrefix()
          << "bool "<< GetClassNameString() << "::do_accept(ASN1::Visitor& visitor)\n"
          << "{\n"
          << "  if (Inherited::do_accept(visitor))\n"
          << "  {\n"
          << "    if (!visitor.get_env())\n"
          << "      return true;\n"
          << decoder.str()
          << "  }\n"
          << "  return false;\n"
          << "}\n\n";
  }
  decoder << ends;
  indent -= 4;
  hdr << indent << "}; // end class " << shortClassNameString << "\n\n";

  cxx << tmpcxx.str();
  tmpcxx << ends;
  isGenerated = true;
}


const char * SequenceType::GetAncestorClass() const
{
  return "ASN1::SEQUENCE";
}


bool SequenceType::CanReferenceType() const
{
  return true;
}


bool SequenceType::UseType(const TypeBase& type) const
{
  TypesVector::const_iterator itr, last = fields.end();
  for (itr=fields.begin() ; itr != last; ++itr)
    if ((*itr)->UseType(type))
      return true;
  return false;
}


void SequenceType::GenerateComponent(TypeBase& field, std::ostream & hdr, std::ostream & cxx , std::ostream & inl, int id)
{
  if (field.IsRemovedType())
    return;

  std::string typeName =  field.GetTypeName();
  std::string componentIdentifier = field.GetIdentifier();
  std::string componentName = field.GetName();

  bool bIsOptional = (id >= (int)numFields || fields[id]->IsOptional() || fields[id]->HasDefaultValue());

  hdr.precision(hdr.precision() + 4);

  Indent indent(hdr.precision());

  // generate component scope class

  hdr << endl
      << indent << "class " << componentIdentifier << endl
      << indent << "{\n"
      << indent << "public:\n";

  indent += 4;

  field.SetOuterClassName(GetClassNameString() + "::" + componentIdentifier);
  field.SetTemplatePrefix(templatePrefix);
  field.SetName("value_type");

  hdr.precision(hdr.precision() + 4);
  if (!field.IsSequenceOfType() || !needFwdDeclare[id])
  {
    Tag ftag = field.GetTag();
    if(ftag.mode==Tag::Explicit)
      field.SetTag(field.GetDefaultTag());

    field.GenerateCplusplus(hdr, cxx, inl);
    field.SetTag(ftag);
  }
  else // recursive reference
  {
    hdr << indent << "class " << componentIdentifier << ";\n";
    SequenceOfType& type = *static_cast<SequenceOfType*>(&field);
    type.SetNonTypedef(true);
    std::stringstream dummy;
    type.GenerateCplusplus(inl, cxx, dummy);
  }
  hdr.precision(hdr.precision() - 4);

  field.SetName(componentName);

  hdr << indent << "typedef value_type&        reference;\n"
      << indent << "typedef const value_type&  const_reference;\n"
      << indent << "typedef value_type*        pointer;\n"
      << indent << "typedef const value_type*  const_pointer;\n";

  indent -= 4;
  hdr  << indent << "}; // end class " << componentIdentifier << "\n\n";

  std::string primitiveFieldType = field.GetPrimitiveType(componentIdentifier);
  std::string typenameKeyword;

  if (templatePrefix.length()) {
    typenameKeyword = "typename ";
    if (primitiveFieldType.find(componentIdentifier + "::") != -1)
      primitiveFieldType = "typename " + primitiveFieldType;
  }

  std::stringstream varName;
  Unfreezer unfreezer1(varName);
  varName << "*static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(fields[" << id << "])";

  std::stringstream constVarName;
  Unfreezer unfreezer2(constVarName);
  constVarName << "*static_cast<" << typenameKeyword << componentIdentifier << "::const_pointer>(fields[" << id << "])";

  // generate accessor/mutator functions

  if (!bIsOptional)// mandatory component
  {
    hdr << indent << typenameKeyword << componentIdentifier << "::const_reference get_" << componentIdentifier << " () const";
    if(noInlineFiles)
      hdr << " { return " << constVarName.str() << "; }\n";
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::const_reference " << GetClassNameString() << "::get_" << componentIdentifier << " () const\n"
          << "{ return " << constVarName.str() << "; }\n\n";
    }

    hdr << indent <<  typenameKeyword << componentIdentifier << "::reference ref_" << componentIdentifier << " ()";
    if(noInlineFiles)
      hdr << " { return " << varName.str() << "; }\n";
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::ref_" << componentIdentifier << " ()\n"
          << "{ return " << varName.str() << "; }\n\n";
    }

    hdr << indent << typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ()";
    if(noInlineFiles)
      hdr << " { return " << varName.str() << "; }\n";
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::set_" << componentIdentifier << " ()\n"
          << "{ return " << varName.str() << "; }\n\n";
    }

    hdr << indent << typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ("<< primitiveFieldType << " v)";
    // SUN cannot handle properly this function if inlined
    if(noInlineFiles || templatePrefix.length())
    {
      hdr << endl
          << indent << "{ return " << varName.str() << " = v; }\n";
    }
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::set_" << componentIdentifier << " ("<< primitiveFieldType << " v)\n"
          << "{ return " << varName.str() << " = v; }\n\n";
    }
  }
  else // optional component
  {
    std::string enumName = "e_" + componentIdentifier;
    if (field.GetTypeName() != "ASN1::Null")
    {
      hdr << indent << typenameKeyword << componentIdentifier << "::const_reference get_" << componentIdentifier << " () const";
      if(noInlineFiles)
      {
        hdr << endl
            << indent << "{\n"
            << indent << "  assert(hasOptionalField(" << enumName <<"));\n"
            << indent << "  return " << constVarName.str() << ";\n"
            << indent << "}\n";
      }
      else
      {
        hdr << ";\n";

        inl << GetTemplatePrefix()
            << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::const_reference " << GetClassNameString() << "::get_" << componentIdentifier << " () const\n"
            << "{\n"
            << "  assert(hasOptionalField(" << enumName <<"));\n"
            << "  return " << constVarName.str() << ";\n"
            << "}\n\n";
      }

      hdr << indent << typenameKeyword << componentIdentifier << "::reference ref_" << componentIdentifier << " ()";
      if(noInlineFiles)
      {
        hdr << endl
            << indent << "{\n"
            << indent << "  assert(hasOptionalField(" << enumName <<"));\n"
            << indent << "  return " << varName.str() << ";\n"
            << indent << "}\n";
      }
      else
      {
        hdr << ";\n";

        inl << GetTemplatePrefix()
            << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::ref_" << componentIdentifier << " ()\n"
            << "{\n"
            << "  assert(hasOptionalField(" << enumName <<"));\n"
            << "  return " << varName.str() << ";\n"
            << "}\n\n";
      }
    }

    hdr << indent << typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ()";
    if(noInlineFiles)
    {
      hdr << endl
          << indent << "{\n"
          << indent << "  includeOptionalField( "<< enumName << ", " << id << ");\n"
          << indent << "  return " << varName.str() << ";\n"
          << indent << "}\n";
    }
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::set_" << componentIdentifier << " ()\n"
          << "{\n"
          << "  includeOptionalField( "<< enumName << ", " << id << ");\n"
          << "  return " << varName.str() << ";\n"
          << "}\n\n";
    }

    if (field.GetTypeName() != "ASN1::Null")
    {
      hdr << indent << typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ("<< primitiveFieldType << " v)";
      // SUN cannot handle properly this function if inlined
      if(noInlineFiles || templatePrefix.length())
      {
        hdr << endl
            << indent << "{\n"
            << indent << "  includeOptionalField( "<< enumName << ", " << id << ");\n"
            << indent << "  return " << varName.str() << " = v;\n"
            << indent << "}\n";
      }
      else
      {
        hdr << ";\n";

        inl << GetTemplatePrefix()
            << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::set_" << componentIdentifier << " ("<< primitiveFieldType << " v)\n"
            << "{\n"
            << "  includeOptionalField( "<< enumName << ", " << id << ");\n"
            << "  return " << varName.str() << " = v;\n"
            << "}\n\n";
      }
    }

    hdr << indent << "void omit_" << componentIdentifier << " ()";
    if(noInlineFiles)
      hdr << " { removeOptionalField( " << enumName << "); }\n";
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline void " << GetClassNameString() << "::omit_" << componentIdentifier << " ()\n"
          << "{ removeOptionalField( " << enumName << "); }\n\n";
    }

    hdr << indent << "bool " << componentIdentifier << "_isPresent () const";
    if(noInlineFiles)
      hdr << " { return hasOptionalField( " << enumName << "); }\n";
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline bool " << GetClassNameString() << "::" << componentIdentifier << "_isPresent () const\n"
          << "{ return hasOptionalField( " << enumName << "); }\n\n";
    }
  }
  hdr.precision(hdr.precision()-4);
}

bool SequenceType::CanBeFwdDeclared(bool ) const
{ return !IsParameterizedType(); }


void SequenceType::RemovePERInvisibleConstraint(const ParameterPtr& param)
{
  TypesVector::iterator itr, last=fields.end();
  for (itr = fields.begin(); itr != last; ++itr)
    (*itr)->RemovePERInvisibleConstraint(param);
}

void SequenceType::GenerateForwardDecls(std::ostream & hdr)
{
  // Output forward declarations for choice pointers, but not standard classes
  bool needExtraLine = false;

  if (hdr.precision())
    return;

  for (size_t i = 0; i < fields.size(); i++) {
    TypeBase& field = *fields[i];
    if ( needFwdDeclare[i]  && field.FwdDeclareMe(hdr) ) {
      needExtraLine = true;
    }
  }

  if (needExtraLine)
    hdr << endl;
}

bool SequenceType::ReferencesType(const TypeBase & type) const
{
  if (detectingLoop)
    return false;

  bool ref = false;
  //if (!type.IsChoice())
    detectingLoop = true;
  ref = type.ReferencesType(*this);
  detectingLoop = false;

  for (size_t i = 0; i < fields.size(); i++) {
    TypeBase& field = *fields[i];
    if ( field.ReferencesType(type) )
    {
      if (ref && !dynamic_cast<SequenceOfType*>(&field))// recursive reference detected
      {
        needFwdDeclare[i] = true;
        return false;
      }
      else
        return true;
    }
  }
  return false;
}

TypeBase::RemoveResult SequenceType::CanRemoveType(const TypeBase& type)
{
  TypesVector::iterator itr, last=fields.end();
  for (itr = fields.begin(); itr != last; ++itr)
  {
    TypeBase& field = **itr;
    switch ( field.CanRemoveType(type))
    {
      case MAY_NOT:
          if (!field.IsOptional())
              return FORBIDDEN;
          break;
      case FORBIDDEN:
         return FORBIDDEN;
    }
  }
  return OK;
}

bool SequenceType::RemoveThisType(const TypeBase& type)
{
  TypesVector::iterator itr, last=fields.end();
  for (itr = fields.begin(); itr != last; ++itr) {
    TypeBase& field = **itr;
     if (field.RemoveThisType(type))
       itr->reset(new RemovedType(field));
  }

  return GetName() == type.GetName();
}


void SequenceType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
  Indent indent(hdr.precision()+4) ;
  int nOptional=0;
  int nExtensions=0;
  bool hasNonOptionalFields=false;

  hdr << indent << dllMacroExport << "static const Inherited::InfoType theInfo;\n\n"
      << indent-4 << "private:\n";

  size_t nTotalFields = fields.size();
  bool autoTag = true; // tag.mode != Tag::Implicit;
  bool useDefaultTag = tag.mode != Tag::Automatic;

  if (nTotalFields >0 && type == this)
  {
      hdr << indent << "static const void* fieldInfos[" << nTotalFields << "];\n";
      cxx << type->GetTemplatePrefix()
          << "const void* "<< type->GetClassNameString() << "::fieldInfos[" << nTotalFields << "] = {\n";

      size_t i;
      for (i=0; i < fields.size(); ++i)   {
          TypeBase& field = *fields[i];
          if (useDefaultTag && field.HasNonStandardTag())
              useDefaultTag = false;

          const Tag & fieldTag = field.GetTag();
          if (fieldTag.mode != Tag::Automatic && ! (
              fieldTag.number == (unsigned)i &&
              fieldTag.type == Tag::ContextSpecific &&
              fieldTag.mode == Tag::Implicit)) {
             autoTag = false;
          }

         if (i != 0 )
              cxx << ",\n";

         if (field.IsRemovedType())
              cxx << "    NULL";
          else
              cxx << "    &"
                  << field.GetIdentifier()
                  << "::value_type::theInfo";

      }
      cxx << "\n"
          << "};\n\n";


      if (numFields > 0)
      {
          hdr << indent << "static int fieldIds[" << nTotalFields << "];\n";
          cxx << GetTemplatePrefix()
              << "int " << GetClassNameString() << "::fieldIds[" << nTotalFields << "] = {\n";

          unsigned optionalId  = 0;
          for (size_t i = 0; i < numFields-1; ++i)
          {
              if (fields[i]->IsOptional() || fields[i]->HasDefaultValue())
              {
                  cxx << "    " << optionalId++ << ",\n";
                  nOptional++;
              }
              else
                  cxx << "    -1,\n";
          }

          if (numFields > 0)
          {
              if (fields[numFields-1]->IsOptional() || fields[numFields-1]->HasDefaultValue())
              {
                  cxx << "    " << optionalId++ << "\n";
                  nOptional++;
              }
              else
                  cxx << "    -1,\n";

              cxx << "};\n\n";
          }
      }

      if (!useDefaultTag && !autoTag)
      {
          hdr << indent << "static unsigned fieldTags[" << nTotalFields << "];\n";
          cxx << GetTemplatePrefix()
              << "unsigned " << GetClassNameString() << "::fieldTags[" << nTotalFields << "] = {\n";

          for (size_t i = 0; i < nTotalFields; ++i)
          {
              cxx << "    ";
              fields[i]->GenerateTags(cxx);
              if (i != nTotalFields-1)
                  cxx << ",\n";
          }
          cxx << "\n"
              << "};\n\n";
      }

      std::vector<unsigned char> bitmap;
      nExtensions = fields.size()-numFields;
      bitmap.resize((nExtensions+7)/8);
      for (i = numFields; i < fields.size(); i++) {
          if (!fields[i]->IsOptional() && !fields[i]->HasDefaultValue())
          {
              unsigned bit = i- numFields;
              unsigned mod = bit>>3;
              bitmap[mod] |= 1 << (7 - (bit&7));
              hasNonOptionalFields = true;
          }
      }

      if (hasNonOptionalFields)
      {
          hdr << indent << "static const char* nonOptionalExtensions;\n";

          cxx << GetTemplatePrefix()
              << "const char* " << GetClassNameString() <<  "::nonOptionalExtensions = \"";

          for (i = 0; i < (int)bitmap.size(); ++i)
          {
            cxx << "\\x" << std::hex << (unsigned) bitmap[i] << std::dec ;
          }
          cxx << "\";\n\n";
      }

      hdr << indent << "static const char* fieldNames[" << nTotalFields << "];\n";

      cxx << GetTemplatePrefix()
          << "const char* " << GetClassNameString() <<  "::fieldNames[" << nTotalFields << "] = {\n";

      for (i = 0; i < fields.size(); i++) {
          cxx << "    \"" << fields[i]->GetName() << '\"';
          if (i != fields.size() -1)
              cxx << ",\n";
          else
              cxx << endl
                  << "};\n\n";
      }
  }
  else if (numFields > 0)
  {
      for (size_t i = 0; i < fields.size(); ++i)
      {
         if (fields[i]->IsOptional())
         {
            nOptional++;
         }
      }
  }

  std::string typenameKeyword;
  if (type->GetTemplatePrefix().size())
    typenameKeyword = "typename ";

  cxx << type->GetTemplatePrefix()
      << "const "<< typenameKeyword << type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
      << "    " << type->GetClassNameString() << "::create,\n"
      << "    ";
  type->GenerateTags(cxx);
  cxx << ",\n    0,\n";

  if (extendable)
    cxx << "    true,\n";
  else
    cxx << "    false,\n";

  if (nTotalFields > 0 )
  {
      if (type == this)
          cxx << "    " << GetClassNameString() << "::fieldInfos,\n"
              << "    " << GetClassNameString() << "::fieldIds,\n";
      else
          cxx << "    " << GetIdentifier() << "::theInfo.fieldInfos,\n"
              << "    " << GetIdentifier() << "::theInfo.ids,\n";
  }
  else
      cxx << "    NULL, NULL,\n";

  cxx << "    " << numFields << ", " << nExtensions << ", " << nOptional++ << ",\n"
      << "    ";

  if (hasNonOptionalFields && nTotalFields > 0)
  {
      if (type == this)
          cxx << GetClassNameString() << "::nonOptionalExtensions,\n";
      else
          cxx << "     " << GetIdentifier() << "::theInfo.nonOptionalExtensions,\n";
  }
  else
      cxx << "NULL,\n";

  cxx << "    ";

  if (autoTag || nTotalFields ==0)
      cxx << "NULL,\n";
  else if (useDefaultTag) {
    cxx << "&";
    if (type == this)
        cxx << GetClassNameString() ;
    else
        cxx << GetIdentifier();
    cxx << "::defaultTag,\n";
  }
  else if (type == this)
      cxx << GetClassNameString() << "::fieldTags,\n";
  else
      cxx << "     " << GetIdentifier() << "::theInfo.tags,\n";

  cxx << "    ";
  if (nTotalFields >0)
  {
      if (type == this)
        cxx << GetClassNameString() << "::fieldNames\n";
      else
        cxx << GetIdentifier() << "::theInfo.names\n";
  }
  else
      cxx << "NULL\n";

  cxx << "};\n\n";
}

/////////////////////////////////////////////////////////

SequenceOfType::SequenceOfType(TypePtr base, ConstraintPtr constraint, unsigned tag)
  : TypeBase(tag, Module), nonTypedef (false)
{
  assert(base.get());
  baseType = base;
  if (constraint.get() != NULL){
    AddConstraint(constraint);
  }
}


SequenceOfType::~SequenceOfType()
{
}


void SequenceOfType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  if (baseType.get() == NULL)
    strm << "!!Null Type!!\n";
  else
    strm << *baseType << endl;
  PrintFinish(strm);
}


void SequenceOfType::FlattenUsedTypes()
{
  baseType = baseType->SeqOfFlattenThisType(*this, baseType);
  assert(baseType.get());
}


TypePtr SequenceOfType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  TypePtr result = self;
  if (!baseType->IsPrimitiveType() || baseType->HasConstraints() ||
    baseType->HasNonStandardTag() )
    result.reset(new DefinedType(self, parent));
  return result;
}


bool SequenceOfType::IsPrimitiveType() const
{
  return !HasNonStandardTag() && !nonTypedef;
}



void SequenceOfType::GenerateForwardDecls(std::ostream & hdr)
{
  if (FwdDeclareMe(hdr))
    hdr << endl;
}


const char * SequenceOfType::GetAncestorClass() const
{
  return "ASN1::SEQUENCE_OF";
}


bool SequenceOfType::CanReferenceType() const
{
  return true;
}


bool SequenceOfType::ReferencesType(const TypeBase & type) const
{
  return baseType->ReferencesType(type);
}

bool SequenceOfType::UseType(const TypeBase& type) const
{
  return baseType->UseType(type);
}

std::string  SequenceOfType::GetTypeName() const
{
  std::string result("ASN1::SEQUENCE_OF<");

  result += baseType->GetTypeName();
  if (constraints.size() )
  {
    result +=", ";
    constraints[0]->GetConstraint(result);
  }
  result += "> ";

  return result;
}

bool SequenceOfType::FwdDeclareMe(std::ostream & hdr)
{
  //  return baseType->FwdDeclareMe(hdr);
  return false;
}


void SequenceOfType::RemovePERInvisibleConstraint(const ParameterPtr& param)
{
  TypeBase::RemovePERInvisibleConstraint(param);
  baseType->RemovePERInvisibleConstraint(param);
}


void SequenceOfType::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision()+4);
  hdr << indent << GetIdentifier() << "(size_type n=0) : Inherited(n) {}\n"
      << indent << GetIdentifier() << "(size_type n, const "<< baseType->GetTypeName() << "& val) : Inherited(n, val) {}\n"
      << indent << GetIdentifier() << "(const_iterator first, const_iterator last) : Inherited(first, last) {}\n";
}

TypeBase::RemoveResult SequenceOfType::CanRemoveType(const TypeBase& type)
{
    return baseType->ReferencesType(type) ? FORBIDDEN : OK;
}

void SequenceOfType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
    Indent indent(hdr.precision());
    hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
    cxx << type->GetTemplatePrefix()
        << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
        << "    " ;

    cxx << "ASN1::SEQUENCE_OF_Base::create,\n";

    cxx << "    ";

    type->GenerateTags(cxx);

    cxx << ",\n    0,\n"
        << "    ";

    const SizeConstraintElement* sizeConstraint ;

    if (type->GetConstraints().size() &&
        ((sizeConstraint = type->GetConstraints()[0]->GetSizeConstraint()) != NULL))
    {
      std::string str;
      sizeConstraint->GetConstraint(str);
      cxx << str.substr(0, str.size()-2);
    }
    else
        cxx << "ASN1::Unconstrained, 0, UINT_MAX";

    cxx << ",\n"
        << "    &" << baseType->GetTypeName() << "::theInfo\n"
        << "};\n\n";
}

/////////////////////////////////////////////////////////

SetType::SetType()
  : SequenceType(NULL, false, NULL, Tag::UniversalSet)
{
}


SetType::SetType(SequenceType& seq)
  : SequenceType(seq)
{
  defaultTag.number = tag.number = Tag::UniversalSet;
}


const char * SetType::GetAncestorClass() const
{
  return "ASN1::SET";
}


/////////////////////////////////////////////////////////

SetOfType::SetOfType(TypePtr base, ConstraintPtr constraint)
  : SequenceOfType(base, constraint, Tag::UniversalSet)
{
}

std::string  SetOfType::GetTypeName() const
{
  std::string result("ASN1::SET_OF<");
  result += baseType->GetTypeName();
  if (constraints.size() )
  {
    result +=", ";
    constraints[0]->GetConstraint(result);
  }
  result += "> ";
  return result;
}


/////////////////////////////////////////////////////////

ChoiceType::ChoiceType(TypesVector * a_std,
                       bool extendable,
                       TypesVector * extensions)
  : SequenceType(a_std, extendable, extensions, Tag::IllegalUniversalTag)
{
  defaultTag.type = Tag::Universal;
  defaultTag.number = 0;
  tag.type = Tag::Universal;
  tag.number = 0;
}


void ChoiceType::GenerateComponent(TypeBase& field, std::ostream & hdr, std::ostream & cxx, std::ostream & inl, int id)
{
  if (field.IsRemovedType())
    return;

  std::string typeName =  field.GetTypeName();
  std::string componentIdentifier = field.GetIdentifier();
  std::string componentName = field.GetName();
  Indent indent(hdr.precision());

  // generate component scope class

  hdr << endl << indent << "class " << componentIdentifier << endl
      << indent << "{\n"
      << indent << "public:\n";

  indent += 4;

  hdr << indent << "enum Id { eid = " << id << " };\n";

  field.SetOuterClassName(GetClassNameString() + "::" + componentIdentifier);
  field.SetName("value_type");

  hdr.precision(hdr.precision() + 4);
  {
    Tag ftag = field.GetTag();
    if(ftag.mode==Tag::Explicit)
      field.SetTag(field.GetDefaultTag());

    field.GenerateCplusplus(hdr, cxx, inl);
    field.SetTag(ftag);
  }
  hdr.precision(hdr.precision() - 4);

  field.SetName(componentName);
  str_replace(componentName,"-","_");

  std::string primitiveFieldType = field.GetPrimitiveType(componentIdentifier);
  std::string typenameKeyword;

  bool field_has_typename = false;
  if (templatePrefix.length()) {
    typenameKeyword = "typename ";
    if (primitiveFieldType.find(componentIdentifier + "::") != -1)
    {
      field_has_typename = true;
      primitiveFieldType = "typename " + primitiveFieldType;
    }
  }

  hdr << indent << "typedef value_type&        reference;\n"
      << indent << "typedef const value_type&  const_reference;\n"
      << indent << "typedef value_type*        pointer;\n"
      << indent << "typedef const value_type*  const_pointer;\n";

  indent -= 4;
  hdr  << indent << "}; // end class " << componentIdentifier << "\n\n";

  // generate accessor/mutator functions
  if (field.GetTypeName() != "ASN1::Null")
  {
    hdr << indent << typenameKeyword << componentIdentifier << "::const_reference get_" << componentName << " () const";
    if(noInlineFiles)
    {
      hdr << endl
          << indent << "{\n"
          << indent << "    assert(currentSelection() == " << componentIdentifier << "::eid);\n"
          << indent << "    return *static_cast<" << typenameKeyword  << componentIdentifier << "::const_pointer>(choice.get());\n"
          << indent << "}\n";
    }
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::const_reference " << GetClassNameString() << "::get_" << componentName << " () const\n"
          << "{\n"
          << "    assert(currentSelection() == " << componentIdentifier << "::eid);\n"
          << "    return *static_cast<" << typenameKeyword  << componentIdentifier << "::const_pointer>(choice.get());\n"
          << "}\n\n";
    }

    hdr << indent << typenameKeyword << componentIdentifier << "::reference ref_" << componentName << " ()";
    if(noInlineFiles)
    {
      hdr << endl
          << indent << "{\n"
          << indent << "    assert(currentSelection() == " << componentIdentifier << "::eid);\n"
          << indent << "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(choice.get());\n"
          << indent << "}\n";
    }
    else
    {
      hdr << ";\n";

      inl << GetTemplatePrefix()
          << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::ref_" << componentName << " ()\n"
          << "{\n"
          << "    assert(currentSelection() == " << componentIdentifier << "::eid);\n"
          << "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(choice.get());\n"
          << "}\n\n";
    }
  }

  hdr << indent << typenameKeyword << componentIdentifier << "::reference select_" << componentName << " ()";
  if(noInlineFiles)
  {
    hdr << endl
        << indent << "{\n"
        << indent << "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(setSelection(" << componentIdentifier << "::eid, ASN1::AbstractData::create( &" <<  componentIdentifier << "::value_type::theInfo)));\n"
        << indent << "}\n";
  }
  else
  {
    hdr << ";\n";

    inl << GetTemplatePrefix()
        << "inline " << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::select_" << componentName << " ()\n"
        << "{\n"
        << "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(setSelection(" << componentIdentifier << "::eid, ASN1::AbstractData::create( &" <<  componentIdentifier << "::value_type::theInfo)));\n"
        << "}\n\n";
  }

  if (field.GetTypeName() != "ASN1::Null")
  {
    hdr << indent << typenameKeyword << componentIdentifier << "::reference select_" << componentName << " ("<< primitiveFieldType << " v)";
    if(noInlineFiles)
      hdr << " { return select_" << componentName << "() = v; }\n";
    else
    {
      hdr << ";\n";

      int pos;
      if ( (pos = primitiveFieldType.find(componentIdentifier,0)) == 0)
        primitiveFieldType.insert(pos, GetClassNameString() + "::");

      inl << GetTemplatePrefix()
          << "inline "   << typenameKeyword << GetClassNameString() << "::" << componentIdentifier << "::reference " << GetClassNameString() << "::select_" << componentName << " ("<< primitiveFieldType << " v)\n"
          << "{\n"
          << "    return select_" << componentName << "() = v;\n"
          << "}\n\n";
    }
  }

  hdr << indent << "bool " << componentName << "_isSelected() const";
  if(noInlineFiles)
    hdr << " { return currentSelection() == " << componentIdentifier << "::eid; }\n\n";
  else
  {
    hdr << ";\n\n";

    inl << GetTemplatePrefix()
        << "inline bool " << GetClassNameString() << "::" << componentName << "_isSelected() const\n"
        << "{\n"
        << "    return currentSelection() == " << componentIdentifier << "::eid;\n"
        << "}\n\n";
  }

  // Generate alternative constructors

  hdr << indent << GetIdentifier() << '(' << typenameKeyword << componentIdentifier << "::Id id, ";
  if(!field_has_typename)
    hdr << typenameKeyword;
  hdr << primitiveFieldType << " v)";
  if(noInlineFiles)
  {
    hdr << endl
        << indent << "   : Inherited(&theInfo, id, new "  << typenameKeyword << componentIdentifier << "::value_type(v))"
        << " {}\n";
  }
  else
  {
    hdr << ";\n";

    inl << GetTemplatePrefix()
        << "inline " << GetClassNameString() << "::" << GetIdentifier() << "("  << typenameKeyword << componentIdentifier << "::Id id, ";
    if(!field_has_typename)
      inl << typenameKeyword;
    inl << primitiveFieldType << " v)\n"
        << " : Inherited(&theInfo, id, new "  << typenameKeyword << componentIdentifier << "::value_type(v))"
        << " {}\n\n";
  }
}

bool CompareTag(TypeBase* lhs, TypeBase* rhs)
{
  const Tag lTag = lhs->GetTag(), rTag = rhs->GetTag();
  return ( lTag.type != rTag.type ) ? (lTag.type < rTag.type) : (lTag.number < rTag.number);
}

void ChoiceType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  size_t i;
  size_t nFields = fields.size();

  TypeBase::BeginGenerateCplusplus(hdr, cxx, inl);

  sortedFields.clear();
  sortedFields.reserve(nFields);
  for (i = 0; i < nFields; ++i)
    sortedFields.push_back(fields[i].get());

  // sorting fields breaks choice
//  if (GetTag().mode != Tag::Automatic)
//    std::sort(sortedFields.begin(), sortedFields.end(), CompareTag);

  Indent indent(hdr.precision()+4);

  hdr << indent << "enum Choice_Ids {";
  indent += 2;

  hdr << endl << indent << "unknownSelection_ = -2,"
      << endl << indent << "unselected_ = -1";

  for(i = 0; i < nFields; ++i) {
    const std::string& sfIdentifier = sortedFields[i]->GetIdentifier();
    if(sfIdentifier.empty())
      continue;

    hdr << ",\n" << indent << sfIdentifier << i << " = " << i;
  }
  indent -= 2;

  hdr << endl << indent << "};\n";

  // Generate code for type safe cast operators of selected choice object
  bool needExtraLine = false;

  hdr.precision(hdr.precision()+4);
  std::stringstream tmpcxx;
  Unfreezer unfreezer(tmpcxx);
  for (i = 0; i < nFields; i++) {
    GenerateComponent(*sortedFields[i], hdr, tmpcxx, inl, i);
  }

  if (needExtraLine)
    hdr << endl;

  hdr.precision(hdr.precision()-4);

  //GenerateIds(hdr);

  std::string typenameKeyword;
  if (templatePrefix.length())
    typenameKeyword = "typename ";

  EndGenerateCplusplus(hdr, cxx, inl);
  cxx << tmpcxx.str();
  tmpcxx << ends;

  isGenerated = true;
}

void ChoiceType::GenerateOperators(std::ostream & hdr, std::ostream & , const TypeBase & )
{
  Indent indent(hdr.precision()+4);
  // Generate Copy Constructor, Assignment operator, swap

  hdr << indent << GetIdentifier() << "(const " << GetIdentifier() << " & that)"
      << indent << " : Inherited(that) {}\n\n";

  hdr << indent << GetIdentifier() << " & operator=(const " << GetIdentifier() << " & that)\n"
      << indent << "{ Inherited::operator=(that); return *this; }\n\n";

  hdr << indent << "void swap(" << GetIdentifier() << " & that) { Inherited::swap(that); }\n\n";
}


bool ChoiceType::IsPrimitiveType() const
{
  return false;
}


bool ChoiceType::IsChoice() const
{
  return true;
}

TypePtr ChoiceType::FlattenThisType(TypePtr& self, const TypeBase & parent)
{
  TypePtr ret(new DefinedType(self, parent));
  ret->SetTag(Tag());
  return ret;
}


const char * ChoiceType::GetAncestorClass() const
{
  return "ASN1::CHOICE";
}


TypeBase::RemoveResult ChoiceType::CanRemoveType(const TypeBase& type)
{
    for (size_t i =0; i < fields.size(); ++i)
    {
        if (fields[i]->CanRemoveType(type) == FORBIDDEN )
            return FORBIDDEN;
    }
    return OK;
}

void ChoiceType::GenerateInfo(const TypeBase* type,std::ostream& hdr, std::ostream& cxx)
{
  size_t nFields = fields.size();
  Indent indent(hdr.precision()+4);

  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n"
      << indent -4 << "private:\n";

  bool autoTag = true;
  // generate selection info table
  if (type == this)
  {
      hdr << indent << "static const void* selectionInfos[" << nFields << "];\n";
      cxx << type->GetTemplatePrefix()
          << "const void* " << type->GetClassNameString() << "::selectionInfos[" << nFields << "] = {\n";

      size_t i;
      for (i = 0; i < nFields; i++) {
          const Tag & fieldTag = sortedFields[i]->GetTag();
          if (fieldTag.mode != Tag::Automatic &&
               !( fieldTag.number == (unsigned)i &&
                  fieldTag.type == Tag::ContextSpecific &&
                  fieldTag.mode == Tag::Implicit)) {
              autoTag = false;
          }

          if (sortedFields[i]->IsRemovedType())
              cxx << "    NULL";
          else
              cxx << "    &" <<  sortedFields[i]->GetIdentifier()
                  << "::value_type::theInfo";
          if (i != nFields-1)
              cxx << ",\n";
      }

      cxx << "\n"
          << "};\n\n";

      // generate tag list for BER decoding

      if (!autoTag)
      {
          hdr << indent << "static unsigned selectionTags[" << nFields << "];\n";
          cxx << GetTemplatePrefix()
              << "unsigned " << GetClassNameString() << "::selectionTags[" << nFields << "] = {\n";

          for (i = 0; i < nFields; i++) {
              cxx << "    ";
              sortedFields[i]->GenerateTags(cxx);
              if (i != nFields-1)
                  cxx << ",\n";
              else
                  cxx << endl;
          }
          cxx << "};\n\n";
      }

      hdr << indent << "static const char* selectionNames[" << nFields << "];\n";

      cxx << GetTemplatePrefix()
          << "const char* " << GetClassNameString() << "::selectionNames[" << nFields << "] = {\n";

      for (i = 0; i < nFields; i++) {
          cxx << "    \"" << sortedFields[i]->GetName();

          if (i != nFields-1)
            cxx << "\",\n";
          else
            cxx << "\"\n";
      }
      cxx << "};\n\n";
  }

  std::string typenameKeyword;
  if (type->GetTemplatePrefix().length())
    typenameKeyword="typename ";

  cxx << type->GetTemplatePrefix()
      << "const " << typenameKeyword << type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
      << "    ASN1::CHOICE::create,\n"
      << "    ";
  type->GenerateTags(cxx);
  cxx << ",\n    0,\n";

  if (extendable)
    cxx << "    true,\n";
  else
    cxx << "    false,\n";

  if (type == this)
      cxx << "    " << GetClassNameString() << "::selectionInfos,\n";
  else
      cxx << "    " << GetIdentifier() << "::theInfo.selectionInfos,\n";

  cxx << "    " <<  numFields << ", " << nFields << ",\n"
      << "    ";

  if (!autoTag)
  {
      if (type == this)
        cxx <<   GetClassNameString() << "::selectionTags,\n";
      else
          cxx << GetIdentifier() << "::theInfo.tags,\n";
  }
  else
    cxx << "NULL,\n";

  if (type == this)
      cxx << "    " << GetClassNameString() << "::selectionNames\n";
  else
      cxx << "    " << GetIdentifier() << "::theInfo.names\n";

  cxx << "};\n\n";
}

/////////////////////////////////////////////////////////

EmbeddedPDVType::EmbeddedPDVType()
  : TypeBase(Tag::UniversalEmbeddedPDV, Module)
{
}


const char * EmbeddedPDVType::GetAncestorClass() const
{
  return "ASN1::EMBEDED_PDV";
}


/////////////////////////////////////////////////////////

ExternalType::ExternalType()
  : TypeBase(Tag::UniversalExternalType, Module)
{
}


const char * ExternalType::GetAncestorClass() const
{
  return "ASN1::EXTERNAL";
}


/////////////////////////////////////////////////////////

AnyType::AnyType(const std::string& ident)
  : TypeBase(Tag::UniversalExternalType, Module)
{
    identifier = ident;
}


void AnyType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  if (identifier.size())
    strm << "Defined by " << identifier;
  PrintFinish(strm);
}


const char * AnyType::GetAncestorClass() const
{
  return "ASN1::OpenData";
}
//////////////////////////////////////////////////////////

static const char NumericStringSet[]   = " 0123456789";
static const char PrintableStringSet[] =
                      " '()+,-./0123456789:=?"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "abcdefghijklmnopqrstuvwxyz";
static const char VisibleStringSet[]   =
                      " !\"#$%&'()*+,-./0123456789:;<=>?"
                      "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                      "`abcdefghijklmnopqrstuvwxyz{|}~";
static const char IA5StringSet[]       =
                      "\000\001\002\003\004\005\006\007"
                      "\010\011\012\013\014\015\016\017"
                      "\020\021\022\023\024\025\026\027"
                      "\030\031\032\033\034\035\036\037"
                      " !\"#$%&'()*+,-./0123456789:;<=>?"
                      "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                      "`abcdefghijklmnopqrstuvwxyz{|}~\177";
static const char GeneralStringSet[]   =
                      "\000\001\002\003\004\005\006\007"
                      "\010\011\012\013\014\015\016\017"
                      "\020\021\022\023\024\025\026\027"
                      "\030\031\032\033\034\035\036\037"
                      " !\"#$%&'()*+,-./0123456789:;<=>?"
                      "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                      "`abcdefghijklmnopqrstuvwxyz{|}~\177"
                      "\200\201\202\203\204\205\206\207"
                      "\210\211\212\213\214\215\216\217"
                      "\220\221\222\223\224\225\226\227"
                      "\230\231\232\233\234\235\236\237"
                      "\240\241\242\243\244\245\246\247"
                      "\250\251\252\253\254\255\256\257"
                      "\260\261\262\263\264\265\266\267"
                      "\270\271\272\273\274\275\276\277"
                      "\300\301\302\303\304\305\306\307"
                      "\310\311\312\313\314\315\316\317"
                      "\320\321\322\323\324\325\326\327"
                      "\330\331\332\333\334\335\336\337"
                      "\340\341\342\343\344\345\346\347"
                      "\350\351\352\353\354\355\356\357"
                      "\360\361\362\363\364\365\366\367"
                      "\370\371\372\373\374\375\376\377";

/////////////////////////////////////////////////////////

StringTypeBase::StringTypeBase(int tag)
  : TypeBase(tag, Module)
{
}


std::string StringTypeBase::GetTypeName() const
{
  return GetAncestorClass();
}

void StringTypeBase::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision()+4);

  hdr << indent << GetIdentifier() << "() : Inherited(&theInfo) {}\n"
      << indent << GetIdentifier() << "(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}\n"
      << indent << GetIdentifier() << "(const char* str, const void* info = &theInfo) : Inherited(str, info) {}\n";
}

void StringTypeBase::GenerateOperators(std::ostream & hdr, std::ostream & , const TypeBase & actualType)
{
  std::string atname = actualType.GetIdentifier();
  Indent indent(hdr.precision()+4);
  hdr << indent << atname << "& operator=(const ASN1_STD string& that)\n"
      << indent << "{ Inherited::operator=(that); return *this; }\n"
      << indent << atname << "& operator=(const char* that)\n"
      << indent << "{ Inherited::operator=(that); return *this; }\n";
}

bool StringTypeBase::NeedGenInfo() const
{
    return TypeBase::NeedGenInfo() || constraints.size() > 0;
}

static size_t CountBits(unsigned range)
{
  if (range == 0)
    return sizeof(unsigned)*8;

  size_t nBits = 0;
  while (nBits < (sizeof(unsigned)*8) && range > (unsigned)(1 << nBits))
    nBits++;
  return nBits;
}


void StringTypeBase::GenerateInfo(const TypeBase* type,std::ostream& hdr, std::ostream& cxx)
{
  Indent indent(hdr.precision());
  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
  cxx << type->GetTemplatePrefix()
      << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
      << "    ASN1::AbstractString::create,\n"
      << "    ";
  type->GenerateTags(cxx);
  cxx << ",\n    &" << GetAncestorClass() << "::theInfo,\n";

  const SizeConstraintElement* sizeConstraint = NULL;
  size_t i;
  for (i = 0; i < type->GetConstraints().size(); ++i)
    if ((sizeConstraint = type->GetConstraints()[i]->GetSizeConstraint()) != NULL)
      break;

  if (sizeConstraint != NULL)
  {
    std::string str;
    sizeConstraint->GetConstraint(str);
    cxx << "    " << str.substr(0, str.size()-2) << ",\n";
  }
  else
    cxx << "    ASN1::Unconstrained, 0, UINT_MAX,\n";

  const FromConstraintElement* fromConstraint = NULL;
  for (i = 0; i < type->GetConstraints().size(); ++i)
    if ((fromConstraint = type->GetConstraints()[i]->GetFromConstraint()) != NULL)
      break;

  cxx << "    ";

  int charSetUnalignedBits;
  std::string characterSet;
  if (fromConstraint != NULL &&
        (characterSet = fromConstraint->GetCharacterSet(canonicalSet, canonicalSetSize)).size())
  {
    cxx << '"';
    for (size_t i = 0; i < characterSet.size(); ++i)
    {
      if (isprint(characterSet[i]) && characterSet[i] != '"')
        cxx << characterSet[i];
      else
        cxx << "\\x" << std::hex << (unsigned) characterSet[i];
    }

    cxx << "\", " << characterSet.size()  << ",\n";
    charSetUnalignedBits = CountBits(characterSet.size());

  }
  else
  {
    cxx <<  canonicalSetRep << ", " << canonicalSetSize << ",\n";
    charSetUnalignedBits = CountBits(canonicalSetSize);
  }

  cxx <<  "    " << CountBits(canonicalSetSize) << ",\n";


  int charSetAlignedBits = 1;
  while (charSetUnalignedBits > charSetAlignedBits)
    charSetAlignedBits <<= 1;

  cxx << "    " << charSetUnalignedBits << ", " << charSetAlignedBits << "\n"
      << "};\n\n";
}

/////////////////////////////////////////////////////////

BMPStringType::BMPStringType()
  : StringTypeBase(Tag::UniversalBMPString)
{
}


const char * BMPStringType::GetAncestorClass() const
{
  return "ASN1::BMPString";
}

void BMPStringType::GenerateConstructors(std::ostream & hdr, std::ostream & , std::ostream & )
{
  Indent indent(hdr.precision()+4);

  hdr << indent << GetIdentifier() << "() : Inherited(&theInfo) {}\n"
      << indent << GetIdentifier() << "(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}\n"
      << indent << GetIdentifier() << "(const wchar_t* str, const void* info = &theInfo) : Inherited(str, info) {}\n";
}

void BMPStringType::GenerateOperators(std::ostream & hdr, std::ostream & , const TypeBase & actualType)
{
  std::string atname = actualType.GetIdentifier();
  Indent indent(hdr.precision()+4);
  hdr << indent << atname << "& operator=(const std::wstring& that)\n"
      << indent << "{ Inherited::operator=(that); return *this; }\n"
      << indent << atname << "& operator=(const wchar_t* that)\n"
      << indent << "{ Inherited::operator=(that); return *this; }\n";
}

void BMPStringType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
  Indent indent(hdr.precision());
  hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
  cxx << type->GetTemplatePrefix()
      << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
      << "    ASN1::BMPString::create,\n"
      << "    ";
  GenerateTags(cxx);
  cxx << ",\n    &" << GetAncestorClass() << "::theInfo,\n";

  const SizeConstraintElement* sizeConstraint = NULL;
  size_t i;
  for (i = 0; i < type->GetConstraints().size(); ++i)
    if ((sizeConstraint = type->GetConstraints()[i]->GetSizeConstraint()) != NULL)
      break;

  if (sizeConstraint != NULL)
  {
    std::string str;
    sizeConstraint->GetConstraint(str);
    cxx << "    " << str.substr(0, str.size()-2) << ",\n";
  }
  else
    cxx << "    ASN1::Unconstrained, 0, UINT_MAX,\n";

  const FromConstraintElement* fromConstraint = NULL;
  for (i = 0; i < type->GetConstraints().size(); ++i)
    if ((fromConstraint = type->GetConstraints()[i]->GetFromConstraint()) != NULL)
      break;

  cxx << "    ";

  int range = 0xffff;
  if (fromConstraint != NULL)
     range = fromConstraint->GetRange(cxx);
  else
    cxx <<  0 << ", " << 0xffff ;

  int charSetUnalignedBits = CountBits(range);

  int charSetAlignedBits = 1;
  while (charSetUnalignedBits > charSetAlignedBits)
    charSetAlignedBits <<= 1;

  cxx << ",\n"
      << "    " << charSetUnalignedBits << ", " << charSetAlignedBits << "\n"
      << "};\n\n";
}
/////////////////////////////////////////////////////////

GeneralStringType::GeneralStringType()
  : StringTypeBase(Tag::UniversalGeneralString)
{
  canonicalSet = GeneralStringSet;
  canonicalSetRep = "ASN1::GeneralString::theInfo.characterSet";
  canonicalSetSize = sizeof(GeneralStringSet)-1;
}


const char * GeneralStringType::GetAncestorClass() const
{
  return "ASN1::GeneralString";
}


/////////////////////////////////////////////////////////

GraphicStringType::GraphicStringType()
  : StringTypeBase(Tag::UniversalGraphicString)
{
}


const char * GraphicStringType::GetAncestorClass() const
{
  return "ASN1::GraphicString";
}


/////////////////////////////////////////////////////////

IA5StringType::IA5StringType()
  : StringTypeBase(Tag::UniversalIA5String)
{
  canonicalSet =IA5StringSet;
  canonicalSetRep = "ASN1::IA5String::theInfo.characterSet";
  canonicalSetSize = sizeof(IA5StringSet)-1;
}


const char * IA5StringType::GetAncestorClass() const
{
  return "ASN1::IA5String";
}


/////////////////////////////////////////////////////////

ISO646StringType::ISO646StringType()
  : StringTypeBase(Tag::UniversalVisibleString)
{
}


const char * ISO646StringType::GetAncestorClass() const
{
  return "ASN1::ISO646String";
}


/////////////////////////////////////////////////////////

NumericStringType::NumericStringType()
  : StringTypeBase(Tag::UniversalNumericString)
{
  canonicalSet =NumericStringSet;
  canonicalSetRep = "ASN1::NumericString::theInfo.characterSet";
  canonicalSetSize = sizeof(NumericStringSet)-1;
}


const char * NumericStringType::GetAncestorClass() const
{
  return "ASN1::NumericString";
}


/////////////////////////////////////////////////////////

PrintableStringType::PrintableStringType()
  : StringTypeBase(Tag::UniversalPrintableString)
{
  canonicalSet =PrintableStringSet;
  canonicalSetRep = "ASN1::PrintableString::theInfo.characterSet";
  canonicalSetSize = sizeof(PrintableStringSet)-1;
}


const char * PrintableStringType::GetAncestorClass() const
{
  return "ASN1::PrintableString";
}


/////////////////////////////////////////////////////////

TeletexStringType::TeletexStringType()
  : StringTypeBase(Tag::UniversalTeletexString)
{
}


const char * TeletexStringType::GetAncestorClass() const
{
  return "ASN1::TeletexString";
}


/////////////////////////////////////////////////////////

T61StringType::T61StringType()
  : StringTypeBase(Tag::UniversalTeletexString)
{
}


const char * T61StringType::GetAncestorClass() const
{
  return "ASN1::T61String";
}


/////////////////////////////////////////////////////////

UniversalStringType::UniversalStringType()
  : StringTypeBase(Tag::UniversalUniversalString)
{
}


const char * UniversalStringType::GetAncestorClass() const
{
  return "ASN1::UniversalString";
}


/////////////////////////////////////////////////////////

VideotexStringType::VideotexStringType()
  : StringTypeBase(Tag::UniversalVideotexString)
{
}


const char * VideotexStringType::GetAncestorClass() const
{
  return "ASN1::VideotexString";
}


/////////////////////////////////////////////////////////

VisibleStringType::VisibleStringType()
  : StringTypeBase(Tag::UniversalVisibleString)
{
  canonicalSet =VisibleStringSet;
  canonicalSetRep = "ASN1::VisibleString::theInfo.characterSet";
  canonicalSetSize = sizeof(VisibleStringSet)-1;
}


const char * VisibleStringType::GetAncestorClass() const
{
  return "ASN1::VisibleString";
}


/////////////////////////////////////////////////////////

UnrestrictedCharacterStringType::UnrestrictedCharacterStringType()
  : StringTypeBase(Tag::UniversalUniversalString)
{
}


const char * UnrestrictedCharacterStringType::GetAncestorClass() const
{
  return "ASN1::UnrCHARACTOR_STRING";
}


/////////////////////////////////////////////////////////

GeneralizedTimeType::GeneralizedTimeType()
  : TypeBase(Tag::UniversalGeneralisedTime, Module)
{
}


const char * GeneralizedTimeType::GetAncestorClass() const
{
  return "ASN1::GeneralizedTime";
}

void GeneralizedTimeType::GenerateConstructors(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision()+4);
  TypeBase::GenerateConstructors(hdr, cxx, inl);

  hdr << indent   << GetIdentifier() << "(const char* v) : Inherited(v) {  }\n"
      << indent   << GetIdentifier() << "(int year, int month, int day,\n"
      << indent+2 << "int hour = 0, int minute=0, int second=0,\n"
      << indent+2 << "int millisec = 0, int mindiff = 0, bool utc = false)\n"
      << indent+2 << ": Inherited(year, month, day, hour, minute, second, milliseec, mindiff, utc)"
      << " {}\n" ;
}


/////////////////////////////////////////////////////////

UTCTimeType::UTCTimeType()
  : TypeBase(Tag::UniversalUTCTime, Module)
{
}


const char * UTCTimeType::GetAncestorClass() const
{
  return "ASN1::UTCTime";
}


void UTCTimeType::GenerateConstructors(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision()+2);
  TypeBase::GenerateConstructors(hdr, cxx, inl);

  hdr << indent   << GetIdentifier() << "(const char* v) : Inherited(v) {  }\n"
      << indent   << GetIdentifier() << "(int year, int month, int day,\n"
      << indent+2 << "int hour = 0, int minute=0, int second=0,\n"
      << indent+2 << "int mindiff = 0, bool utc = false)\n"
      << indent+2 << ": Inherited(year, month, day, hour, minute, second, mindiff, utc)"
      << " {}\n";
}

/////////////////////////////////////////////////////////

ObjectDescriptorType::ObjectDescriptorType()
  : TypeBase(Tag::UniversalObjectDescriptor, Module)
{
}


const char * ObjectDescriptorType::GetAncestorClass() const
{
  return "ASN1::ObjectDescriptor";
}


/////////////////////////////////////////////////////////

ObjectIdentifierType::ObjectIdentifierType()
  : TypeBase(Tag::UniversalObjectId, Module)
{
}


void ObjectIdentifierType::BeginParseThisTypeValue() const
{
  InOIDContext = true;
}

void ObjectIdentifierType::EndParseThisTypeValue() const
{
  InOIDContext = false;
}

const char * ObjectIdentifierType::GetAncestorClass() const
{
  return "ASN1::OBJECT_IDENTIFIER";
}


void ObjectIdentifierType::GenerateConstructors(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  Indent indent(hdr.precision()+4);
  TypeBase::GenerateConstructors(hdr, cxx, inl);
  hdr << indent   << "template <class Iterator>\n"
      << indent+2 << GetIdentifier() << "(Iterator first, Iterator last, const void* info = &theInfo)\n"
      << indent   << ": Inherited(first, last, info) {}\n";
}

/////////////////////////////////////////////////////////

ObjectClassFieldType::ObjectClassFieldType(ObjectClassBasePtr  objclass,
                       const std::string& field)
  : TypeBase(Tag::IllegalUniversalTag, Module),
    asnObjectClass(objclass),
    asnObjectClassField(field)
{
}

ObjectClassFieldType::~ObjectClassFieldType()
{
}

const char * ObjectClassFieldType::GetAncestorClass() const
{
  return "";
}


void ObjectClassFieldType::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << asnObjectClass->GetName() << '.' << asnObjectClassField;
  PrintFinish(strm);
}



bool ObjectClassFieldType::CanReferenceType() const
{
  return true;
}


bool ObjectClassFieldType::ReferencesType(const TypeBase & type) const
{
  const TypeBase* actualType = GetFieldType();
  if (actualType && actualType->ReferencesType(type)) // not open type
    return true;
  else // open type , check if it is constrained
  {

    if (std::find_if(constraints.begin(), constraints.end(),
    boost::bind(&Constraint::ReferencesType, _1, boost::cref(type) ) ) != constraints.end())
      return true;

    if (tableConstraint.get())
      return tableConstraint->ReferenceType(type);
  }
  return false;
}

TypeBase* ObjectClassFieldType::GetFieldType()
{
  return asnObjectClass->GetFieldType(asnObjectClassField);
}

const TypeBase* ObjectClassFieldType::GetFieldType() const
{
  return asnObjectClass->GetFieldType(asnObjectClassField);
}

std::string ObjectClassFieldType::GetConstrainedTypeName() const
{
    if (HasConstraints())
    {
        const SubTypeConstraintElement* cons = constraints[0]->GetSubTypeConstraint();
        if (cons)
            return cons->GetSubTypeName();
    }
    return std::string();
}

std::string ObjectClassFieldType::GetTypeName() const
{
  const TypeBase* type = GetFieldType();
  std::string result;
  if (type)
    return type->GetTypeName();
  else if ( (result = GetConstrainedTypeName()).size() )
    return  std::string("ASN1::Constrained_OpenData<") + result + std::string("> ");

  return std::string("ASN1::OpenData");

}

void ObjectClassFieldType::AddTableConstraint(boost::shared_ptr<TableConstraint> constraint)
{
  tableConstraint = constraint;
}

void ObjectClassFieldType::GenerateDecoder(std::ostream& cxx)
{
  if (tableConstraint.get() && tableConstraint->GetAtNotations() && tableConstraint->GetAtNotations()->size() ==1)
  {
      Indent indent(4);

      if (IsOptional())
      {
          cxx << indent << "if (" << GetName() << "_isPresent())\n"
              << indent << "{\n";
          indent +=2;
      }

      const StringList & lst = *(tableConstraint->GetAtNotations());
      std::string keyname = lst[0];
      if (keyname[0]== '.')
          keyname = keyname.substr(1);
      std::string fieldIdentifier = asnObjectClassField.substr(1);
      cxx << indent << tableConstraint->GetObjectSetIdentifier() << " objSet(*visitor.get_env());\n"
          << indent << "if (objSet.get())\n"
          << indent << "{\n"
          << indent << "  if (objSet->count(get_" << keyname << "()))\n"
          << indent << "  {\n"
          << indent << "    ref_" << GetName() << "().grab(objSet->find(get_" <<  keyname
                            << "())->get_" << fieldIdentifier << "());\n"
          << indent << "    return visitor.revisit(ref_" << GetName() << "());\n"
          << indent << "  }\n"
          << indent << "  else\n"
          << indent << "    return objSet.extensible();\n"
          << indent << "}\n"
          << indent << "else\n"
          << indent << "  return true;\n";

       if (IsOptional())
       {
           indent-=2;
           cxx << indent << "}\n";
       }
  }
}

void ObjectClassFieldType::GenerateInfo(const TypeBase* type, std::ostream& hdr, std::ostream& cxx)
{
  std::string constrainedType = GetConstrainedTypeName();
  if (constrainedType.size())
  {
      Indent indent(hdr.precision()) ;
      hdr << indent << dllMacroExport << "static const InfoType theInfo;\n";
      cxx << GetTemplatePrefix()
      << "const "<< type->GetClassNameString() << "::InfoType " <<  type->GetClassNameString() << "::theInfo = {\n"
          << "    TypeConstrainedOpenData::create,\n"
          << "    ";

      type->GenerateTags(cxx);
      cxx << ",\n    &" << GetAncestorClass() << "::theInfo,\n"
          << "    &" << constrainedType << "::theInfo;\n"
          << "};\n\n";
  }
  else
  {
      TypeBase::GenerateInfo(type, hdr, cxx);
  }
}
/////////////////////////////////////////////////////////

ImportedType::ImportedType(const std::string & theName, bool param)
  : TypeBase(Tag::IllegalUniversalTag, Module)
{
  identifier = name = theName;
  parameterised = param;
}

ImportedType::ImportedType(const TypePtr& ref)
  : TypeBase(Tag::IllegalUniversalTag, Module), reference(ref)
{
  identifier = name = ref->GetName();
  parameterised = ref->HasParameters();
}


const char * ImportedType::GetAncestorClass() const
{
  return identifier.c_str();
}


void ImportedType::AdjustIdentifier(bool usingNamespace)
{
  if (usingNamespace)
    identifier = cModuleName + "::" + MakeIdentifierC(name);
  else
    identifier = modulePrefix + '_' + MakeIdentifierC(name);
}


void ImportedType::GenerateCplusplus(std::ostream &, std::ostream &, std::ostream &)
{
}


void ImportedType::SetModuleName(const std::string& mname)
{
  moduleName = mname;
  cModuleName = MakeIdentifierC(mname);
  modulePrefix = Module->GetImportModuleName(mname);
}



bool ImportedType::IsParameterisedImport() const
{
  return parameterised;
}

void ImportedType::BeginParseThisTypeValue() const
{
  if (reference.get())
    reference->BeginParseThisTypeValue();
  else
    // used when this type is an INTEGER and the subsequent value is a NamedNumber.
    IdentifierTokenContext = VALUEREFERENCE;
}

void ImportedType::EndParseThisTypeValue() const
{
  if (reference.get())
    reference->EndParseThisTypeValue();
  else
    IdentifierTokenContext = IDENTIFIER;
}

bool ImportedType::IsPrimitiveType() const
{
    if (reference.get())
        return reference->IsPrimitiveType();
    else
        return false;
}


/////////////////////////////////////////////////////////
TypeFromObject::TypeFromObject(InformationObjectPtr  obj, const std::string& fld)
: TypeBase(Tag::IllegalUniversalTag, Module)
  ,refObj(obj)
  ,field(fld)
{
}

TypeFromObject::~TypeFromObject()
{
}

const char * TypeFromObject::GetAncestorClass() const
{
  return NULL;
}

void TypeFromObject::PrintOn(std::ostream & strm) const
{
  PrintStart(strm);
  strm << *refObj << "." << field;
  PrintFinish(strm);
}

void TypeFromObject::GenerateCplusplus(std::ostream & hdr, std::ostream &, std::ostream&)
{
  if (hdr.precision() == 0)
  {
    hdr << "//\n"
           "// " << GetName() << "\n"
           "//\n"
           "\n";
  }

  hdr << Indent(hdr.precision()) << "typedef ";
  hdr << "PASN_OpenType";
  hdr << ' ' << GetIdentifier() << ";\n"
         "\n"
         "\n";
}
///////////////////////////////////////////////////////

RemovedType::RemovedType(const TypeBase& type)
: TypeBase(type.GetTag().number, type.GetModule())
{
  isOptional = true;
}

const char * RemovedType::GetAncestorClass() const
{
  return "";
}

/////////////////////////////////////////////////////////

void ValueBase::SetValueName(const std::string& name)
{
  valueName = name;
}


void ValueBase::PrintBase(std::ostream & strm) const
{
  if (!valueName.empty())
    strm << endl << indent() << valueName << " ::= ";
}


void ValueBase::GenerateCplusplus(std::ostream &, std::ostream &, std::ostream &) const
{
  std::cerr << StdError(Warning) << "unsupported value type." << endl;
}

void ValueBase::GenerateConst(std::ostream &, std::ostream &) const
{
  std::cerr << StdError(Warning) << "unsupported const value type." << endl;
}

/////////////////////////////////////////////////////////

DefinedValue::DefinedValue(const std::string& name)
  : referenceName(name)
{
  unresolved = true;
}

DefinedValue::DefinedValue(const ValuePtr& base)
: actualValue(base), unresolved(false)
{
}

DefinedValue::DefinedValue(const std::string& name, const ValuePtr& base)
: actualValue(base), unresolved(false)
{
  referenceName = name;
}


void DefinedValue::PrintOn(std::ostream & strm) const
{
  PrintBase(strm);
  if (referenceName != "")
  strm << referenceName;
  else
  strm << *actualValue;
}


void DefinedValue::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl) const
{
  if (unresolved) {
    unresolved = false;
    actualValue = Module->FindValue(referenceName);
  }

  if (actualValue.get() != NULL)
    actualValue->GenerateCplusplus(hdr, cxx, inl);
  else
    cxx << referenceName;
}


/////////////////////////////////////////////////////////

BooleanValue::BooleanValue(bool newVal)
{
  value = newVal;
}


void BooleanValue::PrintOn(std::ostream & strm) const
{
  PrintBase(strm);
  strm << (value ? "true" : "false");
}


void BooleanValue::GenerateCplusplus(std::ostream &, std::ostream & cxx, std::ostream&) const
{
  cxx << (value ? "true" : "false");
}


/////////////////////////////////////////////////////////

IntegerValue::IntegerValue(boost::int64_t newVal)
{
  value = newVal;
}

#if defined(_MSC_VER) && (_MSC_VER <=1200)

std::ostream& operator << (std::ostream& os, __int64 n)
{
  char str[40];
  os << _i64toa(n, str, 10);
  return os;
}

#endif

void IntegerValue::PrintOn(std::ostream & strm) const
{
  PrintBase(strm);

  strm << value;
  if (value > INT_MAX)
    strm << 'U';
}


void IntegerValue::GenerateCplusplus(std::ostream &, std::ostream & cxx, std::ostream &) const
{
  cxx << value;
  if (value > INT_MAX)
    cxx << 'U';
}

void IntegerValue::GenerateConst(std::ostream &hdr, std::ostream & cxx) const
{
  if(!GetName().empty()) {
    hdr << "extern const ASN1::INTEGER " << GetName() << ";\n\n";
    cxx << "const ASN1::INTEGER " << GetName() << '(' << value;

    if (value > INT_MAX)
      cxx << 'U';

    cxx << ");\n\n";
  }
}


/////////////////////////////////////////////////////////

RealValue::RealValue(double newVal)
{
  value = newVal;
}


/////////////////////////////////////////////////////////

OctetStringValue::OctetStringValue(const std::string& newVal)
{
  value.assign(newVal.begin(), newVal.end());
}


/////////////////////////////////////////////////////////

BitStringValue::BitStringValue(const std::string& newVal)
{
  value = newVal;
}


BitStringValue::BitStringValue(StringList * newVal)
{
  value = '{';

  for(StringList::iterator i = newVal->begin(); i!=newVal->end(); ++i) {
    if(i!=newVal->begin())
      value+=',';
    (value+=' ').append(*i);
  }

  value.append(" }");

  delete newVal;
}

void BitStringValue::PrintOn(std::ostream &os) const
{
  os << value;
}


/////////////////////////////////////////////////////////

CharacterValue::CharacterValue(char c)
{
  value = c;
}


CharacterValue::CharacterValue(char t1, char t2)
{
  value = (t1<<8) + t2;
}


CharacterValue::CharacterValue(char q1, char q2, char q3, char q4)
{
  value = (q1<<24) + (q2<<16) + (q3<<8) + q4;
}


void CharacterValue::PrintOn(std::ostream & strm) const
{
  strm << "'\\x" << std::hex << value << '\'' << std::dec;
}


void CharacterValue::GenerateCplusplus(std::ostream &, std::ostream & cxx, std::ostream &) const
{
  cxx << value;
}


/////////////////////////////////////////////////////////

CharacterStringValue::CharacterStringValue(const std::string& newVal)
{
  value = newVal;
}


CharacterStringValue::CharacterStringValue(StringList& newVal)
{
  std::accumulate(newVal.begin(), newVal.end(), value);
}


void CharacterStringValue::PrintOn(std::ostream & strm) const
{
  if (value[0] == '\"' && value[2] == '\"')
    strm << '\'' << value[1] << '\'';
  else
    strm << value;
}


void CharacterStringValue::GenerateCplusplus(std::ostream &, std::ostream & cxx, std::ostream &) const
{
  cxx << value;
}

void CharacterStringValue::GetValue(std::string& v) const
{
  v = value.substr(1, value.size()-2);
}


/////////////////////////////////////////////////////////

ObjectIdentifierValue::ObjectIdentifierValue(const std::string& newVal)
{
  value.push_back(newVal);
}


ObjectIdentifierValue::ObjectIdentifierValue(StringList& newVal)
{
  value.swap(newVal);
}


void ObjectIdentifierValue::GenerateCplusplus(std::ostream &hdr, std::ostream & cxx, std::ostream &) const
{
  cxx << value.size();
  for(int i=0; i<value.size(); ++i) {
    cxx << ", " << atol(value[i].c_str());
  }
}

void ObjectIdentifierValue::GenerateConst(std::ostream & hdr, std::ostream & cxx) const
{
  hdr << "extern const ASN1::OBJECT_IDENTIFIER " << GetName() << ";\n\n";
  cxx << "const ASN1::OBJECT_IDENTIFIER " << GetName()  << '(';

  std::stringstream dummy;
  GenerateCplusplus(hdr, cxx, dummy);

  cxx << ");\n\n";
}

void ObjectIdentifierValue::PrintOn(std::ostream & strm) const
{
  PrintBase(strm);
  if (value.empty())
    strm << "empty object identifier";
  else {
    strm << value[0];
    for (size_t i = 1; i < value.size(); i++)
      strm << '.' << value[i];
  }
  strm << endl;
}


/////////////////////////////////////////////////////////

void MinValue::PrintOn(std::ostream & strm) const
{
  strm << "INT_MIN";
}


void MinValue::GenerateCplusplus(std::ostream &, std::ostream & cxx, std::ostream &) const
{
  cxx << "MinimumValue";
}


/////////////////////////////////////////////////////////

void MaxValue::PrintOn(std::ostream & strm) const
{
  strm << "INT_MAX";
}


void MaxValue::GenerateCplusplus(std::ostream &, std::ostream & cxx, std::ostream &) const
{
  cxx << "MaximumValue";
}


/////////////////////////////////////////////////////////

SequenceValue::SequenceValue(ValuesList * list)
{
  if (list != NULL) {
    values.swap(*list);
    delete list;
  }
}


void SequenceValue::PrintOn(std::ostream & strm) const
{
  strm << "{ ";
  for (size_t i = 0; i < values.size(); i++) {
    if (i > 0)
      strm << ", ";
    strm << *values[i];
  }
  strm << " }";
}

/////////////////////////////////////////////////////

void ChoiceValue::PrintOn(std::ostream & strm) const
{
  strm << fieldname << " : " << *value;
}

void ChoiceValue::GenerateCplusplus(std::ostream & , std::ostream & cxx, std::ostream & ) const
{
  cxx  << type->GetTypeName() << "::" << fieldname << "::eid, (" << *value << ")" ;
}

/////////////////////////////////////////////////////////


ImportModule::ImportModule(std::string * name, SymbolList * syms)
  : fullModuleName(*name),
    shortModuleName(Module->GetImportModuleName(*name))
{
  delete name;

  ModuleDefinition* from = FindModule(fullModuleName.c_str());
  if (syms)
  {
    symbols = *syms;
    delete syms;

    for (size_t i = 0; i < symbols.size(); i++) {
      if (from)
        symbols[i]->AppendToModule(from,Module);
      else
        symbols[i]->AppendToModule(fullModuleName, Module);
    }
  }

  filename = ToLower(MakeIdentifierC(fullModuleName));
}


void ImportModule::PrintOn(std::ostream & strm) const
{
  strm << "  " << fullModuleName << " (" << shortModuleName << "):\n";
  for (size_t i = 0; i < symbols.size(); i++)
    strm << "    " << *symbols[i];
  strm << endl;
}


void ImportModule::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream &)
{
  ModuleDefinition* module = FindModule(fullModuleName.c_str());
  if(!module)
  {
    hdr << "#include <" << filename << ".h>\n";

    for (size_t i = 0; i < symbols.size(); i++)
    {
      if (symbols[i]->IsType() && symbols[i]->IsParameterisedImport())
      {
        cxx << "#include \"" << filename << "_t" << cppExt << "\"\n";
        break;
      }
    }
  }
  else
  {
    if(module != UsefulModule)
      hdr << "#include \"" << filename << ".h\"\n";

    for (size_t i = 0; i < symbols.size(); i++)
    {
      if (symbols[i]->IsType() && symbols[i]->IsParameterisedImport())
      {
        TypeBase* type = (module->FindType(symbols[i]->GetName())).get();
        if (!type || !type->HasParameters())
          break;

        cxx << "#include \"" << filename << "_t" << cppExt << "\"\n";
        break;
      }
    }
  }
}

void ImportModule::GenerateUsingDirectives(std::ostream & strm) const
{
  std::string name = MakeIdentifierC(fullModuleName);
  for (size_t i = 0; i < symbols.size(); ++i)
    symbols[i]->GenerateUsingDirective(name, strm);
}

void ImportModule::Adjust()
{
  bool needCreateSubModules = false;
  // find if there're nonparameterized type
  for (size_t i = 0 ; i < symbols.size(); ++i)
  {
    if (symbols[i]->IsType())
    {
      if (symbols[i]->IsParameterisedImport())
        return;

      needCreateSubModules = true;
    }
  }

  if (needCreateSubModules)
  {
    ModuleDefinition* module = FindModule(fullModuleName.c_str());
    if (module)
      filename = module->CreateSubModules(symbols);
  }
}


bool ImportModule::HasValuesOrObjects() const
{
  for (size_t i = 0; i < symbols.size(); ++i)
    if (symbols[i]->IsValuesOrObjects())
      return true;
  return false;
}

std::string ImportModule::GetCModuleName() const
{
  return MakeIdentifierC(fullModuleName);
}


/////////////////////////////////////////////////////////

ModuleDefinition::ModuleDefinition(const std::string& name, Tag::Mode defTagMode)
  : moduleName(name)
{
  defaultTagMode = defTagMode;
  exportAll = false;
  indentLevel = 1;

  for (size_t i = 0; i < Modules.size(); ++i)
  {
    std::string str = Modules[i]->moduleName;
    identifiers[str]= MODULEREFERENCE;
  }
  // Create sorted list for faster searching.
}

ModuleDefinition::~ModuleDefinition()
{
}


void ModuleDefinition::SetDefinitiveObjId(StringList& id)
{
  definitiveId.swap(id);
}

void ModuleDefinition::AddIdentifier(std::string* name, int idType)
{
  identifiers[*name]= idType;
  delete name;
}


void ModuleDefinition::AddImportedIdentifiers(StringList& imports_sl, const std::string& name)
{
  identifiers[name]= MODULEREFERENCE;
  ModuleDefinition* module = FindModule(name.c_str());
  if (module)
  {
    for (size_t i = 0; i < imports_sl.size(); ++i)
    {
      std::string identifier = imports_sl[i];
      str_replace(identifier,"{}","");
      identifiers[identifier]= module->GetIdentifierType(identifier);
    }
  }
  else
  {
    for (size_t i = 0; i < imports_sl.size(); ++i)
    {
      int id = TYPEREFERENCE;
      std::string identifier = imports_sl[i];
      if (identifier.find("{}") != -1)
      {
        str_replace(identifier,"{}","");
        id = PARAMETERIZEDTYPEREFERENCE;
      }
      identifiers[identifier]=id;
    }
  }

}

int  ModuleDefinition::GetIdentifierType(const std::string& id)
{
  if (identifiers.count(id))
    return identifiers[id];
  return -1;
}


void ModuleDefinition::SetExportAll()
{
  exportAll = true;
}


void ModuleDefinition::SetExports(SymbolList& syms)
{
  exports.swap(syms);
}


void ModuleDefinition::PrintOn(std::ostream & strm) const
{
  strm << moduleName << "\n"
          "Default Tags: " << Tag::modeNames[defaultTagMode] << "\n"
          "Exports:";
  if (exportAll)
    strm << " ALL";
  else {
    strm << "\n  ";
    for (size_t i = 0; i < exports.size(); i++)
      strm << *exports[i] << ' ';
    strm << endl;
  }

  strm << "Imports:\n" << imports << "\n"
          "Object Classes:\n" << objectClasses << "\n"
          "Types:\n" << types << "\n"
          "Values:\n" << values << "\n"
          "Objects:\n" << informationObjects << "\n"
          "ObjectSets:\n" << informationObjectSets << "\n";
}


void ModuleDefinition::AddType(TypePtr type)
{
  types.push_back(type);
  typeMap[type->GetName()] = type;
}


TypePtr ModuleDefinition::FindType(const std::string & name)
{
  const char* nam = name.c_str();
  if (typeMap.count(nam))
    return typeMap[nam];
  return TypePtr();
}


std::string ModuleDefinition::GetImportModuleName(const std::string & mName)
{
  if (importNames.count(mName))
    return importNames[mName];

  return ShortenName(mName);
}

void ModuleDefinition::AdjustModuleName(const std::string & sourcePath,bool isSubModule)
{
  shortModuleName = ShortenName(moduleName);
  cModuleName = MakeIdentifierC(moduleName);
  path = sourcePath + DIR_SEPARATOR;
  if (!isSubModule)
    path += ToLower(cModuleName);
  else if (types.size())
    path += (shortModuleName + "_" +ToLower(MakeIdentifierC(types[0]->GetName())));
  else
  {
    path += imports[0]->GetFileName();
    std::string tmp = imports[1]->GetFileName();
    path += tmp.substr(tmp.size()-tmp.find('_'));
  }

}

bool ModuleDefinition::ReorderTypes()
{
  // Reorder types
  // Determine if we need a separate file for template closure
  bool hasTemplates = false;
  size_t loopDetect = 0;
  size_t bubble = 0;

  typedef std::list<boost::shared_ptr<TypeBase> > TypesList;
  TypesList rtypes(types.begin(), types.end());

  TypesList::iterator itr , bubble_itr=rtypes.begin();

  while (bubble < types.size()) {

    bool makesReference = false;
    TypeBase*  bubbleType = bubble_itr->get();
    if (bubbleType->CanReferenceType()) {
      for (itr = bubble_itr; itr != rtypes.end(); ++itr) {
        if (bubbleType != itr->get() && bubbleType->ReferencesType(**itr)) {
          makesReference = true;
          break;
        }
      }
    }

    if (makesReference) {
      rtypes.push_back(*bubble_itr);
      rtypes.erase(bubble_itr++);
      if (loopDetect > rtypes.size()) {
        std::cerr << StdError(Fatal)
                  << "Recursive type definition: " << bubbleType->GetName()
                  << " references " << (*itr)->GetName() <<endl;
        break;
      }

      loopDetect++;
    }
    else {
      loopDetect = bubble;
      bubble++;
      ++bubble_itr;
    }

    if (bubbleType->HasParameters())
      hasTemplates = true;
  }

  //types.assign(rtypes.begin(), rtypes.end());
  types.clear();
  std::copy( rtypes.begin(), rtypes.end(), std::back_inserter(types));

  return hasTemplates;
}

void ModuleDefinition::GenerateCplusplus(const std::string & dir,
                                         unsigned classesPerFile,
                                         bool verbose)
{
  size_t i,classesCount = 1;

  Module = this;
  std::string dpath(dir);
  if (dpath.length())
    dpath += DIR_SEPARATOR;

  if (verbose)
    std::cout << "Processing files " << this->GetFileName() << "...\n";

  dpath += GetFileName();

  // Remove all PER Invisible constraint
  for (i = 0; i < types.size(); i++)
    types[i]->RemovePERInvisibleConstraints();

  CreateObjectSetTypes();

  // Flatten types by generating types for "inline" definitions
  for (i = 0; i < types.size(); i++) {
     types[i]->FlattenUsedTypes();
  }

  if (verbose)
    std::cout << "Sorting " << types.size() << " types..." << endl;

  bool hasTemplates = ReorderTypes();

  // Adjust all of the C++ identifiers prepending module name
  for (i = 0; i < types.size(); i++)
    types[i]->AdjustIdentifier(true);

  // Generate the code
  if (verbose)
    std::cout << "Generating code (" << types.size() << " classes) ..." << endl;


  // Output the special template closure file, if necessary
  std::string templateFilename;
  if (hasTemplates) {
    OutputFile templateFile;

    if (!templateFile.Open(dpath, "_t", cppExt))
      return;

    templateFile << "namespace " << cModuleName << "{\n";

    for (i = 0; i < types.size(); i++) {
      if (types[i]->HasParameters()) {
                    std::stringstream dummy;
                    Unfreezer uf(dummy);
                    types[i]->GenerateCplusplus(dummy, templateFile, dummy);
      }
    }

    templateFile << "} // namespace " << cModuleName << "\n\n";

    if (verbose)
      std::cout << "Completed " << templateFile.GetFilePath() << endl;

    templateFilename = ::GetFileName(dpath) + "_t" + cppExt;//templateFile.GetFilePath().GetFileName();
  }

  // Start the header file
  {
    OutputFile hdrFile;
    if (!hdrFile.Open(dpath, "", ".h"))
      return;

    hdrFile << "#ifndef __" << ToUpper(GetFileName()) << "_H\n"
               "#define __" << ToUpper(GetFileName()) << "_H\n"
               "\n"
               "#include \"asn1.h\"\n"
               "\n";

    hdrFile << std::setprecision(0);
    // Start the first (and maybe only) cxx file

    unsigned numFiles =1;
    if (classesPerFile >0)
      numFiles = types.size() / classesPerFile  +  (types.size() % classesPerFile ? 1 : 0);
    else
      classesPerFile = types.size();

    OutputFile cxxFile;

    if (!cxxFile.Open(dpath, numFiles > 1 ? "_1" : "" , cppExt))
      return;

    std::stringstream inl;
    Unfreezer unfreezer(inl);

    std::string headerName = ::GetFileName(dpath) + ".h";

    if (includeConfigH)
       cxxFile << "#ifdef HAVE_CONFIG_H" << endl
               << "#include <config.h>" << endl
               << "#endif\n\n";

   // if this define is generated - do_accept() in cxx file does not find inline function
//    cxxFile << "#define " << cModuleName << "_CXX\n";
    cxxFile << "#include \"" << headerName << "\"\n"
               "\n";

    // Include the template closure file.
    if (hasTemplates)
      cxxFile << "#include \"" << templateFilename << "\"\n\n";

    for_all(imports, boost::bind(&ImportModule::GenerateCplusplus, _1,
      boost::ref(hdrFile), boost::ref(cxxFile), boost::ref(inl)));

    if (!imports.empty()) {
      hdrFile << "\n\n";
      cxxFile << "\n\n";
    }

    for (i = 0; i < subModules.size() ; ++i)
      hdrFile << "#include \"" << subModules[i]->GetFileName() << ".h\"\n";

     if (dllMacroDLL.size() > 0)
     {
        hdrFile << endl
                << "#ifndef " << dllMacroExport << endl
                << "#if defined( " << dllMacroDLL << ") && defined(_MSC_VER)\n"
                << "#define " << dllMacroExport << " __declspec(dllimport)\n"
                << "#else\n"
                << "#define " << dllMacroExport << endl
                << "#endif // " << dllMacroDLL << endl
                << "#endif // " << dllMacroExport << "\n\n";
     }

    hdrFile << "namespace " << cModuleName << " {\n"
               "\n";
    for_all(imports, boost::bind(&ImportModule::GenerateUsingDirectives,
                                 _1,
                                 boost::ref(hdrFile)));

    cxxFile << "namespace " << cModuleName << "{\n\n";

    for(i = 0; i < values.size(); i++)
      values[i]->GenerateConst(hdrFile, cxxFile);

    for (i = 0; i < types.size(); i++) {
      if (i > 0 && i%classesPerFile == 0)
      {
        cxxFile << "} // namespace " << cModuleName << "\n\n";
        cxxFile.Close();
        classesCount = i/classesPerFile+1;

        if (verbose)
          std::cout << "Completed " << cxxFile.GetFilePath() << endl;

        std::stringstream suffix;
        Unfreezer unfreezer(suffix);
        suffix << '_' << i/classesPerFile+1;

        if (!cxxFile.Open(dpath, suffix.str().c_str(), cppExt))
        {
          std::cout << "Cannot open file " << dpath << "_" << i/classesPerFile+1 << cppExt << "\n";
          return;
        }

        if (includeConfigH)
            cxxFile << "#ifdef HAVE_CONFIG_H\n"
                       "#include <config.h>\n"
                       "#endif\n\n";

   // if this define is generated - do_accept() in cxx file does not find inline function
//        cxxFile << "#define " << cModuleName << "_CXX\n";
        cxxFile << "#include \"" << headerName << "\"\n"
                   "\n";

        // Include the template closure file.
        if (hasTemplates)
          cxxFile << "#include \"" << templateFilename << "\"\n";


        for_all(imports, boost::bind(&ImportModule::GenerateCplusplus, _1,
                boost::ref(hdrFile), boost::ref(cxxFile), boost::ref(inl)));


        if (!imports.empty()) {
          hdrFile << "\n\n";
          cxxFile << "\n\n";
        }


        cxxFile << "namespace " << cModuleName << "{\n"
                   "\n";
      }

      std::cerr << "Generating " << types[i]->GetName() << endl;

      if (types[i]->HasParameters()) {
        std::stringstream dummy;
        types[i]->GenerateCplusplus(hdrFile, dummy, inl);
      }
      else {
        types[i]->GenerateCplusplus(hdrFile, cxxFile, inl);
      }
    }

    i = classesCount+1;

    // Generate Information Classes

    for (i = 0; i < objectClasses.size(); ++i)
      objectClasses[i]->GenerateCplusplus(hdrFile, cxxFile, inl);


    // Generate Information Objects & Information ObjectSets
    GenerateClassModule(hdrFile, cxxFile, inl);


    //if (useNamespaces)
    cxxFile << "} // namespace " << cModuleName << "\n";

    if (!inl.str().empty())
    {
      OutputFile inlFile;
      if (!inlFile.Open(dpath, "", ".inl"))
        return;

     // if this define is generated - do_accept() in cxx file does not find inline function
//      inlFile << "#if !defined( " << cModuleName << "_CXX) && !defined(NO_"
      inlFile << "#if !defined(NO_"
              <<  ToUpper(GetFileName()) << "_INLINES)\n";

      if (!useReinterpretCast.empty())
      {
          // Workaround for compilers (e.g. VAC++5/AiX),
          // that won't compile the static_casts.
          inlFile << "\n#ifdef " << useReinterpretCast << "\n";
          inlFile << "#define static_cast reinterpret_cast\n";
          inlFile << "#endif\n\n";
      }

      inlFile << inl.str();

      if (!useReinterpretCast.empty())
      {
          inlFile << "#ifdef " << useReinterpretCast << "\n";
          inlFile << "#undef static_cast\n";
          inlFile << "#endif\n\n";
      }
      inlFile << "#endif\n";
      hdrFile << "#include \"" << ::GetFileName(dpath) + ".inl"//inlFile.GetFilePath().GetFileName()
              << "\"\n\n";
    }
	inl << ends;

    // Close off the files
    hdrFile << "} // namespace " << cModuleName << "\n\n";
    hdrFile << "#endif // __" << ToUpper(GetFileName()) << "_H\n";

    if (verbose)
      std::cout << "Completed " << cxxFile.GetFilePath() << endl;
  }
}


void ModuleDefinition::GenerateClassModule(std::ostream& hdrFile, std::ostream& cxxFile, std::ostream& inl)
{
  size_t i;
  std::stringstream tmphdr, tmpcxx;
  Unfreezer unfreezer1(tmphdr),unfreezer2(tmpcxx);
  for (i = 0 ; i < informationObjects.size(); ++i)
    informationObjects[i]->GenerateCplusplus(tmphdr, tmpcxx, inl);

  for (i = 0 ; i < informationObjects.size(); ++i)
    if (dynamic_cast<DefaultObjectDefn*>(informationObjects[i].get()))
    {
      InformationObject& obj = *informationObjects[i];
      std::string name = MakeIdentifierC(obj.GetName());
      if (obj.IsExtendable())
        tmphdr << "    " << name << "& get_" << name << "()\n"
               << "    {  return m_" << name << "; }\n";
      tmphdr << "    const " << name << "& get_" << name << "() const\n"
             << "    {  return m_" << name << "; }\n";
    }

  for (i = 0; i < informationObjectSets.size(); ++i)
  {
    InformationObjectSet& objSet = *informationObjectSets[i];
    const std::string &name = MakeIdentifierC(objSet.GetName()),
                      &className = MakeIdentifierC(objSet.GetObjectClass()->GetName());

    if (!objSet.HasParameters())
    {
      if (objSet.IsExtendable())
        tmphdr << "    " << className << "& get_" << name << "()\n"
               << "    {  return m_" << name << "; }\n";

      tmphdr << "    const " << className << "& get_" << name << "() const\n"
             << "    {  return m_" << name << "; }\n";
    }
  }

  if (!tmphdr.str().empty())
  {
    hdrFile << "class Module : public ASN1::Module\n"
            << "{\n"
            << "public:\n"
            << "    Module(";

    bool needComma = false;
    for (i = 0; i < imports.size(); ++i)
      if (imports[i]->HasValuesOrObjects())
      {
        if (needComma)
          hdrFile << ", ";
        hdrFile << imports[i]->GetCModuleName() << "::Module* " ;
        needComma = true;
      }

    hdrFile << ");\n";

    hdrFile << tmphdr.str();
	tmphdr << ends;

    hdrFile << "private:\n";
    for (i = 0 ; i < informationObjects.size(); ++i)
    {
      InformationObject& obj = *informationObjects[i];
      std::string name = MakeIdentifierC(obj.GetName());
      if (dynamic_cast<DefaultObjectDefn*>(&obj))
        hdrFile << "    " << name << " m_" << name << ";\n";
    }

    for (i = 0; i < informationObjectSets.size(); ++i)
    {
      InformationObjectSet& objSet = *informationObjectSets[i];
      if (!objSet.HasParameters())
        hdrFile << "    " << MakeIdentifierC(objSet.GetObjectClass()->GetName()) << " m_" << MakeIdentifierC(objSet.GetName()) << ";\n";
    }

    hdrFile << "}; // class Module\n\n";

    cxxFile << "#ifdef _MSC_VER\n"
            << "#pragma warning(disable: 4355)\n"
            << "#endif\n\n"
            << tmpcxx.str()
            << "Module::Module(";
    tmpcxx << ends;

    for (i = 0, needComma = false; i < imports.size(); ++i)
      if (imports[i]->HasValuesOrObjects())
      {
        if (needComma)
          cxxFile << ", ";
        cxxFile << imports[i]->GetCModuleName() << "::Module* "<< imports[i]->GetLowerCaseName() ;
        needComma = true;
      }
    cxxFile << ")\n";

    cxxFile << "{\n";
    for (i = 0, needComma = false; i < imports.size(); ++i)
      if (imports[i]->HasValuesOrObjects())
      {
        cxxFile << "  assert(" << imports[i]->GetLowerCaseName() << ");\n";
      }

    cxxFile << "  moduleName = \"" << moduleName << "\";\n";

    for (i = 0 ; i < informationObjects.size(); ++i)
    {
      InformationObject& obj = *informationObjects[i];
      if (dynamic_cast<DefaultObjectDefn*>(&obj))
        obj.GenerateInstanceCode(cxxFile);
    }

    std::for_each(informationObjectSets.begin(), informationObjectSets.end(),
                  boost::bind(&InformationObjectSet::GenerateInstanceCode, _1, boost::ref(cxxFile)));

    cxxFile << "}\n\n";
  }
}

ValuePtr ModuleDefinition::FindValue(const std::string& name)
{
  return FindWithName(values, name);
}

ObjectClassBasePtr ModuleDefinition::FindObjectClass(const std::string & name)
{
  ObjectClassBasePtr result = FindWithName(objectClasses, name);
  if (result.get())
    return result;

  if (name == "TYPE-IDENTIFIER")
  {
    // add the definition of TYPE-IDENTIFIER

    boost::shared_ptr<ObjectClassDefn> type_Identifier(new ObjectClassDefn);
    type_Identifier->SetName("TYPE-IDENTIFIER");
    std::auto_ptr<FieldSpecsList> fieldSpecs(new FieldSpecsList);

    boost::shared_ptr<FixedTypeValueFieldSpec> idField(
           new FixedTypeValueFieldSpec("&id"
                                 , TypePtr(new ObjectIdentifierType), false, true));

    fieldSpecs->push_back(idField);

    FieldSpecPtr typeField(new TypeFieldSpec("&Type"));

    fieldSpecs->push_back(typeField);
    type_Identifier->SetFieldSpecs(fieldSpecs);

    TokenGroupPtr tokens(new TokenGroup);
    tokens->AddToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
    tokens->AddToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIERD")));
    tokens->AddToken(TokenOrGroupSpecPtr(new Literal("BY")));
    tokens->AddToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));

    type_Identifier->SetWithSyntaxSpec(tokens);
    AddObjectClass(type_Identifier);
    return type_Identifier;
  }

  if (this != UsefulModule) {
    ObjectClassBasePtr useful = UsefulModule->FindObjectClass(name);

    if(useful) {
      ObjectClassBasePtr oc(new ImportedObjectClass(UsefulModule->GetName(),
                            name,
                            useful.get()));
      oc->SetName(name);
      AddObjectClass(oc);

      SymbolList *lst = new SymbolList;
      lst->push_back(SymbolPtr(new ObjectClassReference(name, false)));

      AddImport(ImportModulePtr(new ImportModule(new std::string(UsefulModule->GetName()), lst)));

      return oc;
    }
  }

  return result;
}

const InformationObject* ModuleDefinition::FindInformationObject(const std::string & name)
{
  return FindWithName(informationObjects, name).get();
}

const InformationObjectSet* ModuleDefinition::FindInformationObjectSet(const std::string & name)
{
  return FindWithName(informationObjectSets, name).get();
}

void ModuleDefinition::ResolveObjectClassReferences() const
{
  for (size_t i = 0; i < objectClasses.size(); ++i)
  objectClasses[i]->ResolveReference();
}



std::string ModuleDefinition::GetFileName()
{
  return ToLower(GetFileTitle(path));
}


std::string ModuleDefinition::CreateSubModules(SymbolList& exportedSymbols)
{
  unsigned clsPerFile = classesPerFile;
  if(!clsPerFile)
    clsPerFile = 101;

  if (types.size() <= clsPerFile)
    return GetFileName();

  size_t i, j;
  Module = this;
  for (i = 0; i < types.size(); ++i)
    types[i]->ResolveReference();

  typedef std::list<std::string> StringList;
  StringList unhandledSymbols;
  ModuleDefinitionPtr subModule(new ModuleDefinition(moduleName, defaultTagMode));

  TypesVector& exportedTypes = subModule->types;

  for (i = 0; i < exportedSymbols.size() ; ++i)
    if (exportedSymbols[i]->IsType())
    {
      TypePtr type = FindType(exportedSymbols[i]->GetName());
      if (type.get() != NULL)
      {
        subModule->AddType(type);
        typeMap.erase(type->GetName());
        types.erase(std::remove(types.begin(), types.end(), type), types.end());
      }
      else // this type may be in subModuls or importedModules
      {
        unhandledSymbols.push_back(exportedSymbols[i]->GetName());
      }
    }

  for (j = 0; j < exportedTypes.size(); ++j)
  {
    i = 0;
    while (i < types.size())
    {
      if ( exportedTypes[j]->UseType(*types[i]) )
      {
        subModule->AddType(types[i]);
        typeMap.erase(types[i]->GetName());
        types.erase(types.begin()+i);
      }
      else
        ++i;
    }
  }


  for (i = 0 ; i < subModules.size(); ++i)
  {
    bool hasSymbolInThisModule = false;
    StringList::iterator it = unhandledSymbols.begin();
    while (it != unhandledSymbols.end())
    {
      if (subModules[i]->HasType(*it))
      {
        if (!hasSymbolInThisModule)
        {
          ImportModule* im = new ImportModule(new std::string(GetName()), NULL);
          im->SetFileName(subModules[i]->GetFileName());
          subModule->imports.push_back(ImportModulePtr(im));
          hasSymbolInThisModule = true;
        }
        unhandledSymbols.erase(it++);
      }
      else
        ++it;
    }

    for (j = 0; j < exportedTypes.size() && !hasSymbolInThisModule; ++j)
    {
      for (size_t k = 0; k < subModules[i]->types.size() && !hasSymbolInThisModule; ++k)
      {
        if ( exportedTypes[j]->UseType(*subModules[i]->types[k]) )
        {
           ImportModule* im = new ImportModule(new std::string(GetName()), NULL);
           im->SetFileName(subModules[i]->GetFileName());
           subModule->imports.push_back(ImportModulePtr(im));
           hasSymbolInThisModule = true;
        }
      }
    }
  }

  if (exportedTypes.size() ==0 && subModule->imports.size() == 1 )
  {
    std::string result = subModule->imports[0]->GetFileName();
    return result;
  }
  else if (types.size()==0 && exportedTypes.size())
  { // all types has been exported, move them back
      for (i = 0; i < exportedTypes.size(); ++i)
          AddType(exportedTypes[i]);
      return GetFileName();
  }
  else if (exportedTypes.size() || subModule->imports.size() > 1)
  {
    for (i = 0 ; i < exportedTypes.size(); ++i)
    {
       ImportedType* importedType = dynamic_cast<ImportedType*>(exportedTypes[i].get());
       if (importedType)
       {
           ImportModule* im= subModule->FindImportedModule(importedType->GetModuleName());
           if (im == NULL)
           {
             im = new ImportModule(new std::string(importedType->GetModuleName()), NULL);
             ImportModule* theImportedModule = this->FindImportedModule(importedType->GetModuleName());
             assert(theImportedModule);
             im->SetFileName(theImportedModule->GetFileName());
             subModule->imports.push_back(ImportModulePtr(im));
           }
       }
    }

    subModule->AdjustModuleName(GetFileDirectory(path), true);
    subModules.push_back(subModule);
    Modules.push_back(subModule);
    return subModule->GetFileName();
  }
  else
  {
    std::cerr << "Unexpected Situation, Do not use selective imports option\n";
    std::cerr << "unresolved imported types (" << exportedTypes.size() << ") :" << exportedTypes << endl;
    std::cerr << subModule->imports.size() << endl;
    exit(1);
    //return "";
  }
}

void ModuleDefinition::AdjustImportedModules()
{
  for_all(imports, boost::mem_fn(&ImportModule::Adjust));
}

bool ModuleDefinition::IsExported(const std::string& name) const
{
  if(exportAll)
    return true;

  for(int i=0; i<exports.size(); ++i) {
    if(name==exports[i]->GetName())
      return true;
  }
  return false;
}

void ModuleDefinition::CreateObjectSetTypes()
{
  for (size_t i = 0; i < informationObjectSets.size(); ++i)
  {
    TypePtr objSetType(new ObjectSetType(informationObjectSets[i]));
    bool is_used(false);

    if(IsExported(informationObjectSets[i]->GetName()))
      is_used = true;
    else {
      for (size_t j = 0; j < types.size(); ++j)
      {
        if (types[j]->ReferencesType(*objSetType))
        {
          is_used = true;
          break;
        }
      }
    }

    if(is_used)
      AddType(objSetType);

  }
}

void ModuleDefinition::AddToRemoveList(const std::string& reference)
{
    removeList.push_back(reference);
}

void MoveTypes(TypesVector& sourceList, TypesVector& toBeExtractedList, TypesVector& targetList)
{
    for (size_t i = 0; i < sourceList.size(); ++i)
    {
        TypeBase& type = *sourceList[i];
        size_t j = 0;
        while (j < toBeExtractedList.size())
        {
            if (type.UseType(*toBeExtractedList[j]))
            {
                targetList.push_back(toBeExtractedList[j]);
                toBeExtractedList.erase(toBeExtractedList.begin()+j);
            }
            else
                ++j;
        }
    }
}

void ModuleDefinition::RemoveReferences(bool verbose)
{
  size_t i, j;
  TypesVector referencesToBeRemoved;
  for (i = 0 ; i < removeList.size(); ++i)
  {
      const std::string& ref = removeList[i];
      TypePtr typeToBeRemoved = FindType(ref);
      if (typeToBeRemoved.get() == NULL)
      {
          std::cout << GetName() << "." << ref << " doesn't exist, unable to remove it\n";
          continue;
      }

      // check if this type can be removed
      for (j = 0; j < types.size() ; ++j)
          if (!types[j]->CanRemoveType(*typeToBeRemoved) == TypeBase::OK)
              break;

      if (j == types.size()) // this type can be removed
      {
          // actual remove this type
          referencesToBeRemoved.push_back(typeToBeRemoved);
          types.erase(std::remove(types.begin(), types.end(),typeToBeRemoved), types.end());
          // remove all the references to this type
          for (j = 0; j < types.size(); ++j)
              types[j]->RemoveThisType(*typeToBeRemoved);
      }
      else
      {
          std::cout << "Unable to remove " << GetName() << "." << ref << "\n";
      }
  }

  MoveTypes(referencesToBeRemoved, types, referencesToBeRemoved);
  TypesVector typesToBePreserved;
  MoveTypes(types, referencesToBeRemoved, typesToBePreserved);
  MoveTypes(typesToBePreserved, referencesToBeRemoved, typesToBePreserved);

  types.insert(types.end(), typesToBePreserved.begin(), typesToBePreserved.end());

  for (i = 0; i < referencesToBeRemoved.size(); ++i)
      typeMap.erase(referencesToBeRemoved[i]->GetName());

  if (verbose)
  {
    for (i = 0; i < referencesToBeRemoved.size(); ++i)
      std::cout << "Remove Type : " << referencesToBeRemoved[i]->GetName() << "\n";
  }

}

ImportModule* ModuleDefinition::FindImportedModule(const std::string& theModuleName)
{
  //for (size_t i = 0; i < imports.GetSize(); ++i)
  //{
  //    if (imports[i].GetModuleName() == theModuleName)
  //        return &imports[i];
  //}
  //return NULL;
  return FindWithName(imports, theModuleName).get();
}

bool  ModuleDefinition::HasType(const std::string& name)
{
  return FindType(name).get() != NULL;
}

//////////////////////////////////////////////////////////////////////////////
FieldSpec::FieldSpec(const std::string& nam, bool optional)
: name(nam), isOptional(optional)
{
  identifier = MakeIdentifierC(name.substr(1));
}


FieldSpec::~FieldSpec()
{
}


void FieldSpec::PrintOn(std::ostream& strm) const
{
  strm << name << '\t' << GetField() ;
  if (isOptional)
    strm << " OPTIONAL";
}

void FieldSpec::GenerateTypeField(const std::string& ,
                   const std::string& ,
                   const TypeBase* ,
                   const std::string& ,
                   std::ostream& , std::ostream& , std::ostream& ) const
{
}
////////////////////////////////////////////////////////////////////////

TypeFieldSpec::TypeFieldSpec(const std::string& nam, bool optional, TypePtr defaultType)
: FieldSpec(nam, optional), type(defaultType)
{
}

TypeFieldSpec::~TypeFieldSpec()
{
}

bool TypeFieldSpec::HasDefault() const
{
  return type.get() != NULL;
}


std::string TypeFieldSpec::GetField() const
{
  return std::string("");
}

TypePtr TypeFieldSpec::GetDefaultType()
{
  return type;
}


int TypeFieldSpec::GetToken() const
{
  return TYPEFIELDREFERENCE;
}


std::string TypeFieldSpec::GetDefault() const
{
  if (type.get())
    return type->GetName();
  else
    return std::string("");
}


void TypeFieldSpec::PrintOn(std::ostream& strm) const
{
  strm << name ;

  if (isOptional)
    strm << " OPTIONAL";

  if (GetDefault().size() != 0)
  {
    strm << '\t' << "DEFAULT " << GetDefault();
  }
}

void TypeFieldSpec::ResolveReference() const
{
  if (type.get())
    type->ResolveReference();
}

void TypeFieldSpec::Generate_info_type_constructor(std::ostream& cxx) const
{
  if (!type.get())
    cxx << "  m_" << GetIdentifier() << "= NULL;\n";
}

void TypeFieldSpec::Generate_info_type_memfun(std::ostream& hdr) const
{
  Indent indent(hdr.precision()+4);
  hdr << indent << "ASN1::AbstractData* get_" << GetIdentifier() << "() const { return ";

  if (!type.get())
    hdr << "m_" << GetIdentifier() << " ? ASN1::AbstractData::create(m_"
        << GetIdentifier() << ") : NULL";
  else
    hdr << "ASN1::AbstractData::create(&" << type->GetTypeName() << "::theInfo)";

  hdr << "; }\n";
}

void TypeFieldSpec::Generate_info_type_mem(std::ostream& hdr) const
{
  Indent indent(hdr.precision()+4);
  if (!type.get())
    hdr << indent << "const void* m_" << GetIdentifier() << ";\n";
}


void TypeFieldSpec::Generate_value_type(std::ostream& hdr) const
{
  Indent indent(hdr.precision()+4);
  std::string fname = GetIdentifier();
  hdr << indent << "ASN1::AbstractData* get_" << fname << "() const { return second->get_"
      << fname << "(); }\n";
}

void TypeFieldSpec::GenerateTypeField(const std::string& templatePrefix,
                      const std::string& classNameString,
                      const TypeBase* keyType,
                      const std::string& objClassName,
                      std::ostream& hdr, std::ostream& cxx, std::ostream& ) const
{
  hdr << "    ASN1::AbstractData* get_" << GetIdentifier() << "(const "
      << keyType->GetTypeName() << "& key) const;\n";

  cxx << templatePrefix
      << "ASN1::AbstractData* " << classNameString << "::get_" <<  GetIdentifier() << "(const "
      << keyType->GetTypeName() << "& key) const\n"
         "{\n"
         "  if (objSet)\n"
         "  {\n"
         "    " << objClassName << "::const_iterator it = objSet->find(key);\n"
         "  if (it != objSet->end())\n"
         "      return it->get_" << GetIdentifier() << "();\n"
         "  }\n"
         "  return NULL;\n"
         "}\n\n";
}


////////////////////////////////////////////////////////////////////////////

FixedTypeValueFieldSpec::FixedTypeValueFieldSpec(const std::string& nam, TypePtr t,
                         bool optional, bool unique)
: FieldSpec(nam, optional), isUnique(unique), type(t)
{
}

FixedTypeValueFieldSpec::~FixedTypeValueFieldSpec()
{
}

void FixedTypeValueFieldSpec::SetDefault(ValuePtr value)
{
  defaultValue = value;
}

bool FixedTypeValueFieldSpec::HasDefault() const
{
  return defaultValue.get() != NULL;
}


std::string FixedTypeValueFieldSpec::GetField() const
{
  return type->GetName();
}


void FixedTypeValueFieldSpec::BeginParseSetting(FieldSettingList*) const
{
  ValueTypeContext = type;
  type->BeginParseValue();
}

void FixedTypeValueFieldSpec::EndParseSetting() const
{
  type->EndParseValue();
}

int FixedTypeValueFieldSpec::GetToken() const
{
  return FIXEDTYPEVALUEFIELDREFERENCE;
}

void FixedTypeValueFieldSpec::PrintOn(std::ostream& strm) const
{
  strm << name << '\t' << GetField() << '\t' << type->GetTypeName();
  if (isUnique)
    strm << " UNIQUE";

  if (isOptional)
    strm << " OPTIONAL";

  if (defaultValue.get() != NULL)
    strm << *defaultValue;
}

void FixedTypeValueFieldSpec::ResolveReference() const
{
  type->ResolveReference();
}

TypeBase* FixedTypeValueFieldSpec::GetFieldType()
{
  return type.get();
}

const TypeBase* FixedTypeValueFieldSpec::GetFieldType() const
{
  return type.get();
}

bool FixedTypeValueFieldSpec::GetKey(TypePtr& keyType, std::string& keyName)
{
  if (isUnique || std::string(type->GetAncestorClass()) == "ASN1::OBJECT_IDENTIFIER")
  {
    keyType = type;
    keyName = GetName();
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////

FixedTypeValueSetFieldSpec::FixedTypeValueSetFieldSpec(const std::string& nam,
                             TypePtr t,
                             bool optional)
: FieldSpec(nam, optional), type(t)
{
}


FixedTypeValueSetFieldSpec::~FixedTypeValueSetFieldSpec()
{
}

bool FixedTypeValueSetFieldSpec::HasDefault() const
{
  return defaultValueSet.get() != NULL;
}

void FixedTypeValueSetFieldSpec::BeginParseSetting(FieldSettingList*) const
{
  ValueTypeContext = type;
  type->BeginParseValueSet();
}

void FixedTypeValueSetFieldSpec::EndParseSetting() const
{
}

int FixedTypeValueSetFieldSpec::GetToken() const
{
  return FIXEDTYPEVALUESETFIELDREFERENCE;
}

void FixedTypeValueSetFieldSpec::PrintOn(std::ostream& strm) const
{
  strm << name << '\t' << GetField() << '\t';
  strm << type->GetTypeName();
  if (isOptional)
    strm << " OPTIONAL";

  if (defaultValueSet.get() != NULL)
    strm << *defaultValueSet;
}

void FixedTypeValueSetFieldSpec::ResolveReference() const
{
  type->ResolveReference();
}

TypeBase* FixedTypeValueSetFieldSpec::GetFieldType()
{
  return type.get();
}

const TypeBase* FixedTypeValueSetFieldSpec::GetFieldType() const
{
  return type.get();
}


/////////////////////////////////////////////////////////////////////

VariableTypeValueFieldSpec::VariableTypeValueFieldSpec(const std::string& nam,
                             const std::string& fieldname,
                             bool optional)
: FieldSpec(nam, optional), fieldName(fieldname)
{
}

VariableTypeValueFieldSpec::~VariableTypeValueFieldSpec()
{
}

bool VariableTypeValueFieldSpec::HasDefault() const
{
  return defaultValue.get() != NULL;
}

TypePtr GetFieldType(FieldSettingList* parsedFields, const std::string& fieldname)
{
  if (parsedFields != NULL)
  {
    for (size_t i = 0; i < parsedFields->size(); ++i)
    {
      FieldSetting& fieldSetting = *((*parsedFields)[i]);
      if (fieldSetting.GetName() == fieldname)
      {
        TypeSetting* setting = dynamic_cast<TypeSetting*>(fieldSetting.GetSetting());
        if (setting)
          return setting->GetType();
      }
    }
  }

  std::cerr << StdError(Fatal) << "Invalid Object Field : "<< fieldname << endl;
  return TypePtr();
}

void VariableTypeValueFieldSpec::EstablishFieldRelation(FieldSpecsList* specs)
{
  for (size_t i = 0; i < specs->size(); ++i)
  {
    FieldSpec& spec = *((*specs)[i]);
    if (spec.GetName() == fieldName)
    {
      TypeFieldSpec* tfspec = dynamic_cast<TypeFieldSpec*>(&spec);
      if (tfspec) {
        defaultType = tfspec->GetDefaultType();
        return;
      }
    }
  }

  std::cerr << StdError(Fatal) << "Invalid Object Class Definition at Field : "<< fieldName << endl;
}

void VariableTypeValueFieldSpec::BeginParseSetting(FieldSettingList* parsedFields) const
{
  TypePtr type = ::GetFieldType(parsedFields, GetField());
  if (type.get() == NULL)
    type = defaultType;

  if (type.get() != NULL) {
    ValueTypeContext = type;
    type->BeginParseValue();
  }
}

void VariableTypeValueFieldSpec::EndParseSetting() const
{
  ValueTypeContext->EndParseValue();
}

int VariableTypeValueFieldSpec::GetToken() const
{
  return VARIABLETYPEVALUEFIELDREFERENCE;
}


void VariableTypeValueFieldSpec::PrintOn(std::ostream& strm) const
{
  FieldSpec::PrintOn(strm);
  if (defaultValue.get() != NULL)
    strm << *defaultValue;
}

void VariableTypeValueFieldSpec::ResolveReference() const
{
  if (defaultType.get())
    defaultType->ResolveReference();
}

////////////////////////////////////////////////////////////////

VariableTypeValueSetFieldSpec::VariableTypeValueSetFieldSpec(const std::string& nam,
                               const std::string& fieldname,
                               bool optional)
: FieldSpec(nam, optional), fieldName(fieldname)
{
}

VariableTypeValueSetFieldSpec::~VariableTypeValueSetFieldSpec()
{
}

bool VariableTypeValueSetFieldSpec::HasDefault() const
{
  return defaultValueSet.get() != NULL;
}

void VariableTypeValueSetFieldSpec::EstablishFieldRelation(FieldSpecsList* specs)
{
  for (size_t i = 0; i < specs->size(); ++i)
  {
    FieldSpec& spec = *(*specs)[i];
    if (spec.GetName() == fieldName && dynamic_cast<TypeFieldSpec*>(&spec))
    {
      return;
    }
  }

  std::cerr << StdError(Fatal) << "Invalid Object Class Definition at Field : "<< fieldName << endl;
}

void VariableTypeValueSetFieldSpec::BeginParseSetting(FieldSettingList* parsedFields) const
{
  TypePtr type = ::GetFieldType(parsedFields, GetField());

  if (type.get() != NULL) {
    ValueTypeContext = type;
    type->BeginParseValueSet();
  }
}

void VariableTypeValueSetFieldSpec::EndParseSetting() const
{
}

int VariableTypeValueSetFieldSpec::GetToken() const
{
  return VARIABLETYPEVALUESETFIELDREFERENCE;
}

void VariableTypeValueSetFieldSpec::PrintOn(std::ostream& strm) const
{
  FieldSpec::PrintOn(strm);
  if (defaultValueSet.get() != NULL)
    strm << *defaultValueSet;
}

void VariableTypeValueSetFieldSpec::ResolveReference() const
{
  if (defaultValueSet.get())
    defaultValueSet->ResolveReference();
}

/////////////////////////////////////////////////////////////////////////////////

ObjectFieldSpec::ObjectFieldSpec(const std::string& nam,
                 DefinedObjectClass* oclass,
                 bool optional)
: FieldSpec(nam, optional),objectClass(oclass)
{
}

ObjectFieldSpec::~ObjectFieldSpec()
{
}

bool ObjectFieldSpec::HasDefault() const
{
  return obj.get() != NULL;
}


std::string ObjectFieldSpec::GetField() const
{
  return objectClass->GetName();
}

void ObjectFieldSpec::SetDefault(InformationObjectPtr dftObj)
{
  obj = dftObj;
}


void ObjectFieldSpec::BeginParseSetting(FieldSettingList*) const
{
  objectClass->BeginParseObject();
}

void ObjectFieldSpec::EndParseSetting() const
{
  objectClass->EndParseObject();
}

int ObjectFieldSpec::GetToken() const
{
  return OBJECTFIELDREFERENCE;
}

void ObjectFieldSpec::PrintOn(std::ostream & strm) const
{
  FieldSpec::PrintOn(strm);
  if (obj.get())
    strm << *obj;
}

void ObjectFieldSpec::ResolveReference() const
{
  objectClass->ResolveReference();
}

/////////////////////////////////////////////////////////////////////////////////


ObjectSetFieldSpec::ObjectSetFieldSpec(const std::string& nam, DefinedObjectClassPtr oclass,  bool optional)
: FieldSpec(nam, optional),objectClass(oclass)
{
}


ObjectSetFieldSpec::~ObjectSetFieldSpec()
{
}

bool ObjectSetFieldSpec::HasDefault() const
{
  return objSet.get() != NULL;
}


std::string ObjectSetFieldSpec::GetField() const
{
  return objectClass->GetName();
}

void ObjectSetFieldSpec::BeginParseSetting(FieldSettingList*) const
{
  objectClass->BeginParseObjectSet();
}

void ObjectSetFieldSpec::EndParseSetting() const
{
}

int ObjectSetFieldSpec::GetToken() const
{
  return OBJECTSETFIELDREFERENCE;
}

void ObjectSetFieldSpec::PrintOn(std::ostream & strm) const
{
  FieldSpec::PrintOn(strm);
  if (objSet.get())
    strm << *objSet;
}

void ObjectSetFieldSpec::ResolveReference() const
{
  objectClass->ResolveReference();
}

void ObjectSetFieldSpec::FwdDeclare(std::ostream& hdr) const
{
  if (objectClass->GetName() == "ERROR")
  {
      // ERROR is defined to 0 when <windows.h> is included,
      // undefine it.
      hdr << "#undef ERROR\n";
  }

  hdr << "class " << MakeIdentifierC(objectClass->GetName()) << ";\n"
      << endl;
}

void ObjectSetFieldSpec::Generate_info_type_constructor(std::ostream& cxx) const
{
  cxx << "  m_" << GetIdentifier() << "= NULL;\n";
}

void ObjectSetFieldSpec::Generate_info_type_memfun(std::ostream& hdr) const
{
  Indent indent(hdr.precision()+4);
  hdr << indent << "const " << MakeIdentifierC(objectClass->GetName()) <<"* get_" << GetIdentifier()
      << "() const { return m_" << GetIdentifier() << "; }\n";
  // default Object Set not supported;
}

void ObjectSetFieldSpec::Generate_info_type_mem(std::ostream& hdr) const
{
  Indent indent(hdr.precision()+4);
  hdr << indent << "const " << MakeIdentifierC(objectClass->GetName()) <<"* m_" << GetIdentifier() << ";\n";
    // default Object Set not supported;
}

void ObjectSetFieldSpec::Generate_value_type(std::ostream& hdr) const
{
  Indent indent(hdr.precision()+4);
  hdr << indent << "const "<< MakeIdentifierC(objectClass->GetName()) << "* get_" << GetIdentifier() << "() const { return second->get_"
      <<  GetIdentifier() << "(); }\n";
}

void ObjectSetFieldSpec::GenerateTypeField(const std::string& templatePrefix,
                         const std::string& classNameString,
                         const TypeBase* keyType,
                         const std::string& objClassName,
                         std::ostream& hdr, std::ostream& cxx, std::ostream& ) const
{
  hdr << "    const " << MakeIdentifierC(objectClass->GetName()) << "* get_" << GetIdentifier()
      << "(const " << keyType->GetTypeName() << "& key) const;\n";

  cxx << templatePrefix
      << "const "<< MakeIdentifierC(objectClass->GetName()) << "* " << classNameString
      << "::get_" << GetIdentifier() << "(const " << keyType->GetTypeName() <<"& key) const\n"
      << "{\n"
         "  if (objSet)\n"
         "  {\n"
         "    " << objClassName << "::const_iterator it = objSet->find(key);\n"
         "  if (it != objSet->end())\n"
         "      return it->get_Errors();\n"
         "  }\n"
         "  return NULL;\n"
         "}\n\n";
}

///////////////////////////////////////////////////////////////////////////////

void ObjectClassBase::SetName(const std::string& nam)
{
  name = nam;
}

int ObjectClassBase::GetFieldToken(const char* fieldname) const
{
  const FieldSpec* spec;
  if ((spec = GetField(fieldname)))
    return spec->GetToken();

  return -1;
}

//////////////////////////////////////////////////////////////////////////////
ObjectClassDefn::ObjectClassDefn()
: fieldSpecs(NULL)
{
}

ObjectClassDefn::~ObjectClassDefn()
{
}

void ObjectClassDefn::SetFieldSpecs(std::auto_ptr<FieldSpecsList> list)
{
  fieldSpecs = list;
  for (size_t i = 0; i < fieldSpecs->size(); ++i)
    (*fieldSpecs)[i]->EstablishFieldRelation(fieldSpecs.get());
}

FieldSpec* ObjectClassDefn::GetField(const std::string& fieldName)
{
  FieldSpec* result = FindWithName(*fieldSpecs, fieldName).get();
  if (result)
    return result;

  std::cerr << "Undefined Object Class Field : "<< GetName() << '.'<< fieldName << "\n";
  return NULL;
}

const FieldSpec* ObjectClassDefn::GetField(const std::string& fieldName) const
{
  FieldSpec* result = FindWithName(*fieldSpecs, fieldName).get();
  if (result)
    return result;

  std::cerr << "Undefined Object Class Field : "<< GetName() << '.'<< fieldName << "\n";
  return NULL;
}

void ObjectClassDefn::SetWithSyntaxSpec(TokenGroupPtr list)
{
  withSyntaxSpec = list;
  bool result = true;

  if (list.get() == NULL)
    return;

  for (size_t i = 0; i < withSyntaxSpec->size(); ++i)
    if (!(*withSyntaxSpec)[i].ValidateField(fieldSpecs.get()))
      result = false;

  if (result == false)
    exit(1);
}


bool ObjectClassDefn::VerifyDefaultSyntax(FieldSettingList* fieldSettings) const
{
  size_t fieldIndex=0, settingIndex=0;
  while (fieldIndex < fieldSpecs->size() && settingIndex < fieldSettings->size())
  {
    if ((*fieldSpecs)[fieldIndex]->GetName() == (*fieldSettings)[settingIndex]->GetName())
    {
      fieldIndex++;
      settingIndex++;
      continue;
    }
    else if ((*fieldSpecs)[fieldIndex]->IsOptional() || (*fieldSpecs)[fieldIndex]->HasDefault())
    {
      fieldIndex++;
      continue;
    }
    else
    {
      std::cerr << StdError(Fatal) << "Unrecognized field name : "
                << (*fieldSettings)[settingIndex]->GetName() <<  endl;
      exit(1);
    }
  }
  return true;
}

TokenGroupPtr ObjectClassDefn::GetWithSyntax() const
{
  return withSyntaxSpec;
}

void ObjectClassDefn::PreParseObject() const
{
  if (withSyntaxSpec.get())
    withSyntaxSpec->PreMakeDefaultSyntax(NULL);
}

void ObjectClassDefn::BeginParseObject() const
{
  classStack->push(const_cast<ObjectClassDefn*>(this));
  PreParseObject();
  BraceTokenContext = OBJECT_BRACE;
}

void ObjectClassDefn::EndParseObject() const
{
  BraceTokenContext = '{';
//  classStack->Pop();
}

void ObjectClassDefn::BeginParseObjectSet() const
{
  classStack->push(const_cast<ObjectClassDefn*>(this));
  PreParseObject();
  BraceTokenContext = OBJECTSET_BRACE;
  InObjectSetContext++;
}

void ObjectClassDefn::EndParseObjectSet() const
{
  if (--InObjectSetContext == 0)
    BraceTokenContext = '{';
  else
    BraceTokenContext = OBJECT_BRACE;

  if (withSyntaxSpec.get())
    withSyntaxSpec->CancelMakeDefaultSyntax();
}

void ObjectClassDefn::PrintOn(std::ostream& strm) const
{
  strm << name << "\t::= CLASS\n{\n";
  size_t i;
  for (i = 0 ; i < fieldSpecs->size(); ++i)
  {
    strm << '\t' << *(*fieldSpecs)[i];
    if (i != fieldSpecs->size()-1)
      strm << ',';
    strm << endl;
  }
  strm << "}\n";
  if (withSyntaxSpec.get())
  {
    strm << "WITH SYNTAX\n"
         << "{\n"
         << *withSyntaxSpec
         << "}\n";
  }
}

void ObjectClassDefn::ResolveReference() const
{
  std::for_each(fieldSpecs->begin(), fieldSpecs->end(), boost::mem_fn(&FieldSpec::ResolveReference));
}

void ObjectClassDefn::ResolveKey()
{
  if (!keyType.get())
  {
    std::find_if(fieldSpecs->begin(), fieldSpecs->end(),
      boost::bind(&FieldSpec::GetKey, _1, boost::ref(keyType), boost::ref(keyName)));
  }
}

void ObjectClassDefn::GenerateCplusplus(std::ostream& hdr, std::ostream& cxx, std::ostream&)
{
  if (GetName() == "TYPE-IDENTIFIER" || GetName() == "ABSTRACT-SYNTAX")
    return;

  hdr << "\n"
      << "//\n"
      << "// " << GetName() << "\n"
      << "//\n\n";

  size_t i;
  for (i = 0; i < fieldSpecs->size(); ++i)
  {
    std::stringstream strm;
    Unfreezer unfreezer(strm);
    (*fieldSpecs)[i]->FwdDeclare(strm);
    std::string str = strm.str();
    strm << ends;
    if (str.find(GetName()) != -1)
      continue;
    hdr << str;
  }

  ResolveKey();

  const std::string &className = MakeIdentifierC(GetName());

  hdr << "class " << className << "\n"
      << "{\n"
      << "public:\n"
      << "    typedef "<< keyType->GetTypeName() << " key_type;\n"
      << "    class info_type\n"
      << "    {\n"
      << "    public:\n";

  if (fieldSpecs->size())
  {
    hdr << "        info_type();\n";
    cxx << className << "::info_type::info_type()\n"
        << "{\n";

    std::for_each(fieldSpecs->begin(), fieldSpecs->end(),
                  boost::bind(&FieldSpec::Generate_info_type_constructor, _1, boost::ref(cxx)) );

    cxx << "}\n\n";
  }
  hdr << std::setprecision(4);

  std::for_each(fieldSpecs->begin(), fieldSpecs->end(),
                boost::bind(&FieldSpec::Generate_info_type_memfun, _1, boost::ref(hdr)) );

  hdr << "    protected:\n";

  std::for_each(fieldSpecs->begin(), fieldSpecs->end(),
                boost::bind(&FieldSpec::Generate_info_type_mem, _1, boost::ref(hdr)) );

  hdr << "    };\n\n"
      << "    typedef const info_type* mapped_type;\n\n"
      << "    class value_type : public ASN1_STD pair<key_type, mapped_type>\n"
      << "    {\n"
      << "        typedef ASN1_STD pair<key_type, mapped_type> Inherited;\n"
      << "    public:\n"
         "        value_type(const key_type& key, mapped_type mt) : Inherited(key,mt) {}\n";

  std::for_each(fieldSpecs->begin(), fieldSpecs->end(),
                boost::bind(&FieldSpec::Generate_value_type, _1, boost::ref(hdr)) );

  hdr << "    };\n\n"
         "    typedef value_type& reference;\n"
         "    typedef const value_type& const_reference;\n"
         "    typedef value_type* pointer;\n"
         "    typedef const value_type* const_pointer;\n"
         "    typedef ASN1::AssocVector<key_type, mapped_type> map_type;\n\n"
         "private:\n"
         "#if defined(HP_aCC_RW)\n"
         "    typedef ASN1_STD bidirectional_iterator<value_type> my_iterator_traits;\n"
         "    typedef ASN1_STD bidirectional_iterator<const value_type> my_const_iterator_traits;\n"
         "#else\n"
         "    typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, value_type> my_iterator_traits;\n"
         "    typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, const value_type> my_const_iterator_traits;\n"
         "#endif\n"
         "public:\n"
         "    class iterator : public my_iterator_traits\n"
         "    {\n"
         "    public:\n"
         "        iterator() {}\n"
         "        iterator(map_type::iterator i) : itsIter(i) {}\n"
         "        map_type::iterator base()   const { return itsIter; }\n"
         "        const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }\n"
         "        const_pointer operator->()  const { return static_cast<const_pointer>(&*itsIter); }\n"
         "        iterator& operator++()           { ++itsIter; return *this; }\n"
         "        iterator& operator--()           { --itsIter; return *this; }\n"
         "        iterator operator++(int)         { iterator t(*this); ++itsIter; return t; }\n"
         "        iterator operator--(int)         { iterator t(*this); --itsIter; return t; }\n\n"
         "        bool operator==(const iterator& r) const    { return itsIter == r.itsIter; }\n"
         "        bool operator!=(const iterator& r) const    { return itsIter != r.itsIter; }\n"
         "    private:\n"
         "        map_type::iterator itsIter;\n"
         "    };\n"
         "    class const_iterator : public my_const_iterator_traits\n"
         "    {\n"
         "    public:\n"
         "        const_iterator() {}\n"
         "        const_iterator(" << className << "::iterator i) : itsIter(i.base()) {}\n"
         "        const_iterator(map_type::const_iterator i) : itsIter(i) {}\n"
         "        map_type::const_iterator base() const { return itsIter; }\n\n"
         "        const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }\n"
         "        const_pointer operator->() const { return static_cast<const_pointer>(&*itsIter); }\n\n"
         "        const_iterator& operator++()          { ++itsIter; return *this; }\n"
         "        const_iterator& operator--()          { --itsIter; return *this; }\n"
         "        const_iterator operator++(int)        { const_iterator t(*this); ++itsIter; return t; }\n"
         "        const_iterator operator--(int)        { const_iterator t(*this); --itsIter; return t; }\n\n"
         "        bool operator==(const const_iterator& r) const    { return itsIter == r.itsIter; }\n"
         "        bool operator!=(const const_iterator& r) const    { return itsIter != r.itsIter; }\n"
         "    private:\n"
         "        map_type::const_iterator itsIter;\n"
         "    };\n\n"
         "//#if defined(BOOST_NO_STD_ITERATOR) || defined (BOOST_MSVC_STD_ITERATOR)\n"
         "//    typedef std::reverse_bidirectional_iterator<iterator, value_type> reverse_iterator;\n"
         "//    typedef std::reverse_bidirectional_iterator<const_iterator, const value_type> const_reverse_iterator;\n"
         "//#else\n"
         "//    typedef std::reverse_iterator<iterator, value_type> reverse_iterator;\n"
         "//    typedef std::reverse_iterator<const_iterator, const value_type> const_reverse_iterator;\n"
         "//#endif\n\n"
         "    typedef map_type::key_compare key_compare;\n"
         "    typedef map_type::difference_type difference_type;\n"
         "    typedef map_type::size_type size_type;\n\n"
      << "    " << className << "(){}\n"
         "    template <class InputIterator>\n"
         "            " << className << "(InputIterator first, InputIterator last)\n"
         "        : rep(first, last) {}\n"
         "    " << className << "(const " << className << "& that)\n"
         "        : rep(that.rep) {}\n\n"
         "    " << className << "& operator=(const " << className << "& that)\n"
         "        { " << className << " tmp(that); swap(tmp);  return *this; }\n\n"
         "    // iterators\n"
         "    iterator begin() { return iterator(rep.begin()); }\n"
         "    const_iterator begin() const { return const_iterator(rep.begin()); }\n"
         "    iterator end() { return iterator(rep.end()); }\n"
         "    const_iterator end() const { return const_iterator(rep.end()); }\n\n"
         "//    reverse_iterator rbegin() { return reverse_iterator(end()); }\n"
         "//    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }\n"
         "//    reverse_iterator rend() { return reverse_iterator(begin()); }\n"
         "//    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }\n"
         "    // capacity\n"
         "    bool empty() const { return rep.empty(); }\n"
         "    size_type size() const { return rep.size(); }\n"
         "    // modifiers\n"
         "    ASN1_STD pair<iterator, bool> insert(const value_type& x)\n"
         "    {\n"
         "        ASN1_STD pair<map_type::iterator, bool> r = rep.insert(x);\n"
         "        return ASN1_STD pair<iterator, bool>(r.first, r.second);\n"
         "    }\n"
         "    iterator insert(iterator position, const value_type& x)\n"
         "    { return iterator(rep.insert(position.base(), x)); }\n"
         "    void insert(const_iterator first, const_iterator last)\n"
         "    { rep.insert(first.base(), last.base()); }\n"
         "    void erase(iterator position) { rep.erase(position.base()); }\n"
         "    void erase(const key_type& key) { rep.erase(key); }\n"
         "    void erase(iterator first, iterator last) { rep.erase(first.base(), last.base()); }\n"
         "    void swap(" << className << "& that) { rep.swap(that.rep); }\n"
         "    void clear() { rep.clear(); }\n"
         "    key_compare key_comp() const { return rep.key_comp(); }\n"
         "    // operations\n"
         "    iterator find(const key_type& key) { return iterator(rep.find(key)); }\n"
         "    const_iterator find(const key_type& key) const { return const_iterator(rep.find(key)); }\n"
         "    size_type count(const key_type& key) const { return rep.count(key); }\n"
         "private:\n"
         "    map_type rep;\n"
         "};\n";

  hdr << std::setprecision(0);
}


/////////////////////////////////////////////////////////////
DefinedObjectClass::DefinedObjectClass(ObjectClassBase* ref)
: reference(ref)
{
}


DefinedObjectClass::DefinedObjectClass(const std::string& nam, ObjectClassBase* ref)
: referenceName(nam), reference(ref)
{
}

ObjectClassBase* DefinedObjectClass::GetReference()
{
    if (reference == NULL)
        reference = Module->FindObjectClass(referenceName).get();
    return reference;
}

const ObjectClassBase* DefinedObjectClass::GetReference() const
{
    ResolveReference();
    assert(reference);
    return reference;
}

FieldSpec* DefinedObjectClass::GetField(const std::string& fieldName)
{
    return GetReference()->GetField(fieldName);
}

const FieldSpec* DefinedObjectClass::GetField(const std::string& fieldName) const
{
    return GetReference()->GetField(fieldName);
}

bool DefinedObjectClass::VerifyDefaultSyntax(FieldSettingList* fieldSettings) const
{
    return GetReference()->VerifyDefaultSyntax(fieldSettings);
}


bool DefinedObjectClass::HasLiteral(const std::string& str) const
{
    return GetReference()->HasLiteral(str);
}


TokenGroupPtr DefinedObjectClass::GetWithSyntax() const
{
    return GetReference()->GetWithSyntax();
}

void DefinedObjectClass::PreParseObject() const
{
    GetReference()->PreParseObject();
}

void DefinedObjectClass::BeginParseObject() const
{
    GetReference()->BeginParseObject();
}

void DefinedObjectClass::EndParseObject() const
{
    GetReference()->EndParseObject();
}

void DefinedObjectClass::BeginParseObjectSet() const
{
    GetReference()->BeginParseObjectSet();
}

void DefinedObjectClass::EndParseObjectSet() const
{
    GetReference()->EndParseObjectSet();
}

void DefinedObjectClass::PrintOn(std::ostream& strm) const
{
    if (name.size() != 0)
        strm << name << " ::= ";
    strm << referenceName;
}

void DefinedObjectClass::ResolveReference() const
{
    if (reference == NULL)
        reference = Module->FindObjectClass(referenceName).get();
}

TypeBase* DefinedObjectClass::GetFieldType(const std::string& fieldName)
{
    return GetReference()->GetFieldType(fieldName);
}

const TypeBase* DefinedObjectClass::GetFieldType(const std::string& fieldName) const
{
    return GetReference()->GetFieldType(fieldName);
}


//////////////////////////////////////////////////////////////////

void Literal::PrintOn(std::ostream & strm) const
{
  strm << name;
  // use skipws flag to indicate whether to output endl or not
  if (((strm.flags() & std::ios::skipws) == 0) && name == ",")
     strm << endl;
}


TokenOrGroupSpec::MakeDefaultSyntaxResult  Literal::MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList*)
{
    if (token->MatchLiteral(name))
        return CONSUMED_AND_EXHAUSTED;
    else
        return FAIL;
}


/////////////////////////////////////////////////////////////////////

void PrimitiveFieldName::PrintOn(std::ostream & strm) const
{
  strm << name;
  // use skipws flag to indicate whether to output endl or not
  if ((strm.flags() & std::ios::skipws) == 0)
      strm  << endl;
}

bool PrimitiveFieldName::ValidateField(FieldSpecsList* fields)
{
  if (field = FindWithName(*fields, name).get())
    return true;

  std::cerr << "Illegal field name \"" << name << "\"\n";
  return false;
}

void PrimitiveFieldName::PreMakeDefaultSyntax(FieldSettingList* settings)
{
    assert(field);
    field->BeginParseSetting(settings);
}

void  PrimitiveFieldName::CancelMakeDefaultSyntax() const
{
    assert(field);
    field->EndParseSetting();
}

TokenOrGroupSpec::MakeDefaultSyntaxResult  PrimitiveFieldName::MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* settings)
{
  assert(field);
  TokenOrGroupSpec::MakeDefaultSyntaxResult result = FAIL;
  FieldSettingPtr setting = token->MatchSetting(name);
  if (setting.get() != NULL)
  {
    settings->push_back(FieldSettingPtr(setting));
    result =  CONSUMED_AND_EXHAUSTED;
  }

  field->EndParseSetting();
  return result;
}

///////////////////////////////////////////////////////////////////

void TokenGroup::PrintOn(std::ostream & strm) const
{
  if (optional)
  {
    strm << "[" ;
    strm.setf(std::ios::skipws);     // use skipws flag to indicate not to output endl for PrimitiveField or ','
  }

  for (size_t i = 0; i < tokenOrGroupSpecList.size(); ++i)
  {
    if ((strm.flags() & std::ios::skipws) ==0 && i ==0)
      strm << '\t';
    strm << *tokenOrGroupSpecList[i];
    if (i != tokenOrGroupSpecList.size()-1)
      strm << '\t';
  }

  if (optional)
      strm << "]\n";

  strm.unsetf(std::ios::skipws);
}

bool TokenGroup::ValidateField(FieldSpecsList* fields)
{
    for (size_t i = 0; i < tokenOrGroupSpecList.size(); ++i)
        if (!tokenOrGroupSpecList[i]->ValidateField(fields))
           return false;
     return true;

}

TokenOrGroupSpec::MakeDefaultSyntaxResult  TokenGroup::MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* settings)
{
    assert(token);
    assert(settings);

    MakeDefaultSyntaxResult result = NOT_CONSUMED;
    while ( cursor < tokenOrGroupSpecList.size())
    {
        if ((result = tokenOrGroupSpecList[cursor]->MakeDefaultSyntax(token, settings)) == NOT_CONSUMED)
        {
            cursor++;
            continue;
        }
        break;
    }

    switch (result)
    {
    case FAIL:
        if (optional && cursor == 0)
            result = NOT_CONSUMED;
        break;
    case CONSUMED_AND_EXHAUSTED:
        if (++cursor < tokenOrGroupSpecList.size())
        {
            result = CONSUMED_AND_NOT_EXHAUSTED;
            tokenOrGroupSpecList[cursor]->PreMakeDefaultSyntax(settings);
        }
        break;
    case NOT_CONSUMED: // exhausted, i.e. cursor == tokenOrGroupSpecList.GetSize()
        break;
    case CONSUMED_AND_NOT_EXHAUSTED    :
        break;
    }


    return result;
}

TokenGroup::TokenGroup(const TokenGroup& other)
: tokenOrGroupSpecList(other.tokenOrGroupSpecList), optional(other.optional), cursor(other.cursor)
{
}

void TokenGroup::PreMakeDefaultSyntax(FieldSettingList* settings)
{
    if (cursor < tokenOrGroupSpecList.size())
        tokenOrGroupSpecList[cursor]->PreMakeDefaultSyntax(settings);
}

void TokenGroup::CancelMakeDefaultSyntax() const
{
    if (tokenOrGroupSpecList.size())
        tokenOrGroupSpecList[0]->CancelMakeDefaultSyntax();
}


bool TokenGroup::HasLiteral(const std::string& str) const
{
    for (size_t i = 0; i < tokenOrGroupSpecList.size(); ++i)
    {
        if (tokenOrGroupSpecList[i]->HasLiteral(str))
            return true;
    }
    return false;
}

void TokenGroup::Reset()
{
  cursor = 0;
  for_all(tokenOrGroupSpecList, boost::mem_fn(&TokenOrGroupSpec::Reset));
}

/////////////////////////////////////////////////////////////////////////////////

FieldSetting::FieldSetting(const std::string& fieldname,  std::auto_ptr<Setting> aSetting)
: name(fieldname), setting(aSetting)
{
    identifier = name.c_str()+1;
}

FieldSetting::~FieldSetting()
{
}

void FieldSetting::PrintOn(std::ostream & strm) const
{
    strm << name << '\t' << *setting;
}


void FieldSetting::GenerateCplusplus(const std::string& prefix, std::ostream & hdr, std::ostream & cxx, std::ostream & inl, unsigned& flag)
{
    setting->GenerateCplusplus(prefix, identifier, hdr, cxx, inl, flag);
}

void FieldSetting::GenerateInitializationList(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  std::stringstream tmp;
  Unfreezer unfreezer(tmp);
  setting->GenerateInitializationList(hdr, tmp, inl);
  if (!tmp.str().empty()) {
     cxx << identifier << "(" << tmp.str() << ")";
     tmp << ends;
  }
}

void FieldSetting::GenerateInfo(std::ostream& hdr)
{
  setting->GenerateInfo(identifier, hdr);
}

bool FieldSetting::IsExtendable() const
{
  return setting->IsExtendable();
}

void FieldSetting::GenerateInstanceCode(const std::string& prefix, std::ostream& cxx) const
{
  setting->GenerateInstanceCode(prefix + identifier + ".", cxx);
}

/////////////////////////////////////////////////////////////////////////////////

InformationObject::~InformationObject()
{
}

const std::string& InformationObject::GetClassName() const
{
    return GetObjectClass()->GetName();
}

void InformationObject::PrintBase(std::ostream & strm) const
{
    if (name.size())
    {
        strm << name << ' ';
        if (parameters.get())
            strm << *parameters << ' ';
        strm << GetClassName() << " ::= ";
    }
}

void InformationObject::SetParameters(std::auto_ptr<ParameterList> list)
{
    parameters = list;
}

/////////////////////////////////////////////////////////////////////////////////

DefinedObject::DefinedObject(const std::string& nam, const InformationObject* ref)
: referenceName(nam), reference(ref)
{
    name = nam;
}


const InformationObject* DefinedObject::GetReference() const
{
    if (reference == NULL)
        reference = Module->FindInformationObject(referenceName);
    if (reference == NULL)
    {
        std::cerr << StdError(Fatal) << "Invalid Object" << GetName();
        exit(1);
    }
    return reference;
}

const ObjectClassBase* DefinedObject::GetObjectClass() const
{
    if (DummyParameters)
    {
        Parameter* param = DummyParameters->GetParameter(GetName().c_str());
        if ((param != NULL) && dynamic_cast<ObjectParameter*>(param))
        {
            ObjectParameter* p = (ObjectParameter*) param;
            return p->GetGovernor();
        }
    }
    return GetReference()->GetObjectClass();
}

bool DefinedObject::VerifyObjectDefinition()
{
    const InformationObject* obj = GetReference();
    if (obj && GetClassName() == obj->GetClassName())
        return true;
    std::cerr << StdError(Fatal) << "Unmatched Object Definition : " << GetName();
    return false;
}

const Setting* DefinedObject::GetSetting(const std::string& fieldname) const
{
    return GetReference()->GetSetting(fieldname);
}

void DefinedObject::PrintOn(std::ostream & strm) const
{
    PrintBase(strm);
    strm << referenceName << endl;
}

void DefinedObject::GenerateInstanceCode(std::ostream& cxx) const
{
  if (dynamic_cast<const DefaultObjectDefn*>(GetReference()))
    cxx << "get_" << MakeIdentifierC(referenceName) << "().make()";
  else
    reference->GenerateInstanceCode(cxx);
}
/////////////////////////////////////////////////////////////////////////////////

void ImportedObject::GenerateInstanceCode(std::ostream& cxx) const
{
  cxx << ToLower(MakeIdentifierC(moduleName)) << "->get_" << MakeIdentifierC(referenceName) << "().make()";
}

/////////////////////////////////////////////////////////////////////////////////

bool DefaultObjectDefn::SetObjectClass(const ObjectClassBase* definedClass)
{
    referenceClass = definedClass;
    return VerifyObjectDefinition();
}

bool DefaultObjectDefn::VerifyObjectDefinition()
{
    assert(referenceClass);
    return referenceClass->VerifyDefaultSyntax(settings.get());
}

const Setting* DefaultObjectDefn::GetSetting(const std::string& fieldname) const
{

  const FieldSetting*  result = FindWithName(*settings, fieldname).get();

  if (result) {
    return  result->GetSetting();
  }

    return NULL;
}

void DefaultObjectDefn::PrintOn(std::ostream & strm) const
{
    PrintBase(strm);
    strm << "{\n";
    for (size_t i = 0; i < settings->size(); ++i)
        strm << '\t' << *(*settings)[i] << endl;
    strm << "}\n";
}


void DefaultObjectDefn::GenerateCplusplus(std::ostream& hdr , std::ostream & cxx, std::ostream & inl)
{
  std::stringstream tmphdr;
  Unfreezer unfreezer(tmphdr);
  tmphdr << std::setprecision(8);
  unsigned flags =0;
  size_t i;
  std::string prefix("Module::");
  prefix += name;
  for (i = 0; i < settings->size(); ++i)
    (*settings)[i]->GenerateCplusplus(prefix, tmphdr, cxx, inl, flags);


  if (!tmphdr.str().empty())
  {
    int has_type_setting = (flags & Setting::has_type_setting);
    int has_objectSet_setting = (flags & Setting::has_objectSet_setting);

    const std::string &className = MakeIdentifierC(referenceClass->GetName());

    std::string keyName = referenceClass->GetKeyName().substr(1);
    hdr << "    class " << name << endl
        << "    {\n"
        << "    public:\n"
        << "        " << name << "();\n"
        << tmphdr.str()
        << "        " << className << "::value_type make() const\n"
           "        { return " << className << "::value_type(" << keyName << ",&info ); }\n"
           "    private:\n";
    tmphdr << ends;

    if (has_type_setting || has_objectSet_setting)
    {
      Indent indent(8);
      hdr << indent << "class Info : public " << className << "::info_type\n"
          << indent << "{\n"
          << indent << "public:\n";

      if (has_objectSet_setting)
        hdr << indent+4 << "Info(" << name << "* parent)\n";
      else
        hdr << indent+4 << "Info()\n";
      hdr << indent+4 << "{\n"
          << std::setprecision(16);

      std::for_each(settings->begin(), settings->end(),
                    boost::bind(&FieldSetting::GenerateInfo, _1, boost::ref(hdr)));
      hdr << indent +4 << "}\n";
      hdr << indent << "};\n";
    }
    else
    {
      hdr << "        typedef " << className << "::info_type Info;\n";
    }
    hdr << "        Info info;\n"
           "    }; // class "<< name << "\n";
    cxx << "Module::" << name << "::" << name << "()";

    bool hasInitizationList = false;
    for (i = 0 ; i < settings->size(); ++i)
    {
      std::stringstream tmp;
      Unfreezer unfreezer(tmp);
      (*settings)[i]->GenerateInitializationList(hdr, tmp, inl);
      if (!tmp.str().empty())
      {
        if (!hasInitizationList)
          cxx << "\n  : " << tmp.str();
        else
        {
          cxx << ", " << tmp.str();
          hasInitizationList = true;
        }
        tmp << ends;
      }
    }

    if (has_objectSet_setting)
      cxx << ", info(this)";

    cxx << "\n"
        << "{\n"
        << "}\n\n";
  }
}

bool DefaultObjectDefn::IsExtendable() const
{
  if (std::find_if(settings->begin(), settings->end(),
                   boost::mem_fn(&FieldSetting::IsExtendable))
      != settings->end())
    return true;

  return false;
}

void DefaultObjectDefn::GenerateInstanceCode(std::ostream& cxx) const
{
   std::string nam("  m_");
   nam += MakeIdentifierC(name);
   nam += ".";

   std::for_each(settings->begin(), settings->end(),
                  boost::bind(&FieldSetting::GenerateInstanceCode,
                  _1, nam, boost::ref(cxx)) );
}
/////////////////////////////////////////////////////////////////////////////////

ObjectFromObject::ObjectFromObject(InformationObjectPtr referenceObj,
                   const std::string& fld)
: refObj(referenceObj), field(fld)
{
}

ObjectFromObject::~ObjectFromObject()
{
}

const ObjectClassBase* ObjectFromObject::GetObjectClass() const
{
    return refObj->GetObjectClass();
}


void ObjectFromObject::PrintOn(std::ostream & strm) const
{
    if (name.size() > 0)
        strm << name << ' ' << refObj->GetClassName() << " ::= ";
    strm << *refObj << "." << field;
}

const Setting* ObjectFromObject::GetSetting(const std::string& fieldname) const
{
    return refObj->GetSetting(fieldname);
}

bool ObjectFromObject::VerifyObjectDefinition()
{
    return true;
}

void ObjectFromObject::GenerateInstanceCode(std::ostream& ) const
{
  // not implemented
}

/////////////////////////////////////////////////////////////////////////////////


FieldSettingPtr SettingToken::MatchSetting(const std::string& fieldName)
{
  return FieldSettingPtr (new FieldSetting(fieldName, setting));
}

/////////////////////////////////////////////////////////////////////////////////

void TypeSetting::GenerateCplusplus(const std::string& prefix, const std::string& name, std::ostream& hdr, std::ostream& cxx, std::ostream& inl, unsigned& flag)
{
    type->SetOuterClassName(prefix);
    type->SetName(name);
    type->GenerateCplusplus(hdr, cxx, inl);
    flag |= has_type_setting;
}

void TypeSetting::GenerateInfo(const std::string& name,std::ostream& hdr)
{
  Indent indent(hdr.precision());
  hdr << indent << "m_" << name << "= &" << name << "::theInfo;\n";
}

void TypeSetting::PrintOn(std::ostream & strm) const
{
  strm << *type;
}


///////////////////////////////////////////////////////////////////////////////

void ValueSetting::GenerateCplusplus(const std::string& , const std::string& name, std::ostream& hdr, std::ostream& , std::ostream& , unsigned& flag)
{
  Indent indent(hdr.precision());
  if (type->GetTypeName() != "ASN1::BOOLEAN")
      hdr << indent << "const " << type->GetTypeName() << ' ' << name << ";\n";
  flag |= has_value_setting;
}

void ValueSetting::GenerateInitializationList(std::ostream& hdr, std::ostream& cxx, std::ostream& inl)
{
    if (type->GetTypeName() != "ASN1::BOOLEAN")
        value->GenerateCplusplus(hdr, cxx, inl);
}


ValueSetting::ValueSetting(TypePtr typeBase, ValuePtr valueBase)
: type(typeBase), value(valueBase)
{
}

ValueSetting::~ValueSetting()
{
}

void ValueSetting::PrintOn(std::ostream & strm) const
{
    if (type.get())
        strm << type->GetTypeName() << " : ";
    strm << *value;
}

////////////////////////////////////////////////////////

ValueSetSetting::ValueSetSetting(ValueSetPtr set)
:valueSet(set)
{
}

ValueSetSetting::~ValueSetSetting()
{
}

void ValueSetSetting::PrintOn(std::ostream & strm) const
{
    strm << *valueSet;
}

void ValueSetSetting::GenerateCplusplus(const std::string& , const std::string& , std::ostream& , std::ostream& , std::ostream& , unsigned& flag)
{
  flag |= has_valueSet_setting;
}

/////////////////////////////////////////////////////////////////////////////////

ObjectSetting::ObjectSetting(InformationObjectPtr obj, ObjectClassBase* objClass)
: objectClass(objClass), object(obj)
{
}

void ObjectSetting::PrintOn(std::ostream & strm) const
{
    strm << *object;
}

void ObjectSetting::GenerateCplusplus(const std::string& , const std::string& , std::ostream& , std::ostream& , std::ostream& , unsigned& flag)
{
  flag |= has_object_setting;
}

ObjectSetting::~ObjectSetting()
{
}

/////////////////////////////////////////////////////////////////////////////////

ObjectSetSetting::~ObjectSetSetting()
{
}

void ObjectSetSetting::PrintOn(std::ostream & strm) const
{
    strm << *objectSet;
}

void ObjectSetSetting::GenerateCplusplus(const std::string& , const std::string& name, std::ostream& hdr, std::ostream& , std::ostream& , unsigned& flag)
{
  Indent indent(hdr.precision());
  hdr << indent;
//  if (!objectSet->IsExtendable())
//    hdr << "const ";
  hdr << MakeIdentifierC(objectClass->GetName()) << " " << name << ";\n" ;

  flag |= has_objectSet_setting;
}

void ObjectSetSetting::GenerateInfo(const std::string& name,std::ostream& hdr)
{
  Indent indent(hdr.precision());
  hdr << indent << "m_" << name << "= &(parent-> " << name << ");\n";
}

void ObjectSetSetting::GenerateInstanceCode(const std::string& prefix, std::ostream& cxx) const
{
  objectSet->GenerateObjectSetInstanceCode(prefix + "insert(", cxx);
}

/////////////////////////////////////////////////////////////////////////////////

InformationObjectSetDefn::InformationObjectSetDefn(const std::string& nam,
                        ObjectClassBasePtr objClass,
                        ConstraintPtr set,
                        ParameterListPtr list)
: name(nam),objectClass(objClass), rep(set), parameters(list)
{
}

InformationObjectSetDefn::~InformationObjectSetDefn()
{
}

const ObjectClassBase* InformationObjectSetDefn::GetObjectClass() const
{
    return objectClass.get();
}

ValueSetPtr InformationObjectSetDefn::GetValueSetFromValueField(const std::string& field) const
{
    return rep->GetValueSetFromValueField(field);
}

ValueSetPtr InformationObjectSetDefn::GetValueSetFromValueSetField(const std::string& field) const
{
    return rep->GetValueSetFromValueSetField(field);
}

ConstraintPtr InformationObjectSetDefn::GetObjectSetFromObjectField(const std::string& field) const
{
    return rep->GetObjectSetFromObjectField(field);
}

ConstraintPtr InformationObjectSetDefn::GetObjectSetFromObjectSetField(const std::string& field) const
{
    return rep->GetObjectSetFromObjectSetField(field);
}

void InformationObjectSetDefn::PrintOn(std::ostream & strm) const
{
  if (name.size())
  {
      strm << name << ' ' ;
      if (parameters.get())
          strm << *parameters << ' ';
      strm << *objectClass << " ::= " ;
  }

  strm << '{';
  rep->PrintElements(strm);
  strm << "}\n";
}

void InformationObjectSetDefn::GenerateInstanceCode(std::ostream& cxx) const
{
  if (!HasParameters())
  {
    rep->GenerateObjectSetInstanceCode("  m_" + MakeIdentifierC(name)+".insert(",cxx);
  }
}

void InformationObjectSetDefn::GenerateType(std::ostream& hdr, std::ostream& cxx, std::ostream& ) const
{
  std::string typeName = MakeIdentifierC(GetName());
  std::string classNameString = typeName;
  std::string templatePrefix;

  if (parameters.get())
    parameters->GenerateCplusplus(templatePrefix, classNameString);

  hdr << "\n"
         "//\n"
         "// " << classNameString << "\n"
         "//\n\n";

  std::string objClassName = MakeIdentifierC(GetObjectClass()->GetName());

  hdr << templatePrefix
      << "class " << typeName << "\n"
         "{\n"
         "public:\n"
         "    typedef " << objClassName << " element_type;\n"
         "    " << typeName << "(ASN1::CoderEnv& env);\n";

  cxx << "\n"
         "//\n"
         "// " << classNameString << "\n"
         "//\n\n";

  bool needDeleteObjSet = false;
  cxx << templatePrefix
      << classNameString << "::" << typeName << "(ASN1::CoderEnv& env)\n"
      << "{\n";
  needDeleteObjSet = GenerateTypeConstructor(cxx);
  cxx << "}\n\n";
  if (needDeleteObjSet)
    hdr << "    ~" << typeName << "() { delete objSet; }\n";

  hdr << "    ";
  if (!IsExtendable()) hdr << "const ";
    hdr << objClassName << "* get() const { return objSet; }\n";

  hdr << "    ";
  if (!IsExtendable())
    hdr << "const ";
  hdr << objClassName << "* operator->() const { return objSet; }\n";

  hdr << "    bool extensible() const { return ";
  if (IsExtendable())
    hdr << "true";
  else
    hdr << "false";
  hdr << "; }\n";

  hdr << "private:\n    ";
  if (!IsExtendable())
    hdr << "const ";

  hdr << objClassName << "* objSet;\n"
         "};\n\n";
}

bool InformationObjectSetDefn::GenerateTypeConstructor(std::ostream& cxx) const
{
  std::stringstream tmp;
  Unfreezer unfreezer(tmp);
  rep->GenerateObjSetAccessCode(tmp);
  if (!tmp.str().empty())
  {
    cxx << tmp.str();
    tmp << ends;
    return true;
  }

  cxx << "  ASN1::Module* module = env.find(\"" << Module->GetName() << "\");\n"
      << "  if (module)\n"
      << "  objSet =  &(static_cast<"<< Module->GetCModuleName() << "::Module*>(module)->get_"
      << MakeIdentifierC(GetName()) << "());\n"
      << "  else\n"
      << "    objSet = NULL;\n";
  return false;
}


/////////////////////////////////////////////////////////////////////////////////

DefinedObjectSet::DefinedObjectSet(const std::string& ref)
: referenceName(ref)
, reference(NULL)
{
}

const InformationObjectSet* DefinedObjectSet::GetReference() const
{
    if (reference == NULL)
        reference = Module->FindInformationObjectSet(referenceName);
    if (reference == NULL)
    {
        std::cerr << StdError(Fatal) << "Invalid ObjectSet " << GetName();
        exit(1);
    }
    return reference;
}


const ObjectClassBase* DefinedObjectSet::GetObjectClass() const
{
    if (DummyParameters)
    {
        Parameter* param = DummyParameters->GetParameter(GetName().c_str());
        if (param && dynamic_cast<ObjectParameter*>(param))
        {
            ObjectParameter* p = (ObjectParameter*) param;
            return p->GetGovernor();
        }
    }
    return GetReference()->GetObjectClass();
}

ValueSetPtr DefinedObjectSet::GetValueSetFromValueField(const std::string& field) const
{
    return GetReference()->GetValueSetFromValueField(field);
}

ValueSetPtr DefinedObjectSet::GetValueSetFromValueSetField(const std::string& field) const
{
    return GetReference()->GetValueSetFromValueSetField(field);
}

ConstraintPtr DefinedObjectSet::GetObjectSetFromObjectField(const std::string& field) const
{
    return GetReference()->GetObjectSetFromObjectField(field);
}

ConstraintPtr DefinedObjectSet::GetObjectSetFromObjectSetField(const std::string& field) const
{
    return GetReference()->GetObjectSetFromObjectSetField(field);
}

void DefinedObjectSet::PrintOn(std::ostream & strm) const
{
    strm << referenceName;
}

bool DefinedObjectSet::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}
/////////////////////////////////////////////////////////////////////////////////

ParameterizedObjectSet::ParameterizedObjectSet(const std::string& ref,
                         ActualParameterListPtr args)
: DefinedObjectSet(ref), arguments(args)
{
}

ParameterizedObjectSet::~ParameterizedObjectSet()
{
}

std::string ParameterizedObjectSet::GetName() const
{
  std::stringstream strm;
  Unfreezer unfreezer(strm);
  strm << referenceName << *arguments;
  return std::string(strm.str());
}

void ParameterizedObjectSet::PrintOn(std::ostream & strm) const
{
    strm << referenceName << *arguments;
}

/////////////////////////////////////////////////////////////////////////////////

ObjectSetFromObject::ObjectSetFromObject(InformationObjectPtr obj, const std::string& fld)
: refObj(obj), field(fld)
{
}

ObjectSetFromObject::~ObjectSetFromObject()
{
}

const ObjectClassBase* ObjectSetFromObject::GetObjectClass() const
{
    return refObj->GetObjectClass();
}

std::string ObjectSetFromObject::GetName() const
{
    return refObj->GetName() + field;
}

ValueSetPtr ObjectSetFromObject::GetValueSetFromValueField(const std::string& fld) const
{
    return GetRepresentation()->GetValueSetFromValueField(fld);
}

ValueSetPtr ObjectSetFromObject::GetValueSetFromValueSetField(const std::string& fld) const
{
    return GetRepresentation()->GetValueSetFromValueSetField(fld);
}

ConstraintPtr ObjectSetFromObject::GetObjectSetFromObjectField(const std::string& fld) const
{
    return GetRepresentation()->GetObjectSetFromObjectField(fld);
}

ConstraintPtr ObjectSetFromObject::GetObjectSetFromObjectSetField(const std::string& fld) const
{
    return GetRepresentation()->GetObjectSetFromObjectSetField(fld);
}

void ObjectSetFromObject::PrintOn(std::ostream & strm) const
{
    strm << *refObj << '.' << field;
}

Constraint* ObjectSetFromObject::GetRepresentation() const
{
    if (rep.get() == NULL)
    {
        SingleObjectConstraintElement elem(refObj);
        rep = elem.GetObjectSetFromObjectSetField(field);
    }
    return rep.get();
}

bool ObjectSetFromObject::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}



/////////////////////////////////////////////////////////////////////////////////

ObjectSetFromObjects::ObjectSetFromObjects(ObjectSetConstraintElementPtr objSet,
                       const std::string& fld)
: refObjSet(objSet), field(fld)
{
}

ObjectSetFromObjects::~ObjectSetFromObjects()
{
}

const ObjectClassBase* ObjectSetFromObjects::GetObjectClass() const
{
    return refObjSet->GetObjectClass();
}

std::string ObjectSetFromObjects::GetName() const
{
    return refObjSet->GetName() + field;
}

ValueSetPtr ObjectSetFromObjects::GetValueSetFromValueField(const std::string& fld) const
{
    return GetRepresentation()->GetValueSetFromValueField(fld);
}

ValueSetPtr ObjectSetFromObjects::GetValueSetFromValueSetField(const std::string& fld) const
{
    return GetRepresentation()->GetValueSetFromValueSetField(fld);
}

ConstraintPtr ObjectSetFromObjects::GetObjectSetFromObjectField(const std::string& fld) const
{
    return GetRepresentation()->GetObjectSetFromObjectField(fld);
}

ConstraintPtr ObjectSetFromObjects::GetObjectSetFromObjectSetField(const std::string& fld) const
{
    return GetRepresentation()->GetObjectSetFromObjectSetField(fld);
}

void ObjectSetFromObjects::PrintOn(std::ostream & strm) const
{
    strm << *refObjSet << '.' << field;
}

ConstraintPtr ObjectSetFromObjects::GetRepresentation() const
{
    if (rep.get() == NULL)
    {
        rep = refObjSet->GetObjectSetFromObjectField(field);
        if (rep.get() == NULL)
            rep = refObjSet->GetObjectSetFromObjectSetField(field);
    }
    return rep;
}

bool ObjectSetFromObjects::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}

void ObjectSetFromObjects::GenerateObjSetAccessCode(std::ostream& strm)
{
  strm << "  " << MakeIdentifierC(refObjSet->GetName()) <<  " tmp(env);\n"
       << "  if (tmp.get())\n"
       << "  {\n"
       << "    element_type *tempObjSet = new element_type;\n"
       << "    typename " << MakeIdentifierC(refObjSet->GetName()) << "::element_type::const_iterator first = tmp->begin(), last = tmp->end();\n"
       << "    for (; first != last; ++first)\n"
       << "    {\n"
       << "       const element_type* infoFromObjects = first->get_" << field.substr(1) << "();\n"
       << "       if (infoFromObjects)\n"
       << "         tempObjSet->insert(infoFromObjects->begin(), infoFromObjects->end());\n"
       << "    }\n"
       << "    objSet = tempObjSet;\n"
       << "  }\n";
}



/////////////////////////////////////////////////////////////////////////////////

DefaultSyntaxBuilder::DefaultSyntaxBuilder(TokenGroupPtr tkGrp)
: tokenGroup(tkGrp)
, setting(new FieldSettingList)
{
}

DefaultSyntaxBuilder::~DefaultSyntaxBuilder()
{
}

void DefaultSyntaxBuilder::AddToken(DefinedSyntaxToken* token)
{
    TokenOrGroupSpec::MakeDefaultSyntaxResult result =
            tokenGroup->MakeDefaultSyntax(token, setting.get());
    if ( result == TokenOrGroupSpec::FAIL || result == TokenOrGroupSpec::NOT_CONSUMED)
    {
        std::cerr << StdError(Fatal) << "Invalid Object Definition\n";
        exit(1);
    }

}

std::auto_ptr<FieldSettingList> DefaultSyntaxBuilder::GetDefaultSyntax()
{
    EndSyntaxToken token;
    if (tokenGroup->MakeDefaultSyntax(&token, setting.get())
        == TokenOrGroupSpec::NOT_CONSUMED)
    {
        tokenGroup->Reset();
        return setting;
    }

    std::cerr << StdError(Fatal) << "Incomplete Object Definition\n";
    return std::auto_ptr<FieldSettingList>();
}

void DefaultSyntaxBuilder::ResetTokenGroup()
{
  tokenGroup->Reset();
}

//////////////////////////////////////////////////////////////////////

SingleObjectConstraintElement::SingleObjectConstraintElement(InformationObjectPtr obj)
: object(obj)
{
}

SingleObjectConstraintElement::~SingleObjectConstraintElement()
{
}

void SingleObjectConstraintElement::PrintOn(std::ostream & strm ) const
{
    strm << *object;
}

ValueSetPtr SingleObjectConstraintElement::GetValueSetFromValueField(const std::string& fld) const
{
    ValueSetting* setting = (ValueSetting*) object->GetSetting(fld);
    if (setting)
    {
      std::auto_ptr<ConstraintElementVector> elems ( new ConstraintElementVector );
      elems->push_back(ConstraintElementPtr(new SingleValueConstraintElement(setting->GetValue())));
      ConstraintPtr con(new Constraint(elems, false));
      return ValueSetPtr(new ValueSetDefn(TypePtr(new DefinedType(setting->GetType(), "") ) , con));
    }
    else
    {
        std::cerr << StdError(Fatal) << "Retrieve non-extisted object field : " <<  fld << "\n";
        exit(1);
    }
    //return ValueSetPtr(new ValueSetDefn);
}

ValueSetPtr SingleObjectConstraintElement::GetValueSetFromValueSetField(const std::string& fld) const
{
    ValueSetSetting* setting = (ValueSetSetting*) object->GetSetting(fld);
    if (setting)
        return setting->GetValueSet();
    else
    {
        std::cerr << StdError(Fatal) << "Retrieve non-extisted object field : "<<  fld << "\n";
        exit(1);
    }

    //return ValueSetPtr(new ValueSetDefn);
}

ConstraintPtr SingleObjectConstraintElement::GetObjectSetFromObjectField(const std::string& fld) const
{
  ConstraintPtr result;
  ObjectSetting* setting = (ObjectSetting*) object->GetSetting(fld);
  if (setting)
  {
    std::auto_ptr<ConstraintElementVector> elems ( new ConstraintElementVector );
    elems->push_back(ConstraintElementPtr(
    new SingleObjectConstraintElement(setting->GetObject())));
    result.reset(new Constraint(elems, false));
  }
  return result;
}

ConstraintPtr SingleObjectConstraintElement::GetObjectSetFromObjectSetField(const std::string& fld) const
{
    ObjectSetSetting* setting = (ObjectSetSetting*) object->GetSetting(fld);
    if (setting)
        return setting->GetObjectSet();
    else
        return ConstraintPtr();
}

bool SingleObjectConstraintElement::HasPERInvisibleConstraint(const Parameter& ) const
{
    return false;
}

void SingleObjectConstraintElement::GenerateObjectSetInstanceCode(const std::string& prefix, std::ostream& cxx) const
{
  cxx << prefix;
  object->GenerateInstanceCode(cxx);
  cxx << ");\n";
}

/////////////////////////////////////////////////////////////////////////

ValueSetDefn::ValueSetDefn()
: elements(NULL)
{
}

ValueSetDefn::ValueSetDefn(TypePtr base, ConstraintPtr cons)
: type(base), elements(new ConstraintElementVector)
{
    elements->swap(cons->GetStandardElements());

    elements->insert(elements->end(),
                     cons->GetExtensionElements().begin(),
                     cons->GetExtensionElements().end());

    extendable = cons->IsExtendable();
}

ValueSetDefn::~ValueSetDefn()
{
}

void ValueSetDefn::Union(ValueSetPtr& other)
{
    if (type.get() == NULL)
        type.reset(new DefinedType(other->GetType(), ""));
    if (elements.get() == NULL)
        elements = std::auto_ptr<ConstraintElementVector>( new ConstraintElementVector);
    elements->insert(elements->end(), other->GetElements()->begin(), other->GetElements()->end());
}

TypePtr ValueSetDefn::MakeValueSetType()
{
    type->AddConstraint(ConstraintPtr(new Constraint(elements, extendable)));
    type->SetAsValueSetType();
    return type;
}

void ValueSetDefn::Intersect(ValueSetPtr& other)
{
  assert(other.get());

  if (elements.get() == NULL || elements->size() == 0)
    type.reset(new DefinedType(other->GetType(),""));

  std::auto_ptr<ConstraintElementVector> root(new ConstraintElementVector);
  root->swap(*elements);
  root->insert(root->end(), other->GetElements()->begin(), other->GetElements()->end());
  elements = std::auto_ptr<ConstraintElementVector>(new ConstraintElementVector);
  elements->push_back(ConstraintElementPtr(new ElementListConstraintElement(root)));
}

void ValueSetDefn::PrintOn(std::ostream & strm) const
{
    strm << '{';
    PrintVector(strm, *elements, '|');
    strm << '}';
}


void ValueSetDefn::ResolveReference() const
{
    if (type.get())
        type->ResolveReference();
}

////////////////////////////////////////////////////////////////////////////////////////

ValueSetFromObject::ValueSetFromObject(InformationObjectPtr obj
                     , const std::string& fld)
: object(obj), field(fld)
{
}

ValueSetFromObject::~ValueSetFromObject()
{
}

void ValueSetFromObject::Union(ValueSetPtr& other)
{
    GetRepresentation()->Union(other);
}

void ValueSetFromObject::Intersect(ValueSetPtr& other)
{
    GetRepresentation()->Intersect(other);
}

TypePtr ValueSetFromObject::MakeValueSetType()
{
    return GetRepresentation()->MakeValueSetType();
}

void ValueSetFromObject::PrintOn(std::ostream & strm) const
{
    strm << object->GetName() << ".&" << field;
}



TypePtr ValueSetFromObject::GetType()
{
    return GetRepresentation()->GetType();
}

ConstraintElementVector* ValueSetFromObject::GetElements()
{
    return GetRepresentation()->GetElements();
}

ValueSetPtr ValueSetFromObject::GetRepresentation()
{
    if (rep.get() == NULL)
    {
        SingleObjectConstraintElement cons(object);
        rep = cons.GetValueSetFromValueSetField(field);
    }
    return rep;
}

//////////////////////////////////////////////////////////////////


ValueSetFromObjects::ValueSetFromObjects(ObjectSetConstraintElementPtr objSet,
                     const std::string& fld)
: objectSet(objSet), field(fld)
{
}

ValueSetFromObjects::~ValueSetFromObjects()
{
}

void ValueSetFromObjects::Union(ValueSetPtr& other)
{
    GetRepresentation()->Union(other);
}

void ValueSetFromObjects::Intersect(ValueSetPtr& other)
{
    GetRepresentation()->Intersect(other);
}

TypePtr ValueSetFromObjects::MakeValueSetType()
{
    return GetRepresentation()->MakeValueSetType();
}

void ValueSetFromObjects::PrintOn(std::ostream & strm) const
{
    strm << *objectSet << "." << field;
}


TypePtr ValueSetFromObjects::GetType()
{
    return GetRepresentation()->GetType();
}

ConstraintElementVector* ValueSetFromObjects::GetElements()
{
    return GetRepresentation()->GetElements();
}

ValueSetPtr ValueSetFromObjects::GetRepresentation()
{
    if (rep.get() == NULL)
    {
        if (isupper(field[1]))
            rep = objectSet->GetValueSetFromValueSetField(field);
        else
            rep = objectSet->GetValueSetFromValueField(field);
    }
    return rep;
}

///////////////////////////////////////////////////////////////////////////////////

Symbol::Symbol(const std::string& sym, bool param)
: name(sym), parameterized(param)
{
}

void Symbol::PrintOn(std::ostream& strm) const
{
    strm << name;
    if (parameterized) strm << "{}";
}
///////////////////////////////////////////////////////////////////////////////////

void TypeReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to)
{
  assert(from);
  assert(to);
  TypePtr refType = from->FindType(name);
  boost::shared_ptr<ImportedType> type;
  if (refType.get())
    type.reset(new ImportedType(refType));
  else
    type.reset(new ImportedType(name, false));
  type->SetModuleName(from->GetName());
  to->AddType(type);
}

void TypeReference::AppendToModule(const std::string& fromName, ModuleDefinition* to)
{
  assert(to);
  boost::shared_ptr<ImportedType> type(new ImportedType(name, parameterized));
  type->SetModuleName(fromName);
  to->AddType(type);
}

void TypeReference::GenerateUsingDirective(const std::string& , std::ostream & ) const
{
   // strm << "using " << moduleName << "::" << name << ";\n";
}


///////////////////////////////////////////////////////////////////////////////////

void ValueReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to)
{
    assert(from);
    assert(to);
    to->AddValue(ValuePtr(new ImportedValue(from->GetName(),name,from->FindValue(name))));
}

///////////////////////////////////////////////////////////////////////////////////

void ObjectClassReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to)
{
  assert(from);
  assert(to);
  ObjectClassBasePtr oc(new ImportedObjectClass(from->GetName(),
              name,
              from->FindObjectClass(name).get() ) );
  to->AddObjectClass(oc);
}

void ObjectClassReference::GenerateUsingDirective(const std::string& moduleName, std::ostream & strm) const
{
  strm << "using " << moduleName << "::" << MakeIdentifierC(name) << ";\n";
}

///////////////////////////////////////////////////////////////////////////////////

void ObjectSetReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to)
{
  assert(from);
  assert(to);
  InformationObjectSetPtr ios(new ImportedObjectSet(from->GetName(),from->FindInformationObjectSet(name)));
                              to->AddInformationObjectSet(ios);
}

void ObjectSetReference::GenerateUsingDirective(const std::string& moduleName, std::ostream & strm) const
{
  strm << "using " << moduleName << "::" << MakeIdentifierC(name) << ";\n";
}

///////////////////////////////////////////////////////////////////////////////////

void ObjectReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to)
{
  assert(from);
  assert(to);
  InformationObjectPtr io(new ImportedObject(from->GetName(),name,from->FindInformationObject(name)));
                          to->AddInformationObject(io);
}

///////////////////////////////////////////////////////////////////////////////////
Parameter::Parameter(const std::string& nam)
: name(nam), identifierType(TYPEREFERENCE)
{
}



Parameter::Parameter(const std::string& nam, int type)
: name(nam), identifierType(type)
{
}

Parameter::Parameter(const Parameter& other)
: name(other.name), identifierType(other.identifierType)
{
}


int Parameter::GetIdentifierType()
{
    return identifierType;
}

const std::string& Parameter::GetName() const
{
    return name;
}

void Parameter::PrintOn(std::ostream& strm) const
{
    strm << name;
}

bool Parameter::ReferencedBy(const TypeBase& type) const
{
  DefinedType t(name);
  return type.ReferencesType(t);
}

ActualParameterPtr Parameter::MakeActualParameter() const
{
  return ActualParameterPtr(new ActualTypeParameter(TypePtr(new DefinedType(name))));
}

/////////////////////////////////////////////////

ValueParameter::ValueParameter(TypePtr gover, const std::string& nam)
: Parameter(nam, isupper(nam[0]) ? TYPEREFERENCE : VALUEREFERENCE)
, governor(gover)
{
}

ValueParameter::ValueParameter(const ValueParameter& other)
: Parameter(other)
 , governor(other.governor)
{
}

ValueParameter::~ValueParameter()
{
}

void ValueParameter::PrintOn(std::ostream& strm) const
{
    strm << governor->GetTypeName() << " : " << name;
}

ActualParameterPtr ValueParameter::MakeActualParameter() const
{
    if (isupper(name[0]))
        return ActualParameterPtr(new ActualValueSetParameter(TypePtr(new DefinedType(name))));
    else
        return ActualParameterPtr(new ActualValueParameter(ValuePtr(new DefinedValue(name))));
}

///////////////////////////////////////////////////

ObjectParameter::ObjectParameter(DefinedObjectClassPtr gover,
                 const std::string& nam)
: Parameter(nam, isupper(nam[0]) ? OBJECTSETREFERENCE : OBJECTREFERENCE)
, governor(gover)
{
}

ObjectParameter::~ObjectParameter()
{
}

void ObjectParameter::PrintOn(std::ostream& strm) const
{
    strm << governor->GetName() << " : " << name;
}


bool ObjectParameter::ReferencedBy(const TypeBase&  type) const
{
  DefinedType t(name);
  return type.ReferencesType(t);
}

ActualParameterPtr ObjectParameter::MakeActualParameter() const
{
  if (isupper(name[0]))
     return ActualParameterPtr(new ActualObjectSetParameter(
                 boost::shared_ptr<ObjectSetConstraintElement>(new DefinedObjectSet(name))));
  else
     return ActualParameterPtr(new ActualObjectParameter(InformationObjectPtr(new DefinedObject(name))));
}

//////////////////////////////////////////////////////////

void ParameterList::Append(ParameterPtr param)
{
    rep.push_back(param);
}

int ParameterList::GetIdentifierType(const char* identifier)
{
    Parameter* p = GetParameter(identifier);
    return (p != NULL) ? p->GetIdentifierType() : -1;
}

Parameter* ParameterList::GetParameter(const char* identifier)
{
    for (size_t i = 0; i < rep.size() ; ++i)
    {
        if (rep[i]->GetName() == identifier)
            return rep[i].get();
    }
    return NULL;
}

void ParameterList::PrintOn(std::ostream& strm) const
{
    if (!rep.empty())
    {
        strm << " {";
        for (size_t i = 0; i < rep.size(); ++i)
        {
            strm << *rep[i];
            if (i != rep.size() - 1)
                strm << ", ";
        }
        strm << " } ";
    }
}


void ParameterList::GenerateCplusplus(std::string& templatePrefix, std::string& classNameString)
{
    if (rep.size())
    {
        templatePrefix = "template <";
        classNameString += '<';
        bool outputDelimeter = false;
        for (size_t i = 0; i < rep.size(); i++) {
            if (outputDelimeter) {
                templatePrefix += ", ";
                classNameString += ", ";
            }

            std::string ident = MakeIdentifierC(rep[i]->GetName());
            templatePrefix += "class " + ident;
            classNameString += ident;
            outputDelimeter = true;
        }
        templatePrefix += ">\n";
        classNameString += '>';
    }
}

boost::shared_ptr<ParameterList> ParameterList::GetReferencedParameters(const TypeBase& type) const
{
  boost::shared_ptr<ParameterList> result(new ParameterList);
  for (size_t i = 0; i < rep.size(); ++i)
  {
    if (rep[i]->ReferencedBy(type))
      result->rep.push_back(rep[i]);
  }
  if (result->rep.empty())
    result.reset();
  return result;
}

ActualParameterListPtr ParameterList::MakeActualParameters() const
{
  ActualParameterListPtr lst(new ActualParameterList);
  for (size_t i = 0; i < rep.size(); ++i)
  {
        lst->push_back(rep[i]->MakeActualParameter());
    }
    return lst;
}


/////////////////////////////////////////////////////////////////////

ActualTypeParameter::ActualTypeParameter(TypePtr type)
: param (type)
{
}

ActualTypeParameter::~ActualTypeParameter()
{
}

bool ActualTypeParameter::ReferencesType(const TypeBase & type) const
{
    return param->ReferencesType(type);
}

bool ActualTypeParameter::UseType(const TypeBase & type) const
{
    return param->UseType(type);
}

bool ActualTypeParameter::GenerateTemplateArgument(std::string& name) const
{
    name += param->GetTypeName();
    return true;
}

void ActualTypeParameter::PrintOn(std::ostream & strm) const
{
    strm << param->GetTypeName();
}

///////////////////////////////////////////////////////////

ActualValueParameter::ActualValueParameter(ValuePtr value)
: param(value)
{
}

ActualValueParameter::~ActualValueParameter()
{
}

void ActualValueParameter::PrintOn(std::ostream & strm) const
{
    strm << param->GetName();
}

/////////////////////////////////////////////////////////////

ActualValueSetParameter::ActualValueSetParameter(TypePtr valueSet)
: param(valueSet)
{
}

ActualValueSetParameter::~ActualValueSetParameter()
{
}

bool ActualValueSetParameter::ReferencesType(const TypeBase & type) const
{
    return param->ReferencesType(type);
}

bool ActualValueSetParameter::UseType(const TypeBase & type) const
{
    return param->UseType(type);
}

bool ActualValueSetParameter::GenerateTemplateArgument(std::string& name) const
{
    name += param->GetTypeName();
    return true;
}


void ActualValueSetParameter::PrintOn(std::ostream & strm) const
{
    strm << '{' << param->GetTypeName() << '}';
}

/////////////////////////////////////////////////////////////

ActualObjectParameter::ActualObjectParameter(InformationObjectPtr obj)
: param(obj)
{
}

ActualObjectParameter::~ActualObjectParameter()
{
}

void ActualObjectParameter::PrintOn(std::ostream & strm) const
{
    strm << param->GetName();
}

bool ActualObjectParameter::GenerateTemplateArgument(std::string& name) const
{
    name += param->GetName();
    return true;
}

bool ActualObjectParameter::UseType(const TypeBase & type) const
{
  return type.GetTypeName() == param->GetName();
}

bool ActualObjectParameter::ReferencesType(const TypeBase & type) const
{
  return type.GetTypeName() == param->GetName();
}

/////////////////////////////////////////////////////////////////////

ActualObjectSetParameter::ActualObjectSetParameter(boost::shared_ptr<ObjectSetConstraintElement> objectSet)
: param(objectSet)
{
}

ActualObjectSetParameter::~ActualObjectSetParameter()
{
}

void ActualObjectSetParameter::PrintOn(std::ostream & strm) const
{
    strm << '{' << param->GetName() << '}' ;
}

bool ActualObjectSetParameter::GenerateTemplateArgument(std::string& name) const
{
  name += param->GetName();
  str_replace(name,"{","<");
  str_replace(name,"}","> ");
  return true;
}

bool ActualObjectSetParameter::UseType(const TypeBase & type) const
{
  return type.GetTypeName() == param->GetName();
}

bool ActualObjectSetParameter::ReferencesType(const TypeBase & type) const
{
  return type.GetTypeName() == param->GetName();
}
//////////////////////////////////////////////////////////////

ObjectSetType::ObjectSetType(InformationObjectSetPtr os)
: TypeBase(0, Module)
, objSet(os)
{
  name = objSet->GetName();
  identifier = MakeIdentifierC(name);
}

const char * ObjectSetType::GetAncestorClass() const
{
  return identifier.c_str();
}

void ObjectSetType::GenerateCplusplus(std::ostream & hdr, std::ostream & cxx, std::ostream & inl)
{
  objSet->GenerateType(hdr, cxx, inl);
}

bool ObjectSetType::HasParameters() const
{
  return objSet->HasParameters();
}


UsefulModuleDef::UsefulModuleDef() : ModuleDefinition("ASN1", Tag::Explicit)
{
  // add the definition of ABSTRACT_SYNTAX
  boost::shared_ptr<ObjectClassDefn> type_Identifier(new ObjectClassDefn);

  type_Identifier->SetName("ABSTRACT-SYNTAX");

  {
    std::auto_ptr<FieldSpecsList> fieldSpecs(new FieldSpecsList);

    FieldSpecPtr typeField(new TypeFieldSpec("&Type"));

    fieldSpecs->push_back(typeField);

    boost::shared_ptr<FixedTypeValueFieldSpec> idField(
             new FixedTypeValueFieldSpec("&id"
                         , TypePtr(new ObjectIdentifierType), false, true));

    fieldSpecs->push_back(idField);

    type_Identifier->SetFieldSpecs(fieldSpecs);
  }

  TokenGroupPtr tokens(new TokenGroup);
  tokens->AddToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
  tokens->AddToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIED")));
  tokens->AddToken(TokenOrGroupSpecPtr(new Literal("BY")));
  tokens->AddToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));
  tokens->SetOptional();

  type_Identifier->SetWithSyntaxSpec(tokens);
  type_Identifier->ResolveKey();

  AddObjectClass(type_Identifier);
}
