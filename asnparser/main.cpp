/*
 * main.cxx
 *
 * PWLib application source file for context
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

#include <string>
#include <stdexcept>
#include "main.h"
#include <boost/mem_fn.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/functional.hpp>
#include <typeinfo>
#include <algorithm>
#include <vector>
#include <numeric>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <iterator>
#include <limits.h>

#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

#define IDDEBUG 1
#define YYDEBUG 1
#include "asn_grammar.h"
#include "asn_lex.h"
#include "asn_ref_lex.h"



/* Debug traces.  */
#ifndef IDDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define IDDEBUG 1
#  else
#   define IDDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define IDDEBUG 0
# endif /* ! defined YYDEBUG */
#else
int iddebug;
#endif  /* ! defined IDDEBUG */


using namespace std;
using boost::shared_ptr;

unsigned lineNumber;
string  fileName;

unsigned fatals, warnings;

extern bool dumpLowerCaseToken;
extern bool dumpUpperCaseToken;

int LexEcho = 0;
extern int isUpper(const char* text);
//
//  yyerror required function for flex
//
#ifdef REENTRANT_PARSER
ModuleList				Modules;
UsefulModuleDef *		UsefulModule = nullptr;

static	stack<ParserContext*>	contexts;
static 	vector<string>			files;
static 	vector<FILE*>			fds;
static const char nl = '\n';
static const char TAB = '\t';

ParserContext::ParserContext(FILE* file) : file(file) {
	IdentifierTokenContext = IDENTIFIER;
	NullTokenContext= NULL_TYPE;
	classStack = new ClassStack;
	ReferenceTokenContext = MODULEREFERENCE;
}
ParserContext::~ParserContext() {
	if (file) fclose(file);
	delete classStack;
}
int idparse (ParserContext* context, const string& path);
int  iderror(ParserContext *, const string& path, char const *str) {
//  extern char * yytext;
	clog << "First  stage " << StdError(Fatal) << str << " near token \"" << idtext <<"\"" << nl;
	return 0;
}

int  yyerror(YYLTYPE* location, yyscan_t scanner, ParserContext * context, char const *str) {
#if 0
	extern char * yytext;
	clog << "Second stage " << StdError(Fatal) << str << " near token \"" << yytext <<"\"" << nl;
#else
	clog << "Second stage " << StdError(Fatal) << str << " at " << location->first_line << ":" << location->first_column << endl;
	context->Module = nullptr;
#endif
	return 0;
}

#else
int idparse ();
void yyerror(const char * str) {
	extern char * yytext;
	cerr << "Second stage " << StdError(Fatal) << str << " near token \"" << yytext <<"\"" << nl;
}
void iderror(const char * str) {
	extern char * idtext;
	cerr << "First  stage " << StdError(Fatal) << str << " near token \"" << idtext <<"\"" << nl;
}
#endif
static const char* tokenAsString(int token);
static const char* ArcNames[] = {
	"ITU-T",
	"ISO",
	"joint-iso-itu-t"
};
static const char * const StandardClasses[] = {
	"ASN1::Null",
	"ASN1::BOOLEAN",
	"ASN1::REAL",
	"ASN1::INTEGER",
	"ASN1::ENUMERATED",
	"ASN1::BinaryReal",
	"ASN1::RELATIVE_OID",
	"ASN1::OBJECT_IDENGIFIER",
	"ASN1::BIT_STRING",
	"ASN1::OCTET_STRING",
	"ASN1::NumericString",
	"ASN1::T61String",
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
	"asm", "auto", "bitand", "bool", "break", "case", "catch", "char",
	"class", "const", "const_cast",
	"continue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
	"export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int",
	"long", "mutable", "namespace", "new", "operator",
	"private", "protected", "public", "register",
	"reinterpret_cast", "return", "short", "signed", "sizeof", "static", "static_cast", "struct",
	"switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename",
	"union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while"
};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


void addRemoveItem(const char* item);
static string toLower(const string& str);
static string toUpper(const string& str);
static string getFileName(const string& fullname);
static string getFileDirectory(const string& fullname);
static string getFileTitle(const string& fullname);
static string shortenName(const string& name);
static string makeCppName(const string& identifier);
static void str_replace(string& str, const char* src, const char* target, string::size_type pos = 0);
static string getPath(const char* name = nullptr);
static vector<string> readDirectory(const string& directory, const string& extension);

class OutputFile;
static void insertForwardDeclaration(const OutputFile& hdr, streampos insertionpoint, stringstream& fwd);

/////////////////////////////////////////////////////////
//
//  Application
//

static size_t fileCount = 0;
static int verbose=0;
static string asndir;
static string dllMacro;
static string dllMacroDEFINED;
static string dllMacroEXPORTS;
static string dllMacroDLL;
static string dllMacroAPI;
static string dllMacroSTATIC;
static string dllMacroLIB_SUFFIX;
static string dllMacroRTS;
static string dllMacroNO_AUTOMATIC_LIBS;
static bool includeConfigH = true;
static string useReinterpretCast;

static unsigned classesPerFile = 0;

static const char * const incExt = ".h";
static const char * cppExt = ".cxx";
static bool noInlineFiles = false;

class UsefulModuleDef : public ModuleDefinition {
  public:
	UsefulModuleDef();
	virtual void generateCplusplus(const string& modName, unsigned numFiles, bool verbose) {}
};

int main(int argc, char** argv) {
	ios_base::sync_with_stdio(false);

	const char* opt = "i:cdeno:s:vm:Cr:";

	int c;
	bool generateCpp = true;
	string path;

	iddebug = 0;
	yydebug = 0;

	while ((c=getopt(argc, argv, opt)) != -1) {
		switch (c) {
		case 'c':
			generateCpp = true;
			break;

		case 'i':
			asndir  = optarg;
			break;

		case 'd':
			yydebug = 1;
//			iddebug = 1;
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
			dllMacroRTS				= optarg;
			dllMacro				= makeCppName(optarg);
			dllMacroDEFINED			= dllMacro + "_DEFINED";
			dllMacroEXPORTS			= dllMacro + "_EXPORTS";
			dllMacroDLL				= dllMacro + "_DLL";
			dllMacroAPI				= dllMacro + "_API";
			dllMacroSTATIC			= dllMacro + "_STATIC";
			dllMacroLIB_SUFFIX		= dllMacro + "_LIB_SUFFIX";
			dllMacroNO_AUTOMATIC_LIBS = dllMacro + "_NO_AUTOMATIC_LIBS";
			break;

		case 'C':
			includeConfigH = false;
			break;

		case 'r':
			useReinterpretCast = optarg;
			break;
		}
	}

	fileCount = argc - optind;

	if (asndir.empty() && fileCount < 1 ) {
		cerr << "usage: ASN1cmp [options] asnfile..." << nl
			 << "  -v          Verbose output (multiple times for more verbose)" << nl
			 << "  -i          Local directory of the asn files" << nl
			 << "  -d          Debug output (copious!)" << nl
			 << "  -c          generate C++ files" << nl
			 << "  -e          generated C++ files with .cpp extension" << nl
			 << "  -n          Use inline definitions rather than .inl files" << nl
			 << "  -s  n       Split output if it has more than n (default 1000) classes" << nl
			 << "  -o  dir     Output directory" << nl
			 << "  -m  name    Macro name for generating DLLs under windows with MergeSym" << nl
			 << "  -C          If given, the generated .cxx files won't include config.h" << nl
			 << "  -r  name    Use reinterpret_casts rather than static_casts in the" << nl
			 << "              generated .inl files, if -Dname is given to the compiler" << nl
			 << endl;
		return 1;
	}
	if (!asndir.empty() && fileCount == 0) {
		vector<string> asns  = readDirectory(getPath(), "asn");
		vector<string> asn1s = readDirectory(getPath(), "asn1");
		copy( asns.begin(), asns.end(), back_inserter(files));
		copy( asn1s.begin(), asn1s.end(), back_inserter(files));
		fileCount = files.size();
	} else {
		for (int no = optind; no < argc; ++no) {
			const char* cstring = argv[no];
			const char* dot = strrchr(cstring, '.');
			if (!dot) {
				cerr << "ASN1 compiler: file has not suffix " << fileName << endl;
				return 1;
			}
			const char* suffix = cstring + (dot - cstring) + 1;
			if (strcmp(suffix, "asn") && strcmp(suffix, "asn1")) {
				cerr << "ASN1 compiler: invalid suffix" << suffix << endl;
				return 1;
			}
			files.push_back(argv[no]);
		}
		fileCount = argc - optind;
	}
	clog << "ASN.1 compiler version 2.4" << nl << endl;


	fds.resize(fileCount);
	ParserContext context;
	contexts.push(&context);
	UsefulModule = new UsefulModuleDef();

	Modules.push_back(ModuleDefinitionPtr(UsefulModule));

	for (int no = 0 ; no < fileCount; ++no)  {
		fileName   = files[no];
		string filePath = getPath(fileName.c_str());

		idin = fds[no] = fopen(filePath.c_str(),"r");
		if (!idin) {
			cerr << "ASN1 compiler: cannot open " << filePath << endl;
			return 1;
		}
		clog << "ASN1 compiler: processing of " << filePath << endl;

		lineNumber = 1;
		fatals     = 0;
		warnings   = 0;

		if (verbose)
			clog << "First  Stage Parsing... " << fileName << endl;

		idparse(&context, filePath);
		rewind(idin); // rewind the file
	}
	for(int no = 0; no < Modules.size(); no++) {
		Modules[no]->dump();
	}


	for (int no = 0; no < fileCount; ++no)  {
		fileName   = files[no];
		lineNumber = 1;
		fatals     = 0;
		warnings   = 0;
		if (verbose)
			clog << "Second Stage Parsing... " << fileName << endl;

		contexts.top()->file = fds[no];
		yylex_init(&context.lexer);
		yyset_in(fds[no], context.lexer);
//		yyrestart(fds[no], context.lexer);
		yyparse(context.lexer, contexts.top());
		yylex_destroy(context.lexer);
		fclose(fds[no]);
		contexts.top()->file = nullptr;
	}

	for (int no= 0 ; no < Modules.size(); ++no) {
		Modules[no]->AdjustModuleName(path);
	}

	for (int no = 0; no < contexts.top()->RemoveList.size(); ++no) {
		int dotpos = contexts.top()->RemoveList[no].find('.');
		string modulename = contexts.top()->RemoveList[no].substr(0, dotpos);
		ModuleDefinition* module = findModule(modulename.c_str());
		if (module)
			module->addToRemoveList(contexts.top()->RemoveList[no].substr(dotpos+1, contexts.top()->RemoveList[no].size()-1));
	}

	for (int no = 0 ; no < Modules.size(); ++no) {
		Modules[no]->RemoveReferences(verbose !=0 );
		Modules[no]->AdjustImportedModules();
	}

	for (int no = 0; no < Modules.size(); ++no) {
		contexts.top()->Module = Modules[no].get();
		if (verbose > 1)
			cerr << "Module " << *contexts.top()->Module << endl;

		if (generateCpp)
			contexts.top()->Module->generateCplusplus(path,  classesPerFile, verbose!=0);
	}

	contexts.pop();
	return 0;
}
UsefulModuleDef::UsefulModuleDef() : ModuleDefinition("ASN1", "", Tag::Explicit) {
	/*
	ABSTRACT-SYNTAX ::= CLASS {
	  &id		OBJECT IDENTIFIER,
	  &Type		,
	  &property BIT STRING { handles-invalid-encodingd(0) } DEFAULT {]
	} WITH SYNTAX {
	  &Type IDENTIFIED BY &id [HASH PROPERTY &property]
	}
	*/
	{
		// add the definition of ABSTRACT_SYNTAX
		boost::shared_ptr<ObjectClassDefn> type_Identifier(new ObjectClassDefn);
		type_Identifier->setName("ABSTRACT-SYNTAX");

		auto_ptr<FieldSpecsList> fieldSpecs(new FieldSpecsList);
		FieldSpecPtr typeField(new TypeFieldSpec("&Type"));
		fieldSpecs->push_back(typeField);

		FixedTypeValueFieldSpec* ftvfs = new FixedTypeValueFieldSpec("&id", TypePtr(new ObjectIdentifierType), false, true);
		boost::shared_ptr<FixedTypeValueFieldSpec> idField(ftvfs);

		fieldSpecs->push_back(idField);
		type_Identifier->setFieldSpecs(fieldSpecs);

		TokenGroupPtr tokens(new TokenGroup);
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIED")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("BY")));
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));

		type_Identifier->setWithSyntaxSpec(tokens);

		addObjectClass(type_Identifier);
	}

	/*
	TYPE-IDENTIFIER ::= CLASS {
	  &id	OBJECT IDENTIFIER UNIQUE,
	  &Type
	} WITH SYNTAX {
	  &Type IDENTIFIED BY &id
	}
	*/
#if 1
	{
		// add the definition of TYPE-IDENTIFIER
		// add the definition of TYPE-IDENTIFIER
		ObjectClassDefnPtr type_Identifier(new ObjectClassDefn);
		type_Identifier->setName("TYPE-IDENTIFIER");
		auto_ptr<FieldSpecsList> fieldSpecs(new FieldSpecsList);

		FixedTypeValueFieldSpecPtr idField(new FixedTypeValueFieldSpec("&id", TypePtr(new ObjectIdentifierType), false, true));

		fieldSpecs->push_back(idField);

		FieldSpecPtr typeField(new TypeFieldSpec("&Type"));

		fieldSpecs->push_back(typeField);
		type_Identifier->setFieldSpecs(fieldSpecs);

		TokenGroupPtr tokens(new TokenGroup);
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIED")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("BY")));
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));

		type_Identifier->setWithSyntaxSpec(tokens);
		addObjectClass(type_Identifier);
	}

#else
	// add the definition of TYPE-IDENTIFIER
	ObjectClassDefnPtr type_Identifier(new ObjectClassDefn);
	type_Identifier->setName("TYPE-IDENTIFIER");
	auto_ptr<FieldSpecsList> fieldSpecs(new FieldSpecsList);

	FixedTypeValueFieldSpecPtr idField(new FixedTypeValueFieldSpec("&id", TypePtr(new ObjectIdentifierType), false, true));

	fieldSpecs->push_back(idField);

	FieldSpecPtr typeField(new TypeFieldSpec("&Type"));

	fieldSpecs->push_back(typeField);
	type_Identifier->setFieldSpecs(fieldSpecs);

	TokenGroupPtr tokens(new TokenGroup);
	tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
	tokens->addToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIED")));
	tokens->addToken(TokenOrGroupSpecPtr(new Literal("BY")));
	tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));

	type_Identifier->setWithSyntaxSpec(tokens);
	addObjectClass(type_Identifier);
	return type_Identifier;



	{
		// add the definition of TYPE-IDENTIFIER
		boost::shared_ptr<ObjectClassDefn> type_Identifier(new ObjectClassDefn);
		type_Identifier->setName("TYPE-IDENTIFIER");
		TokenGroupPtr tokens(new TokenGroup);
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIED")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("BY")));
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));
		tokens->setOptional();

		type_Identifier->setWithSyntaxSpec(tokens);
		type_Identifier->ResolveKey();
		addObjectClass(type_Identifier);
	}
#endif
}




/////////////////////////////////////////
//
//  miscellaneous
//
template <class Cont, class Fun>
void for_all(Cont& cont, Fun fun) {
	for_each(cont.begin(), cont.end(), fun);
}

class IndentFacet : public codecvt<char, char, mbstate_t> {
  public:
	explicit IndentFacet(int indent_level, size_t ref = 0)
		: codecvt<char, char, mbstate_t>(ref), count(indent_level) {}
	typedef codecvt_base::result result;
	typedef codecvt<char, char, mbstate_t> parent;
	typedef parent::intern_type internT;
	typedef parent::extern_type externT;
	typedef parent::state_type  stateT;

	int &state(stateT &s) const {
		return *reinterpret_cast<int *>(&s);
	}

  protected:
	virtual result do_out(stateT &need_indentation,
						  const internT *from, const internT *from_end, const internT *&from_next,
						  externT *to, externT *to_end, externT *&to_next
						 ) const override;

	// Override so the do_out() virtual function is called.
	virtual bool do_always_noconv() const throw() override {
		return count == 0;
	}
	int count = 0;

};

IndentFacet::result IndentFacet::do_out(stateT &need_indentation,
										const internT *from, const internT *from_end, const internT *&from_next,
										externT *to, externT *to_end, externT *&to_next) const {
	result res = codecvt_base::noconv;
	for (; (from < from_end) && (to < to_end); ++from, ++to) {
		// 0 indicates that the last character seen was a newline.
		// thus we will print a tab before it. Ignore it the next
		// character is also a newline
		if ((state(need_indentation) == 0) && (*from != '\n')) {
			res = codecvt_base::ok;
			state(need_indentation) = 1;
			for (int i = 0; i < count; ++i) {
				*to = '\t';
				++to;
			}
			if (to == to_end) {
				res = codecvt_base::partial;
				break;
			}
		}
		*to = *from; // Copy the next character.

		// If the character copied was a '\n' mark that state
		if (*from == '\n') {
			state(need_indentation) = 0;
		}
	}

	if (from != from_end) {
		res = codecvt_base::partial;
	}
	from_next = from;
	to_next = to;

	return res;
};

static const int index = ios_base::xalloc();
static int pushcount = 0;

ostream & push(ostream& os) {
	pushcount++;
	auto ilevel = ++os.iword(index);
	os.imbue(locale(os.getloc(), new IndentFacet(ilevel)));
	return os;
}

ostream& pop(ostream& os) {
	auto ilevel = (os.iword(index) > 0) ? --os.iword(index) : 0;
	os.imbue(locale(os.getloc(), new IndentFacet(ilevel)));
	return os;
}

/// Clears the ostream indentation set, but NOT the raii_guard.
ostream& clear(ostream& os) {
	os.iword(index) = 0;
	os.imbue(locale(os.getloc(), new IndentFacet(0)));
	return os;
}

#define tab push
#define bat pop

class OutputFile : public ofstream {
  public:
	OutputFile() {}
	OutputFile(const OutputFile&) = delete;
	OutputFile& operator = (const OutputFile&) = delete;
	~OutputFile() { close(); }

	bool open(const string& path, const char* suffix, const char * extension, openmode mode  = ios_base::out);
	void close();
	const string& getName() const { return filename; }
	const string& getPath() const { return path; }
	const string& getExtension() const { return extension; }

  private:
	  string path;
	  string extension;
	  string filename;
};


bool OutputFile::open(const string& path,  const char * suffix,  const char * extension, openmode mode) {
	this->path = path;
	this->extension = extension;
	filename = path + suffix + extension;
	ofstream::open(filename.c_str(), binary | mode);
	if (is_open()) {
		if (mode == out) {
			*this << "//" << nl;
			*this << "// " << getFileName(filename) << nl;
			*this << "//" << nl;
			*this << "// Code automatically generated by " << "ASN1 compiler" << nl;
			*this << "//" << nl;
		}
		return true;
	}

	cerr << "context : cannot create " << filename << endl;
	return false;
}


void OutputFile::close() {
	if (is_open()) {
//		ostream& strm = *this;
		*this << nl << "// end of " << getFileName(filename) << nl;
	}
	ofstream::close();
}

void str_replace(string& str, const char* src, const char* target, string::size_type pos) {
	const size_t l = strlen(src);

	while ( (pos = str.find(src,pos)) != string::npos)
		str.replace(pos, l, target);
}

struct str_less : binary_function<const char*, const char*, bool> {
	bool operator () (const char* lhs, const char* rhs) const {
		return strcmp(lhs, rhs)<0;
	}
};

template <class T>
ostream& operator << (ostream& os, const vector<boost::shared_ptr<T> >& cont) {
	typename vector<boost::shared_ptr<T> >::const_iterator it, last = cont.end();
	for (it = cont.begin(); it != last; ++it)
		os << **it;
	return os;
}


/////////////////////////////////////////
//
//  intermediate structures from parser
//
class CompareNamedNumber {
  public:
	CompareNamedNumber (int val) : val(val) {}
	bool operator()(NamedNumberPtr ptr) const {
		return ptr->getNumber()==val;
	}
  private:
	int val;
};



NamedNumber::NamedNumber(string * nam)
	: name(*nam) {
	delete nam;
	number = 0;
	autonumber = true;
}


NamedNumber::NamedNumber(string * nam, int num)
	: name(*nam) {
	delete nam;
	number = num;
	autonumber = false;
}


NamedNumber::NamedNumber(string * nam, const string& ref)
	: name(*nam), reference(ref) {
	delete nam;
	number = 0;
	autonumber = false;
}


ostream& operator << (ostream& strm, const NamedNumber& obj) {
	strm << obj.name << " (";
	if (obj.reference.size()==0)
		strm << obj.number;
	else
		strm << obj.reference;
	strm << ')';
	return strm;
}


void NamedNumber::setAutoNumber(const NamedNumber& prev) {
	if (autonumber) {
		number = prev.number + 1;
		autonumber = false;
	}
}


/////////////////////////////////////////////////////////

Tag::Tag(unsigned tagNum, Mode m) {
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


void Tag::printOn(ostream& strm) const {
	if (type != Universal || number != IllegalUniversalTag) {
		strm << '[';
		if (type != ContextSpecific)
			strm << classNames[type] << ' ';
		strm << number << "] " << modeNames[mode] << ' ';
	}
}


/////////////////////////////////////////////////////////

Constraint::Constraint(ConstraintElementPtr& elmt) {
	standard.push_back(elmt);
	extendable = false;
}


Constraint::Constraint(auto_ptr<ConstraintElementVector> a_std, bool extend, auto_ptr<ConstraintElementVector> ext) {
	if (a_std.get() != nullptr) {
		standard.swap(*a_std);
	}
	extendable = extend;
	if (ext.get() != nullptr) {
		extensions.swap(*ext);
	}
}

Constraint::Constraint(const Constraint& other)
	: standard(other.standard)
	, extendable(other.extendable)
	, extensions(other.extensions) {
}



void Constraint::printOn(ostream& strm) const {
	strm << '(';
	PrintElements(strm);
	strm << ')';
}

void PrintVector(ostream& strm ,const ConstraintElementVector& elements, char delimiter) {
	ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();

	if (i != e) {
		(*i)->printOn(strm);
		++i;
	}

	for (; i != e; ++i) {
		strm << ' ' << delimiter << ' ';
		(*i)->printOn(strm);
	}
}

void Constraint::PrintElements(ostream& strm) const {

	PrintVector(strm, standard, '|');
	if (extendable) {
		strm << tab;
		if (standard.size() > 0)
			strm << ", ";
		strm << "...";
		if (extensions.size() > 0)
			strm << ',';
		strm << ' ';

		PrintVector(strm, extensions, '|');
		strm << bat;
	}
}


void Constraint::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	switch (standard.size()) {
	case 0 :
		return;
	case 1 :
		break;
	default :
		cerr << StdError(Warning) << "unsupported UNION constraints, ignored." << endl;
	}

	if (extensions.size() > 0)
		cerr << StdError(Warning) << "unsupported extension constraints, ignored." << endl;

	string fn2 = fn;
	if (fn.find("ASN1::") == -1) {
		if (extendable)
			fn2 += "ASN1::ExtendableConstraint";
		else
			fn2 += "ASN1::FixedConstraint";
	}

	standard[0]->generateCplusplus(fn2, fwd, hdr, cxx, inl);
}

void Constraint::getConstraint(string& str) const {
	if (str.find("ConstrainedObject::, ") == -1 ) {
		if (extendable)
			str += "ASN1::ExtendableConstraint, ";
		else
			str += "ASN1::FixedConstraint, ";
	}
	standard[0]->getConstraint(str);
}



bool Constraint::referencesType(const TypeBase& type) const {
	ConstraintElementVector::size_type  i;

	for (i = 0; i < standard.size(); i++) {
		if (standard[i]->referencesType(type))
			return true;
	}

	for (i = 0; i < extensions.size(); i++) {
		if (extensions[i]->referencesType(type))
			return true;
	}

	return false;
}

ValueSetPtr Constraint::getValueSetFromValueField(const string& field) const {
	const ConstraintElementVector* vectors[] = {&standard,&extensions};
	ValueSetPtr result(new ValueSetDefn);

	for (int k = 0; k < 2; ++k) {
		ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
		for (; i != e ; ++i) {
			ValueSetPtr s = (*i)->getValueSetFromValueField(field);
			result->Union(s);
		}
	}

	return result;
}

ValueSetPtr Constraint::getValueSetFromValueSetField(const string& field) const {
	const ConstraintElementVector* vectors[] = {&standard,&extensions};
	ValueSetPtr result(new ValueSetDefn);

	for (int k = 0; k < 2; ++k) {
		ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
		for (; i != e ; ++i) {
			ValueSetPtr s = (*i)->getValueSetFromValueSetField(field);
			result->Union(s);
		}
	}
	return result;
}

ConstraintPtr Constraint::getObjectSetFromObjectField(const string& field) const {
	const ConstraintElementVector* vectors[] = {&standard,&extensions};
	ConstraintPtr result(new Constraint(extendable));

	for (int k = 0; k < 2; ++k) {
		ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
		for (; i != e; ++i) {
			ConstraintPtr j = (*i)->getObjectSetFromObjectField(field);
			result->standard.insert(result->standard.end(), j->standard.begin(), j->standard.end());
			result->standard.insert(result->standard.end(), j->extensions.begin(), j->extensions.end());
			result->extendable |= j->extendable;
		}
	}
	return result;
}

ConstraintPtr Constraint::getObjectSetFromObjectSetField(const string& field) const {
	const ConstraintElementVector* vectors[] = {&standard,&extensions};
	ConstraintPtr result(new Constraint(extendable));

	for (int k = 0; k < 2; ++k) {
		ConstraintElementVector::const_iterator i = vectors[k]->begin(), e = vectors[k]->end();
		for (; i != e; ++i) {
			ConstraintPtr j = (*i)->getObjectSetFromObjectSetField(field);
			result->standard.insert(result->standard.end(), j->standard.begin(), j->standard.end());
			result->standard.insert(result->standard.end(), j->extensions.begin(), j->extensions.end());
			result->extendable |= j->extendable;
		}
	}
	return result;
}


bool Constraint::hasPERInvisibleConstraint(const Parameter& param) const {
	ConstraintElementVector::const_iterator it = standard.begin(),
											last = standard.end();

	for (; it != last; ++it)
		if ((*it)->hasPERInvisibleConstraint(param))
			return true;

	for (it = extensions.begin(),last = extensions.end();
			it != last;
			++it)
		if ((*it)->hasPERInvisibleConstraint(param))
			return true;

	return false;
}

void Constraint::generateObjectSetInstanceCode(const string& prefix, ostream& cxx) const {
	ConstraintElementVector::const_iterator it = standard.begin(),
											last = standard.end();

	for (; it != last; ++it)
		(*it)->generateObjectSetInstanceCode(prefix, cxx);

	for (it = extensions.begin(), last = extensions.end();
			it != last;
			++it)
		(*it)->generateObjectSetInstanceCode(prefix, cxx);
}


void Constraint::generateObjSetAccessCode(ostream& cxx) {
	ConstraintElementVector::const_iterator it = standard.begin(),
											last = standard.end();

	for (; it != last; ++it)
		(*it)->generateObjSetAccessCode(cxx);

	for (it = extensions.begin(), last = extensions.end(); it != last; ++it)
		(*it)->generateObjSetAccessCode(cxx);
}

const SizeConstraintElement* Constraint::getSizeConstraint() const {
	const SizeConstraintElement* result= nullptr;
	ConstraintElementVector::const_iterator it = standard.begin(),	last = standard.end();

	for (; it != last; ++it)
		if ((result = (*it)->getSizeConstraint()) != nullptr)
			break;
	return result;
}

const FromConstraintElement* Constraint::getFromConstraint() const {
	const FromConstraintElement* result=nullptr;
	ConstraintElementVector::const_iterator it = standard.begin(),
											last = standard.end();

	for (; it != last; ++it)
		if ((result = (*it)->getFromConstraint()) != nullptr)
			break;
	return result;
}

void Constraint::getCharacterSet(string& characterSet) const {
	ConstraintElementVector::const_iterator it = standard.begin(),
											last = standard.end();

	for (; it != last; ++it)
		if ((*it)->getCharacterSet(characterSet))
			return;
}

const SubTypeConstraintElement* Constraint::getSubTypeConstraint() const {
	const SubTypeConstraintElement* result=nullptr;
	ConstraintElementVector::const_iterator it = standard.begin(),
											last = standard.end();

	for (; it != last; ++it)
		if ((result = (*it)->getSubTypeConstraint()) != nullptr)
			break;
	return result;
}

auto_ptr<Constraint> Constraint::Clone() const {
	return auto_ptr<Constraint>(new Constraint(*this));
}


/////////////////////////////////////////////////////////

ConstraintElementBase::ConstraintElementBase() {
}

ConstraintElementBase::~ConstraintElementBase() {
}

void ConstraintElementBase::generateCplusplus(const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cerr << StdError(Warning) << "unsupported constraint, ignored." << endl;
}


bool ConstraintElementBase::referencesType(const TypeBase&) const {
	return false;
}

ValueSetPtr ConstraintElementBase::getValueSetFromValueField(const string& ) const {
	cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
	return ValueSetPtr();
}

ValueSetPtr ConstraintElementBase::getValueSetFromValueSetField(const string& ) const {
	cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
	return ValueSetPtr();
}

ConstraintPtr ConstraintElementBase::getObjectSetFromObjectField(const string& ) const {
	cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
	return ConstraintPtr();
}

ConstraintPtr ConstraintElementBase::getObjectSetFromObjectSetField(const string& ) const {
	cerr << StdError(Fatal) << "Invalid ObjectSet." << endl;
	return ConstraintPtr();
}

const SizeConstraintElement* ConstraintElementBase::getSizeConstraint() const {
	return nullptr;
}

const FromConstraintElement* ConstraintElementBase::getFromConstraint() const {
	return nullptr;
}

bool ConstraintElementBase::getCharacterSet(string& ) const {
	return false;
}

const SubTypeConstraintElement* ConstraintElementBase::getSubTypeConstraint() const {
	return nullptr;
}


/////////////////////////////////////////////////////////

ConstrainAllConstraintElement::ConstrainAllConstraintElement(ConstraintElementPtr excl) {
	setExclusions(excl);
}


/////////////////////////////////////////////////////////
ElementListConstraintElement::ElementListConstraintElement() {
}

ElementListConstraintElement::ElementListConstraintElement(auto_ptr<ConstraintElementVector> list) {
	elements.swap(*list);
}

void ElementListConstraintElement::printOn(ostream& strm) const {
	PrintVector(strm, elements, '^');
}


void ElementListConstraintElement::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++)
		elements[i]->generateCplusplus(fn, fwd, hdr, cxx, inl);
}

void ElementListConstraintElement::getConstraint(string& str) const {
	for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++)
		elements[i]->getConstraint(str);
}

bool ElementListConstraintElement::referencesType(const TypeBase& type) const {
	for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++) {
		if (elements[i]->referencesType(type))
			return true;
	}
	return false;
}

ValueSetPtr ElementListConstraintElement::getValueSetFromValueField(const string& field) const {
	ValueSetPtr result = elements[0]->getValueSetFromValueField(field);

	for (ConstraintElementVector::size_type i = 1; i < elements.size(); i++) {
		ValueSetPtr t = elements[i]->getValueSetFromValueField(field);
		result->Intersect(t);
	}
	return result;
}

ValueSetPtr ElementListConstraintElement::getValueSetFromValueSetField(const string& field) const {
	ValueSetPtr result = elements[0]->getValueSetFromValueField(field);

	for (ConstraintElementVector::size_type i = 1; i < elements.size(); i++) {
		ValueSetPtr t = elements[i]->getValueSetFromValueSetField(field);
		result->Intersect(t);
	}
	return result;
}

void ElementListConstraintElement::AppendElements(
	ConstraintElementVector::const_iterator first,
	ConstraintElementVector::const_iterator last
) {
	elements.insert(elements.end(), first, last);
}

ConstraintPtr ElementListConstraintElement::getObjectSetFromObjectField(const string& field) const {
	boost::shared_ptr<ElementListConstraintElement>
	elem(new  ElementListConstraintElement);
	ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
	for (; i != e; ++i) {
		ConstraintPtr cons = (*i)->getObjectSetFromObjectField(field);
		if (cons.get() != nullptr) {
			elem->AppendElements(cons->getStandardElements().begin(),
								 cons->getStandardElements().end());
			elem->AppendElements(cons->getExtensionElements().end(),
								 cons->getExtensionElements().end());
		}
	}
	if (elem->elements.size()) {
		ConstraintElementPtr elm =
			boost::static_pointer_cast<ConstraintElementBase>(elem);
		return ConstraintPtr(new Constraint(elm));
	}

	return ConstraintPtr();
}

ConstraintPtr ElementListConstraintElement::getObjectSetFromObjectSetField(const string& field) const {
	boost::shared_ptr<ElementListConstraintElement>
	elem(new  ElementListConstraintElement);
	ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
	for (; i != e; ++i) {
		ConstraintPtr cons = (*i)->getObjectSetFromObjectSetField(field);
		if (cons.get() != nullptr) {
			elem->AppendElements(cons->getStandardElements().begin(), cons->getStandardElements().end());
			elem->AppendElements(cons->getExtensionElements().end(),  cons->getExtensionElements().end());
		}
	}
	if (elem->elements.size()) {
		ConstraintElementPtr elm =
			boost::static_pointer_cast<ConstraintElementBase>(elem);
		return ConstraintPtr(new Constraint(elm));
	}

	return ConstraintPtr();
}

bool ElementListConstraintElement::hasPERInvisibleConstraint(const Parameter& param) const {
	ConstraintElementVector::const_iterator it = elements.begin(), last = elements.end();

	for (; it != last; ++it)
		if ((*it)->hasPERInvisibleConstraint(param))
			return true;

	return false;
}

void ElementListConstraintElement::generateObjectSetInstanceCode(const string& prefix, ostream& cxx) const {
	ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
	for (; i != e; ++i)
		(*i)->generateObjectSetInstanceCode(prefix, cxx);
}

void ElementListConstraintElement::generateObjSetAccessCode(ostream& cxx) {
	ConstraintElementVector::const_iterator i = elements.begin(), e = elements.end();
	for (; i != e; ++i)
		(*i)->generateObjSetAccessCode(cxx);
}

const SizeConstraintElement* ElementListConstraintElement::getSizeConstraint() const {
	const SizeConstraintElement* result=nullptr;
	ConstraintElementVector::const_iterator it = elements.begin(), last = elements.end();

	for (; it != last; ++it)
		if ((result = (*it)->getSizeConstraint()) != nullptr)
			break;
	return result;
}

const FromConstraintElement* ElementListConstraintElement::getFromConstraint() const {
	const FromConstraintElement* result=nullptr;
	ConstraintElementVector::const_iterator it = elements.begin(), last = elements.end();

	for (; it != last; ++it)
		if ((result = (*it)->getFromConstraint()) != nullptr)
			break;
	return result;
}


/////////////////////////////////////////////////////////

SingleValueConstraintElement::SingleValueConstraintElement(const ValuePtr& val)
	: value(val) {
}

SingleValueConstraintElement::~SingleValueConstraintElement() {
}


void SingleValueConstraintElement::printOn(ostream& strm) const {
	strm << *value;
}


void SingleValueConstraintElement::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	if (dynamic_cast<const IntegerValue*>(value.get())) {
		cxx << fn << ", ";
		value->generateCplusplus(fwd, hdr, cxx, inl);
		cxx << ", ";
		value->generateCplusplus(fwd, hdr, cxx, inl);
		cxx << ");" << nl;
		return;
	}

	if (dynamic_cast<const CharacterStringValue*>(value.get())) {
		cxx << fn << ", ";
		value->generateCplusplus(fwd, hdr, cxx, inl);
		cxx << ");" << nl;
		return;
	}

	cerr << StdError(Warning) << "Unsupported constraint type, ignoring." << endl;
}

void SingleValueConstraintElement::getConstraint(string& str) const {
	stringstream strm;

	if (dynamic_cast<const IntegerValue*>(value.get())) {
		strm << *value << ", " << *value ;
		str += strm.str();
	} else  if (dynamic_cast<const CharacterStringValue*>(value.get())) {
		strm << *value;
		str += strm.str();
	}
}

bool SingleValueConstraintElement::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}

bool SingleValueConstraintElement::getCharacterSet(string& characterSet) const {
	const CharacterStringValue* val;
	if ((val = dynamic_cast<const CharacterStringValue*>(value.get())) != nullptr) {
		val->getValue(characterSet);
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////

ValueRangeConstraintElement::ValueRangeConstraintElement(ValuePtr lowerBound, ValuePtr upperBound) {
	lower = lowerBound;
	upper = upperBound;
}

ValueRangeConstraintElement::~ValueRangeConstraintElement() {
}


void ValueRangeConstraintElement::printOn(ostream& strm) const {
	strm << *lower << ".." << *upper;
}


void ValueRangeConstraintElement::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << fn << ", ";
	lower->generateCplusplus(fwd, hdr, cxx, inl);
	cxx << ", ";
	upper->generateCplusplus(fwd, hdr, cxx, inl);
	cxx << ");" << nl;
}



void ValueRangeConstraintElement::getConstraint(string& str) const {
	if (dynamic_cast<MinValue*>(lower.get())) {
		str_replace(str, "FixedConstraint", "Unconstrained");
	} else if (dynamic_cast<MaxValue*>(upper.get())) {
		str_replace(str,"FixedConstraint", "PartiallyConstrained");
	} else if (dynamic_cast<CharacterStringValue*>(lower.get())) {
		str_replace(str,"FromConstraint", "FromRangeConstraint");
	}

	stringstream strm;
	strm << *lower << ", " << *upper;
	str += strm.str();
}

bool ValueRangeConstraintElement::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}

bool ValueRangeConstraintElement::getCharacterSet(string& characterSet) const {
	const CharacterValue* l = dynamic_cast<const CharacterValue*>(lower.get());
	const CharacterValue* u = dynamic_cast<const CharacterValue*>(upper.get());
	if ( l && u) {
		for (char c = (char) l->getValue();	c <= (char) u->getValue();	++c) {
			characterSet += c;
		}
		return true;
	}

	const CharacterStringValue* lv = dynamic_cast<const CharacterStringValue*>(lower.get());
	const CharacterStringValue* uv = dynamic_cast<const CharacterStringValue*>(upper.get());
	if (lv && uv) {
		string l, u;
		lv->getValue(l);
		uv->getValue(u);
		if (l.length()==1&& u.length() == 1) {
			for (char c = l[0]; c <= u[0]; ++c)
				characterSet += c;
			return true;
		}

	}

	return false;
}


/////////////////////////////////////////////////////////

SubTypeConstraintElement::SubTypeConstraintElement(TypePtr typ)
	: subtype(typ) {
}

SubTypeConstraintElement::~SubTypeConstraintElement() {
}


void SubTypeConstraintElement::printOn(ostream& strm) const {
	strm << *subtype;
}


void SubTypeConstraintElement::generateCplusplus(const string& str, ostream& , ostream& cxx, ostream& ) const {
	cxx << "    " << subtype->getTypeName() << " typeConstraint;" << nl
		<< "    setConstraints(" ;
	if (str.find("ASN1::ExtendableConstraint") != -1)
		cxx << "ASN1::ExtendableConstraint,";
	else
		cxx << "ASN1::FixedConstraint,";
	cxx << " typeConstraint.getLowerLimit(), typeConstraint.getUpperLimit());" << nl;
}

void SubTypeConstraintElement::getConstraint(string& str) const {
	stringstream strm;
	strm << str << subtype->getTypeName() << "::LowerLimit, "
		 << subtype->getTypeName() << "::UpperLimit";
	str = strm.str();
}


bool SubTypeConstraintElement::referencesType(const TypeBase& type) const {
	return subtype->referencesType(type);
}

bool SubTypeConstraintElement::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}

const SubTypeConstraintElement* SubTypeConstraintElement::getSubTypeConstraint() const {
	return this;
}

string SubTypeConstraintElement::getSubTypeName() const {
	return subtype->getTypeName();
}


/////////////////////////////////////////////////////////

NestedConstraintConstraintElement::NestedConstraintConstraintElement(ConstraintPtr con) {
	constraint = con;
}

NestedConstraintConstraintElement::~NestedConstraintConstraintElement() {
}


bool NestedConstraintConstraintElement::referencesType(const TypeBase& type) const {
	if (constraint.get() == nullptr)
		return false;

	return constraint->referencesType(type);
}

bool NestedConstraintConstraintElement::hasPERInvisibleConstraint(const Parameter& param) const {
	return constraint->hasPERInvisibleConstraint(param);
}

///////////////////////////////////////////////////////

SizeConstraintElement::SizeConstraintElement(ConstraintPtr constraint)
	: NestedConstraintConstraintElement(constraint) {
}

void SizeConstraintElement::printOn(ostream& strm) const {
	strm << "SIZE" << *constraint;
}


void SizeConstraintElement::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	constraint->generateCplusplus(fn, fwd, hdr, cxx, inl);
}

void SizeConstraintElement::getConstraint(string& str) const {
	const char* cstr = "ASN1::FixedConstraint, ";
	const int len = strlen(cstr);
	string::iterator itr = find_end(str.begin(), str.end(), cstr, cstr+len);
	if (itr != str.end())
		str.replace(itr, itr+len, " ASN1::SizeConstraint<");
	constraint->getConstraint(str);
	str += "> ";
}

const SizeConstraintElement* SizeConstraintElement::getSizeConstraint() const {
	return this;
}


/////////////////////////////////////////////////////////

FromConstraintElement::FromConstraintElement(ConstraintPtr constraint)
	: NestedConstraintConstraintElement(constraint) {
}

void FromConstraintElement::printOn(ostream& strm) const {
	strm << "FROM" << *constraint;
}


void FromConstraintElement::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	string newfn = fn;
	str_replace(newfn,"setConstraints(", "setCharacterSet(");
	constraint->generateCplusplus(newfn, fwd, hdr, cxx, inl);
}

void FromConstraintElement::getConstraint(string& ) const {
}

const FromConstraintElement* FromConstraintElement::getFromConstraint() const {
	return this;
}

string FromConstraintElement::getCharacterSet(const char* canonicalSet, int canonicalSetSize) const {
	string characterSet;
	if (!constraint->isExtendable()) {
		string constrainedSet;
		constraint->getCharacterSet(constrainedSet);

		int setSize = constrainedSet.size();
		const char* c =&constrainedSet[0];
		for (int i = 0; i < canonicalSetSize; i++) {
			if (memchr(c, canonicalSet[i], setSize) != nullptr)
				characterSet += canonicalSet[i];
		}
	}
	return characterSet;
}

int FromConstraintElement::getRange(ostream& cxx) const {
	stringstream fwd, inl, hdr, tmpcxx;
	string str;
	constraint->generateCplusplus(str,fwd, hdr, tmpcxx, inl);

	int min, max;
	char c;
	tmpcxx >> str >> min >> c >> max;
	cxx << min << ", " << max;
	return max-min;
}

/////////////////////////////////////////////////////////

WithComponentConstraintElement::WithComponentConstraintElement(string newName,
		ConstraintPtr constraint,
		int pres)
	: NestedConstraintConstraintElement(constraint) {
	name = newName;
	presence = pres;
}

void WithComponentConstraintElement::printOn(ostream& strm) const {
	if (name.empty())
		strm << "WITH COMPONENT";
	else
		strm << name;

	if (constraint.get() != nullptr)
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


void WithComponentConstraintElement::generateCplusplus(const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
}

/////////////////////////////////////////////////////////

InnerTypeConstraintElement::InnerTypeConstraintElement(auto_ptr<ConstraintElementVector> list,
		bool part)
	: ElementListConstraintElement(list) {
	partial = part;
}


void InnerTypeConstraintElement::printOn(ostream& strm) const {
	strm << "WITH COMPONENTS { ";

	if (partial)
		strm << "..., ";

	for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++) {
		if (i > 0)
			strm << ", ";
		elements[i]->printOn(strm);
	}

	strm << " }";
}


void InnerTypeConstraintElement::generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	for (ConstraintElementVector::size_type i = 0; i < elements.size(); i++)
		elements[i]->generateCplusplus(fn, fwd, hdr, cxx, inl);
}

/////////////////////////////////////////////////////////

UserDefinedConstraintElement::UserDefinedConstraintElement(ActualParameterListPtr list)
	: parameters(list) {
}

void UserDefinedConstraintElement::printOn(ostream& strm) const {
	strm << "CONSTRAINED BY { ";
	for (size_t i = 0; i < parameters->size(); i++) {
		if (i > 0)
			strm << ", ";
		strm << * (*parameters)[i];
	}
	strm << " }";
}


void UserDefinedConstraintElement::generateCplusplus(const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
}

////////////////////////////////////////////////////////////

TableConstraint::TableConstraint(boost::shared_ptr<DefinedObjectSet> os,
								 auto_ptr<StringList> as)
	: objSet(os), atNotations(as) {
}

TableConstraint::~TableConstraint() {
}

string TableConstraint::getObjectSetIdentifier() const {
	return makeCppName(objSet->getName());
}


bool TableConstraint::ReferenceType(const TypeBase& type) {
	return type.getTypeName() == getObjectSetIdentifier();
}

/////////////////////////////////////////////////////////

// if tag type assigned from module - breaks BER encoding
TypeBase::TypeBase(unsigned tagNum, ModuleDefinition* md)
	: tag(tagNum, Tag::Implicit), defaultTag(tagNum, Tag::Implicit) { // tag(tagNum, md->getDefaultTagMode()), defaultTag(tagNum, md->getDefaultTagMode())
	isoptional = false;
	isgenerated = false;
	isvaluesettype = false;
	module = md;
}


TypeBase::TypeBase(TypeBase& copy)
	: name(copy.name),
	  identifier(makeCppName(copy.name)),
	  tag(copy.tag),
	  defaultTag(copy.tag) {
	isoptional = copy.isoptional;
	isgenerated = false;
	isvaluesettype = false;
	module = contexts.top()->Module;
}


void TypeBase::printOn(ostream& strm) const {
	PrintStart(strm);
	PrintFinish(strm);
}


void TypeBase::PrintStart(ostream& strm) const {
	strm << tab;
	if (name.size()) {
		strm << name << parameters << ": ";
	}
	strm << tag << getClass() << ' ';
	strm << bat;
}


void TypeBase::PrintFinish(ostream& os) const {
	contexts.top()->Module->setIndentLevel(-1);
	os << " " << constraints;
	if (isoptional)
		os << " OPTIONAL";
	if (defaultValue.get() != nullptr)
		os << " DEFAULT " << *defaultValue;
	os << nl;
}


void TypeBase::setName(const string& newName) {
	name = newName;
	identifier = makeCppName(name);
}

void TypeBase::setDefaultValue(ValuePtr value) {
	defaultValue = value;
}


void TypeBase::adjustIdentifier(bool) {
	identifier = contexts.top()->Module->getPrefix() + makeCppName(name);
}


void TypeBase::setTag(Tag::Type type, unsigned num, Tag::Mode mode) {
	tag.type = type;
	tag.number = num;
	tag.mode = mode;
}


void TypeBase::setParameters(ParameterList& list) {
	parameters.swap(list);
}


void TypeBase::moveConstraints(TypeBase& from) {
	constraints.insert(constraints.end(),  from.constraints.begin(),  from.constraints.end());
	from.constraints.resize(0);
}

void TypeBase::copyConstraints(const TypeBase& from) {
	for (size_t i = 0; i < from.constraints.size(); ++i) {
		auto_ptr<Constraint> cons = from.constraints[i]->Clone();
		constraints.push_back(ConstraintPtr(cons));
	}
}

void TypeBase::flattenUsedTypes() {
}


TypePtr TypeBase::flattenThisType(TypePtr& self, const TypeBase&) {
	return self;
}


void TypeBase::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	if (isPrimitiveType() && !needGenInfo() ) {
		static int count = 0;
		count++;
		if (hdr.iword(index) > 0) {
			hdr << "//7" << nl;
			hdr << "// " << getName() << nl;
			hdr << "//" << nl;
			hdr << "typedef " << getTypeName() << ' ' << getIdentifier() << ";" << nl << nl;
		} else {
			fwd << "//7" << nl;
			fwd << "// " << getName() << nl;
			fwd << "//" << nl;
			fwd << "typedef " << getTypeName() << ' ' << getIdentifier() << ";" << nl << nl;
		}
	} else {
		beginGenerateCplusplus(fwd, hdr, cxx, inl);
		hdr << getIdentifier() << "(const " << getIdentifier() << "& that) : Inherited(that) {}" << nl;
		endGenerateCplusplus(fwd, hdr, cxx, inl);
	}
}


void TypeBase::generateForwardDecls(ostream& fwd, ostream& hdr) {
	if (!(isPrimitiveType() && !needGenInfo())) {
		if (hdr.iword(index)  <= 0) {
			fwd << "//9" << nl;
			fwd << "// " << getName() << nl;
			fwd << "//" << nl;
			fwd << "class " << getIdentifier() << ";" << nl << nl;
		}
	}
}

void TypeBase::generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType) {
//	hdr  << actualType.getIdentifier() << "& operator=(const Inherited& that) { Inherited::operator=(that); return *this; }\n" << nl;
}


string TypeBase::getTypeName() const {
	return getAncestorClass();
}


bool TypeBase::canReferenceType() const {
	return false;
}


bool TypeBase::referencesType(const TypeBase&) const {
	return false;
}


bool TypeBase::isParameterisedImport() const {
	return false;
}


void TypeBase::beginGenerateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	shortClassNameString = getIdentifier();

	parameters.generateCplusplus(templatePrefix, shortClassNameString);

	if (outerClassName.size())
		classNameString = outerClassName + "::" + shortClassNameString;
	else
		classNameString = shortClassNameString;
	if (hdr.iword(index) <= 0) {
		fwd << "//1" << nl
			<< "// " << getName() << nl
			<< "//" << nl;
		fwd << "class " << getIdentifier() << ';' << nl;
	}
	hdr << "//1" << nl
		<< "// " << getName() << nl
		<< "//" << nl;

	cxx << "//1" << nl
		<< "// " << getName() << nl
		<< "//" << nl;

	generateForwardDecls(fwd, hdr);

	if (outerClassName.size() == 0) {
		hdr << templatePrefix;
	}
	hdr << "class ";
	if (templatePrefix.empty())
		hdr << dllMacroAPI << " ";

	hdr << getIdentifier() << " : public " << getTypeName() << " {" << nl;
	hdr << tab;
	hdr << "typedef " << getTypeName() << " Inherited;" << nl;

	hdr << bat << "protected:" << nl << tab;
	hdr	<< "typedef Inherited::InfoType InfoType;" << nl
		<< getIdentifier() << "(const void* info) : Inherited(info) {}" << nl;
	hdr << bat << "public:" << nl << tab;

	generateConstructors(fwd, hdr, cxx, inl);
}


void TypeBase::endGenerateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	generateOperators(fwd, hdr, cxx, *this);
	// Output header file declaration of class
	hdr <<  shortClassNameString << " * clone() const";
	if(noInlineFiles) {
		hdr << nl << "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }"  << nl;
	} else {
		hdr << ";" << nl;

		inl << getTemplatePrefix()
			<< "inline " << getClassNameString() << "* " << getClassNameString() << "::clone() const" << nl
			<< "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }" <<  nl << nl;
	}
	cxx  << nl;

	hdr <<  "static bool equal_type(const ASN1::AbstractData& type)";
	if(noInlineFiles) {
		hdr << nl << "{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }"  << nl;
	} else {
		hdr << ";" << nl;

		inl << getTemplatePrefix()
			<< "inline bool " << getClassNameString() << "::equal_type(const ASN1::AbstractData& type)" << nl
			<< "{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }" <<  nl << nl;
	}

	generateInfo(this, fwd, hdr, cxx);

	hdr << bat <<  "}; // end class " << shortClassNameString <<  nl << nl;

	isgenerated = true;
}

void TypeBase::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr << "static const InfoType theInfo;" << nl;
	const string& templatePrefix = getTemplatePrefix();
	if (templatePrefix.empty())
		cxx << dllMacroAPI << " ";
	else
		cxx << templatePrefix;
	cxx
			<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
			<< "    " << getAncestorClass() << "::create," << nl
			<< "    ";
	type->generateTags(cxx);
	cxx << "," << nl << "   &" << getAncestorClass() << "::theInfo," << nl
		<< "};" << nl << nl;
}


void TypeBase::generateTags(ostream& cxx) const {
	const int expl (tag.isDefined()&& tag.mode==Tag::Explicit ? 1 : 0);

	cxx << "0x" << hex << setw(8) << setfill('0')
		<< (tag.type << 30 | (expl<<29) | tag.number)
		<< dec << setw(0) << setfill(' ');
}


void TypeBase::generateCplusplusConstraints(const string& prefix, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	for (size_t i = 0; i < constraints.size(); i++)
		constraints[i]->generateCplusplus( "  "+ prefix + "setConstraints(", fwd, hdr, cxx, inl);
}

void TypeBase::beginParseValue() const {
	beginParseThisTypeValue();
	contexts.top()->NullTokenContext = NULL_VALUE;
}

void TypeBase::endParseValue() const {
	contexts.top()->BraceTokenContext = contexts.top()->InObjectSetContext ? OBJECT_BRACE : '{';
	endParseThisTypeValue();
	contexts.top()->ValueTypeContext.reset();
	contexts.top()->NullTokenContext = NULL_TYPE;
}

void TypeBase::beginParseValueSet() const {
	contexts.top()->BraceTokenContext = VALUESET_BRACE;
	beginParseValue();
}

void TypeBase::endParseValueSet() const {
	endParseValue();
}

bool TypeBase::forwardDeclareMe(ostream& hdr) {
	if (canBeFwdDeclared(true)) {
		hdr << "class " << getTypeName()<< ";" << nl;
		return true;
	}
	return false;
}

string TypeBase::getPrimitiveType(const string& myName) const {
	if(!myName.empty())
		return myName + "::const_reference";

	return string();
}

bool TypeBase::canBeFwdDeclared(bool ) const {
	return false;
}

bool TypeBase::hasConstraints() const {
	return constraints.size() > 0;
}

void TypeBase::RemovePERInvisibleConstraint(const ParameterPtr& vp) {
	constraints.erase(
		remove_if(constraints.begin(), constraints.end(),
				  boost::bind(&Constraint::hasPERInvisibleConstraint, _1, boost::ref(*vp))),
		constraints.end());
}


void TypeBase::RemovePERInvisibleConstraints() {
	for_each(parameters.rep.begin(), parameters.rep.end(),
			 boost::bind(&TypeBase::RemovePERInvisibleConstraint, this, _1));
}


const string& TypeBase::getCModuleName() const {
	return module->getCModuleName();
}


void TypeBase::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	hdr  << getIdentifier() << "() : Inherited(&theInfo) {}" << nl;
}


TypePtr TypeBase::SeqOfflattenThisType(const TypeBase& parent, TypePtr result) {
	if (!isPrimitiveType() || needGenInfo() || hasNonStandardTag())
		result.reset(new DefinedType(result, parent));
	return result;;

}

bool TypeBase::needGenInfo() const {
	if (hasNonStandardTag()&& isupper(name[0]))
		return true;
	return false;
}


const char* TypeBase::getClass() const {
	return typeid(*this).name();
}
/////////////////////////////////////////////////////////

DefinedType::DefinedType(const string& name)
	: TypeBase(Tag::IllegalUniversalTag, contexts.top()->Module),
	  referenceName(name) {
	unresolved = true;
}

DefinedType::DefinedType(TypePtr refType)
	: TypeBase(*refType) {
	copyConstraints(*refType);
	baseType = refType;
	unresolved = false;
}

DefinedType::DefinedType(TypePtr refType, TypePtr& bType)
	: TypeBase(*refType),
	  referenceName(bType->getName()) {
	moveConstraints(*refType);

	baseType = bType;
	unresolved = false;
}


DefinedType::DefinedType(TypePtr refType, const string& refName)
	: TypeBase(*refType) {
	moveConstraints(*refType);
	ConstructFromType(refType, refName);
}


DefinedType::DefinedType(TypePtr refType, const TypeBase& parent)
	: TypeBase(*refType) {
	if (name.size())
		ConstructFromType(refType, parent.getName() + '_' + name);
	else
		ConstructFromType(refType, parent.getName() + "_subtype");
}

void DefinedType::ConstructFromType(TypePtr& refType, const string& refName) {
	referenceName = refName;
	refType->setName(refName);

	if (refName != "" || !refType->isPrimitiveType() || refType->hasConstraints()) {
		contexts.top()->Module->addType(refType);
	}

	baseType = refType;
	unresolved = false;
}


void DefinedType::printOn(ostream& strm) const {
	PrintStart(strm);
	strm << referenceName << ' ';
	PrintFinish(strm);
}


bool DefinedType::canReferenceType() const {
	return true;
}


bool DefinedType::isChoice() const {
	if (baseType.get())
		return baseType->isChoice();
	return false;
}


bool DefinedType::isParameterizedType() const {
	if (baseType.get())
		return baseType->isParameterizedType();
	return false;
}


bool DefinedType::referencesType(const TypeBase& type) const {
	resolveReference();
	return type.getName() == referenceName;
}

bool DefinedType::useType(const TypeBase& type) const {
	return type.getName() == referenceName;
}

void DefinedType::generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType) {
	if (baseType.get()) {
		string basicTypeName = baseType->getPrimitiveType(string());

		if (!basicTypeName.empty()) {
//      if (basicTypeName.find("::value_type::") != -1)
			//      basicTypeName = basicTypeName.substr(basicTypeName.find_last_of(':')+1);
			hdr << getIdentifier() << "(" << basicTypeName << " v, const void* info =&theInfo) : Inherited(v, info) {}" << nl;
		}

//FIXME
// In fact, generateOperator generates constructors when inherited type
// is also inherited. See testsuite/17-tags-OK.asn1
//	T3 ::= [3] IMPLICIT T2
//	T1 ::= [1] INTEGER
//	T2 ::= [2] EXPLICIT T1
// One should add and use generateConstructors instead using generateOperators
// but do not have a usage of the below line
//		baseType->generateOperators(fwd, hdr, cxx, actualType);
// which generates the T2 constructor in class T3
// 	T3(int_type v, const void* info =&theInfo) : Inherited(v, info) {}
//	T2(int_type v, const void* info =&theInfo) : Inherited(v, info) {}
//FIXME
	} else
		clog << "cannot find type " << referenceName << " to generate operators" << nl;
}


const char * DefinedType::getAncestorClass() const {
	if (baseType.get())
		return baseType->getAncestorClass();
	return nullptr;
}


string DefinedType::getTypeName() const {
	resolveReference();
	if (baseType.get() == nullptr)
		return makeCppName(referenceName);

	string result = baseType->getIdentifier();

	if (baseType->getIdentifier().size() == 0 || result == getIdentifier())
		return baseType->getTypeName();

	if (getCModuleName() != contexts.top()->Module->getCModuleName())
		result = getCModuleName() + "::" + result;
	return result;
}

void DefinedType::beginParseThisTypeValue() const {
	resolveReference();
	if (baseType.get())
		baseType->beginParseThisTypeValue();
	else {
		// used when this type is an INTEGER and the subsequent value is a NamedNumber.
		contexts.top()->IdentifierTokenContext = VALUEREFERENCE;
	}
}

void DefinedType::endParseThisTypeValue() const {
	if (baseType.get())
		baseType->endParseThisTypeValue();
	else
		contexts.top()->IdentifierTokenContext = IDENTIFIER;
}

void DefinedType::resolveReference() const {
	if (unresolved) {
		unresolved = false;

		if (contexts.top()->Module == nullptr)
			contexts.top()->Module = module;
		baseType = contexts.top()->Module->findType(referenceName);

		// AR Tag should not be fetched from base type
		// That only confuses SEQUENCE code generation
#if 0
		if (baseType.get() != nullptr) {
			if (!hasNonStandardTag())
				((Tag&)defaultTag) = ((Tag&)tag) = baseType->getTag();
		}
#endif
	}
}

void DefinedType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	if (constraints.empty()&& !hasNonStandardTag()) {
		if (hdr.iword(index) > 0) {
			hdr << "//8" << nl;
			hdr << "// " << getName() << nl;
			hdr << "//" << nl;
			hdr << "typedef " << getTypeName() << ' ' << getIdentifier() << ";" << nl;
		} else {
			fwd << "//8" << nl;
			fwd << "// " << getName() << nl;
			fwd << "//" << nl;
			fwd << "typedef " << getTypeName() << ' ' << getIdentifier() << ";" << nl;
		}
	} else {
		TypeBase::generateCplusplus(fwd, hdr, cxx, inl);
	}
}


bool DefinedType::canBeFwdDeclared(bool isComponent) const {
	resolveReference();
	if (isComponent&& baseType.get())
		return baseType->canBeFwdDeclared();
	else if (constraints.empty()&& !hasNonStandardTag())
		return false;
	else
		return true;
}

const string& DefinedType::getCModuleName() const {
	if (getName() == "" ) {
		resolveReference();
		if (baseType.get())
			baseType->getCModuleName();
	}
	return module->getCModuleName();
}

TypeBase::RemoveResult DefinedType::canRemoveType(const TypeBase& type) {
	return referencesType(type) ? MAY_NOT : OK;
}

bool DefinedType::removeThisType(const TypeBase& type) {
	return referencesType(type);
}

string DefinedType::getPrimitiveType(const string&idInContext) const {
	if (baseType.get())
		return baseType->getPrimitiveType(idInContext);
	else
		return TypeBase::getPrimitiveType(idInContext);
}

bool DefinedType::needGenInfo() const {
	return TypeBase::needGenInfo() || !constraints.empty();
}

void DefinedType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	if (baseType.get())
		baseType->generateInfo(type, fwd, hdr, cxx);
}

TypePtr DefinedType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	TypePtr result = self;
	if (needGenInfo()) {
		if (parent.hasParameters()) {
			size_t i;
			const SubTypeConstraintElement*  subcons = nullptr;
			for (i=0; i != constraints.size(); ++i) {
				if ((subcons = constraints[i]->getSubTypeConstraint()) != nullptr) {
					const TypePtr subtype = subcons->getSubType();
					ParameterListRep paramList= parent.getParameters().rep;
					for (i = 0; i < paramList.size(); ++i) {
						if (paramList[i]->getName() == subtype->getTypeName()) {
							parameters.rep.push_back(paramList[i]);
							result.reset(new ParameterizedType(self, parent,  *parameters.MakeActualParameters()));
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
bool DefinedType::isPrimitiveType() const {
	return false;
}

/////////////////////////////////////////////////////////

ParameterizedType::ParameterizedType(const string& name, ActualParameterList& args)
	: DefinedType(name) {
	arguments.swap(args);
}

ParameterizedType::ParameterizedType(TypePtr& refType, const TypeBase& parent, ActualParameterList& args)
	: DefinedType(refType, parent) {
	arguments.swap(args);
}


void ParameterizedType::printOn(ostream& strm) const {
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


bool ParameterizedType::isParameterizedType() const {
	return true;
}


bool ParameterizedType::referencesType(const TypeBase& type) const {
	if (find_if(arguments.begin(), arguments.end(),
				boost::bind(&ActualParameter::referencesType, _1, boost::cref(type))) != arguments.end())
		return true;

	return DefinedType::referencesType(type);
}

bool ParameterizedType::useType(const TypeBase& type) const {
	if (find_if(arguments.begin(), arguments.end(),
				boost::bind(&ActualParameter::useType, _1, boost::cref(type))) != arguments.end())
		return true;

	return DefinedType::useType(type);
}

string ParameterizedType::getTypeName() const {
	string typeName = DefinedType::getTypeName();
	if (isParameterizedType()) {
		typeName += '<';
		for (size_t i = 0; i < arguments.size(); ++i) {
			if (arguments[i]->generateTemplateArgument(typeName))
				typeName += ", ";
		}
		typeName[typeName.size()-2] = '>';
	}

	return typeName;
}

TypeBase::RemoveResult ParameterizedType::canRemoveType(const TypeBase& type) {
	if (find_if(arguments.begin(), arguments.end(),
				boost::bind(&ActualParameter::referencesType, _1, boost::cref(type))) != arguments.end())
		return FORBIDDEN;

	return DefinedType::canRemoveType(type);
}


/////////////////////////////////////////////////////////

SelectionType::SelectionType(const string& name, TypePtr base)
	: TypeBase(Tag::IllegalUniversalTag, contexts.top()->Module),
	  selection(name) {
	assert(base.get());
	baseType = base;
}


SelectionType::~SelectionType() {
}


void SelectionType::printOn(ostream& strm) const {
	PrintStart(strm);
	strm << selection << '<' << *baseType;
	PrintFinish(strm);
}


void SelectionType::flattenUsedTypes() {
	baseType = baseType->flattenThisType(baseType, *this);
}


TypePtr SelectionType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	return TypePtr(new DefinedType(self, parent));
}


void SelectionType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	cerr << StdError(Fatal) << "cannot generate code for Selection type" << endl;
	isgenerated = true;
}


const char * SelectionType::getAncestorClass() const {
	return "";
}


bool SelectionType::canReferenceType() const {
	return true;
}


bool SelectionType::referencesType(const TypeBase& type) const {
	return baseType->referencesType(type);
}

bool SelectionType::useType(const TypeBase& type) const {
	return baseType->useType(type);
}


/////////////////////////////////////////////////////////

BooleanType::BooleanType()
	: TypeBase(Tag::UniversalBoolean, contexts.top()->Module) {
}


void BooleanType::generateOperators(ostream& fwd, ostream & hdr, ostream& cxx, const TypeBase& actualType) {
	hdr << tab << actualType.getIdentifier() << "& operator=(bool v)"
		<< " { BOOLEAN::operator=(v);  return *this; }" << nl << bat;
}


const char * BooleanType::getAncestorClass() const {
	return "ASN1::BOOLEAN";
}


void BooleanType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	hdr << tab << getIdentifier() << "(bool b = false, const void* info =&theInfo) : Inherited(b, info) {}" << nl
		<< getIdentifier() << "(const void* info) : Inherited(info) {}" << nl << bat;
}


/////////////////////////////////////////////////////////

IntegerType::IntegerType()
	: TypeBase(Tag::UniversalInteger, contexts.top()->Module) {
}


IntegerType::IntegerType(NamedNumberList& lst)
	: TypeBase(Tag::UniversalInteger, contexts.top()->Module) {
	allowedValues.swap(lst);
}

const char * IntegerType::getAncestorClass() const {
	return "ASN1::INTEGER";
}

void IntegerType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	if (!allowedValues.empty()) {
		beginGenerateCplusplus(fwd, hdr, cxx, inl);

		int maxEnumValue = 0;
		NamedNumberList::iterator first, last = allowedValues.end();
		for (first = allowedValues.begin() ; first != last ; ++first) {
			int num = (*first)->getNumber();
			if (maxEnumValue < num)
				maxEnumValue = num;
		}

		// generate enumerations and complete the constructor implementation
		hdr << "enum NamedNumber {" << nl;
		hdr << tab;


		int prevNum = -1;
		for (first = allowedValues.begin() ; first != last ; ++first) {
			if (first != allowedValues.begin()) {
				hdr << "," << nl;
			}

			hdr << makeCppName((*first)->getName());

			int num = (*first)->getNumber();
			if (num != prevNum+1) {
				hdr << " = " << num;
			}
			prevNum = num;

		}
		hdr << bat;
		hdr << nl << "};" << nl << nl;

		for (first = allowedValues.begin() ; first != last ; ++first) {
			string fname = makeCppName((*first)->getName());
			hdr << "bool is_" << fname << "() const { return value == " << fname << "; }" << nl
				<< "void set_" << fname << "() { value = " << fname << "; }\n" << nl;
		}

		endGenerateCplusplus(fwd, hdr, cxx, inl);
	} else {
		TypeBase::generateCplusplus(fwd, hdr, cxx, inl);
		/*
		if (type->getConstraints().size())
		{
		  hdr <<  "enum {" << nl
		      << "  LowerLimit = " <<  << "," << nl
		      << "  UpperLimit = " <<  << "," << nl
		      << "};" << nl;
		}
		*/
	}
}

void IntegerType::generateInfo(const TypeBase* type, ostream& hdr , ostream& cxx) {
	hdr << "static const InfoType theInfo;" << nl;
	if ( !allowedValues.empty() ) {
		hdr << bat << "private:" << nl << tab
			<< "static const NameEntry nameEntries[" << allowedValues.size() << "];" << nl;

		cxx << type->getTemplatePrefix()
			<< "const " << type->getClassNameString() << "::NameEntry " << type->getClassNameString()
			<< "::nameEntries[" << allowedValues.size() << "] = {" << nl;

		NamedNumberList::iterator itr, last = allowedValues.end();
		for (itr = allowedValues.begin() ; itr != last ; ++itr ) {
			if (itr != allowedValues.begin())
				cxx << "," << nl;

			cxx << "    { " << (*itr)->getNumber() << ", \""
				<< (*itr)->getName() << "\" }";
		}

		cxx  << nl	<< "};\n" << nl;
	}

	cxx << type->getTemplatePrefix() << "const " ;

	if (type->getTemplatePrefix().length())
		cxx << "typename ";

	cxx  << type->getClassNameString() << "::InfoType " << type->getClassNameString() << "::theInfo = {" << nl
		 << "    create," << nl ;

	cxx << "    ";

	type->generateTags(cxx);

	cxx << "," << nl << "   &" << getAncestorClass() << "::theInfo," << nl	<< "    ";

	if (type->getConstraints().size()) {
		string strm;
		type->getConstraints()[0]->getConstraint(strm);
		cxx << strm;
	} else
		cxx << "ASN1::Unconstrained, 0, UINT_MAX";

	cxx  << nl;
	if ( allowedValues.size()  ) {
		cxx << "    , nameEntries, " << allowedValues.size()  << nl;
	}

	cxx << "};\n" << nl;
}


string IntegerType::getTypeName() const {
	if (allowedValues.size())
		return "ASN1::IntegerWithNamedNumber";

	if (constraints.size()) {
		string result("ASN1::Constrained_INTEGER<");
		constraints[0]->getConstraint(result);
		result += "> ";
		return result;
	} else
		return getAncestorClass();
}

void IntegerType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	hdr << getIdentifier() << "(int_type v =0, const void* info =&theInfo) : Inherited(v, info) {}" << nl;
}

void IntegerType::generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType) {

	if (!allowedValues.empty()) {
		hdr << getIdentifier() << "(NamedNumber v, const void* info =&theInfo) : Inherited(v, info) {}" << nl;
		hdr << actualType.getIdentifier() << "& operator=(int_type v) { setValue(v); return *this; }" << nl;
		hdr << "operator NamedNumber() const { return NamedNumber(getValue()); }" << nl << nl;
	}
}


bool IntegerType::needGenInfo() const {
	return TypeBase::needGenInfo() || allowedValues.size() > 0;
}

TypePtr IntegerType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	TypePtr result = self;
	if (needGenInfo()&& parent.hasParameters()) {
		size_t i;
		const SubTypeConstraintElement*  subcons = nullptr;
		for (i=0; i != constraints.size(); ++i) {
			if ((subcons = constraints[i]->getSubTypeConstraint())!= nullptr) {
				const TypePtr subtype = subcons->getSubType();
				ParameterListRep paramList= parent.getParameters().rep;
				for (size_t i = 0; i < paramList.size(); ++i) {
					if (paramList[i]->getName() == subtype->getTypeName()) {
						parameters.rep.push_back(paramList[i]);
						result.reset(new ParameterizedType(self,  parent, *parameters.MakeActualParameters()));
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

bool IntegerType::canReferenceType() const {
	if (constraints.size())
		return true;

	return false;
}


bool IntegerType::referencesType(const TypeBase& type) const {
	if (constraints.size())
		return constraints[0]->referencesType(type);
	else
		return false;
}

void IntegerType::beginParseThisTypeValue() const {
	if(!allowedValues.empty())
		contexts.top()->IdentifierTokenContext = VALUEREFERENCE;
}

void IntegerType::endParseThisTypeValue() const {
	contexts.top()->IdentifierTokenContext = IDENTIFIER;
}
/////////////////////////////////////////////////////////

EnumeratedType::EnumeratedType(NamedNumberList& enums, bool extend, NamedNumberList* ext)
	: TypeBase(Tag::UniversalEnumeration, contexts.top()->Module),
	  maxEnumValue(0) {
	enumerations.swap(enums);
	numEnums = enumerations.size();
	extendable = extend;
	if (ext != nullptr) {
		enumerations.splice( enumerations.end(), *ext);
		delete ext;
	}
}


void EnumeratedType::printOn(ostream& strm) const {
	PrintStart(strm);
	strm << nl;

	size_t i;
	NamedNumberList::const_iterator itr, last = enumerations.end();
	for (i = 0, itr = enumerations.begin() ; i < numEnums; i++, ++itr)
		strm << tab << **itr << nl << bat;

	if (extendable) {
		strm << "..." << nl;
		for (; itr != last; ++itr)
			strm << tab << **itr << nl << bat;
	}
	PrintFinish(strm);
}


TypePtr EnumeratedType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	return TypePtr(new DefinedType(self, parent));
}


void EnumeratedType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	beginGenerateCplusplus(fwd, hdr, cxx, inl);

	NamedNumberList::iterator itr, last = enumerations.end();
	for (itr = enumerations.begin(); itr != last; ++itr) {
		int num = (*itr)->getNumber();
		if (maxEnumValue < num)
			maxEnumValue = num;
	}

	// generate enumerations and complete the constructor implementation
	hdr << "enum NamedNumber {" << nl;
	hdr << tab << "unknownEnumeration_ = -1," << nl;

	int prevNum = -1;
	for (itr = enumerations.begin(); itr != last; ++itr) {
		if (itr != enumerations.begin()) {
			hdr << "," << nl;
		}

		hdr << makeCppName((*itr)->getName());

		int num = (*itr)->getNumber();
		if (num != prevNum+1) {
			hdr << " = " << num;
		}
		prevNum = num;
	}
	hdr << bat;
	hdr  << nl <<   "};" << nl	 << nl;

	// Constructor with NameNumber. Cannot goes into generateConstructor because
	// generateConstructor is called by beginGenerateCplusplus and NamedNumber is not yet generated
	// TODO to be refactored.
	hdr << bat << "protected:" << nl << tab;
	hdr << getIdentifier() << "(NamedNumber v, const void* info =&theInfo) : Inherited(v, info) {}" << nl << nl;
	hdr << bat << "public:" << nl << tab;

	generateCplusplusConstraints(string(), fwd, hdr, cxx, inl);

	for (itr = enumerations.begin(); itr != last; ++itr) {
		string fname = makeCppName((*itr)->getName());
		hdr << "bool is_" << fname << "() const { return value == " << fname << "; }" << nl
			<< "void set_" << fname << "() { value = " << fname << "; }\n" << nl;
	}

	endGenerateCplusplus(fwd, hdr, cxx, inl);
	hdr << bat;
}


void EnumeratedType::generateOperators(ostream& fwd, ostream& hdr, ostream& , const TypeBase& actualType) {

	hdr << getIdentifier() << "(const NamedNumber v) : Inherited(&theInfo)"
		<< " { setFromInt(v); }" << nl;

	hdr << actualType.getIdentifier() << "& operator=(const NamedNumber v)";
	hdr << " { setFromInt(v);  return *this; }" << nl;

	hdr << "operator NamedNumber() const { return NamedNumber(asInt()); }\n" << nl;

	hdr << "bool operator == (NamedNumber rhs) const { return value == rhs; }" << nl
		<< "bool operator != (NamedNumber rhs) const { return value != rhs; }" << nl
		<< "bool operator <  (NamedNumber rhs) const { return value <  rhs; }" << nl
		<< "bool operator >  (NamedNumber rhs) const { return value >  rhs; }" << nl
		<< "bool operator <= (NamedNumber rhs) const { return value <= rhs; }" << nl
		<< "bool operator >= (NamedNumber rhs) const { return value >= rhs; }" << nl
		<< "bool operator == (const " << getIdentifier() << "&rhs) const { return value == rhs.value; }" << nl
		<< "bool operator != (const " << getIdentifier() << "&rhs) const { return value != rhs.value; }" << nl
		<< nl;

	hdr << "void swap (" << getIdentifier() << "& that) { Inherited::swap(that); }" << nl;
}


const char * EnumeratedType::getAncestorClass() const {
	return "ASN1::ENUMERATED";
}

void EnumeratedType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	hdr << getIdentifier() << "() : Inherited(&theInfo) {}" << nl;
}

bool EnumeratedType::isPrimitiveType() const {
	return false;
}

void EnumeratedType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr << "static const InfoType theInfo;" << nl;

	hdr << bat << "private:" << nl
		<< tab << "static const char * nameList[];" << nl;

	cxx << type->getTemplatePrefix()
		<< "const char * " << type->getClassNameString()
		<< "::nameList[] = {" << nl
		<< "    \"";

	NamedNumberList::iterator itr, last = enumerations.end();

	for(int i=0; i<=maxEnumValue; i++) {
		itr = find_if(enumerations.begin(), last, CompareNamedNumber (i));
		if (i>0)
			cxx << "\",\n    \"";

		if(itr!=last)
			cxx << (*itr)->getName();
		else
			cxx << "<undefined>=" << i ;
	}

	cxx << "\"" << nl << "  };"<< nl  << nl;

	const string& templatePrefix = getTemplatePrefix();
	if (templatePrefix.empty())
		cxx << dllMacroAPI << " ";
	else
		cxx << templatePrefix;
	cxx
			<< "const " << type->getClassNameString() << "::InfoType " << type->getClassNameString() << "::theInfo = {" << nl
			<< "    ASN1::ENUMERATED::create," << nl
			<< "    ";

	type->generateTags(cxx);

	cxx << "," << nl << "    0"
		<< "," << nl << "    " << extendable
		<< "," << nl << "    " << maxEnumValue << ", nameList" << nl
		<< "};" << nl << nl;
}

void EnumeratedType::beginParseThisTypeValue() const {
	contexts.top()->IdentifierTokenContext = VALUEREFERENCE;
}

void EnumeratedType::endParseThisTypeValue() const {
	contexts.top()->IdentifierTokenContext = IDENTIFIER;
}

/////////////////////////////////////////////////////////

RealType::RealType()
	: TypeBase(Tag::UniversalReal, contexts.top()->Module) {
}


const char * RealType::getAncestorClass() const {
	return "ASN1::REAL";
}

void RealType::generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType) {
	hdr << tab;
	hdr << "    " << actualType.getIdentifier() << "& operator=(double v)";
	hdr << " { BinaryReal::operator=(v);  return *this; }" << nl;
	hdr << bat;
}


void RealType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	hdr << tab;
	hdr  << "    " << getIdentifier() << "(double v = 0) : Inherited(v) {}" << nl ;
	hdr << bat;
}

/////////////////////////////////////////////////////////

BitStringType::BitStringType()
	: TypeBase(Tag::UniversalBitString, contexts.top()->Module) {
}


BitStringType::BitStringType(NamedNumberList& lst)
	: TypeBase(Tag::UniversalBitString, contexts.top()->Module) {
	allowedBits.swap(lst);
}


const char * BitStringType::getAncestorClass() const {
	return "ASN1::BIT_STRING";
}


string BitStringType::getTypeName() const {
	if (constraints.size()) {
		string result("ASN1::Constrained_BIT_STRING<");
		constraints[0]->getConstraint(result);
		result += "> ";
		return result;
	} else
		return getAncestorClass();
}

bool BitStringType::needGenInfo() const {
	return TypeBase::needGenInfo() || allowedBits.size() > 0;
}

void BitStringType::beginParseThisTypeValue() const {
	contexts.top()->IdentifierTokenContext = BIT_IDENTIFIER;
}

void BitStringType::endParseThisTypeValue() const {
	contexts.top()->IdentifierTokenContext = IDENTIFIER;
}

int BitStringType::getToken() const {
	return BIT_IDENTIFIER;
}

void BitStringType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	if (!allowedBits.empty()) {
		beginGenerateCplusplus(fwd, hdr, cxx, inl);

		// generate enumerations and complete the constructor implementation
		hdr << nl << "enum NamedBits {" << nl;

		int prevNum = -1;

		NamedNumberList::iterator itr, last = allowedBits.end();

		for (itr = allowedBits.begin() ; itr != last ; ++itr) {
			if (itr != allowedBits.begin()) {
				hdr << "," << nl;
			}

			hdr << tab << makeCppName((*itr)->getName()) << bat;

			int num = (*itr)->getNumber();
			if (num != prevNum+1) {
				hdr << " = " << num;
			}
			prevNum = num;
		}

		hdr  << nl << "};" << nl << nl;

		endGenerateCplusplus(fwd, hdr, cxx, inl);
	} else
		TypeBase::generateCplusplus(fwd, hdr, cxx, inl);
}

void BitStringType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr << tab;
	hdr <<  "static const InfoType theInfo;" << nl;

	int maxNamedValue = 0;

	if(!allowedBits.empty()) {

		hdr << bat << "private:" << nl << tab
			<< "static const char * nameList[];" << nl;

		cxx << type->getTemplatePrefix()
			<< "const char * " << type->getClassNameString()
			<< "::nameList[] = {" << nl
			<< "    \"";

		NamedNumberList::iterator itr, last = allowedBits.end();

		for (itr = allowedBits.begin() ; itr != last ; ++itr) {
			int num = (*itr)->getNumber();
			if (maxNamedValue < num)
				maxNamedValue = num;
		}

		for(int i=0; i<=maxNamedValue; i++) {
			itr = find_if(allowedBits.begin(), last, CompareNamedNumber (i));
			if (i>0)
				cxx << "\"," << nl << "    \"";

			if(itr!=last)
				cxx << (*itr)->getName();
			else
				cxx << '0' ;
		}
		cxx << "\"" << nl << "};" << nl << nl;
	}


	const string& templatePrefix = type->getTemplatePrefix();
	if (templatePrefix.empty())
		cxx << dllMacroAPI << " ";
	else
		cxx << templatePrefix;

	cxx	<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl;
	cxx << "    ASN1::BIT_STRING::create," << nl;
	cxx << "    ";

	type->generateTags(cxx);

	cxx << "," << nl << "   &" << getAncestorClass() << "::theInfo"
		"," << nl << "    ";

	const SizeConstraintElement* sizeConstraint ;

	if (type->getConstraints().size()&&
			((sizeConstraint = type->getConstraints()[0]->getSizeConstraint()) != nullptr)) {
		string str;
		sizeConstraint->getConstraint(str);
		cxx << str.substr(0, str.size()-2);
	} else
		cxx << "ASN1::Unconstrained, 0, UINT_MAX";

	if(!allowedBits.empty()) {
		cxx << "," << nl << "    " << maxNamedValue+1
			<< ", nameList";
	} else
		cxx << "," << nl << "    " << "0, 0";

	cxx << nl << "};" << nl << nl;
	hdr << bat;
}

/////////////////////////////////////////////////////////

OctetStringType::OctetStringType()
	: TypeBase(Tag::UniversalOctetString, contexts.top()->Module) {
}



const char * OctetStringType::getAncestorClass() const {
	return "ASN1::OCTET_STRING";
}


string OctetStringType::getTypeName() const {
	if (constraints.size()) {
		string result(getConstrainedType());
		result += '<';
		constraints[0]->getConstraint(result);
		result += "> ";
		return result;
	} else
		return getAncestorClass();
}

const char* OctetStringType::getConstrainedType() const {
	return "ASN1::Constrained_OCTET_STRING";
}

void OctetStringType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	TypeBase::generateConstructors(fwd, hdr, cxx, inl);

	hdr <<  getIdentifier()  << "(size_type n, unsigned char v) : Inherited(n, v) {}" << nl
		<<  "template <class Iterator>" << nl
		<<  tab << getIdentifier() << "(Iterator first, Iterator last) : Inherited(first, last) {}" << nl
		<<  getIdentifier() << "(const ASN1::octets& that) : Inherited(that) {}" << nl << bat;

}

void OctetStringType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr << tab;
	hdr << "static const InfoType theInfo;" << nl;
	const string& templatePrefix = type->getTemplatePrefix();
	if (templatePrefix.empty())
		cxx << dllMacroAPI << " ";
	else
		cxx << templatePrefix;

	cxx	<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    " ;

	cxx << "ASN1::OCTET_STRING::create," << nl;
	cxx << "    ";

	type->generateTags(cxx);

	cxx << "," << nl << "   &" << getAncestorClass() << "::theInfo," << nl
		<< "    ";

	const SizeConstraintElement* sizeConstraint ;

	if (type->getConstraints().size()&&
			(sizeConstraint = type->getConstraints()[0]->getSizeConstraint()) != nullptr) {
		string str;
		sizeConstraint->getConstraint(str);
		cxx << str.substr(0, str.size()-2);
	} else
		cxx << "ASN1::Unconstrained, 0, UINT_MAX";

	cxx  << nl << "};" << nl << nl;
	hdr << bat;
}

/////////////////////////////////////////////////////////

NullType::NullType()
	: TypeBase(Tag::UniversalNull, contexts.top()->Module) {
}

void NullType::beginParseThisTypeValue() const {
	contexts.top()->NullTokenContext = NULL_VALUE;
}

void NullType::endParseThisTypeValue() const {
	contexts.top()->NullTokenContext = NULL_TYPE;
}

const char * NullType::getAncestorClass() const {
	return "ASN1::Null";
}


/////////////////////////////////////////////////////////

SequenceType::SequenceType(TypesVector* a_std,
						   bool extend,
						   TypesVector * ext,
						   unsigned tagNum)
	: TypeBase(tagNum, contexts.top()->Module), detectingLoop(false) {
	if (a_std != nullptr) {
		numFields = a_std->size();
		a_std->swap(fields);
		delete a_std;
	} else
		numFields = 0;

	extendable = extend;
	if (ext != nullptr) {
		fields.insert(fields.end(), ext->begin(), ext->end());
		delete ext;
	}
	needFwdDeclare.resize(fields.size());
}


void SequenceType::printOn(ostream& strm) const {
	PrintStart(strm);
	strm << nl;

	size_t i;
	for (i = 0; i < numFields; ++i)
		strm << *fields[i];

	if (extendable) {
		strm << tab << "..." << nl;

		for (; i < fields.size(); ++i)
			strm << *fields[i];

		strm << bat;
	}

	PrintFinish(strm);
}


void SequenceType::flattenUsedTypes() {
	TypesVector::iterator itr, last = fields.end();

	for (itr = fields.begin(); itr != last; ++itr) {
		//[AR] Reset the base type's tag so explicit tags can be recognised
		TypePtr ptr(*itr);
		*itr = (*itr)->flattenThisType(*itr, *this);
		if(ptr!=*itr)
			ptr->setTag(ptr->getDefaultTag());
	}
}


TypePtr SequenceType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	if (parent.hasParameters()) {
		ParameterListPtr params = parent.getParameters().getReferencedParameters(*this);
		if (params.get()) {
			setParameters(*params);
			return TypePtr(new ParameterizedType(self, parent, *parameters.MakeActualParameters()));
		}
	}
	return TypePtr(new DefinedType(self, parent));
}


bool SequenceType::isPrimitiveType() const {
	return false;
}


void SequenceType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	size_t i;
	beginGenerateCplusplus(fwd, hdr, cxx, inl);

	hdr <<  getIdentifier() << "(const " << shortClassNameString << "& that) : Inherited(that) {}" << nl << nl;
	hdr <<  shortClassNameString << "& operator=(const " << shortClassNameString << "& that)"
		<<  " { Inherited::operator=(that); return *this; }\n" << nl;
	// Output enum for optional parameters
	bool outputEnum = false;
	bool outputNumber = false;
	unsigned optionalId = 0;
	TypesVector::iterator itr, last = fields.end();
	for (i = 0, itr=fields.begin() ; itr != last ; ++i, ++itr) {
		TypeBase& field = **itr;
		if (i >= numFields || field.isOptional()  || field.hasDefaultValue()) {
			if (outputEnum)
				hdr << "," << nl;
			else {
				hdr <<  "enum OptionalFields {" << nl;
				outputEnum = true;
			}

			if (!field.isRemovedType()) {
				hdr <<  "  e_" << field.getIdentifier();
				if (outputNumber)
					hdr << " = " << optionalId++;
			} else {
				optionalId++;
				outputNumber = true;
			}
		}
	}

	if (outputEnum)
		hdr  << nl <<   "};" << nl;

	// Output the Component scope classes and accessor/mutator functions
	stringstream tmpcxx;
	for (i = 0, itr=fields.begin() ; itr != last ; ++i, ++itr) {
		generateComponent(**itr, fwd, hdr, tmpcxx, inl, i);
	}

	stringstream decoder;
	for (itr=fields.begin() ; itr != last; ++itr) {
		(*itr)->generateDecoder(decoder);
	}

	hdr <<  "void swap(" << getIdentifier() << "& that)";
	if(noInlineFiles)
		hdr << " { Inherited::swap(that); }" << nl;
	else {
		hdr << ";" << nl;

		inl << getTemplatePrefix()
			<< "inline void " << getClassNameString() << "::swap(" << getIdentifier() << "& that)" << nl
			<< "{ Inherited::swap(that); }\n" << nl;
	}

	generateOperators(fwd, hdr, cxx, *this);

	// Output header file declaration of class
	hdr <<  shortClassNameString << " * clone() const";
	if(noInlineFiles) {
		hdr << nl
			<<  "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }"  << nl;
	} else {
		hdr << ";" << nl;

		inl << getTemplatePrefix()
			<< "inline " << getClassNameString() << "* " << getClassNameString() << "::clone() const" << nl
			<< "{ return static_cast<"<< shortClassNameString << "*> (Inherited::clone()); }" <<  nl << nl;
	}

	cxx  << nl;

	generateInfo(this, fwd, hdr, cxx);

	if (!decoder.str().empty()) {
		hdr << "static ASN1::AbstractData* create(const void*);" << nl
			<< "bool do_accept(ASN1::Visitor& visitor);" << nl;

		cxx << getTemplatePrefix()
			<< "ASN1::AbstractData* " << getClassNameString() << "::create(const void* info) {" << nl
			<< "    return new " << getIdentifier() << "(info);" << nl
			<< "}" << nl << nl;

		cxx	<< getTemplatePrefix()
			<< "bool "<< getClassNameString() << "::do_accept(ASN1::Visitor& visitor) {" << nl
			<< "  if (Inherited::do_accept(visitor)) {" << nl
			<< "    if (!visitor.get_env())" << nl
			<< "      return true;" << nl
			<< decoder.str()
			<< "  }" << nl
			<< "  return false;" << nl
			<< "}" << nl << nl;
	}
	decoder << ends;
	hdr << bat;
	hdr << "}; // end class " << shortClassNameString <<  nl << nl;

	cxx << tmpcxx.str();
	tmpcxx << ends;
	isgenerated = true;
}


const char * SequenceType::getAncestorClass() const {
	return "ASN1::SEQUENCE";
}


bool SequenceType::canReferenceType() const {
	return true;
}


bool SequenceType::useType(const TypeBase& type) const {
	TypesVector::const_iterator itr, last = fields.end();
	for (itr=fields.begin() ; itr != last; ++itr)
		if ((*itr)->useType(type))
			return true;
	return false;
}


void SequenceType::generateComponent(TypeBase& field, ostream& fwd, ostream& hdr, ostream& cxx , ostream& inl, int id) {
	if (field.isRemovedType())
		return;

	string typeName =  field.getTypeName();
	string componentIdentifier = field.getIdentifier();
	string componentName = field.getName();

	bool bisOptional = (id >= (int)numFields || fields[id]->isOptional() || fields[id]->hasDefaultValue());

	hdr.precision(hdr.precision() + 4);

	// generate component scope class

	hdr << nl;
	hdr	<< "class " << componentIdentifier <<" {" << nl
		<< "public:" << nl;

	hdr << tab;

	field.setOuterClassName(getClassNameString() + "::" + componentIdentifier);
	field.setTemplatePrefix(templatePrefix);
	field.setName("value_type");

	hdr.precision(hdr.precision() + 4);
	if (!field.isSequenceOfType() || !needFwdDeclare[id]) {
		Tag ftag = field.getTag();
		if(ftag.mode==Tag::Explicit)
			field.setTag(field.getDefaultTag());

		field.generateCplusplus(fwd, hdr, cxx, inl);
		field.setTag(ftag);
	} else { // recursive reference
		hdr << "class " << componentIdentifier << ";" << nl;
		SequenceOfType& type = *static_cast<SequenceOfType*>(&field);
		type.setNonTypedef(true);
		stringstream dummy;
		type.generateCplusplus(fwd, inl, cxx, dummy);
	}
	hdr.precision(hdr.precision() - 4);

	field.setName(componentName);

	hdr << "typedef value_type&        reference;" << nl
		<< "typedef const value_type&  const_reference;" << nl
		<< "typedef value_type*        pointer;" << nl
		<< "typedef const value_type*  const_pointer;" << nl;

	hdr << bat;
	hdr  << "}; // end class " << componentIdentifier <<  nl << nl;

	string primitiveFieldType = field.getPrimitiveType(componentIdentifier);
	string typenameKeyword;

	if (templatePrefix.length()) {
		typenameKeyword = "typename ";
		if (primitiveFieldType.find(componentIdentifier + "::") != -1)
			primitiveFieldType = "typename " + primitiveFieldType;
	}

	stringstream varName;
	varName << "*static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(fields[" << id << "])";

	stringstream constVarName;
	constVarName << "*static_cast<" << typenameKeyword << componentIdentifier << "::const_pointer>(fields[" << id << "])";

	// generate accessor/mutator functions

	if (!bisOptional) { // mandatory component
		hdr <<  typenameKeyword << componentIdentifier << "::const_reference get_" << componentIdentifier << " () const";
		if(noInlineFiles)
			hdr << " { return " << constVarName.str() << "; }" << nl;
		else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::const_reference " << getClassNameString() << "::get_" << componentIdentifier << " () const" << nl
				<< "{ return " << constVarName.str() << "; }\n" << nl;
		}

		hdr <<   typenameKeyword << componentIdentifier << "::reference ref_" << componentIdentifier << " ()";
		if(noInlineFiles)
			hdr << " { return " << varName.str() << "; }" << nl;
		else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::ref_" << componentIdentifier << " ()" << nl
				<< "{ return " << varName.str() << "; }\n" << nl;
		}

		hdr <<  typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ()";
		if(noInlineFiles)
			hdr << " { return " << varName.str() << "; }" << nl;
		else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::set_" << componentIdentifier << " ()" << nl
				<< "{ return " << varName.str() << "; }\n" << nl;
		}

		hdr <<  typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ("<< primitiveFieldType << " v)";
		// SUN cannot handle properly this function if inlined
		if(noInlineFiles || templatePrefix.length()) {
			hdr << nl << "{ return " << varName.str() << " = v; }" << nl;
		} else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::set_" << componentIdentifier << " ("<< primitiveFieldType << " v)" << nl
				<< "{ return " << varName.str() << " = v; }\n" << nl;
		}
	} else { // optional component
		string enumName = "e_" + componentIdentifier;
		if (field.getTypeName() != "ASN1::Null") {
			hdr <<  typenameKeyword << componentIdentifier << "::const_reference get_" << componentIdentifier << " () const";
			if(noInlineFiles) {
				hdr << nl
					<< "{" << nl
					<< "  assert(hasOptionalField(" << enumName <<"));" << nl
					<< "  return " << constVarName.str() << ";" << nl
					<< "}" << nl;
			} else {
				hdr << ";" << nl;

				inl << getTemplatePrefix()
					<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::const_reference " << getClassNameString() << "::get_" << componentIdentifier << " () const" << nl
					<< "{" << nl
					<< "  assert(hasOptionalField(" << enumName <<"));" << nl
					<< "  return " << constVarName.str() << ";" << nl
					<< "}\n" << nl;
			}

			hdr  << typenameKeyword << componentIdentifier << "::reference ref_" << componentIdentifier << " ()";
			if(noInlineFiles) {
				hdr << nl
					<< "{" << nl
					<< "  assert(hasOptionalField(" << enumName <<"));" << nl
					<< "  return " << varName.str() << ";" << nl
					<< "}" << nl;
			} else {
				hdr << ";" << nl;

				inl << getTemplatePrefix()
					<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::ref_" << componentIdentifier << " ()" << nl
					<< "{" << nl
					<< "  assert(hasOptionalField(" << enumName <<"));" << nl
					<< "  return " << varName.str() << ";" << nl
					<< "}\n" << nl;
			}
		}

		hdr  << typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ()";
		if(noInlineFiles) {
			hdr << nl
				<< "{" << nl
				<< "  includeOptionalField( "<< enumName << ", " << id << ");" << nl
				<< "  return " << varName.str() << ";" << nl
				<< "}" << nl;
		} else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::set_" << componentIdentifier << " ()" << nl
				<< "{" << nl
				<< "  includeOptionalField( "<< enumName << ", " << id << ");" << nl
				<< "  return " << varName.str() << ";" << nl
				<< "}\n" << nl;
		}

		if (field.getTypeName() != "ASN1::Null") {
			hdr  << typenameKeyword << componentIdentifier << "::reference set_" << componentIdentifier << " ("<< primitiveFieldType << " v)";
			// SUN cannot handle properly this function if inlined
			if(noInlineFiles || templatePrefix.length()) {
				hdr << nl
					<< "{" << nl
					<< "  includeOptionalField( "<< enumName << ", " << id << ");" << nl
					<< "  return " << varName.str() << " = v;" << nl
					<< "}" << nl;
			} else {
				hdr << ";" << nl;

				inl << getTemplatePrefix()
					<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::set_" << componentIdentifier << " ("<< primitiveFieldType << " v)" << nl
					<< "{" << nl
					<< "  includeOptionalField( "<< enumName << ", " << id << ");" << nl
					<< "  return " << varName.str() << " = v;" << nl
					<< "}\n" << nl;
			}
		}

		hdr  << "void omit_" << componentIdentifier << " ()";
		if(noInlineFiles)
			hdr << " { removeOptionalField( " << enumName << "); }" << nl;
		else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline void " << getClassNameString() << "::omit_" << componentIdentifier << " ()" << nl
				<< "{ removeOptionalField( " << enumName << "); }\n" << nl;
		}

		hdr  << "bool " << componentIdentifier << "_isPresent () const";
		if(noInlineFiles)
			hdr << " { return hasOptionalField( " << enumName << "); }" << nl;
		else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline bool " << getClassNameString() << "::" << componentIdentifier << "_isPresent () const" << nl
				<< "{ return hasOptionalField( " << enumName << "); }\n" << nl;
		}
	}
	hdr.precision(hdr.precision()-4);
}

bool SequenceType::canBeFwdDeclared(bool ) const {
	return !isParameterizedType();
}


void SequenceType::RemovePERInvisibleConstraint(const ParameterPtr& param) {
	TypesVector::iterator itr, last=fields.end();
	for (itr = fields.begin(); itr != last; ++itr)
		(*itr)->RemovePERInvisibleConstraint(param);
}

void SequenceType::generateForwardDecls(ostream& fwd, ostream& hdr) {
	// Output forward declarations for choice pointers, but not standard classes
	bool needExtraLine = false;

	if (fwd.precision())
		return;

	for (size_t i = 0; i < fields.size(); i++) {
		TypeBase& field = *fields[i];
		if ( needFwdDeclare[i] && field.forwardDeclareMe(fwd) ) {
			needExtraLine = true;
		}
	}

	if (needExtraLine)
		fwd << nl;
}

bool SequenceType::referencesType(const TypeBase& type) const {
	if (detectingLoop)
		return false;

	bool ref = false;
	//if (!type.isChoice())
	detectingLoop = true;
	ref = type.referencesType(*this);
	detectingLoop = false;

	for (size_t i = 0; i < fields.size(); i++) {
		TypeBase& field = *fields[i];
		if ( field.referencesType(type) ) {
			if (ref&& !dynamic_cast<SequenceOfType*>(&field)) { // recursive reference detected
				needFwdDeclare[i] = true;
				return false;
			} else
				return true;
		}
	}
	return false;
}

TypeBase::RemoveResult SequenceType::canRemoveType(const TypeBase& type) {
	TypesVector::iterator itr, last=fields.end();
	for (itr = fields.begin(); itr != last; ++itr) {
		TypeBase& field = **itr;
		switch ( field.canRemoveType(type)) {
		case MAY_NOT:
			if (!field.isOptional())
				return FORBIDDEN;
			break;
		case FORBIDDEN:
			return FORBIDDEN;
		}
	}
	return OK;
}

bool SequenceType::removeThisType(const TypeBase& type) {
	TypesVector::iterator itr, last=fields.end();
	for (itr = fields.begin(); itr != last; ++itr) {
		TypeBase& field = **itr;
		if (field.removeThisType(type))
			itr->reset(new RemovedType(field));
	}

	return getName() == type.getName();
}


void SequenceType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	int nOptional=0;
	int nExtensions=0;
	bool hasNonOptionalFields=false;

	hdr  << "static const Inherited::InfoType theInfo;\n" << nl
		 << bat << "private:" << nl << tab;

	size_t nTotalFields = fields.size();
	bool autoTag = true; // tag.mode != Tag::Implicit;
	bool useDefaultTag = tag.mode != Tag::Automatic;

	if (nTotalFields >0&& type == this) {
		hdr  << "static const void* fieldInfos[" << nTotalFields << "];" << nl;
		cxx << type->getTemplatePrefix()
			<< "const void* "<< type->getClassNameString() << "::fieldInfos[" << nTotalFields << "] = {" << nl;

		size_t i;
		for (i=0; i < fields.size(); ++i)   {
			TypeBase& field = *fields[i];
			if (useDefaultTag&& field.hasNonStandardTag())
				useDefaultTag = false;

			const Tag& fieldTag = field.getTag();
			if (fieldTag.mode != Tag::Automatic&& ! (
						fieldTag.number == (unsigned)i&&
						fieldTag.type == Tag::ContextSpecific&&
						fieldTag.mode == Tag::Implicit)) {
				autoTag = false;
			}

			if (i != 0 )
				cxx << "," << nl;

			if (field.isRemovedType())
				cxx << "    nullptr";
			else
				cxx << "   &"	<< field.getIdentifier() << "::value_type::theInfo";

		}
		cxx  << nl << "};\n" << nl;


		if (numFields > 0) {
			hdr  << "static int fieldIds[" << nTotalFields << "];" << nl;
			cxx << getTemplatePrefix()
				<< "int " << getClassNameString() << "::fieldIds[" << nTotalFields << "] = {" << nl;

			unsigned optionalId  = 0;
			for (size_t i = 0; i < numFields-1; ++i) {
				if (fields[i]->isOptional() || fields[i]->hasDefaultValue()) {
					cxx << "    " << optionalId++ << "," << nl;
					nOptional++;
				} else
					cxx << "    -1," << nl;
			}

			if (numFields > 0) {
				if (fields[numFields-1]->isOptional() || fields[numFields-1]->hasDefaultValue()) {
					cxx << "    " << optionalId++  << nl;
					nOptional++;
				} else
					cxx << "    -1," << nl;

				cxx << "};\n" << nl;
			}
		}

		if (!useDefaultTag&& !autoTag) {
			hdr  << "static unsigned fieldTags[" << nTotalFields << "];" << nl;
			cxx << getTemplatePrefix()
				<< "unsigned " << getClassNameString() << "::fieldTags[" << nTotalFields << "] = {" << nl;

			for (size_t i = 0; i < nTotalFields; ++i) {
				cxx << "    ";
				fields[i]->generateTags(cxx);
				if (i != nTotalFields-1)
					cxx << "," << nl;
			}
			cxx  << nl
				 << "};\n" << nl;
		}

		vector<unsigned char> bitmap;
		nExtensions = fields.size()-numFields;
		bitmap.resize((nExtensions+7)/8);
		for (i = numFields; i < fields.size(); i++) {
			if (!fields[i]->isOptional()&& !fields[i]->hasDefaultValue()) {
				unsigned bit = i- numFields;
				unsigned mod = bit>>3;
				bitmap[mod] |= 1 << (7 - (bit&7));
				hasNonOptionalFields = true;
			}
		}

		if (hasNonOptionalFields) {
			hdr  << "static const char* nonOptionalExtensions;" << nl;

			cxx << getTemplatePrefix()
				<< "const char* " << getClassNameString() <<  "::nonOptionalExtensions = \"";

			for (i = 0; i < (int)bitmap.size(); ++i) {
				cxx << "\\x" << hex << (unsigned) bitmap[i] << dec ;
			}
			cxx << "\";\n" << nl;
		}

		hdr  << "static const char* fieldNames[" << nTotalFields << "];" << nl;

		cxx << getTemplatePrefix()
			<< "const char* " << getClassNameString() <<  "::fieldNames[" << nTotalFields << "] = {" << nl;

		for (i = 0; i < fields.size(); i++) {
			cxx << "    \"" << fields[i]->getName() << '\"';
			if (i != fields.size() -1)
				cxx << "," << nl;
			else
				cxx << nl
					<< "};\n" << nl;
		}
	} else if (numFields > 0) {
		for (size_t i = 0; i < fields.size(); ++i) {
			if (fields[i]->isOptional()) {
				nOptional++;
			}
		}
	}

	string typenameKeyword;
	if (type->getTemplatePrefix().size())
		typenameKeyword = "typename ";

	cxx << type->getTemplatePrefix()
		<< "const "<< typenameKeyword << type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    " << type->getClassNameString() << "::create," << nl
		<< "    ";
	type->generateTags(cxx);
	cxx << ",\n    0," << nl;

	if (extendable)
		cxx << "    true," << nl;
	else
		cxx << "    false," << nl;

	if (nTotalFields > 0 ) {
		if (type == this)
			cxx << "    " << getClassNameString() << "::fieldInfos," << nl
				<< "    " << getClassNameString() << "::fieldIds," << nl;
		else
			cxx << "    " << getIdentifier() << "::theInfo.fieldInfos," << nl
				<< "    " << getIdentifier() << "::theInfo.ids," << nl;
	} else
		cxx << "    nullptr, nullptr," << nl;

	cxx << "    " << numFields << ", " << nExtensions << ", " << nOptional++ << "," << nl
		<< "    ";

	if (hasNonOptionalFields&& nTotalFields > 0) {
		if (type == this)
			cxx << getClassNameString() << "::nonOptionalExtensions," << nl;
		else
			cxx << "     " << getIdentifier() << "::theInfo.nonOptionalExtensions," << nl;
	} else
		cxx << "nullptr," << nl;

	cxx << "    ";

	if (autoTag || nTotalFields ==0)
		cxx << "nullptr," << nl;
	else if (useDefaultTag) {
		cxx << "&";
		if (type == this)
			cxx << getClassNameString() ;
		else
			cxx << getIdentifier();
		cxx << "::defaultTag," << nl;
	} else if (type == this)
		cxx << getClassNameString() << "::fieldTags," << nl;
	else
		cxx << "     " << getIdentifier() << "::theInfo.tags," << nl;

	cxx << "    ";
	if (nTotalFields >0) {
		if (type == this)
			cxx << getClassNameString() << "::fieldNames" << nl;
		else
			cxx << getIdentifier() << "::theInfo.names" << nl;
	} else
		cxx << "nullptr" << nl;

	cxx << "};\n" << nl;
}

/////////////////////////////////////////////////////////

SequenceOfType::SequenceOfType(TypePtr base, ConstraintPtr constraint, unsigned tag)
	: TypeBase(tag, contexts.top()->Module), nonTypedef (false) {
	assert(base.get());
	baseType = base;
	if (constraint.get() != nullptr) {
		addConstraint(constraint);
	}
}


SequenceOfType::~SequenceOfType() {
}


void SequenceOfType::printOn(ostream& strm) const {
	PrintStart(strm);
	if (baseType.get() == nullptr)
		strm << "!!Null Type!!" << nl;
	else
		strm << *baseType << nl;
	PrintFinish(strm);
}


void SequenceOfType::flattenUsedTypes() {
	baseType = baseType->SeqOfflattenThisType(*this, baseType);
	assert(baseType.get());
}


TypePtr SequenceOfType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	TypePtr result = self;
	if (!baseType->isPrimitiveType() || baseType->hasConstraints() ||
			baseType->hasNonStandardTag() )
		result.reset(new DefinedType(self, parent));
	return result;
}


bool SequenceOfType::isPrimitiveType() const {
	return !hasNonStandardTag()&& !nonTypedef;
}



void SequenceOfType::generateForwardDecls(ostream& fwd, ostream& hdr) {
	if (forwardDeclareMe(fwd))
		fwd << nl;
}


const char * SequenceOfType::getAncestorClass() const {
	return "ASN1::SEQUENCE_OF";
}


bool SequenceOfType::canReferenceType() const {
	return true;
}


bool SequenceOfType::referencesType(const TypeBase& type) const {
	return baseType->referencesType(type);
}

bool SequenceOfType::useType(const TypeBase& type) const {
	return baseType->useType(type);
}

string  SequenceOfType::getTypeName() const {
	string result("ASN1::SEQUENCE_OF<");

	result += baseType->getTypeName();
	if (constraints.size() ) {
		result +=", ";
		constraints[0]->getConstraint(result);
	}
	result += "> ";

	return result;
}

bool SequenceOfType::forwardDeclareMe(ostream& hdr) {
	//  return baseType->forwardDeclareMe(hdr);
	return false;
}


void SequenceOfType::RemovePERInvisibleConstraint(const ParameterPtr& param) {
	TypeBase::RemovePERInvisibleConstraint(param);
	baseType->RemovePERInvisibleConstraint(param);
}


void SequenceOfType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	hdr  << getIdentifier() << "(size_type n=0) : Inherited(n) {}" << nl
		 << getIdentifier() << "(size_type n, const "<< baseType->getTypeName() << "& val) : Inherited(n, val) {}" << nl
		 << getIdentifier() << "(const_iterator first, const_iterator last) : Inherited(first, last) {}" << nl;
}

TypeBase::RemoveResult SequenceOfType::canRemoveType(const TypeBase& type) {
	return baseType->referencesType(type) ? FORBIDDEN : OK;
}

void SequenceOfType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr   << "static const InfoType theInfo;" << nl;
	cxx << type->getTemplatePrefix()
		<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    " ;

	cxx << "ASN1::SEQUENCE_OF_Base::create," << nl;

	cxx << "    ";

	type->generateTags(cxx);

	cxx << ",\n    0," << nl
		<< "    ";

	const SizeConstraintElement* sizeConstraint ;

	if (type->getConstraints().size()&&
			((sizeConstraint = type->getConstraints()[0]->getSizeConstraint()) != nullptr)) {
		string str;
		sizeConstraint->getConstraint(str);
		cxx << str.substr(0, str.size()-2);
	} else
		cxx << "ASN1::Unconstrained, 0, UINT_MAX";

	cxx << "," << nl
		<< "   &" << baseType->getTypeName() << "::theInfo" << nl
		<< "};\n" << nl;
}

/////////////////////////////////////////////////////////

SetType::SetType()
	: SequenceType(nullptr, false, nullptr, Tag::UniversalSet) {
}


SetType::SetType(SequenceType& seq)
	: SequenceType(seq) {
	defaultTag.number = tag.number = Tag::UniversalSet;
}


const char * SetType::getAncestorClass() const {
	return "ASN1::SET";
}


/////////////////////////////////////////////////////////

SetOfType::SetOfType(TypePtr base, ConstraintPtr constraint)
	: SequenceOfType(base, constraint, Tag::UniversalSet) {
}

string  SetOfType::getTypeName() const {
	string result("ASN1::SET_OF<");
	result += baseType->getTypeName();
	if (constraints.size() ) {
		result +=", ";
		constraints[0]->getConstraint(result);
	}
	result += "> ";
	return result;
}


/////////////////////////////////////////////////////////

ChoiceType::ChoiceType(TypesVector * a_std,
					   bool extendable,
					   TypesVector * extensions)
	: SequenceType(a_std, extendable, extensions, Tag::IllegalUniversalTag) {
	defaultTag.type = Tag::Universal;
	defaultTag.number = 0;
	tag.type = Tag::Universal;
	tag.number = 0;
}


void ChoiceType::generateComponent(TypeBase& field, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, int id) {
	if (field.isRemovedType())
		return;

	string typeName =  field.getTypeName();
	string componentIdentifier = field.getIdentifier();
	string componentName = field.getName();

	// generate component scope class

	hdr << nl  << "class " << componentIdentifier << " {" << nl
		<< "public:" << nl;

	hdr << tab;

	hdr  << "enum Id { eid = " << id << " };" << nl;

	field.setOuterClassName(getClassNameString() + "::" + componentIdentifier);
	field.setName("value_type");

	hdr.precision(hdr.precision() + 4);
	{
		Tag ftag = field.getTag();
		if(ftag.mode==Tag::Explicit)
			field.setTag(field.getDefaultTag());

		field.generateCplusplus(fwd, hdr, cxx, inl);
		field.setTag(ftag);
	}
	hdr.precision(hdr.precision() - 4);

	field.setName(componentName);
	str_replace(componentName,"-","_");

	string primitiveFieldType = field.getPrimitiveType(componentIdentifier);
	string typenameKeyword;

	bool field_has_typename = false;
	if (templatePrefix.length()) {
		typenameKeyword = "typename ";
		if (primitiveFieldType.find(componentIdentifier + "::") != -1) {
			field_has_typename = true;
			primitiveFieldType = "typename " + primitiveFieldType;
		}
	}

	hdr  << "typedef value_type&        reference;" << nl
		 << "typedef const value_type&  const_reference;" << nl
		 << "typedef value_type*        pointer;" << nl
		 << "typedef const value_type*  const_pointer;" << nl;

	hdr << bat;
	hdr   << "}; // end class " << componentIdentifier <<  nl << nl;

	// generate accessor/mutator functions
	if (field.getTypeName() != "ASN1::Null") {
		hdr  << typenameKeyword << componentIdentifier << "::const_reference get_" << componentName << " () const";
		if(noInlineFiles) {
			hdr << nl
				<< "{" << nl
				<< "    assert(currentSelection() == " << componentIdentifier << "::eid);" << nl
				<< "    return *static_cast<" << typenameKeyword  << componentIdentifier << "::const_pointer>(choice.get());" << nl
				<< "}" << nl;
		} else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::const_reference " << getClassNameString() << "::get_" << componentName << " () const" << nl
				<< "{" << nl
				<< "    assert(currentSelection() == " << componentIdentifier << "::eid);" << nl
				<< "    return *static_cast<" << typenameKeyword  << componentIdentifier << "::const_pointer>(choice.get());" << nl
				<< "}\n" << nl;
		}

		hdr  << typenameKeyword << componentIdentifier << "::reference ref_" << componentName << " ()";
		if(noInlineFiles) {
			hdr << nl
				<< "{" << nl
				<< "    assert(currentSelection() == " << componentIdentifier << "::eid);" << nl
				<< "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(choice.get());" << nl
				<< "}" << nl;
		} else {
			hdr << ";" << nl;

			inl << getTemplatePrefix()
				<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::ref_" << componentName << " ()" << nl
				<< "{" << nl
				<< "    assert(currentSelection() == " << componentIdentifier << "::eid);" << nl
				<< "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(choice.get());" << nl
				<< "}\n" << nl;
		}
	}

	hdr  << typenameKeyword << componentIdentifier << "::reference select_" << componentName << " ()";
	if(noInlineFiles) {
		hdr << nl
			<< "{" << nl
			<< "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(setSelection(" << componentIdentifier << "::eid, ASN1::AbstractData::create(&" <<  componentIdentifier << "::value_type::theInfo)));" << nl
			<< "}" << nl;
	} else {
		hdr << ";" << nl;

		inl << getTemplatePrefix()
			<< "inline " << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::select_" << componentName << " ()" << nl
			<< "{" << nl
			<< "    return *static_cast<" << typenameKeyword << componentIdentifier << "::pointer>(setSelection(" << componentIdentifier << "::eid, ASN1::AbstractData::create(&" <<  componentIdentifier << "::value_type::theInfo)));" << nl
			<< "}\n" << nl;
	}

	if (field.getTypeName() != "ASN1::Null") {
		hdr  << typenameKeyword << componentIdentifier << "::reference select_" << componentName << " ("<< primitiveFieldType << " v)";
		if(noInlineFiles)
			hdr << " { return select_" << componentName << "() = v; }" << nl;
		else {
			hdr << ";" << nl;

			int pos;
			if ((pos = primitiveFieldType.find(componentIdentifier, 0)) == 0)
				;;//FIXME  primitiveFieldType.insert(pos, getClassNameString() + "::");

			inl << getTemplatePrefix()
				<< "inline "   << typenameKeyword << getClassNameString() << "::" << componentIdentifier << "::reference " << getClassNameString() << "::select_" << componentName << " ("<< primitiveFieldType << " v)" << nl
				<< "{" << nl
				<< "    return select_" << componentName << "() = v;" << nl
				<< "}\n" << nl;
		}
	}

	hdr  << "bool " << componentName << "_isSelected() const";
	if(noInlineFiles)
		hdr << " { return currentSelection() == " << componentIdentifier << "::eid; }\n" << nl;
	else {
		hdr << ";\n" << nl;

		inl << getTemplatePrefix()
			<< "inline bool " << getClassNameString() << "::" << componentName << "_isSelected() const" << nl
			<< "{" << nl
			<< "    return currentSelection() == " << componentIdentifier << "::eid;" << nl
			<< "}\n" << nl;
	}

	// generate alternative constructors

	hdr  << getIdentifier() << '(' << typenameKeyword << componentIdentifier << "::Id id, ";
	if(!field_has_typename)
		hdr << typenameKeyword;
	hdr << primitiveFieldType << " v)";
	if(noInlineFiles) {
		hdr << nl
			<< "   : Inherited(&theInfo, id, new "  << typenameKeyword << componentIdentifier << "::value_type(v))"
			<< " {}" << nl;
	} else {
		hdr << ";" << nl;

		inl << getTemplatePrefix()
			<< "inline " << getClassNameString() << "::" << getIdentifier() << "("  << typenameKeyword << componentIdentifier << "::Id id, ";
		if(!field_has_typename)
			inl << typenameKeyword;
		inl << primitiveFieldType << " v)" << nl
			<< " : Inherited(&theInfo, id, new "  << typenameKeyword << componentIdentifier << "::value_type(v))"
			<< " {}\n" << nl;
	}
}

bool CompareTag(TypeBase* lhs, TypeBase* rhs) {
	const Tag lTag = lhs->getTag(), rTag = rhs->getTag();
	return ( lTag.type != rTag.type ) ? (lTag.type < rTag.type) : (lTag.number < rTag.number);
}

void ChoiceType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	size_t i;
	size_t nFields = fields.size();

	TypeBase::beginGenerateCplusplus(fwd, hdr, cxx, inl);

	sortedFields.clear();
	sortedFields.reserve(nFields);
	for (i = 0; i < nFields; ++i)
		sortedFields.push_back(fields[i].get());

	// sorting fields breaks choice
//  if (getTag().mode != Tag::Automatic)
//    sort(sortedFields.begin(), sortedFields.end(), CompareTag);

	hdr  << "enum Choice_Ids {";
	hdr << tab;

	hdr << nl  << "unknownSelection_ = -2,"
		<< nl  << "unselected_ = -1";

	for(i = 0; i < nFields; ++i) {
		const string& sfIdentifier = sortedFields[i]->getIdentifier();
		if(sfIdentifier.empty())
			continue;

		hdr << "," << nl  << sfIdentifier << i << " = " << i;
	}
	hdr << bat;

	hdr << nl  << "};" << nl;

	// generate code for type safe cast operators of selected choice object
	bool needExtraLine = false;

	hdr.precision(hdr.precision()+4);
	stringstream tmpcxx;
	for (i = 0; i < nFields; i++) {
		generateComponent(*sortedFields[i], fwd, hdr, tmpcxx, inl, i);
	}

	if (needExtraLine)
		hdr << nl;

	hdr.precision(hdr.precision()-4);

	//generateIds(hdr);

	string typenameKeyword;
	if (templatePrefix.length())
		typenameKeyword = "typename ";

	endGenerateCplusplus(fwd, hdr, cxx, inl);
	cxx << tmpcxx.str();
	tmpcxx << ends;

	isgenerated = true;
}

void ChoiceType::generateOperators(ostream& fwd, ostream& hdr, ostream& , const TypeBase& ) {
	// generate Copy Constructor, Assignment operator, swap

	hdr  << getIdentifier() << "(const " << getIdentifier() << "& that) : Inherited(that) {}" << nl << nl;
	hdr  << getIdentifier() << "& operator=(const " << getIdentifier() << "& that) { Inherited::operator=(that); return *this; }\n" << nl;
	hdr  << "void swap(" << getIdentifier() << "& that) { Inherited::swap(that); }\n" << nl;
}


bool ChoiceType::isPrimitiveType() const {
	return false;
}


bool ChoiceType::isChoice() const {
	return true;
}

TypePtr ChoiceType::flattenThisType(TypePtr& self, const TypeBase& parent) {
	TypePtr ret(new DefinedType(self, parent));
	ret->setTag(Tag());
	return ret;
}


const char * ChoiceType::getAncestorClass() const {
	return "ASN1::CHOICE";
}


TypeBase::RemoveResult ChoiceType::canRemoveType(const TypeBase& type) {
	for (size_t i =0; i < fields.size(); ++i) {
		if (fields[i]->canRemoveType(type) == FORBIDDEN )
			return FORBIDDEN;
	}
	return OK;
}

void ChoiceType::generateInfo(const TypeBase* type,ostream& fwd, ostream& hdr, ostream& cxx) {
	size_t nFields = fields.size();

	hdr   << "static const InfoType theInfo;" << nl
		  << bat << "private:" << nl << tab;

	bool autoTag = true;
	// generate selection info table
	if (type == this) {
		hdr  << "static const void* selectionInfos[" << nFields << "];" << nl;
		cxx << type->getTemplatePrefix()
			<< "const void* " << type->getClassNameString() << "::selectionInfos[" << nFields << "] = {" << nl;

		size_t i;
		for (i = 0; i < nFields; i++) {
			const Tag& fieldTag = sortedFields[i]->getTag();
			if (fieldTag.mode != Tag::Automatic&&
					!( fieldTag.number == (unsigned)i&&
					   fieldTag.type == Tag::ContextSpecific&&
					   fieldTag.mode == Tag::Implicit)) {
				autoTag = false;
			}

			if (sortedFields[i]->isRemovedType())
				cxx << "    nullptr";
			else
				cxx << "   &" <<  sortedFields[i]->getIdentifier()
					<< "::value_type::theInfo";
			if (i != nFields-1)
				cxx << "," << nl;
		}

		cxx  << nl
			 << "};\n" << nl;

		// generate tag list for BER decoding

		if (!autoTag) {
			hdr  << "static unsigned selectionTags[" << nFields << "];" << nl;
			cxx << getTemplatePrefix()
				<< "unsigned " << getClassNameString() << "::selectionTags[" << nFields << "] = {" << nl;

			for (i = 0; i < nFields; i++) {
				cxx << "    ";
				sortedFields[i]->generateTags(cxx);
				if (i != nFields-1)
					cxx << "," << nl;
				else
					cxx << nl;
			}
			cxx << "};\n" << nl;
		}

		hdr  << "static const char* selectionNames[" << nFields << "];" << nl;

		cxx << getTemplatePrefix()
			<< "const char* " << getClassNameString() << "::selectionNames[" << nFields << "] = {" << nl;

		for (i = 0; i < nFields; i++) {
			cxx << "    \"" << sortedFields[i]->getName();

			if (i != nFields-1)
				cxx << "\"," << nl;
			else
				cxx << "\"" << nl;
		}
		cxx << "};\n" << nl;
	}

	string typenameKeyword;
	if (type->getTemplatePrefix().length())
		typenameKeyword="typename ";

	cxx << type->getTemplatePrefix()
		<< "const " << typenameKeyword << type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    ASN1::CHOICE::create," << nl
		<< "    ";
	type->generateTags(cxx);
	cxx << ",\n    0," << nl;

	if (extendable)
		cxx << "    true," << nl;
	else
		cxx << "    false," << nl;

	if (type == this)
		cxx << "    " << getClassNameString() << "::selectionInfos," << nl;
	else
		cxx << "    " << getIdentifier() << "::theInfo.selectionInfos," << nl;

	cxx << "    " <<  numFields << ", " << nFields << "," << nl
		<< "    ";

	if (!autoTag) {
		if (type == this)
			cxx <<   getClassNameString() << "::selectionTags," << nl;
		else
			cxx << getIdentifier() << "::theInfo.tags," << nl;
	} else
		cxx << "nullptr," << nl;

	if (type == this)
		cxx << "    " << getClassNameString() << "::selectionNames" << nl;
	else
		cxx << "    " << getIdentifier() << "::theInfo.names" << nl;

	cxx << "};\n" << nl;
}

/////////////////////////////////////////////////////////

EmbeddedPDVType::EmbeddedPDVType()
	: TypeBase(Tag::UniversalEmbeddedPDV, contexts.top()->Module) {
}


const char * EmbeddedPDVType::getAncestorClass() const {
	return "ASN1::EMBEDED_PDV";
}


/////////////////////////////////////////////////////////

ExternalType::ExternalType()
	: TypeBase(Tag::UniversalExternalType, contexts.top()->Module) {
}


const char * ExternalType::getAncestorClass() const {
	return "ASN1::EXTERNAL";
}


/////////////////////////////////////////////////////////

AnyType::AnyType(const string& ident)
	: TypeBase(Tag::UniversalExternalType, contexts.top()->Module) {
	identifier = ident;
}


void AnyType::printOn(ostream& strm) const {
	PrintStart(strm);
	if (identifier.size())
		strm << "Defined by " << identifier;
	PrintFinish(strm);
}


const char * AnyType::getAncestorClass() const {
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
	: TypeBase(tag, contexts.top()->Module) {
}


string StringTypeBase::getTypeName() const {
	return getAncestorClass();
}

void StringTypeBase::generateConstructors(ostream& fwd, ostream& hdr, ostream& , ostream& ) {
	hdr  << getIdentifier() << "() : Inherited(&theInfo) {}" << nl
		 << getIdentifier() << "(const base_string& str, const void* info =&theInfo) : Inherited(str, info) {}" << nl
		 << getIdentifier() << "(const char* str, const void* info =&theInfo) : Inherited(str, info) {}" << nl;
}

void StringTypeBase::generateOperators(ostream& fwd, ostream& hdr, ostream& , const TypeBase& actualType) {
	string atname = actualType.getIdentifier();
	hdr  << atname << "& operator=(const ASN1_STD string& that)" << nl
		 << "{ Inherited::operator=(that); return *this; }" << nl
		 << atname << "& operator=(const char* that)" << nl
		 << "{ Inherited::operator=(that); return *this; }" << nl;
}

bool StringTypeBase::needGenInfo() const {
	return TypeBase::needGenInfo() || constraints.size() > 0;
}

static size_t CountBits(unsigned range) {
	if (range == 0)
		return sizeof(unsigned)*8;

	size_t nBits = 0;
	while (nBits < (sizeof(unsigned)*8)&& range > (unsigned)(1 << nBits))
		nBits++;
	return nBits;
}


void StringTypeBase::generateInfo(const TypeBase* type,ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr  << "static const InfoType theInfo;" << nl;
	cxx << type->getTemplatePrefix()
		<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    ASN1::AbstractString::create," << nl
		<< "    ";
	type->generateTags(cxx);
	cxx << "," << nl << "   &" << getAncestorClass() << "::theInfo," << nl;

	const SizeConstraintElement* sizeConstraint = nullptr;
	size_t i;
	for (i = 0; i < type->getConstraints().size(); ++i)
		if ((sizeConstraint = type->getConstraints()[i]->getSizeConstraint()) != nullptr)
			break;

	if (sizeConstraint != nullptr) {
		string str;
		sizeConstraint->getConstraint(str);
		cxx << "    " << str.substr(0, str.size()-2) << "," << nl;
	} else {
		cxx << "    ASN1::Unconstrained," << nl;
		cxx << "    0," << nl;
		cxx << "    UINT_MAX," << nl;
	}

	const FromConstraintElement* fromConstraint = nullptr;
	for (i = 0; i < type->getConstraints().size(); ++i)
		if ((fromConstraint = type->getConstraints()[i]->getFromConstraint()) != nullptr)
			break;

	cxx << "    ";

	int charSetUnalignedBits;
	string characterSet;
	if (fromConstraint != nullptr &&
			(characterSet = fromConstraint->getCharacterSet(canonicalSet, canonicalSetSize)).size()) {
		cxx << '"';
		for (size_t i = 0; i < characterSet.size(); ++i) {
			if (isprint(characterSet[i])&& characterSet[i] != '"')
				cxx << characterSet[i];
			else
				cxx << "\\x" << hex << (unsigned) characterSet[i];
		}

		cxx << "\", " << characterSet.size()  << "," << nl;
		charSetUnalignedBits = CountBits(characterSet.size());

	} else {
		if (canonicalSetRep) {
			cxx << canonicalSetRep << "," << nl;
			cxx << canonicalSetSize << "," << nl;
			charSetUnalignedBits = CountBits(canonicalSetSize);
		} else {
			cxx << "    " << "nullptr," << nl;
			cxx << "    " << "0," << nl;
			charSetUnalignedBits = CountBits(0);
		}
	}
	

	cxx <<  "    " << CountBits(canonicalSetSize) << "," << nl;


	int charSetAlignedBits = 1;
	while (charSetUnalignedBits > charSetAlignedBits)
		charSetAlignedBits <<= 1;

	cxx << "    " << charSetUnalignedBits << ", " << nl;
	cxx << "    " << charSetAlignedBits  << nl;
	cxx	<< "};" << nl << nl;
}

/////////////////////////////////////////////////////////

UTF8StringType::UTF8StringType()
	: StringTypeBase(Tag::UniversalUTF8String) {
}


const char * UTF8StringType::getAncestorClass() const {
	return "ASN1::UTF8String";
}

void UTF8StringType::generateConstructors(ostream& fwd, ostream& hdr, ostream& , ostream& ) {

	hdr  << getIdentifier() << "() : Inherited(&theInfo) {}" << nl
		 << getIdentifier() << "(const base_string& str, const void* info =&theInfo) : Inherited(str, info) {}" << nl
		 << getIdentifier() << "(const wchar_t* str, const void* info =&theInfo) : Inherited(str, info) {}" << nl;
}

void UTF8StringType::generateOperators(ostream& fwd, ostream& hdr, ostream& , const TypeBase& actualType) {
	string atname = actualType.getIdentifier();
	hdr  << atname << "& operator=(const std::wstring& that)" << nl
		 << "{ Inherited::operator=(that); return *this; }" << nl
		 << atname << "& operator=(const wchar_t* that)" << nl
		 << "{ Inherited::operator=(that); return *this; }" << nl;
}

void UTF8StringType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr  << "static const InfoType theInfo;" << nl;
	cxx << type->getTemplatePrefix()
		<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    ASN1::BMPString::create," << nl
		<< "    ";
	generateTags(cxx);
	cxx << ",\n   &" << getAncestorClass() << "::theInfo," << nl;

	const SizeConstraintElement* sizeConstraint = nullptr;
	size_t i;
	for (i = 0; i < type->getConstraints().size(); ++i)
		if ((sizeConstraint = type->getConstraints()[i]->getSizeConstraint()) != nullptr)
			break;

	if (sizeConstraint != nullptr) {
		string str;
		sizeConstraint->getConstraint(str);
		cxx << "    " << str.substr(0, str.size()-2) << "," << nl;
	} else
		cxx << "    ASN1::Unconstrained, 0, UINT_MAX," << nl;

	const FromConstraintElement* fromConstraint = nullptr;
	for (i = 0; i < type->getConstraints().size(); ++i)
		if ((fromConstraint = type->getConstraints()[i]->getFromConstraint()) != nullptr)
			break;

	cxx << "    ";

	int range = 0xffff;
	if (fromConstraint != nullptr)
		range = fromConstraint->getRange(cxx);
	else
		cxx <<  0 << ", " << 0xffff ;

	int charSetUnalignedBits = CountBits(range);

	int charSetAlignedBits = 1;
	while (charSetUnalignedBits > charSetAlignedBits)
		charSetAlignedBits <<= 1;

	cxx << "," << nl
		<< "    " << charSetUnalignedBits << ", " << charSetAlignedBits  << nl
		<< "};\n" << nl;
}

/////////////////////////////////////////////////////////

BMPStringType::BMPStringType()
	: StringTypeBase(Tag::UniversalBMPString) {
}


const char * BMPStringType::getAncestorClass() const {
	return "ASN1::BMPString";
}

void BMPStringType::generateConstructors(ostream& fwd, ostream& hdr, ostream& , ostream& ) {

	hdr  << getIdentifier() << "() : Inherited(&theInfo) {}" << nl
		 << getIdentifier() << "(const base_string& str, const void* info =&theInfo) : Inherited(str, info) {}" << nl
		 << getIdentifier() << "(const wchar_t* str, const void* info =&theInfo) : Inherited(str, info) {}" << nl;
}

void BMPStringType::generateOperators(ostream& fwd, ostream& hdr, ostream& , const TypeBase& actualType) {
	string atname = actualType.getIdentifier();
	hdr  << atname << "& operator=(const std::wstring& that)" << nl
		 << "{ Inherited::operator=(that); return *this; }" << nl
		 << atname << "& operator=(const wchar_t* that)" << nl
		 << "{ Inherited::operator=(that); return *this; }" << nl;
}

void BMPStringType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	hdr  << "static const InfoType theInfo;" << nl;
	cxx << type->getTemplatePrefix()
		<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
		<< "    ASN1::BMPString::create," << nl
		<< "    ";
	generateTags(cxx);
	cxx << ",\n   &" << getAncestorClass() << "::theInfo," << nl;

	const SizeConstraintElement* sizeConstraint = nullptr;
	size_t i;
	for (i = 0; i < type->getConstraints().size(); ++i)
		if ((sizeConstraint = type->getConstraints()[i]->getSizeConstraint()) != nullptr)
			break;

	if (sizeConstraint != nullptr) {
		string str;
		sizeConstraint->getConstraint(str);
		cxx << "    " << str.substr(0, str.size()-2) << "," << nl;
	} else
		cxx << "    ASN1::Unconstrained, 0, UINT_MAX," << nl;

	const FromConstraintElement* fromConstraint = nullptr;
	for (i = 0; i < type->getConstraints().size(); ++i)
		if ((fromConstraint = type->getConstraints()[i]->getFromConstraint()) != nullptr)
			break;

	cxx << "    ";

	int range = 0xffff;
	if (fromConstraint != nullptr)
		range = fromConstraint->getRange(cxx);
	else
		cxx <<  0 << ", " << 0xffff ;

	int charSetUnalignedBits = CountBits(range);

	int charSetAlignedBits = 1;
	while (charSetUnalignedBits > charSetAlignedBits)
		charSetAlignedBits <<= 1;

	cxx << "," << nl
		<< "    " << charSetUnalignedBits << ", " << charSetAlignedBits  << nl
		<< "};\n" << nl;
}

/////////////////////////////////////////////////////////

GeneralStringType::GeneralStringType()
	: StringTypeBase(Tag::UniversalGeneralString) {
	canonicalSet = GeneralStringSet;
	canonicalSetRep = "ASN1::GeneralString::theInfo.characterSet";
	canonicalSetSize = sizeof(GeneralStringSet)-1;
}


const char * GeneralStringType::getAncestorClass() const {
	return "ASN1::GeneralString";
}


/////////////////////////////////////////////////////////

GraphicStringType::GraphicStringType()
	: StringTypeBase(Tag::UniversalGraphicString) {
}


const char * GraphicStringType::getAncestorClass() const {
	return "ASN1::GraphicString";
}


/////////////////////////////////////////////////////////

IA5StringType::IA5StringType()
	: StringTypeBase(Tag::UniversalIA5String) {
	canonicalSet =IA5StringSet;
	canonicalSetRep = "ASN1::IA5String::theInfo.characterSet";
	canonicalSetSize = sizeof(IA5StringSet)-1;
}


const char * IA5StringType::getAncestorClass() const {
	return "ASN1::IA5String";
}


/////////////////////////////////////////////////////////

ISO646StringType::ISO646StringType()
	: StringTypeBase(Tag::UniversalVisibleString) {
}


const char * ISO646StringType::getAncestorClass() const {
	return "ASN1::ISO646String";
}


/////////////////////////////////////////////////////////

NumericStringType::NumericStringType()
	: StringTypeBase(Tag::UniversalNumericString) {
	canonicalSet =NumericStringSet;
	canonicalSetRep = "ASN1::NumericString::theInfo.characterSet";
	canonicalSetSize = sizeof(NumericStringSet)-1;
}


const char * NumericStringType::getAncestorClass() const {
	return "ASN1::NumericString";
}


/////////////////////////////////////////////////////////

PrintableStringType::PrintableStringType()
	: StringTypeBase(Tag::UniversalPrintableString) {
	canonicalSet =PrintableStringSet;
	canonicalSetRep = "ASN1::PrintableString::theInfo.characterSet";
	canonicalSetSize = sizeof(PrintableStringSet)-1;
}


const char * PrintableStringType::getAncestorClass() const {
	return "ASN1::PrintableString";
}


/////////////////////////////////////////////////////////

TeletexStringType::TeletexStringType()
	: StringTypeBase(Tag::UniversalTeletexString) {
}


const char * TeletexStringType::getAncestorClass() const {
	return "ASN1::TeletexString";
}


/////////////////////////////////////////////////////////

T61StringType::T61StringType()
	: StringTypeBase(Tag::UniversalTeletexString) {
}


const char * T61StringType::getAncestorClass() const {
	return "ASN1::T61String";
}


/////////////////////////////////////////////////////////

UniversalStringType::UniversalStringType()
	: StringTypeBase(Tag::UniversalUniversalString) {
}


const char * UniversalStringType::getAncestorClass() const {
	return "ASN1::UniversalString";
}


/////////////////////////////////////////////////////////

VideotexStringType::VideotexStringType()
	: StringTypeBase(Tag::UniversalVideotexString) {
}


const char * VideotexStringType::getAncestorClass() const {
	return "ASN1::VideotexString";
}


/////////////////////////////////////////////////////////

VisibleStringType::VisibleStringType()
	: StringTypeBase(Tag::UniversalVisibleString) {
	canonicalSet =VisibleStringSet;
	canonicalSetRep = "ASN1::VisibleString::theInfo.characterSet";
	canonicalSetSize = sizeof(VisibleStringSet)-1;
}


const char * VisibleStringType::getAncestorClass() const {
	return "ASN1::VisibleString";
}


/////////////////////////////////////////////////////////

UnrestrictedCharacterStringType::UnrestrictedCharacterStringType()
	: StringTypeBase(Tag::UniversalUniversalString) {
}


const char * UnrestrictedCharacterStringType::getAncestorClass() const {
	return "ASN1::UnrCHARACTOR_STRING";
}


/////////////////////////////////////////////////////////

GeneralizedTimeType::GeneralizedTimeType()
	: TypeBase(Tag::UniversalGeneralisedTime, contexts.top()->Module) {
}


const char * GeneralizedTimeType::getAncestorClass() const {
	return "ASN1::GeneralizedTime";
}

void GeneralizedTimeType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	TypeBase::generateConstructors(fwd, hdr, cxx, inl);

	hdr << getIdentifier() << "(const char* v) : Inherited(v) {  }" << nl
		<< getIdentifier() << "(int year, int month, int day," << nl;
	hdr << tab;
	hdr << "int hour = 0, int minute=0, int second=0," << nl
		<< "int millisec = 0, int mindiff = 0, bool utc = false)" << nl
		<< ": Inherited(year, month, day, hour, minute, second, milliseec, mindiff, utc)"
		<< " {}" << nl ;
	hdr << bat;
}


/////////////////////////////////////////////////////////

UTCTimeType::UTCTimeType()
	: TypeBase(Tag::UniversalUTCTime, contexts.top()->Module) {
}


const char * UTCTimeType::getAncestorClass() const {
	return "ASN1::UTCTime";
}


void UTCTimeType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	TypeBase::generateConstructors(fwd, hdr, cxx, inl);

	hdr << getIdentifier() << "(const char* v) : Inherited(v) {  }" << nl
		<< getIdentifier() << "(int year, int month, int day," << nl;
	hdr << tab;
	hdr << "int hour = 0, int minute=0, int second=0," << nl
		<< "int mindiff = 0, bool utc = false)" << nl
		<< ": Inherited(year, month, day, hour, minute, second, mindiff, utc)"
		<< " {}" << nl;
	hdr << bat;
}

/////////////////////////////////////////////////////////

ObjectDescriptorType::ObjectDescriptorType()
	: TypeBase(Tag::UniversalObjectDescriptor, contexts.top()->Module) {
}


const char * ObjectDescriptorType::getAncestorClass() const {
	return "ASN1::ObjectDescriptor";
}


/////////////////////////////////////////////////////////

RelativeOIDType::RelativeOIDType()
	: TypeBase(Tag::UniversalRelativeOID, contexts.top()->Module) {
}


void RelativeOIDType::beginParseThisTypeValue() const {
	contexts.top()->InOIDContext = true;
	contexts.top()->BraceTokenContext = OID_BRACE;
}

void RelativeOIDType::endParseThisTypeValue() const {
	contexts.top()->InOIDContext = false;
	contexts.top()->BraceTokenContext = '{'; // TODO: was OID_BRACE, is this correct?;
}

const char * RelativeOIDType::getAncestorClass() const {
	return "ASN1::RELATIVE_OID";
}

void RelativeOIDType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	TypeBase::generateConstructors(fwd, hdr, cxx, inl);
	hdr << "template <class Iterator>" << nl;
	hdr	<< getIdentifier() << "(Iterator first, Iterator last, const void* info =&theInfo)" << nl
		<< ": Inherited(first, last, info) {}" << nl;
}

/////////////////////////////////////////////////////////

ObjectIdentifierType::ObjectIdentifierType()
	: TypeBase(Tag::UniversalObjectId, contexts.top()->Module) {
}


void ObjectIdentifierType::beginParseThisTypeValue() const {
	contexts.top()->InOIDContext = true;
	contexts.top()->BraceTokenContext = OID_BRACE;
}

void ObjectIdentifierType::endParseThisTypeValue() const {
	contexts.top()->InOIDContext = false;
	contexts.top()->BraceTokenContext = '{'; // TODO: was OID_BRACE, is this correct?;
}

const char * ObjectIdentifierType::getAncestorClass() const {
	return "ASN1::OBJECT_IDENTIFIER";
}


void ObjectIdentifierType::generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	TypeBase::generateConstructors(fwd, hdr, cxx, inl);
	hdr << "template <class Iterator>" << nl;
	hdr	<< getIdentifier() << "(Iterator first, Iterator last, const void* info =&theInfo)" << nl
		<< ": Inherited(first, last, info) {}" << nl;
}

/////////////////////////////////////////////////////////

ObjectClassFieldType::ObjectClassFieldType(ObjectClassBasePtr  objclass,
		const string& field)
	: TypeBase(Tag::IllegalUniversalTag, contexts.top()->Module),
	  asnObjectClass(objclass),
	  asnObjectClassField(field) {
}

ObjectClassFieldType::~ObjectClassFieldType() {
}

const char * ObjectClassFieldType::getAncestorClass() const {
	return "";
}


void ObjectClassFieldType::printOn(ostream& strm) const {
	PrintStart(strm);
	strm << asnObjectClass->getName() << '.' << asnObjectClassField;
	PrintFinish(strm);
}



bool ObjectClassFieldType::canReferenceType() const {
	return true;
}


bool ObjectClassFieldType::referencesType(const TypeBase& type) const {
	const TypeBase* actualType = getFieldType();
	if (actualType&& actualType->referencesType(type)) // not open type
		return true;
	else { // open type , check if it is constrained

		if (find_if(constraints.begin(), constraints.end(),
					boost::bind(&Constraint::referencesType, _1, boost::cref(type) ) ) != constraints.end())
			return true;

		if (tableConstraint.get())
			return tableConstraint->ReferenceType(type);
	}
	return false;
}

TypeBase* ObjectClassFieldType::getFieldType() {
	return asnObjectClass->getFieldType(asnObjectClassField);
}

const TypeBase* ObjectClassFieldType::getFieldType() const {
	return asnObjectClass->getFieldType(asnObjectClassField);
}

string ObjectClassFieldType::getConstrainedTypeName() const {
	if (hasConstraints()) {
		const SubTypeConstraintElement* cons = constraints[0]->getSubTypeConstraint();
		if (cons)
			return cons->getSubTypeName();
	}
	return string();
}

string ObjectClassFieldType::getTypeName() const {
	const TypeBase* type = getFieldType();
	string result;
	if (type)
		return type->getTypeName();
	else if ( (result = getConstrainedTypeName()).size() )
		return  string("ASN1::Constrained_openData<") + result + string("> ");

	return string("ASN1::OpenData");

}

void ObjectClassFieldType::addTableConstraint(boost::shared_ptr<TableConstraint> constraint) {
	tableConstraint = constraint;
}

void ObjectClassFieldType::generateDecoder(ostream& cxx) {
	if (tableConstraint.get() && tableConstraint->getAtNotations() && tableConstraint->getAtNotations()->size() == 1) {

		if (isOptional()) {
			cxx  << "if (" << getName() << "_isPresent())" << "{" << nl;
		}

		const StringList& lst = *(tableConstraint->getAtNotations());
		string keyname = lst[0];
		if (keyname[0]== '.')
			keyname = keyname.substr(1);

		string fieldIdentifier = asnObjectClassField.substr(1);
		cxx << tableConstraint->getObjectSetIdentifier() << " objSet(*visitor.get_env());" << nl;

		string cppkeyname = makeCppName(keyname);
		string cpprefname = makeCppName(getName());
		cxx	 << "if (objSet.get()) {" << nl
			 << "    if (objSet->count(get_" << cppkeyname << "())) {" << nl
			 << "        ref_" << cpprefname << "().grab(objSet->find(get_" <<  cppkeyname
			 << "())->get_" << fieldIdentifier << "());" << nl
			 << "        return visitor.revisit(ref_" << cpprefname << "());" << nl
			 << "    } else" << nl
			 << "        return objSet.extensible();" << nl
			 << "} else" << nl
			 << "  return true;" << nl;

		if (isOptional()) {
			cxx  << "}" << nl;
		}
	}
}

void ObjectClassFieldType::generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx) {
	string constrainedType = getConstrainedTypeName();
	if (constrainedType.size()) {
		hdr  << "static const InfoType theInfo;" << nl;
		cxx << getTemplatePrefix()
			<< "const "<< type->getClassNameString() << "::InfoType " <<  type->getClassNameString() << "::theInfo = {" << nl
			<< "    TypeConstrainedopenData::create," << nl
			<< "    ";

		type->generateTags(cxx);
		cxx << ",\n   &" << getAncestorClass() << "::theInfo," << nl
			<< "   &" << constrainedType << "::theInfo;" << nl
			<< "};\n" << nl;
	} else {
		TypeBase::generateInfo(type, fwd, hdr, cxx);
	}
}
/////////////////////////////////////////////////////////

ImportedType::ImportedType(const string& theName, bool param)
	: TypeBase(Tag::IllegalUniversalTag, contexts.top()->Module) {
	identifier = name = theName;
	parameterised = param;
}

ImportedType::ImportedType(const TypePtr& ref)
	: TypeBase(Tag::IllegalUniversalTag, contexts.top()->Module), reference(ref) {
	identifier = name = ref->getName();
	parameterised = ref->hasParameters();
}


const char * ImportedType::getAncestorClass() const {
	return identifier.c_str();
}


void ImportedType::adjustIdentifier(bool usingNamespace) {
	if (usingNamespace)
		identifier = cppModuleName + "::" + makeCppName(name);
	else
		identifier = modulePrefix + '_' + makeCppName(name);
}


void ImportedType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
}


void ImportedType::setModuleName(const string& mname) {
	moduleName = mname;
	cppModuleName = makeCppName(mname);
	modulePrefix = contexts.top()->Module->getImportModuleName(mname);
}



bool ImportedType::isParameterisedImport() const {
	return parameterised;
}

void ImportedType::beginParseThisTypeValue() const {
	if (reference.get())
		reference->beginParseThisTypeValue();
	else
		// used when this type is an INTEGER and the subsequent value is a NamedNumber.
		contexts.top()->IdentifierTokenContext = VALUEREFERENCE;
}

void ImportedType::endParseThisTypeValue() const {
	if (reference.get())
		reference->endParseThisTypeValue();
	else
		contexts.top()->IdentifierTokenContext = IDENTIFIER;
}

bool ImportedType::isPrimitiveType() const {
	if (reference.get())
		return reference->isPrimitiveType();
	else
		return false;
}


/////////////////////////////////////////////////////////
TypeFromObject::TypeFromObject(InformationObjectPtr  obj, const string& fld)
	: TypeBase(Tag::IllegalUniversalTag, contexts.top()->Module)
	,refObj(obj)
	,field(fld) {
}

TypeFromObject::~TypeFromObject() {
}

const char * TypeFromObject::getAncestorClass() const {
	return nullptr;
}

void TypeFromObject::printOn(ostream& strm) const {
	PrintStart(strm);
	strm << *refObj << "." << field;
	PrintFinish(strm);
}

void TypeFromObject::generateCplusplus(ostream& fwd, ostream& hdr, ostream&, ostream&) {
	if (hdr.precision() == 0) {
		hdr << "//2" << nl
			<< "// " << getName()  << nl
			<< "//" << nl
			<< nl;
	}

	hdr << "typedef " << "PASN_openType" << ' ' << getIdentifier() << ";" << nl	 << nl << nl;
}
///////////////////////////////////////////////////////

RemovedType::RemovedType(const TypeBase& type)
	: TypeBase(type.getTag().number, type.getModule()) {
	isoptional = true;
}

const char * RemovedType::getAncestorClass() const {
	return "";
}

/////////////////////////////////////////////////////////

void ValueBase::setValueName(const string& name) {
	valueName = name;
}


void ValueBase::PrintBase(ostream& strm) const {
	if (!valueName.empty())
		strm << nl << valueName << " ::= ";
}


void ValueBase::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cerr << StdError(Warning) << "unsupported value type." << endl;
}

void ValueBase::generateConst(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cerr << StdError(Warning) << "unsupported const value type." << endl;
}

/////////////////////////////////////////////////////////

DefinedValue::DefinedValue(const string& name)
	: referenceName(name) {
	unresolved = true;
}

DefinedValue::DefinedValue(const ValuePtr& base)
	: actualValue(base), unresolved(false) {
}

DefinedValue::DefinedValue(const string& name, const ValuePtr& base)
	: actualValue(base), unresolved(false) {
	referenceName = name;
}


void DefinedValue::printOn(ostream& strm) const {
	PrintBase(strm);
	if (referenceName != "")
		strm << referenceName;
	else
		strm << *actualValue;
}


void DefinedValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	if (unresolved) {
		unresolved = false;
		actualValue = contexts.top()->Module->findValue(referenceName);
	}

	if (actualValue.get() != nullptr)
		actualValue->generateCplusplus(fwd, hdr, cxx, inl);
	else
		cxx << referenceName;
}


/////////////////////////////////////////////////////////

BooleanValue::BooleanValue(bool newVal) {
	value = newVal;
}


void BooleanValue::printOn(ostream& strm) const {
	PrintBase(strm);
	strm << (value ? "true" : "false");
}


void BooleanValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << (value ? "true" : "false");
}


/////////////////////////////////////////////////////////

IntegerValue::IntegerValue(boost::int64_t newVal) {
	value = newVal;
}

#if defined(_MSC_VER)&& (_MSC_VER <=1200)

ostream& operator << (ostream& os, __int64 n) {
	char str[40];
	os << _i64toa(n, str, 10);
	return os;
}

#endif

void IntegerValue::printOn(ostream& strm) const {
	PrintBase(strm);

	strm << value;
	if (value > INT_MAX)
		strm << 'U';
}


void IntegerValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << value;
	if (value > INT_MAX)
		cxx << 'U';
}

void IntegerValue::generateConst(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	if(!getName().empty()) {
		string work = getName();
		str_replace(work, "-", "_");
		hdr << "extern const ASN1::INTEGER " << work << ";\n" << nl;
		cxx << "const ASN1::INTEGER " << work << '(' << value;

		if (value > INT_MAX)
			cxx << 'U';

		cxx << ");\n" << nl;
	}
}


/////////////////////////////////////////////////////////

RealValue::RealValue(double newVal) {
	value = newVal;
}


/////////////////////////////////////////////////////////

OctetStringValue::OctetStringValue(const string& newVal) {
	value.assign(newVal.begin(), newVal.end());
}


/////////////////////////////////////////////////////////

BitStringValue::BitStringValue(const string& newVal) {
	value = newVal;
}


BitStringValue::BitStringValue(StringList * newVal) {
	value = '{';

	for(StringList::iterator i = newVal->begin(); i!=newVal->end(); ++i) {
		if(i!=newVal->begin())
			value+=',';
		(value+=' ').append(*i);
	}

	value.append(" }");

	delete newVal;
}

void BitStringValue::printOn(ostream&os) const {
	os << value;
}


/////////////////////////////////////////////////////////

CharacterValue::CharacterValue(char c) {
	value = c;
}


CharacterValue::CharacterValue(char t1, char t2) {
	value = (t1<<8) + t2;
}


CharacterValue::CharacterValue(char q1, char q2, char q3, char q4) {
	value = (q1<<24) + (q2<<16) + (q3<<8) + q4;
}


void CharacterValue::printOn(ostream& strm) const {
	strm << "'\\x" << hex << value << '\'' << dec;
}


void CharacterValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << value;
}


/////////////////////////////////////////////////////////

CharacterStringValue::CharacterStringValue(const string& newVal) {
	value = newVal;
}


CharacterStringValue::CharacterStringValue(StringList& newVal) {
	accumulate(newVal.begin(), newVal.end(), value);
}


void CharacterStringValue::printOn(ostream& strm) const {
	if (value[0] == '\"'&& value[2] == '\"')
		strm << '\'' << value[1] << '\'';
	else
		strm << value;
}


void CharacterStringValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << value;
}

void CharacterStringValue::getValue(string& v) const {
	v = value.substr(1, value.size()-2);
}


/////////////////////////////////////////////////////////



ObjectIdentifierValue::ObjectIdentifierValue(ObjIdComponentList& newVal) {
	value.swap(newVal);
}


void ObjectIdentifierValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	ObjIdComponentList::const_iterator it;
	for (it = getComponents().cbegin(); it != getComponents().cend(); ++it) {
		cxx << it->getNumberForm().getNumber();
		if (distance(getComponents().cbegin(), it) + 1< getComponents().size())
			cxx << ", ";
	}
}

void ObjectIdentifierValue::generateConst(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	string cppName = makeCppName(getName());
	hdr << "extern " << dllMacroAPI << " const ASN1::OBJECT_IDENTIFIER " << cppName << ";" << nl << nl;
	cxx << "const " << dllMacroAPI << " ASN1::OBJECT_IDENTIFIER " << cppName  << " {" ;
	stringstream dummy;
	generateCplusplus(fwd, hdr, cxx, dummy);
	cxx << "};" << nl << nl;
}

void ObjectIdentifierValue::printOn(ostream& strm) const {
	PrintBase(strm);
	if (value.empty())
		strm << "empty object identifier";
	else {
		strm << " {";
		ObjIdComponentList::const_iterator it;
		for (it = getComponents().cbegin(); it != getComponents().cend(); ++it) {
			strm << *it;
		if (distance(getComponents().cbegin(), it) + 1< getComponents().size())
			strm << ' ';
		}
		strm << "}";
	}
	strm << nl;
}


/////////////////////////////////////////////////////////

void MinValue::printOn(ostream& strm) const {
	strm << "INT_MIN";
}


void MinValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << "MinimumValue";
}


/////////////////////////////////////////////////////////

void MaxValue::printOn(ostream& strm) const {
	strm << "INT_MAX";
}


void MaxValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx << "MaximumValue";
}


/////////////////////////////////////////////////////////

SequenceValue::SequenceValue(ValuesList * list) {
	if (list != nullptr) {
		values.swap(*list);
		delete list;
	}
}


void SequenceValue::printOn(ostream& strm) const {
	strm << "{ ";
	for (size_t i = 0; i < values.size(); i++) {
		if (i > 0)
			strm << ", ";
		strm << *values[i];
	}
	strm << " }";
}

/////////////////////////////////////////////////////

void ChoiceValue::printOn(ostream& strm) const {
	strm << fieldname << " : " << *value;
}

void ChoiceValue::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	cxx  << type->getTypeName() << "::" << fieldname << "::eid, (" << *value << ")" ;
}

/////////////////////////////////////////////////////////


ImportModule::ImportModule(string * name, SymbolList * syms)
	: fullModuleName(*name),
	  shortModuleName(contexts.top()->Module->getImportModuleName(*name)) {
	delete name;

	ModuleDefinition* from = findModule(fullModuleName.c_str());
	if (syms) {
		symbols = *syms;
		delete syms;

		for (size_t i = 0; i < symbols.size(); i++) {
			if (from)
				symbols[i]->AppendToModule(from,contexts.top()->Module);
			else
				symbols[i]->AppendToModule(fullModuleName, contexts.top()->Module);
		}
	}

	filename = toLower(makeCppName(fullModuleName));
}


void ImportModule::printOn(ostream& strm) const {
	strm << "  " << fullModuleName << " (" << shortModuleName << "):" << nl;
	for (size_t i = 0; i < symbols.size(); i++)
		strm << "    " << *symbols[i];
	strm << nl;
}


void ImportModule::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream&) {
	ModuleDefinition* module = findModule(fullModuleName.c_str());
	if(!module) {
		hdr << "#include \"" << filename << ".h\"" << nl;

		for (size_t i = 0; i < symbols.size(); i++) {
			if (symbols[i]->isType()&& symbols[i]->isParameterisedImport()) {
				cxx << "#include \"" << filename << "_t" << cppExt << "\"" << nl;
				break;
			}
		}
	} else {
		if(module != UsefulModule)
			hdr << "#include \"" << filename << ".h\"" << nl;

		for (size_t i = 0; i < symbols.size(); i++) {
			if (symbols[i]->isType()&& symbols[i]->isParameterisedImport()) {
				TypeBase* type = (module->findType(symbols[i]->getName())).get();
				if (!type || !type->hasParameters())
					break;

				cxx << "#include \"" << filename << "_t" << cppExt << "\"" << nl;
				break;
			}
		}
	}
}

void ImportModule::generateUsingDirectives(ostream& strm) const {
	string name = makeCppName(fullModuleName);
	for (size_t i = 0; i < symbols.size(); ++i)
		symbols[i]->generateUsingDirective(name, strm);
}

void ImportModule::Adjust() {
	bool needCreateSubModules = false;
	// find if there're nonparameterized type
	for (size_t i = 0 ; i < symbols.size(); ++i) {
		if (symbols[i]->isType()) {
			if (symbols[i]->isParameterisedImport())
				return;

			needCreateSubModules = true;
		}
	}

	if (needCreateSubModules) {
		ModuleDefinition* module = findModule(fullModuleName.c_str());
		if (module)
			filename = module->CreateSubModules(symbols);
	}
}


bool ImportModule::hasValuesOrObjects() const {
	for (size_t i = 0; i < symbols.size(); ++i)
		if (symbols[i]->isValuesOrObjects())
			return true;
	return false;
}

string ImportModule::getCModuleName() const {
	return makeCppName(fullModuleName);
}


/////////////////////////////////////////////////////////

ModuleDefinition::ModuleDefinition(const string& name)
	: moduleName(name) {
	exportAll = false;
	indentLevel = 1;

	for (size_t i = 0; i < Modules.size(); ++i) {
		string str = Modules[i]->moduleName;
		identifiers[str]= MODULEREFERENCE;
	}
	// Create sorted list for faster searching.
}
ModuleDefinition::ModuleDefinition(const string& name, const string& filePath, Tag::Mode defTagMode)
	: moduleName(name), modulePath(filePath) {
	defaultTagMode = defTagMode;
	exportAll = false;
	indentLevel = 1;

	for (size_t i = 0; i < Modules.size(); ++i) {
		string str = Modules[i]->moduleName;
		identifiers[str]= MODULEREFERENCE;
	}
	// Create sorted list for faster searching.
}

ModuleDefinition::~ModuleDefinition() {
}


void ModuleDefinition::setDefinitiveObjId(StringList& id) {
	definitiveId.swap(id);
}

void ModuleDefinition::addIdentifier(string* name, int idType) {
	identifiers[*name]= idType;
	delete name;
}


void ModuleDefinition::addImportedIdentifiers(StringList& imports_sl, const string& name) {
	identifiers[name]= MODULEREFERENCE;
	ModuleDefinition* module = findModule(name.c_str());
	if (module) {
		for (size_t i = 0; i < imports_sl.size(); ++i) {
			string identifier = imports_sl[i];
			str_replace(identifier,"{}","");
			identifiers[identifier]= module->getIdentifierType(identifier);
		}
	} else {
		int id = 0;
		for (size_t i = 0; i < imports_sl.size(); ++i) {
			string identifier = imports_sl[i];
			if (isUpper(identifier.c_str())) {
				id = OBJECTCLASSREFERENCE;
			} else if (isupper(identifier.at(0))) {
				id = TYPEREFERENCE;
				if (identifier.find("{}") != -1) {
					str_replace(identifier,"{}","");
					id = PARAMETERIZEDTYPEREFERENCE;
				}
			} else {
				id = OBJECTREFERENCE;
				if (identifier.find("{}") != -1) {
					str_replace(identifier,"{}","");
					id = PARAMETERIZEDOBJECTREFERENCE;
				}
			}
			identifiers[identifier]=id;
		}
	}

}

int  ModuleDefinition::getIdentifierType(const string& id) {
	if (identifiers.count(id))
		return identifiers[id];
	return -1;
}


void ModuleDefinition::setExportAll() {
	exportAll = true;
}


void ModuleDefinition::setExports(SymbolList& syms) {
	exports.swap(syms);
}


void ModuleDefinition::printOn(ostream& strm) const {
	strm << moduleName  << nl
		 << "Default Tags: "
		 << Tag::modeNames[defaultTagMode]  << nl
		 << "Exports:";

	if (exportAll)
		strm << " ALL";
	else {
		strm << nl << "  ";
		for (size_t i = 0; i < exports.size(); i++)
			strm << *exports[i] << ' ';
		strm << nl;
	}

	strm << "Imports:" << nl << imports  << nl
		 << "Object Classes:" << nl << objectClasses  << nl
		 << "Types:" << nl << types  << nl
		 << "Values:" << nl << values  << nl
		 << "Objects:" << nl << informationObjects  << nl
		 << "ObjectSets:" << nl << informationObjectSets  << nl;
}


void ModuleDefinition::addType(TypePtr type) {
	types.push_back(type);
	typeMap[type->getName()] = type;
}


TypePtr ModuleDefinition::findType(const string& name) {
	const char* nam = name.c_str();
	if (typeMap.count(nam))
		return typeMap[nam];
	return TypePtr();
}


string ModuleDefinition::getImportModuleName(const string& mName) {
	if (importNames.count(mName))
		return importNames[mName];

	return shortenName(mName);
}

void ModuleDefinition::AdjustModuleName(const string& sourcePath,bool isSubModule) {
	shortModuleName = shortenName(moduleName);
	cppModuleName = makeCppName(moduleName);
	generatedPath = sourcePath + DIR_SEPARATOR;
	if (!isSubModule)
		generatedPath += moduleName;
	else if (types.size())
		generatedPath += (shortModuleName + "_" +toLower(makeCppName(types[0]->getName())));
	else {
		generatedPath += imports[0]->getFileName();
		string tmp = imports[1]->getFileName();
		generatedPath += tmp.substr(tmp.size()-tmp.find('_'));
	}

}

bool ModuleDefinition::ReorderTypes() {
	// Reorder types
	// Determine if we need a separate file for template closure
	bool hasTemplates = false;
	size_t loopDetect = 0;
	size_t bubble = 0;

	typedef list<boost::shared_ptr<TypeBase> > TypesList;
	TypesList rtypes(types.begin(), types.end());

	TypesList::iterator itr , bubble_itr=rtypes.begin();

	while (bubble < types.size()) {

		bool makesReference = false;
		TypeBase*  bubbleType = bubble_itr->get();
		if (bubbleType->canReferenceType()) {
			for (itr = bubble_itr; itr != rtypes.end(); ++itr) {
				if (bubbleType != itr->get()&& bubbleType->referencesType(**itr)) {
					makesReference = true;
					break;
				}
			}
		}

		if (makesReference) {
			rtypes.push_back(*bubble_itr);
			rtypes.erase(bubble_itr++);
			if (loopDetect > rtypes.size()) {
				cerr << StdError(Fatal)
					 << "Recursive type definition: " << bubbleType->getName()
					 << " references " << (*itr)->getName() <<endl;
				break;
			}

			loopDetect++;
		} else {
			loopDetect = bubble;
			bubble++;
			++bubble_itr;
		}

		if (bubbleType->hasParameters())
			hasTemplates = true;
	}

	//types.assign(rtypes.begin(), rtypes.end());
	types.clear();
	copy( rtypes.begin(), rtypes.end(), back_inserter(types));

	return hasTemplates;
}

void ModuleDefinition::generateCplusplus(const string& dir,	unsigned classesPerFile, bool verbose) {
	size_t i,classesCount = 1;

	contexts.top()->Module = this;
	string dpath(dir);
	if (dpath.length())
		dpath += DIR_SEPARATOR;

	if (verbose)
		clog << "Processing files " << this->getFileName() << "..." << nl;

	dpath += getFileName();

	// Remove all PER Invisible constraint
	for (i = 0; i < types.size(); i++)
		types[i]->RemovePERInvisibleConstraints();

	CreateObjectSetTypes();

	// flatten types by generating types for "inline" definitions
	for (i = 0; i < types.size(); i++) {
		types[i]->flattenUsedTypes();
	}

	if (verbose)
		clog << "Sorting " << types.size() << " types..." << endl;

	bool hasTemplates = ReorderTypes();

	// Adjust all of the C++ identifiers prepending module name
	for (i = 0; i < types.size(); i++)
		types[i]->adjustIdentifier(true);

	// generate the code
	if (verbose)
		clog << "Generating code (" << types.size() << " classes) ..." << endl;


	// Output the special template closure file, if necessary
	string templateFilename;
	if (hasTemplates) {
		OutputFile templateFile;

		if (!templateFile.open(dpath, "_t", cppExt))
			return;

		templateFile << "namespace " << cppModuleName << "{" << nl;

		for (i = 0; i < types.size(); i++) {
			if (types[i]->hasParameters()) {
				stringstream dummy;
				stringstream fwd;
				types[i]->generateCplusplus(fwd, dummy, templateFile, dummy);
			}
		}

		templateFile << "} // namespace " << cppModuleName <<  nl << nl;

		if (verbose)
			clog << "Completed " << templateFile.getName() << endl;

		templateFilename = ::getFileName(dpath) + "_t" + cppExt;//templateFile.getFilePath().getFileName();
	}

	// Start the header file
	stringstream fwd;
	fwd << nl;
	fwd << "//" << nl;
	fwd << "// Type forward declaration" << nl;
	fwd << "//" << nl << nl;


	unsigned numFiles = 1;
	if (classesPerFile > 0)
		numFiles = types.size() / classesPerFile + (types.size() % classesPerFile ? 1 : 0);
	else
		classesPerFile = types.size();

	OutputFile cxx;
	if (!cxx.open(dpath, numFiles > 1 ? "_1" : "", cppExt))
		return;
	stringstream inl;

	string headerName = ::getFileName(dpath) + incExt;

	OutputFile hdr;
	if (!hdr.open(dpath, "tmp", incExt))
		return;

	hdr << "#ifndef " << cppModuleName << "_H_" << nl;
	hdr << "#define " << cppModuleName << "_H_" << nl << nl;
	hdr << "#include \"asn1.h\"" << nl;
	hdr << nl;

	hdr << setprecision(0);
	// Start the first (and maybe only) cxx file

	if (includeConfigH)
		cxx << "#ifdef HAVE_CONFIG_H" << nl;
	cxx << "#include \"config.h\"" << nl;
	cxx << "#endif\n" << nl;

// if this define is generated - do_accept() in cxx file does not find inline function
//			cxx << "#define " << cppModuleName << "_CXX" << nl;
	cxx << "#include \"" << headerName << "\"" << nl << nl;

	// Include the template closure file.
	if (hasTemplates)
		cxx << "#include \"" << templateFilename << "\"" << nl << nl;

	for_all(imports, boost::bind(&ImportModule::generateCplusplus, _1,
								 boost::ref(fwd), boost::ref(hdr), boost::ref(cxx), boost::ref(inl)));

	if (!imports.empty()) {
		hdr <<  nl << nl;
		cxx <<  nl << nl;
	}

	for (i = 0; i < subModules.size() ; ++i)
		hdr << "#include \"" << subModules[i]->getFileName() << ".h\"" << nl;

	if (dllMacro.size() > 0) {
		hdr << nl;
		hdr << "#ifndef " << dllMacroDEFINED << nl;
		hdr << "#define " << dllMacroDEFINED << nl;
		hdr << nl;

		hdr << "#include \"Platform.h\"" << nl;
		hdr << nl;

		hdr << "#if defined(_WIN32)" << nl;
		hdr << "	#include \"Platform_WIN32.h\"" << nl;
		hdr << "#elif defined(__VMS)" << nl;
		hdr << "	#include \"Platform_VMS.h\"" << nl;
		hdr << "#elif defined(ALS_VXWORKS)" << nl;
		hdr << "	#include \"Platform_VX.h\"" << nl;
		hdr << "#elif defined(ALS_OS_FAMILY_UNIX)" << nl;
		hdr << "	#include \"Platform_POSIX.h\"" << nl;
		hdr << "#endif" << nl;
		hdr << nl;

		hdr << "//3" << nl;
		hdr << "// Ensure that " << dllMacroDLL << " is default unless " << dllMacroSTATIC << " is defined" << nl;
		hdr << "//" << nl;
		hdr << "#if defined(_WIN32)&& defined(_DLL)" << nl;
		hdr << "	#if !defined(" << dllMacroDLL << ")&& !defined(" << dllMacroSTATIC << ")" << nl;
		hdr << "		#define " << dllMacroDLL << nl;
		hdr << "	#endif" << nl;
		hdr << "#endif" << nl;
		hdr << nl;

		hdr << "#if defined(_MSC_VER)" << nl;
		hdr << "	#if defined(" << dllMacroDLL << ")" << nl;
		hdr << "		#if defined(_DEBUG)" << nl;
		hdr << "			#define " << dllMacroLIB_SUFFIX << " \"d.lib\"" << nl;
		hdr << "		#else" << nl;
		hdr << "			#define " << dllMacroLIB_SUFFIX << " \".lib\"" << nl;
		hdr << "		#endif" << nl;
		hdr << "	#elif defined(_DLL)" << nl;
		hdr << "		#if defined(_DEBUG)" << nl;
		hdr << "			#define " << dllMacroLIB_SUFFIX << " \"mdd.lib\"" << nl;
		hdr << "		#else" << nl;
		hdr << "			#define " << dllMacroLIB_SUFFIX << " \"md.lib\"" << nl;
		hdr << "		#endif" << nl;
		hdr << "	#else" << nl;
		hdr << "		#if defined(_DEBUG)" << nl;
		hdr << "			#define " << dllMacroLIB_SUFFIX << " \"mtd.lib\"" << nl;
		hdr << "		#else" << nl;
		hdr << "			#define " << dllMacroLIB_SUFFIX << " \"mt.lib\"" << nl;
		hdr << "		#endif" << nl;
		hdr << "	#endif" << nl;
		hdr << "#endif" << nl;
		hdr << nl;

		hdr << "//" << nl;
		hdr << "// The following block is the standard way of creating macros which make exporting" << nl;
		hdr << "// from a DLL simpler. All files within this DLL are compiled with the " << dllMacroEXPORTS << nl;
		hdr << "// symbol defined on the command line. this symbol should not be defined on any project" << nl;
		hdr << "// that uses this DLL. This way any other project whose source files include this file see" << nl;
		hdr << "// " << dllMacroAPI << " functions as being imported from a DLL, wheras this DLL sees symbols" << nl;
		hdr << "// defined with this macro as being exported." << nl;
		hdr << "//" << nl;
		hdr << "#if defined(_WIN32)&& defined(" << dllMacroDLL << ")" << nl;
		hdr << "	#if defined(" << dllMacroEXPORTS << ")" << nl;
		hdr << "		#define " << dllMacroAPI << " __declspec(dllexport)" << nl;
		hdr << "	#else" << nl;
		hdr << "		#define " << dllMacroAPI << " __declspec(dllimport)" << nl;
		hdr << "	#endif" << nl;
		hdr << "#endif" << nl;
		hdr << nl;

		hdr << "#if !defined(" << dllMacroAPI << ")" << nl;
		hdr << "	#define " << dllMacroAPI << nl;
		hdr << "#endif" << nl;
		hdr << nl;

		hdr << "//" << nl;
		hdr << "// Automatically link " << dllMacro << " library." << nl;
		hdr << "//" << nl;
		hdr << "#if defined(_MSC_VER)" << nl;
		hdr << "	#if !defined(" << dllMacroNO_AUTOMATIC_LIBS << ")&& !defined(" << dllMacroEXPORTS << ")" << nl;
		hdr << "		#pragma comment(lib, \"" << dllMacroRTS << "\" " << dllMacroLIB_SUFFIX << ")" << nl;
		hdr << "	#endif" << nl;
		hdr << "#endif" << nl;
		hdr << "#endif" << nl;
		hdr << nl;
	}

	hdr << "namespace " << cppModuleName << " {" << endl;
	for_all(imports, boost::bind(&ImportModule::generateUsingDirectives, _1, boost::ref(hdr)));


#if 0
	hdr << "//" << nl;
	hdr << "// Type forward declaration" << nl;
	hdr << "//" << nl;
	for (i = 0; i < types.size(); i++) {
		TypePtr type = types[i];
		if (!type->isPrimitiveType())
			hdr << "class " << type->getCppName() << ";" << endl;
	}

	hdr << "//" << nl;
	hdr << "// Object Class forward declaration" << nl;
	hdr << "//" << nl;
	for (i = 0; i < objectClasses.size(); ++i) {
		ObjectClassBasePtr objectClass = objectClasses[i];
		if (objectClass->getReferenceName() == "TYPE-IDENTIFIER") {
			hdr << "typedef ASN1::TYPE_IDENTIFIER " << objectClass->getCppName() << ";" << endl;
		} else
			hdr << "class " << objectClass->getCppName() << ";" << endl;
	}
#endif
	hdr << endl;
	const streampos insertionPoint = hdr.tellp();

	cxx << "namespace " << cppModuleName << "{" << nl	 << nl;

	for (auto value : values)
		value->generateConst(fwd, hdr, cxx, inl);


	for (i = 0; i < types.size(); i++) {
		if (i > 0 && i%classesPerFile == 0) {
			cxx << "} // namespace " << cppModuleName <<  nl << nl;
			cxx.close();
			classesCount = i/classesPerFile+1;

			if (verbose)
				clog << "Completed " << cxx.getName() << endl;

			stringstream suffix;
			suffix << '_' << i/classesPerFile+1 ;

			if (!cxx.open(dpath, suffix.str().c_str(), cppExt)) {
				clog << "cannot open file " << dpath << "_" << i/classesPerFile+1 << cppExt  << nl;
				return;
			}

			if (includeConfigH)
				cxx << "#ifdef HAVE_CONFIG_H" << nl
					<< "#include \"config.h\"" << nl
					<< "#endif\n" << nl;

// if this define is generated - do_accept() in cxx file does not find inline function
//				cxx << "#define " << cppModuleName << "_CXX" << nl;
			cxx << "#include \"" << headerName << "\"" << nl << nl;

			// Include the template closure file.
			if (hasTemplates)
				cxx << "#include \"" << templateFilename << "\"" << nl;


			for_all(imports, boost::bind(&ImportModule::generateCplusplus, _1,
										 boost::ref(fwd), boost::ref(hdr), boost::ref(cxx), boost::ref(inl)));


			if (!imports.empty()) {
				hdr <<  nl << nl;
				cxx <<  nl << nl;
			}


			cxx << "namespace " << cppModuleName << "{" << nl	 << nl;
		}

		clog << "Generating " << types[i]->getName() << endl;

		if (types[i]->hasParameters()) {
			stringstream dummy;
			types[i]->generateCplusplus(fwd, hdr, dummy, inl);
		} else {
			types[i]->generateCplusplus(fwd, hdr, cxx, inl);
		}

	}

	i = classesCount+1; // FIXME ???

	// generate Information Classes
	for (int no = 0; no < objectClasses.size(); ++no) {
		ObjectClassBasePtr objectClass = objectClasses[no];
		objectClass->generateCplusplus(fwd, hdr, cxx, inl);
	}

	// generate Information Objects& Information ObjectSets
	generateClassModule(fwd, hdr, cxx, inl);


	//if (useNamespaces)
	cxx << "} // namespace " << cppModuleName  << nl;

	if (!inl.str().empty()) {
		OutputFile inlFile;
		if (!inlFile.open(dpath, "", ".inl"))
			return;

//			if this define is generated - do_accept() in cxx file does not find inline function
//			inlFile << "#if !defined( " << cppModuleName << "_CXX)&& !defined(NO_"
		inlFile << "#if !defined(NO_" <<  cppModuleName << "_INLINES)" << nl;

		if (!useReinterpretCast.empty()) {
			// Workaround for compilers (e.g. VAC++5/AiX),
			// that won't compile the static_casts.
			inlFile << "\n#ifdef " << useReinterpretCast  << nl;
			inlFile << "#define static_cast reinterpret_cast" << nl;
			inlFile << "#endif\n" << nl;
		}

		inlFile << inl.str();

		if (!useReinterpretCast.empty()) {
			inlFile << "#ifdef " << useReinterpretCast  << nl;
			inlFile << "#undef static_cast" << nl;
			inlFile << "#endif\n" << nl;
		}
		inlFile << "#endif" << nl;
		hdr << "#include \"" << moduleName + ".inl\"" //inlFile.getFilePath().getFileName()
			<< nl << nl;
	}
	inl << ends;

	// close off the files
	hdr << "} // namespace " << cppModuleName	<<  nl << nl;

	hdr << "#endif // " << cppModuleName << "_H_" << nl;

	hdr.close();

	insertForwardDeclaration(hdr, insertionPoint, fwd);

	if (verbose)
		clog << "Completed " << cxx.getName() << endl;

}

static void insertForwardDeclaration(const OutputFile& hdr, streampos insertionPoint, stringstream& fwd) {
	ifstream hdrold(hdr.getName());
	ofstream hdrnew(hdr.getPath() + hdr.getExtension());
	fwd.flush();

	if (hdrold && hdrnew) {
		long no;
		for (no = 0; no < insertionPoint; ++no)
			hdrnew.put(hdrold.get());

		ofstream::int_type  car;
		hdrnew << fwd.rdbuf();
		while ((car = hdrold.get()) != char_traits<char>::eof())
			hdrnew.put(car);

		hdrold.close();
		hdrnew.close();
	}
}

void ModuleDefinition::generateClassModule(ostream& fwd, ostream& hdr, ostream& cxxFile, ostream& inl) {
	size_t i;
	stringstream tmphdr, tmpcxx;
	for (i = 0 ; i < informationObjects.size(); ++i)
		informationObjects[i]->generateCplusplus(tmphdr, tmpcxx, inl);

	for (i = 0 ; i < informationObjects.size(); ++i)
		if (dynamic_cast<DefaultObjectDefn*>(informationObjects[i].get())) {
			InformationObject& obj = *informationObjects[i];
			string name = makeCppName(obj.getName());
			if (obj.isExtendable())
				tmphdr <<  name << "& get_" << name << "() \t\t\t\t{  return m_" << name << "; }" << nl;
			tmphdr << "const " << name << "& get_" << name << "() const \t{  return m_" << name << "; }" << nl;
		}

	for (i = 0; i < informationObjectSets.size(); ++i) {
		InformationObjectSet& objSet = *informationObjectSets[i];
		const string name = makeCppName(objSet.getName());
		const string className = makeCppName(objSet.getObjectClass()->getReferenceName());

		if (!objSet.hasParameters()) {
			if (objSet.isExtendable())
				tmphdr <<  className << "& get_" << name << "() \t\t\t\t{  return m_" << name << "; }" << nl;

			tmphdr << "const " << className << "& get_" << name << "() const \t{  return m_" << name << "; }" << nl;
		}
	}
	hdr << "//6" << nl;
	hdr << "// Module" << nl;
	hdr << "//" << nl;
	if (!tmphdr.str().empty()) {
		hdr << "class " << dllMacroAPI << " Module : public ASN1::Module {" << nl
			<< "public:" << nl << tab
			<< "Module(";

		bool needComma = false;
		for (i = 0; i < imports.size(); ++i)
			if (imports[i]->hasValuesOrObjects()) {
				if (needComma)
					hdr << ", ";
				hdr << imports[i]->getCModuleName() << "::Module* " ;
				needComma = true;
			}

		hdr << ");" << nl;

		hdr << tmphdr.str();
		tmphdr << ends;

		hdr << bat << "private:" << nl << tab;
		for (i = 0 ; i < informationObjects.size(); ++i) {
			InformationObject& obj = *informationObjects[i];
			string name = makeCppName(obj.getName());
			string className = makeCppName(obj.getObjectClass()->getReferenceName());
			if (dynamic_cast<DefaultObjectDefn*>(&obj))
				hdr << className << " m_" << name << ";" << nl;
		}

		for (i = 0; i < informationObjectSets.size(); ++i) {
			InformationObjectSet& objSet = *informationObjectSets[i];
			if (!objSet.hasParameters()) {
				const string name = makeCppName(objSet.getName());
				const string className = makeCppName(objSet.getObjectClass()->getReferenceName());
				hdr << className << " m_" << name << ";" << nl;
			}
		}

		hdr << bat << "}; // class Module" << nl << nl;

		cxxFile << "#ifdef _MSC_VER" << nl
				<< "#pragma warning(disable: 4355)" << nl
				<< "#endif\n" << nl
				<< tmpcxx.str()
				<< "Module::Module(";
		tmpcxx << ends;

		for (i = 0, needComma = false; i < imports.size(); ++i)
			if (imports[i]->hasValuesOrObjects()) {
				if (needComma)
					cxxFile << ", ";
				cxxFile << imports[i]->getCModuleName() << "::Module* "<< imports[i]->getLowerCaseName() ;
				needComma = true;
			}
		cxxFile << ")" << nl;

		cxxFile << "{" << nl;
		for (i = 0, needComma = false; i < imports.size(); ++i)
			if (imports[i]->hasValuesOrObjects()) {
				cxxFile << "  assert(" << imports[i]->getLowerCaseName() << ");" << nl;
			}

		cxxFile << "  moduleName = \"" << moduleName << "\";" << nl;

		for (i = 0 ; i < informationObjects.size(); ++i) {
			InformationObject& obj = *informationObjects[i];
			if (dynamic_cast<DefaultObjectDefn*>(&obj))
				obj.generateInstanceCode(cxxFile);
		}

		for_each(informationObjectSets.begin(), informationObjectSets.end(),
				 boost::bind(&InformationObjectSet::generateInstanceCode, _1, boost::ref(cxxFile)));

		cxxFile << "}\n" << nl;
	}
}

ValuePtr ModuleDefinition::findValue(const string& name) {
	return findWithName(values, name);
}

ObjectClassBasePtr ModuleDefinition::findObjectClass(const string& name) {
	ObjectClassBasePtr result = findWithName(objectClasses, name);
	if (result.get())
		return result;

	if (name == "TYPE-IDENTIFIER") {
		// add the definition of TYPE-IDENTIFIER

		ObjectClassDefnPtr type_Identifier(new ObjectClassDefn);
		type_Identifier->setName("TYPE-IDENTIFIER");
		auto_ptr<FieldSpecsList> fieldSpecs(new FieldSpecsList);

		FixedTypeValueFieldSpecPtr idField(new FixedTypeValueFieldSpec("&id", TypePtr(new ObjectIdentifierType), false, true));

		fieldSpecs->push_back(idField);

		FieldSpecPtr typeField(new TypeFieldSpec("&Type"));

		fieldSpecs->push_back(typeField);
		type_Identifier->setFieldSpecs(fieldSpecs);

		TokenGroupPtr tokens(new TokenGroup);
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&Type")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("IDENTIFIED")));
		tokens->addToken(TokenOrGroupSpecPtr(new Literal("BY")));
		tokens->addToken(TokenOrGroupSpecPtr(new PrimitiveFieldName("&id")));

		type_Identifier->setWithSyntaxSpec(tokens);
		addObjectClass(type_Identifier);
		return type_Identifier;
	}

	if (this != UsefulModule) {
		ObjectClassBasePtr useful = UsefulModule->findObjectClass(name);

		if(useful) {
			ImportedObjectClass* importedObjectClass = new ImportedObjectClass(UsefulModule->getName(),	name, useful.get());
			ObjectClassBasePtr oc(importedObjectClass);
			oc->setName(name);
			addObjectClass(oc);

			SymbolList *lst = new SymbolList;
			lst->push_back(SymbolPtr(new ObjectClassReference(name, false)));

			addImport(ImportModulePtr(new ImportModule(new string(UsefulModule->getName()), lst)));

			return oc;
		}
	}

	return result;
}

const InformationObject* ModuleDefinition::findInformationObject(const string& name) {
	return findWithName(informationObjects, name).get();
}

const InformationObjectSet* ModuleDefinition::findInformationObjectSet(const string& name) {
	return findWithName(informationObjectSets, name).get();
}

void ModuleDefinition::ResolveObjectClassReferences() const {
	for (size_t i = 0; i < objectClasses.size(); ++i)
		objectClasses[i]->resolveReference();
}



string ModuleDefinition::getFileName() {
	return getFileTitle(generatedPath);
}


string ModuleDefinition::CreateSubModules(SymbolList& exportedSymbols) {
	unsigned clsPerFile = classesPerFile;
	if(!clsPerFile)
		clsPerFile = 101;

	if (types.size() <= clsPerFile)
		return getFileName();

	size_t i, j;
	contexts.top()->Module = this;
	for (i = 0; i < types.size(); ++i)
		types[i]->resolveReference();

	typedef list<string> StringList;
	StringList unhandledSymbols;
	ModuleDefinitionPtr subModule(new ModuleDefinition(moduleName, "", defaultTagMode));

	TypesVector& exportedTypes = subModule->types;

	for (i = 0; i < exportedSymbols.size() ; ++i)
		if (exportedSymbols[i]->isType()) {
			TypePtr type = findType(exportedSymbols[i]->getName());
			if (type.get() != nullptr) {
				subModule->addType(type);
				typeMap.erase(type->getName());
				types.erase(remove(types.begin(), types.end(), type), types.end());
			} else { // this type may be in subModuls or importedModules
				unhandledSymbols.push_back(exportedSymbols[i]->getName());
			}
		}

	for (j = 0; j < exportedTypes.size(); ++j) {
		i = 0;
		while (i < types.size()) {
			if ( exportedTypes[j]->useType(*types[i]) ) {
				subModule->addType(types[i]);
				typeMap.erase(types[i]->getName());
				types.erase(types.begin()+i);
			} else
				++i;
		}
	}


	for (i = 0 ; i < subModules.size(); ++i) {
		bool hasSymbolInThisModule = false;
		StringList::iterator it = unhandledSymbols.begin();
		while (it != unhandledSymbols.end()) {
			if (subModules[i]->hasType(*it)) {
				if (!hasSymbolInThisModule) {
					ImportModule* im = new ImportModule(new string(getName()), nullptr);
					im->setFileName(subModules[i]->getFileName());
					subModule->imports.push_back(ImportModulePtr(im));
					hasSymbolInThisModule = true;
				}
				unhandledSymbols.erase(it++);
			} else
				++it;
		}

		for (j = 0; j < exportedTypes.size()&& !hasSymbolInThisModule; ++j) {
			for (size_t k = 0; k < subModules[i]->types.size()&& !hasSymbolInThisModule; ++k) {
				if ( exportedTypes[j]->useType(*subModules[i]->types[k]) ) {
					ImportModule* im = new ImportModule(new string(getName()), nullptr);
					im->setFileName(subModules[i]->getFileName());
					subModule->imports.push_back(ImportModulePtr(im));
					hasSymbolInThisModule = true;
				}
			}
		}
	}

	if (exportedTypes.size() ==0&& subModule->imports.size() == 1 ) {
		string result = subModule->imports[0]->getFileName();
		return result;
	} else if (types.size()==0&& exportedTypes.size()) {
		// all types has been exported, move them back
		for (i = 0; i < exportedTypes.size(); ++i)
			addType(exportedTypes[i]);
		return getFileName();
	} else if (exportedTypes.size() || subModule->imports.size() > 1) {
		for (i = 0 ; i < exportedTypes.size(); ++i) {
			ImportedType* importedType = dynamic_cast<ImportedType*>(exportedTypes[i].get());
			if (importedType) {
				ImportModule* im= subModule->findImportedModule(importedType->getModuleName());
				if (im == nullptr) {
					im = new ImportModule(new string(importedType->getModuleName()), nullptr);
					ImportModule* theImportedModule = this->findImportedModule(importedType->getModuleName());
					assert(theImportedModule);
					im->setFileName(theImportedModule->getFileName());
					subModule->imports.push_back(ImportModulePtr(im));
				}
			}
		}

		subModule->AdjustModuleName(getFileDirectory(generatedPath), true);
		subModules.push_back(subModule);
		Modules.push_back(subModule);
		return subModule->getFileName();
	} else {
		cerr << "Unexpected Situation, Do not use selective imports option" << nl;
		cerr << "unresolved imported types (" << exportedTypes.size() << ") :" << exportedTypes << nl;
		cerr << subModule->imports.size() << nl;
		exit(1);
		//return "";
	}
}

void ModuleDefinition::AdjustImportedModules() {
	for_all(imports, boost::mem_fn(&ImportModule::Adjust));
}

bool ModuleDefinition::isExported(const string& name) const {
	if(exportAll)
		return true;

	for(int i=0; i<exports.size(); ++i) {
		if(name==exports[i]->getName())
			return true;
	}
	return false;
}

void ModuleDefinition::CreateObjectSetTypes() {
	for (size_t i = 0; i < informationObjectSets.size(); ++i) {
		TypePtr objSetType(new ObjectSetType(informationObjectSets[i]));
		bool is_used(false);

		if(isExported(informationObjectSets[i]->getName()))
			is_used = true;
		else {
			for (size_t j = 0; j < types.size(); ++j) {
				if (types[j]->referencesType(*objSetType)) {
					is_used = true;
					break;
				}
			}
		}

		if(is_used)
			addType(objSetType);

	}
}

void ModuleDefinition::addToRemoveList(const string& reference) {
	removeList.push_back(reference);
}

void MoveTypes(TypesVector& sourceList, TypesVector& toBeExtractedList, TypesVector& targetList) {
	for (size_t i = 0; i < sourceList.size(); ++i) {
		TypeBase& type = *sourceList[i];
		size_t j = 0;
		while (j < toBeExtractedList.size()) {
			if (type.useType(*toBeExtractedList[j])) {
				targetList.push_back(toBeExtractedList[j]);
				toBeExtractedList.erase(toBeExtractedList.begin()+j);
			} else
				++j;
		}
	}
}

void ModuleDefinition::RemoveReferences(bool verbose) {
	size_t i, j;
	TypesVector referencesToBeRemoved;
	for (i = 0 ; i < removeList.size(); ++i) {
		const string& ref = removeList[i];
		TypePtr typeToBeRemoved = findType(ref);
		if (typeToBeRemoved.get() == nullptr) {
			clog << getName() << "." << ref << " doesn't exist, unable to remove it" << nl;
			continue;
		}

		// check if this type can be removed
		for (j = 0; j < types.size() ; ++j)
			if (!types[j]->canRemoveType(*typeToBeRemoved) == TypeBase::OK)
				break;

		if (j == types.size()) { // this type can be removed
			// actual remove this type
			referencesToBeRemoved.push_back(typeToBeRemoved);
			types.erase(remove(types.begin(), types.end(),typeToBeRemoved), types.end());
			// remove all the references to this type
			for (j = 0; j < types.size(); ++j)
				types[j]->removeThisType(*typeToBeRemoved);
		} else {
			clog << "Unable to remove " << getName() << "." << ref  << nl;
		}
	}

	MoveTypes(referencesToBeRemoved, types, referencesToBeRemoved);
	TypesVector typesToBePreserved;
	MoveTypes(types, referencesToBeRemoved, typesToBePreserved);
	MoveTypes(typesToBePreserved, referencesToBeRemoved, typesToBePreserved);

	types.insert(types.end(), typesToBePreserved.begin(), typesToBePreserved.end());

	for (i = 0; i < referencesToBeRemoved.size(); ++i)
		typeMap.erase(referencesToBeRemoved[i]->getName());

	if (verbose) {
		for (i = 0; i < referencesToBeRemoved.size(); ++i)
			clog << "Remove Type : " << referencesToBeRemoved[i]->getName()  << nl;
	}

}

ImportModule* ModuleDefinition::findImportedModule(const string& theModuleName) {
	//for (size_t i = 0; i < imports.getSize(); ++i)
	//{
	//    if (imports[i].getModuleName() == theModuleName)
	//        return&imports[i];
	//}
	//return nullptr;
	return findWithName(imports, theModuleName).get();
}

bool  ModuleDefinition::hasType(const string& name) {
	return findType(name).get() != nullptr;
}

void ModuleDefinition::dump() const {
	clog << getName() << endl;
	map<string,int>::const_iterator i;
	for(i = identifiers.cbegin(); i != identifiers.cend(); ++i) {
		clog << "\t" << i->first  << "\t\t = " << tokenAsString(i->second) << endl;
	}
}
//////////////////////////////////////////////////////////////////////////////
FieldSpec::FieldSpec(const string& nam, bool optional)
	: name(nam), isoptional(optional) {
	identifier = makeCppName(name.substr(1));
}


FieldSpec::~FieldSpec() {
}


void FieldSpec::printOn(ostream& strm) const {
	strm << name << '\t' << getField() ;
	if (isoptional)
		strm << " OPTIONAL";
}

void FieldSpec::generateTypeField(const string& ,  const string& ,  const TypeBase* ,  const string& ,
								  ostream& fwd, ostream& , ostream& , ostream& ) const {
}
////////////////////////////////////////////////////////////////////////

TypeFieldSpec::TypeFieldSpec(const string& nam, bool optional, TypePtr defaultType)
	: FieldSpec(nam, optional), type(defaultType) {
}

TypeFieldSpec::~TypeFieldSpec() {
}

bool TypeFieldSpec::hasDefault() const {
	return type.get() != nullptr;
}


string TypeFieldSpec::getField() const {
	return string("");
}

TypePtr TypeFieldSpec::getDefaultType() {
	return type;
}


int TypeFieldSpec::getToken() const {
	return TYPEFIELDREFERENCE;
}


string TypeFieldSpec::getDefault() const {
	if (type.get())
		return type->getName();
	else
		return string("");
}

bool TypeFieldSpec::getKey(TypePtr& keyType, string& keyName) {
	if (type != nullptr && string(type->getAncestorClass()) == "ASN1::OBJECT_IDENTIFIER") {
		keyType = type;
		keyName = getName();
		return true;
	}
	return false;
}

void TypeFieldSpec::printOn(ostream& strm) const {
	strm << name ;

	if (isoptional)
		strm << " OPTIONAL";

	if (getDefault().size() != 0) {
		strm << '\t' << "DEFAULT " << getDefault();
	}
}

void TypeFieldSpec::resolveReference() const {
	if (type.get())
		type->resolveReference();
}

void TypeFieldSpec::generate_info_type_constructor(ostream& cxx) const {
	if (!type.get())
		cxx << "  m_" << getIdentifier() << "= nullptr;" << nl;
}

void TypeFieldSpec::generate_info_type_memfun(ostream& hdr) const {
	hdr  << "ASN1::AbstractData* get_" << getIdentifier()
		 << "() const { return ";

	if (!type.get())
		hdr << "m_" << getIdentifier() << " ? ASN1::AbstractData::create(m_"
			<< getIdentifier() << ") : nullptr";
	else
		hdr << "ASN1::AbstractData::create(&" << type->getTypeName() << "::theInfo)";

	hdr << "; }" << nl;
}

void TypeFieldSpec::generate_info_type_mem(ostream& hdr) const {
	if (!type.get())
		hdr  << "const void* m_" << getIdentifier() << ";" << nl;
}


void TypeFieldSpec::generate_value_type(ostream& hdr) const {
	string fname = getIdentifier();
	hdr  << "ASN1::AbstractData* get_" << fname << "() const { return second->get_"	<< fname << "(); }" << nl;
}

void TypeFieldSpec::generateTypeField(const string& templatePrefix,  const string& classNameString,  const TypeBase* keyType, const string& objClassName,
									  ostream& fwd, ostream& hdr, ostream& cxx, ostream& ) const {
	hdr << "    ASN1::AbstractData* get_" << getIdentifier() << "(const "
		<< keyType->getTypeName() << "& key) const;" << nl;

	cxx << templatePrefix
		<< "ASN1::AbstractData* " << classNameString << "::get_" <<  getIdentifier() << "(const "
		<< keyType->getTypeName() << "& key) const" << nl
		<< "{" << nl
		<< "    if (objSet) {" << nl
		<< "      " << objClassName << "::const_iterator it = objSet->find(key);" << nl
		<< "    if (it != objSet->end())" << nl
		<< "        return it->get_" << getIdentifier() << "();" << nl
		<< "    }" << nl
		<< "  return nullptr;" << nl
		<< "}\n" << nl;
}


////////////////////////////////////////////////////////////////////////////

FixedTypeValueFieldSpec::FixedTypeValueFieldSpec(const string& nam, TypePtr t,
		bool optional, bool unique)
	: FieldSpec(nam, optional), isUnique(unique), type(t) {
}

FixedTypeValueFieldSpec::~FixedTypeValueFieldSpec() {
}

void FixedTypeValueFieldSpec::setDefault(ValuePtr value) {
	defaultValue = value;
}

bool FixedTypeValueFieldSpec::hasDefault() const {
	return defaultValue.get() != nullptr;
}


string FixedTypeValueFieldSpec::getField() const {
	return type->getName();
}


void FixedTypeValueFieldSpec::beginParseSetting(FieldSettingList*) const {
	contexts.top()->ValueTypeContext = type;
	type->beginParseValue();
}

void FixedTypeValueFieldSpec::endParseSetting() const {
	type->endParseValue();
}

int FixedTypeValueFieldSpec::getToken() const {
	return FIXEDTYPEVALUEFIELDREFERENCE;
}

void FixedTypeValueFieldSpec::printOn(ostream& strm) const {
	strm << name << '\t' << getField() << '\t' << type->getTypeName();
	if (isUnique)
		strm << " UNIQUE";

	if (isoptional)
		strm << " OPTIONAL";

	if (defaultValue.get() != nullptr)
		strm << *defaultValue;
}

void FixedTypeValueFieldSpec::resolveReference() const {
	type->resolveReference();
}

TypeBase* FixedTypeValueFieldSpec::getFieldType() {
	return type.get();
}

const TypeBase* FixedTypeValueFieldSpec::getFieldType() const {
	return type.get();
}

bool FixedTypeValueFieldSpec::getKey(TypePtr& keyType, string& keyName) {
	if (isUnique || string(type->getAncestorClass()) == "ASN1::OBJECT_IDENTIFIER") {
		keyType = type;
		keyName = getName();
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////

FixedTypeValueSetFieldSpec::FixedTypeValueSetFieldSpec(const string& nam,
		TypePtr t,
		bool optional)
	: FieldSpec(nam, optional), type(t) {
}


FixedTypeValueSetFieldSpec::~FixedTypeValueSetFieldSpec() {
}

bool FixedTypeValueSetFieldSpec::hasDefault() const {
	return defaultValueSet.get() != nullptr;
}

void FixedTypeValueSetFieldSpec::beginParseSetting(FieldSettingList*) const {
	contexts.top()->ValueTypeContext = type;
	type->beginParseValueSet();
}

void FixedTypeValueSetFieldSpec::endParseSetting() const {
}

int FixedTypeValueSetFieldSpec::getToken() const {
	return FIXEDTYPEVALUESETFIELDREFERENCE;
}

void FixedTypeValueSetFieldSpec::printOn(ostream& strm) const {
	strm << name << '\t' << getField() << '\t';
	strm << type->getTypeName();
	if (isoptional)
		strm << " OPTIONAL";

	if (defaultValueSet.get() != nullptr)
		strm << *defaultValueSet;
}

void FixedTypeValueSetFieldSpec::resolveReference() const {
	type->resolveReference();
}

TypeBase* FixedTypeValueSetFieldSpec::getFieldType() {
	return type.get();
}

const TypeBase* FixedTypeValueSetFieldSpec::getFieldType() const {
	return type.get();
}


/////////////////////////////////////////////////////////////////////

VariableTypeValueFieldSpec::VariableTypeValueFieldSpec(const string& nam,
		const string& fieldname,
		bool optional)
	: FieldSpec(nam, optional), fieldName(fieldname) {
}

VariableTypeValueFieldSpec::~VariableTypeValueFieldSpec() {
}

bool VariableTypeValueFieldSpec::hasDefault() const {
	return defaultValue.get() != nullptr;
}

TypePtr getFieldType(FieldSettingList* parsedFields, const string& fieldname) {
	if (parsedFields != nullptr) {
		for (size_t i = 0; i < parsedFields->size(); ++i) {
			FieldSetting& fieldSetting = *((*parsedFields)[i]);
			if (fieldSetting.getName() == fieldname) {
				TypeSetting* setting = dynamic_cast<TypeSetting*>(fieldSetting.getSetting());
				if (setting)
					return setting->getType();
			}
		}
	}

	cerr << StdError(Fatal) << "Invalid Object Field : "<< fieldname << nl;
	return TypePtr();
}

void VariableTypeValueFieldSpec::EstablishFieldRelation(FieldSpecsList* specs) {
	for (size_t i = 0; i < specs->size(); ++i) {
		FieldSpec& spec = *((*specs)[i]);
		if (spec.getName() == fieldName) {
			TypeFieldSpec* tfspec = dynamic_cast<TypeFieldSpec*>(&spec);
			if (tfspec) {
				defaultType = tfspec->getDefaultType();
				return;
			}
		}
	}

	cerr << StdError(Fatal) << "Invalid Object Class Definition at Field : "<< fieldName << nl;
}

void VariableTypeValueFieldSpec::beginParseSetting(FieldSettingList* parsedFields) const {
	TypePtr type = ::getFieldType(parsedFields, getField());
	if (type.get() == nullptr)
		type = defaultType;

	if (type.get() != nullptr) {
		contexts.top()->ValueTypeContext = type;
		type->beginParseValue();
	}
}

void VariableTypeValueFieldSpec::endParseSetting() const {
	contexts.top()->ValueTypeContext->endParseValue();
}

int VariableTypeValueFieldSpec::getToken() const {
	return VARIABLETYPEVALUEFIELDREFERENCE;
}


void VariableTypeValueFieldSpec::printOn(ostream& strm) const {
	FieldSpec::printOn(strm);
	if (defaultValue.get() != nullptr)
		strm << *defaultValue;
}

void VariableTypeValueFieldSpec::resolveReference() const {
	if (defaultType.get())
		defaultType->resolveReference();
}

////////////////////////////////////////////////////////////////

VariableTypeValueSetFieldSpec::VariableTypeValueSetFieldSpec(const string& nam,
		const string& fieldname,
		bool optional)
	: FieldSpec(nam, optional), fieldName(fieldname) {
}

VariableTypeValueSetFieldSpec::~VariableTypeValueSetFieldSpec() {
}

bool VariableTypeValueSetFieldSpec::hasDefault() const {
	return defaultValueSet.get() != nullptr;
}

void VariableTypeValueSetFieldSpec::EstablishFieldRelation(FieldSpecsList* specs) {
	for (size_t i = 0; i < specs->size(); ++i) {
		FieldSpec& spec = *(*specs)[i];
		if (spec.getName() == fieldName&& dynamic_cast<TypeFieldSpec*>(&spec)) {
			return;
		}
	}

	cerr << StdError(Fatal) << "Invalid Object Class Definition at Field : "<< fieldName << nl;
}

void VariableTypeValueSetFieldSpec::beginParseSetting(FieldSettingList* parsedFields) const {
	TypePtr type = ::getFieldType(parsedFields, getField());

	if (type.get() != nullptr) {
		contexts.top()->ValueTypeContext = type;
		type->beginParseValueSet();
	}
}

void VariableTypeValueSetFieldSpec::endParseSetting() const {
}

int VariableTypeValueSetFieldSpec::getToken() const {
	return VARIABLETYPEVALUESETFIELDREFERENCE;
}

void VariableTypeValueSetFieldSpec::printOn(ostream& strm) const {
	FieldSpec::printOn(strm);
	if (defaultValueSet.get() != nullptr)
		strm << *defaultValueSet;
}

void VariableTypeValueSetFieldSpec::resolveReference() const {
	if (defaultValueSet.get())
		defaultValueSet->resolveReference();
}

/////////////////////////////////////////////////////////////////////////////////

ObjectFieldSpec::ObjectFieldSpec(const string& nam,	 DefinedObjectClass* oclass, bool optional)
	: FieldSpec(nam, optional),objectClass(oclass) {
}

ObjectFieldSpec::~ObjectFieldSpec() {
}

bool ObjectFieldSpec::hasDefault() const {
	return obj.get() != nullptr;
}


string ObjectFieldSpec::getField() const {
	return objectClass->getName();
}

void ObjectFieldSpec::setDefault(InformationObjectPtr dftObj) {
	obj = dftObj;
}


void ObjectFieldSpec::beginParseSetting(FieldSettingList*) const {
	objectClass->beginParseObject();
}

void ObjectFieldSpec::endParseSetting() const {
	objectClass->endParseObject();
}

int ObjectFieldSpec::getToken() const {
	return OBJECTFIELDREFERENCE;
}

void ObjectFieldSpec::printOn(ostream& strm) const {
	FieldSpec::printOn(strm);
	if (obj.get())
		strm << *obj;
}

void ObjectFieldSpec::resolveReference() const {
	objectClass->resolveReference();
}

/////////////////////////////////////////////////////////////////////////////////


ObjectSetFieldSpec::ObjectSetFieldSpec(const string& nam, DefinedObjectClassPtr oclass,  bool optional)
	: FieldSpec(nam, optional),objectClass(oclass) {
}


ObjectSetFieldSpec::~ObjectSetFieldSpec() {
}

bool ObjectSetFieldSpec::hasDefault() const {
	return objSet.get() != nullptr;
}


string ObjectSetFieldSpec::getField() const {
	return objectClass->getName();
}

void ObjectSetFieldSpec::beginParseSetting(FieldSettingList*) const {
	objectClass->beginParseObjectSet();
}

void ObjectSetFieldSpec::endParseSetting() const {
}

int ObjectSetFieldSpec::getToken() const {
	return OBJECTSETFIELDREFERENCE;
}

void ObjectSetFieldSpec::printOn(ostream& strm) const {
	FieldSpec::printOn(strm);
	if (objSet.get())
		strm << *objSet;
}

void ObjectSetFieldSpec::resolveReference() const {
	objectClass->resolveReference();
}

void ObjectSetFieldSpec::FwdDeclare(ostream& fwd) const {
	if (objectClass->getName() == "ERROR") {
		// ERROR is defined to 0 when <windows.h> is included,
		// undefine it.
		fwd << "#undef ERROR" << nl;
	}
	fwd << "class " << makeCppName(objectClass->getReferenceName()) << ";" << nl << nl;
}

void ObjectSetFieldSpec::generate_info_type_constructor(ostream& cxx) const {
	cxx << "  m_" << getIdentifier() << "= nullptr;" << nl;
}

void ObjectSetFieldSpec::generate_info_type_memfun(ostream& hdr) const {
	const string cppReferenceName = makeCppName(objectClass->getReferenceName());
	hdr  << "const " << cppReferenceName <<"* get_" << getIdentifier()
		 << "() const { return m_" << getIdentifier() << "; }" << nl;
	// default Object set not supported;
}

void ObjectSetFieldSpec::generate_info_type_mem(ostream& hdr) const {
	const string cppReferenceName = makeCppName(objectClass->getReferenceName());
	hdr  << "const " << cppReferenceName <<"* m_" << getIdentifier() << ";" << nl;
	// default Object set not supported;
}

void ObjectSetFieldSpec::generate_value_type(ostream& hdr) const {
	const string cppReferenceName = makeCppName(objectClass->getReferenceName());
	hdr  << "const "<< cppReferenceName << "* get_" << getIdentifier() << "() const { return second->get_"
		 <<  getIdentifier() << "(); }" << nl;
}

void ObjectSetFieldSpec::generateTypeField(const string& templatePrefix, const string& classNameString,	const TypeBase* keyType, const string& objClassName,
		ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {
	const string cppReferenceName = makeCppName(objectClass->getReferenceName());
	hdr << "    const " << cppReferenceName << "* get_" << getIdentifier()
		<< "(const " << keyType->getTypeName() << "& key) const;" << nl;

	cxx << templatePrefix
		<< "const "<< cppReferenceName << "* " << classNameString
		<< "::get_" << getIdentifier() << "(const " << keyType->getTypeName() <<"& key) const {" << nl;
	cxx	<< "  if (objSet) {" << nl;
	cxx	<< "    " << objClassName << "::const_iterator it = objSet->find(key);" << nl;
	cxx	<< "    if (it != objSet->end())" << nl;
	cxx	<< "      return it->get_Errors();" << nl;
	cxx	<< "  }" << nl;
	cxx	<< "  return nullptr;" << nl;
	cxx	<< "}\n" << nl;
}

///////////////////////////////////////////////////////////////////////////////

void ObjectClassBase::setName(const string& nam) {
	name = nam;
	cppname = name;
	str_replace(cppname, "-", "_");
}

int ObjectClassBase::getFieldToken(const char* fieldname) const {
	const FieldSpec* spec;
	if ((spec = getField(fieldname)))
		return spec->getToken();
	// FIXME FIXME FIXME
	// Below is an hack for forward reference of  CLASS &fields.
	// See ACSE-1: MECHANISM-NAME.&id is referenced while MECANISM-NAME is not yet parsed
	// This should be repalced by a fixup phase on the discovery of the target CLASS
	// i.e MECANISM-NAME in the sample.
	//
	if (fieldname[0] == '&' && islower(fieldname[1]))
		return FIXEDTYPEVALUEFIELDREFERENCE;
	if (fieldname[0] == '&' && isupper(fieldname[1]))
		return TYPEFIELDREFERENCE;

	return -1;
}

//////////////////////////////////////////////////////////////////////////////
ObjectClassDefn::ObjectClassDefn()
	: fieldSpecs(nullptr) {
}

ObjectClassDefn::~ObjectClassDefn() {
}

void ObjectClassDefn::setFieldSpecs(auto_ptr<FieldSpecsList> list) {
	fieldSpecs = list;
	for (size_t i = 0; i < fieldSpecs->size(); ++i)
		(*fieldSpecs)[i]->EstablishFieldRelation(fieldSpecs.get());
}

FieldSpec* ObjectClassDefn::getField(const string& fieldName) {
	FieldSpec* result = findWithName(*fieldSpecs, fieldName).get();
	if (result)
		return result;

	cerr << "Undefined Object Class Field : "<< getName() << '.'<< fieldName  << nl;
	return nullptr;
}

const FieldSpec* ObjectClassDefn::getField(const string& fieldName) const {
	FieldSpec* result = findWithName(*fieldSpecs, fieldName).get();
	if (result)
		return result;

	cerr << "Undefined Object Class Field : "<< getName() << '.'<< fieldName  << nl;
	return nullptr;
}

void ObjectClassDefn::setWithSyntaxSpec(TokenGroupPtr list) {
	withSyntaxSpec = list;
	bool result = true;

	if (list.get() == nullptr)
		return;

	for (size_t i = 0; i < withSyntaxSpec->size(); ++i)
		if (!(*withSyntaxSpec)[i].ValidateField(fieldSpecs.get()))
			result = false;

	if (result == false)
		exit(1);
}


bool ObjectClassDefn::VerifyDefaultSyntax(FieldSettingList* fieldSettings) const {
	size_t fieldIndex=0, settingIndex=0;
	assert(fieldSettings);
	while (fieldIndex < fieldSpecs->size()&& settingIndex < fieldSettings->size()) {
		if ((*fieldSpecs)[fieldIndex]->getName() == (*fieldSettings)[settingIndex]->getName()) {
			fieldIndex++;
			settingIndex++;
			continue;
		} else if ((*fieldSpecs)[fieldIndex]->isOptional() || (*fieldSpecs)[fieldIndex]->hasDefault()) {
			fieldIndex++;
			continue;
		} else {
			cerr << StdError(Fatal) << "Unrecognized field name : "
				 << (*fieldSettings)[settingIndex]->getName() <<  nl;
			exit(1);
		}
	}
	return true;
}

TokenGroupPtr ObjectClassDefn::getWithSyntax() const {
	return withSyntaxSpec;
}

void ObjectClassDefn::PreParseObject() const {
	if (withSyntaxSpec.get())
		withSyntaxSpec->preMakeDefaultSyntax(nullptr);
}

void ObjectClassDefn::beginParseObject() const {
	contexts.top()->classStack->push(const_cast<ObjectClassDefn*>(this));
	PreParseObject();
	contexts.top()->BraceTokenContext = OBJECT_BRACE;
}

void ObjectClassDefn::endParseObject() const {
	contexts.top()->BraceTokenContext = '{';
//  classStack->Pop();
}

void ObjectClassDefn::beginParseObjectSet() const {
	contexts.top()->classStack->push(const_cast<ObjectClassDefn*>(this));
	PreParseObject();
	contexts.top()->BraceTokenContext = OBJECTSET_BRACE;
	contexts.top()->InObjectSetContext++;
}

void ObjectClassDefn::endParseObjectSet() const {
	if (--contexts.top()->InObjectSetContext == 0)
		contexts.top()->BraceTokenContext = '{';
	else
		contexts.top()->BraceTokenContext = OBJECT_BRACE;

	if (withSyntaxSpec.get())
		withSyntaxSpec->cancelMakeDefaultSyntax();
}

void ObjectClassDefn::printOn(ostream& strm) const {
	strm << name << "\t::= CLASS\n{" << nl;
	size_t i;
	for (i = 0 ; i < fieldSpecs->size(); ++i) {
		strm << '\t' << *(*fieldSpecs)[i];
		if (i != fieldSpecs->size()-1)
			strm << ',';
		strm << nl;
	}
	strm << "}" << nl;
	if (withSyntaxSpec.get()) {
		strm << "WITH SYNTAX" << nl
			 << "{" << nl
			 << *withSyntaxSpec
			 << "}" << nl;
	}
}

void ObjectClassDefn::resolveReference() const {
	for_each(fieldSpecs->begin(), fieldSpecs->end(), boost::mem_fn(&FieldSpec::resolveReference));
}

void ObjectClassDefn::ResolveKey() {
	if (!keyType.get()) {
		find_if(fieldSpecs->begin(), fieldSpecs->end(),
				boost::bind(&FieldSpec::getKey, _1, boost::ref(keyType), boost::ref(keyName)));
	}
}

void ObjectClassDefn::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream&) {
	const string& ocName = getName();
	if (ocName == "TYPE-IDENTIFIER" || ocName == "ABSTRACT-SYNTAX" || ocName == "OPEN")
		return;

	const string className = makeCppName(getName());

	hdr  << nl;
	hdr  << "//4" << nl;
	hdr  << "// " << getName()  << nl;
	hdr  << "//" << nl;

	fwd << nl << "class " << className << ";" << TAB << TAB << "//4 ObjectClass " << nl;


	size_t i;
	for (i = 0; i < fieldSpecs->size(); ++i) {
		stringstream strm;
		(*fieldSpecs)[i]->FwdDeclare(strm);
		string str = strm.str();
		if (str.find(getName()) != -1)
			continue;
		fwd << str;
	}

	ResolveKey();


	hdr << "class "  << dllMacroAPI << " " << className  << " {" << nl
		<< "public:" << nl;
	hdr << tab;
	if  (keyType.get()) {
		hdr << "typedef "<< keyType->getTypeName() << " key_type;" << nl;
	} else {
		hdr << "typedef "<< keyName << " key_type;" << nl;
	}
	hdr << "class info_type {" << nl
		<< "public:" << nl << tab;

	if (fieldSpecs->size()) {
		hdr << "info_type();" << nl;
		cxx << className << "::info_type::info_type()" << nl
			<< "{" << nl;

		for_each(fieldSpecs->begin(), fieldSpecs->end(),
				 boost::bind(&FieldSpec::generate_info_type_constructor, _1, boost::ref(cxx)) );

		cxx << bat << "}\n" << nl;
	}

	for_each(fieldSpecs->begin(), fieldSpecs->end(),
			 boost::bind(&FieldSpec::generate_info_type_memfun, _1, boost::ref(hdr)) );

	hdr << bat << "protected:" << nl << tab;

	for_each(fieldSpecs->begin(), fieldSpecs->end(),
			 boost::bind(&FieldSpec::generate_info_type_mem, _1, boost::ref(hdr)) );

	hdr << bat << "};" << nl << nl;

	hdr << "typedef const info_type* mapped_type;\n" << nl
		<< "class value_type : public ASN1_STD pair<key_type, mapped_type> {" << nl;
	hdr << tab;
	hdr	<< "typedef ASN1_STD pair<key_type, mapped_type> Inherited;" << nl
		<< bat << "public:" << nl << tab
		<< "value_type(const key_type& key, mapped_type mt) : Inherited(key,mt) {}" << nl;

	for_each(fieldSpecs->begin(), fieldSpecs->end(),
			 boost::bind(&FieldSpec::generate_value_type, _1, boost::ref(hdr)) );

	hdr << bat << "};" << nl << nl;

	hdr << "typedef value_type& reference;" << nl
		<< "typedef const value_type& const_reference;" << nl
		<< "typedef value_type* pointer;" << nl
		<< "typedef const value_type* const_pointer;" << nl
		<< "typedef ASN1::AssocVector<key_type, mapped_type> map_type;\n" << nl;

	hdr << bat << "private:" << nl << tab;

	hdr << bat << "#if defined(HP_aCC_RW)" << nl << tab
		<< "typedef ASN1_STD bidirectional_iterator<value_type> my_iterator_traits;" << nl
		<< "typedef ASN1_STD bidirectional_iterator<const value_type> my_const_iterator_traits;" << nl
		<< bat << "#else" << nl << tab
		<< "typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, value_type> my_iterator_traits;" << nl
		<< "typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, const value_type> my_const_iterator_traits;" << nl
		<< bat << "#endif" << nl << tab
		<< bat << "public:" << nl << nl << tab;

	hdr << "class iterator : public my_iterator_traits {" << nl
		<< "public:" << nl
		<< "    iterator() {}" << nl
		<< "    iterator(map_type::iterator i) : itsIter(i) {}" << nl
		<< "    map_type::iterator base()   const { return itsIter; }" << nl
		<< "    const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }" << nl
		<< "    const_pointer operator->()  const { return static_cast<const_pointer>(&*itsIter); }" << nl
		<< "    iterator& operator++()           { ++itsIter; return *this; }" << nl
		<< "    iterator& operator--()           { --itsIter; return *this; }" << nl
		<< "    iterator operator++(int)         { iterator t(*this); ++itsIter; return t; }" << nl
		<< "    iterator operator--(int)         { iterator t(*this); --itsIter; return t; }\n" << nl
		<< "    bool operator==(const iterator& r) const    { return itsIter == r.itsIter; }" << nl
		<< "    bool operator!=(const iterator& r) const    { return itsIter != r.itsIter; }" << nl
		<< "private:" << nl
		<< "    map_type::iterator itsIter;" << nl
		<< "};" << nl << nl

		<< "class const_iterator : public my_const_iterator_traits {" << nl
		<< "public:" << nl
		<< "    const_iterator() {}" << nl
		<< "    const_iterator(" << className << "::iterator i) : itsIter(i.base()) {}" << nl
		<< "    const_iterator(map_type::const_iterator i) : itsIter(i) {}" << nl
		<< "    map_type::const_iterator base() const { return itsIter; }\n" << nl
		<< "    const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }" << nl
		<< "    const_pointer operator->() const { return static_cast<const_pointer>(&*itsIter); }\n" << nl
		<< "    const_iterator& operator++()          { ++itsIter; return *this; }" << nl
		<< "    const_iterator& operator--()          { --itsIter; return *this; }" << nl
		<< "    const_iterator operator++(int)        { const_iterator t(*this); ++itsIter; return t; }" << nl
		<< "    const_iterator operator--(int)        { const_iterator t(*this); --itsIter; return t; }\n" << nl
		<< "    bool operator==(const const_iterator& r) const    { return itsIter == r.itsIter; }" << nl
		<< "    bool operator!=(const const_iterator& r) const    { return itsIter != r.itsIter; }" << nl
		<< "private:" << nl
		<< "    map_type::const_iterator itsIter;" << nl
		<< "};" << nl << nl;

	hdr << bat << "//#if defined(BOOST_NO_STD_ITERATOR) || defined (BOOST_MSVC_STD_ITERATOR)" << nl
		<< "//    typedef reverse_bidirectional_iterator<iterator, value_type> reverse_iterator;" << nl
		<< "//    typedef reverse_bidirectional_iterator<const_iterator, const value_type> const_reverse_iterator;" << nl
		<< "//#else" << nl
		<< "//    typedef reverse_iterator<iterator, value_type> reverse_iterator;" << nl
		<< "//    typedef reverse_iterator<const_iterator, const value_type> const_reverse_iterator;" << nl
		<< "//#endif\n" << nl << tab;

	hdr	<< "typedef map_type::key_compare key_compare;" << nl
		<< "typedef map_type::difference_type difference_type;" << nl
		<< "typedef map_type::size_type size_type;\n" << nl
		<< className << "() {}" << nl

		<< "template <class InputIterator>" << nl
		<< className << "(InputIterator first, InputIterator last) : rep(first, last) {}" << nl
		<< className << "(const " << className << "& that) : rep(that.rep) {}" << nl << nl
		<< className << "& operator=(const " << className << "& that) { " << className << " tmp(that); swap(tmp);  return *this; }\n" << nl

		<< "// iterators" << nl
		<< "iterator begin() { return iterator(rep.begin()); }" << nl
		<< "const_iterator begin() const { return const_iterator(rep.begin()); }" << nl
		<< "iterator end() { return iterator(rep.end()); }" << nl
		<< "const_iterator end() const { return const_iterator(rep.end()); }\n" << nl
		<< "//    reverse_iterator rbegin() { return reverse_iterator(end()); }" << nl
		<< "//    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }" << nl
		<< "//    reverse_iterator rend() { return reverse_iterator(begin()); }" << nl
		<< "//    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }" << nl << nl

		<< "// capacity" << nl
		<< "bool empty() const { return rep.empty(); }" << nl
		<< "size_type size() const { return rep.size(); }" << nl << nl

		<< "// modifiers" << nl
		<< "ASN1_STD pair<iterator, bool> insert(const value_type& x) {" << nl
		<< "    ASN1_STD pair<map_type::iterator, bool> r = rep.insert(x);" << nl
		<< "    return ASN1_STD pair<iterator, bool>(r.first, r.second);" << nl
		<< "}" << nl
		<< "iterator insert(iterator position, const value_type& x) { return iterator(rep.insert(position.base(), x)); }" << nl
		<< "void insert(const_iterator first, const_iterator last)  { rep.insert(first.base(), last.base()); }" << nl
		<< "void erase(iterator position) { rep.erase(position.base()); }" << nl
		<< "void erase(const key_type& key) { rep.erase(key); }" << nl
		<< "void erase(iterator first, iterator last) { rep.erase(first.base(), last.base()); }" << nl
		<< "void swap(" << className << "& that) { rep.swap(that.rep); }" << nl
		<< "void clear() { rep.clear(); }" << nl
		<< "key_compare key_comp() const { return rep.key_comp(); }" << nl << nl

		<< "// operations" << nl
		<< "iterator find(const key_type& key) { return iterator(rep.find(key)); }" << nl
		<< "const_iterator find(const key_type& key) const { return const_iterator(rep.find(key)); }" << nl
		<< "size_type count(const key_type& key) const { return rep.count(key); }" << nl
		<< bat << "private:" << nl << tab
		<< "map_type rep;" << nl
		<< bat << "};" << nl;
}
/////////////////////////////////////////////////////////////
ImportedObjectClass::ImportedObjectClass(const string& modName, const string& nam, ObjectClassBase* ref)
	: DefinedObjectClass(nam, ref), moduleName(modName) {
	module = findWithName(Modules, modName);
}

/////////////////////////////////////////////////////////////
DefinedObjectClass::DefinedObjectClass(ObjectClassBase* ref)
	: reference(ref) {
}


DefinedObjectClass::DefinedObjectClass(const string& nam, ObjectClassBase* ref)
	: referenceName(nam), reference(ref) {
	if (!ref) {
		if (nam == "ABSTRACT-SYNTAX") {
			ObjectClassBasePtr typeIdentifier = UsefulModule->findObjectClass(nam);
			reference = typeIdentifier.get();
		} else if (nam == "TYPE-IDENTIFIER") {
			ObjectClassBasePtr typeIdentifier = UsefulModule->findObjectClass(nam);
			reference = typeIdentifier.get();
		}
	}
}

ObjectClassBase* DefinedObjectClass::getReference() {
	if (reference == nullptr)
		reference = contexts.top()->Module->findObjectClass(referenceName).get();
	return reference;
}

const ObjectClassBase* DefinedObjectClass::getReference() const {
	resolveReference();
//	assert(reference);
	return reference;
}

FieldSpec* DefinedObjectClass::getField(const string& fieldName) {
	ObjectClassBase* ref = getReference();
	if (ref)
		return ref->getField(fieldName);
	return nullptr;
}

const FieldSpec* DefinedObjectClass::getField(const string& fieldName) const {
	const ObjectClassBase* ref = getReference();
	if (ref)
		return ref->getField(fieldName);
	return nullptr;
}

bool DefinedObjectClass::VerifyDefaultSyntax(FieldSettingList* fieldSettings) const {
	return getReference()->VerifyDefaultSyntax(fieldSettings);
}


bool DefinedObjectClass::hasLiteral(const string& str) const {
	return getReference()->hasLiteral(str);
}


TokenGroupPtr DefinedObjectClass::getWithSyntax() const {
	return getReference()->getWithSyntax();
}

void DefinedObjectClass::PreParseObject() const {
	getReference()->PreParseObject();
}

void DefinedObjectClass::beginParseObject() const {
	const ObjectClassBase* r = getReference();
	r->beginParseObject();
}

void DefinedObjectClass::endParseObject() const {
	getReference()->endParseObject();
}

void DefinedObjectClass::beginParseObjectSet() const {
	getReference()->beginParseObjectSet();
}

void DefinedObjectClass::endParseObjectSet() const {
	getReference()->endParseObjectSet();
}

void DefinedObjectClass::printOn(ostream& strm) const {
	if (name.size() != 0)
		strm << name << " ::= ";
	strm << referenceName;
}

void DefinedObjectClass::resolveReference() const {
	if (reference == nullptr) {
		reference = contexts.top()->Module->findObjectClass(referenceName).get();
		ImportedObjectClass* ioc = dynamic_cast<ImportedObjectClass*>(reference);
		if (ioc != nullptr) {
			const ModuleDefinitionPtr& m = ioc->getModule();
			reference = m->findObjectClass(referenceName).get();
			if (reference != nullptr) {
				ObjectClassDefn* ocd = dynamic_cast<ObjectClassDefn*>(reference);
				if (ocd)
					ocd->ResolveKey();
			} else {
				//load ObjectClassDefinition from the Module
				string modulePath = ioc->getModule()->getModulePath();
				if (!modulePath.empty()) {
					clog << "Processing of " << modulePath << endl;

					FILE* fd = fopen(modulePath.c_str(),"r");
					if (!fd) {
						cerr << "ASN1 compiler: cannot open \"" << modulePath  << '"'<< endl;
					} else {
						ParserContext context(fd);
						contexts.push(&context);
						yylex_init(&context.lexer);
						yyset_in(fd, context.lexer);
						yyparse(context.lexer,&context);
						yylex_destroy (context.lexer);
						reference = ioc->getModule()->findObjectClass(referenceName).get();
						ObjectClassDefn* ocd = dynamic_cast<ObjectClassDefn*>(reference);
						if (ocd)
							ocd->ResolveKey();
						contexts.pop();
						fclose(fd);
					}
				}
			}
		}
	}
}

TypeBase* DefinedObjectClass::getFieldType(const string& fieldName) {
	return getReference()->getFieldType(fieldName);
}

const TypeBase* DefinedObjectClass::getFieldType(const string& fieldName) const {
	return getReference()->getFieldType(fieldName);
}

void DefinedObjectClass::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	fwd << "typedef ASN1::" << getReference()->getCppName() << " " << getCppName() << ";" << endl;
}

//////////////////////////////////////////////////////////////////

void Literal::printOn(ostream& strm) const {
	strm << name;
	// use skipws flag to indicate whether to output nl or not
	if (((strm.flags()& ios::skipws) == 0)&& name == ",")
		strm << nl;
}


TokenOrGroupSpec::MakeDefaultSyntaxResult  Literal::MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList*) {
	if (token->MatchLiteral(name))
		return CONSUMED_AND_EXHAUSTED;
	else
		return FAIL;
}


/////////////////////////////////////////////////////////////////////

void PrimitiveFieldName::printOn(ostream& strm) const {
	strm << name;
	// use skipws flag to indicate whether to output nl or not
	if ((strm.flags()& ios::skipws) == 0)
		strm  << nl;
}

bool PrimitiveFieldName::ValidateField(FieldSpecsList* fields) {
	if (field = findWithName(*fields, name).get())
		return true;

	cerr << "Illegal field name \"" << name << "\"" << nl;
	return false;
}

void PrimitiveFieldName::preMakeDefaultSyntax(FieldSettingList* settings) {
	assert(field);
	field->beginParseSetting(settings);
}

void  PrimitiveFieldName::cancelMakeDefaultSyntax(int no) const {
	assert(field);
	field->endParseSetting();
}

TokenOrGroupSpec::MakeDefaultSyntaxResult
PrimitiveFieldName::MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* settings) {
	assert(field);
	TokenOrGroupSpec::MakeDefaultSyntaxResult result = FAIL;
	FieldSettingPtr setting = token->MatchSetting(name);
	if (setting.get() != nullptr) {
		settings->push_back(FieldSettingPtr(setting));
		result =  CONSUMED_AND_EXHAUSTED;
	}

	field->endParseSetting();
	return result;
}

///////////////////////////////////////////////////////////////////

void TokenGroup::printOn(ostream& strm) const {
	if (optional) {
		strm << "[" ;
		strm.setf(ios::skipws);     // use skipws flag to indicate not to output nl for PrimitiveField or ','
	}

	for (size_t i = 0; i < tokenOrGroupSpecList.size(); ++i) {
		if ((strm.flags()& ios::skipws) ==0&& i ==0)
			strm << '\t';
		strm << *tokenOrGroupSpecList[i];
		if (i != tokenOrGroupSpecList.size()-1)
			strm << '\t';
	}

	if (optional)
		strm << "]" << nl;

	strm.unsetf(ios::skipws);
}

bool TokenGroup::ValidateField(FieldSpecsList* fields) {
	for (size_t i = 0; i < tokenOrGroupSpecList.size(); ++i)
		if (!tokenOrGroupSpecList[i]->ValidateField(fields))
			return false;
	return true;

}

TokenOrGroupSpec::MakeDefaultSyntaxResult  TokenGroup::MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* settings) {
	assert(token);
	assert(settings);

	MakeDefaultSyntaxResult result = NOT_CONSUMED;
	while ( cursor < tokenOrGroupSpecList.size()) {
		if ((result = tokenOrGroupSpecList[cursor]->MakeDefaultSyntax(token, settings)) == NOT_CONSUMED) {
			cursor++;
			continue;
		}
		break;
	}

	switch (result) {
	case FAIL:
		if (optional&& cursor == 0)
			result = NOT_CONSUMED;
		break;
	case CONSUMED_AND_EXHAUSTED:
		if (++cursor < tokenOrGroupSpecList.size()) {
			result = CONSUMED_AND_NOT_EXHAUSTED;
			tokenOrGroupSpecList[cursor]->preMakeDefaultSyntax(settings);
		}
		break;
	case NOT_CONSUMED: // exhausted, i.e. cursor == tokenOrGroupSpecList.getSize()
		break;
	case CONSUMED_AND_NOT_EXHAUSTED    :
		break;
	}


	return result;
}

TokenGroup::TokenGroup(const TokenGroup& other)
	: tokenOrGroupSpecList(other.tokenOrGroupSpecList), optional(other.optional), cursor(other.cursor) {
}

void TokenGroup::preMakeDefaultSyntax(FieldSettingList* settings) {
	if (cursor < tokenOrGroupSpecList.size())
		tokenOrGroupSpecList[cursor]->preMakeDefaultSyntax(settings);
}

void TokenGroup::cancelMakeDefaultSyntax(int no) const {
	if (tokenOrGroupSpecList.size())
		tokenOrGroupSpecList[no]->cancelMakeDefaultSyntax(1);
}


bool TokenGroup::hasLiteral(const string& str) const {
	for (size_t i = 0; i < tokenOrGroupSpecList.size(); ++i) {
		if (tokenOrGroupSpecList[i]->hasLiteral(str))
			return true;
	}
	return false;
}

void TokenGroup::Reset() {
	cursor = 0;
	for_all(tokenOrGroupSpecList, boost::mem_fn(&TokenOrGroupSpec::Reset));
}

/////////////////////////////////////////////////////////////////////////////////

FieldSetting::FieldSetting(const string& fieldname,  auto_ptr<Setting> aSetting)
	: name(fieldname), setting(aSetting) {
	identifier = name.c_str()+1;
}

FieldSetting::~FieldSetting() {
}

void FieldSetting::printOn(ostream& strm) const {
	strm << name << '\t' << *setting;
}


void FieldSetting::generateCplusplus(const string& prefix, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag) {
	setting->generateCplusplus(prefix, identifier, fwd, hdr, cxx, inl, flag);
}

void FieldSetting::generateInitializationList(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	stringstream tmp;
	setting->generateInitializationList(fwd, hdr, tmp, inl);
	if (!tmp.str().empty()) {
		cxx << identifier << "(" << tmp.str() << ")";
	}
	tmp << ends;
}

void FieldSetting::generateInfo(ostream& hdr) {
	setting->generateInfo(identifier, hdr);
}

bool FieldSetting::isExtendable() const {
	return setting->isExtendable();
}

void FieldSetting::generateInstanceCode(const string& prefix, ostream& cxx) const {
	setting->generateInstanceCode(prefix + identifier + ".", cxx);
}

/////////////////////////////////////////////////////////////////////////////////

InformationObject::~InformationObject() {
}

const string& InformationObject::getClassName() const {
	return getObjectClass()->getName();
}

void InformationObject::PrintBase(ostream& strm) const {
	if (name.size()) {
		strm << name << ' ';
		if (parameters.get())
			strm << *parameters << ' ';
		strm << getClassName() << " ::= ";
	}
}

void InformationObject::setParameters(auto_ptr<ParameterList> list) {
	parameters = list;
}

/////////////////////////////////////////////////////////////////////////////////

DefinedObject::DefinedObject(const string& nam, const InformationObject* ref)
	: referenceName(nam), reference(ref) {
	name = nam;
}


const InformationObject* DefinedObject::getReference() const {
	if (reference == nullptr)
		reference = contexts.top()->Module->findInformationObject(referenceName);
	if (reference == nullptr) {
		cerr << StdError(Fatal) << "Invalid Object : " << getName();
		exit(1);
	}
	return reference;
}

const ObjectClassBase* DefinedObject::getObjectClass() const {
	if (contexts.top()->DummyParameters) {
		Parameter* param = contexts.top()->DummyParameters->getParameter(getName().c_str());
		if ((param != nullptr)&& dynamic_cast<ObjectParameter*>(param)) {
			ObjectParameter* p = (ObjectParameter*) param;
			return p->getGovernor();
		}
	}
	return getReference()->getObjectClass();
}

bool DefinedObject::VerifyObjectDefinition() {
	const InformationObject* obj = getReference();
	if (obj&& getClassName() == obj->getClassName())
		return true;
	cerr << StdError(Fatal) << "Unmatched Object Definition : " << getName();
	return false;
}

const Setting* DefinedObject::getSetting(const string& fieldname) const {
	return getReference()->getSetting(fieldname);
}

void DefinedObject::printOn(ostream& strm) const {
	PrintBase(strm);
	strm << referenceName << nl;
}

void DefinedObject::generateInstanceCode(ostream& cxx) const {
	if (dynamic_cast<const DefaultObjectDefn*>(getReference()))
		cxx << "get_" << makeCppName(referenceName) << "().make()";
	else
		reference->generateInstanceCode(cxx);
}
/////////////////////////////////////////////////////////////////////////////////

void ImportedObject::generateInstanceCode(ostream& cxx) const {
	cxx << toLower(makeCppName(moduleName)) << "->get_" << makeCppName(referenceName) << "().make()";
}

/////////////////////////////////////////////////////////////////////////////////

bool DefaultObjectDefn::setObjectClass(const ObjectClassBase* definedClass) {
	referenceClass = definedClass;
	return VerifyObjectDefinition();
}

bool DefaultObjectDefn::VerifyObjectDefinition() {
	assert(referenceClass);
	return referenceClass->VerifyDefaultSyntax(settings.get());
}

const Setting* DefaultObjectDefn::getSetting(const string& fieldname) const {

	const FieldSetting*  result = findWithName(*settings, fieldname).get();

	if (result) {
		return  result->getSetting();
	}

	return nullptr;
}

void DefaultObjectDefn::printOn(ostream& strm) const {
	PrintBase(strm);
	strm << "{" << nl;
	for (size_t i = 0; i < settings->size(); ++i)
		strm << '\t' << *(*settings)[i] << nl;
	strm << "}" << nl;
}


void DefaultObjectDefn::generateCplusplus(ostream& fwd, ostream& hdr , ostream& cxx, ostream& inl) {
	stringstream tmphdr;
	tmphdr << setprecision(8);
	unsigned flags =0;
	size_t i;
	string prefix("Module::");
	prefix += name;
	for (i = 0; i < settings->size(); ++i)
		(*settings)[i]->generateCplusplus(prefix, fwd, tmphdr, cxx, inl, flags);


	if (!tmphdr.str().empty()) {
		int has_type_setting = (flags& Setting::has_type_setting);
		int has_objectSet_setting = (flags& Setting::has_objectSet_setting);

		const	string& className = makeCppName(referenceClass->getName());

		string keyName = referenceClass->getKeyName().substr(1);
		hdr << "    class " << name << " {" << nl
			<< "    public:" << nl
			<< "        " << name << "();" << nl
			<< tmphdr.str()
			<< "        " << className << "::value_type make() const" << nl
			<< "        { return " << className << "::value_type(" << keyName << ",&info ); }" << nl
			<< "    private:" << nl;

		tmphdr << ends;

		if (has_type_setting || has_objectSet_setting) {
			hdr  << "class Info : public " << className << "::info_type" << " {" << nl
				 << "public:" << nl;

			if (has_objectSet_setting)
				hdr << "Info(" << name << "* parent)" << nl;
			else
				hdr << "Info()" << nl;
			hdr <<  "{" << nl
				<< setprecision(16);

			for_each(settings->begin(), settings->end(), boost::bind(&FieldSetting::generateInfo, _1, boost::ref(hdr)));
			hdr << "}" << nl;
			hdr  << "};" << nl;
		} else {
			hdr << "        typedef " << className << "::info_type Info;" << nl;
		}
		hdr << "        Info info;" << nl
			<< "    }; // class "<< name  << nl;
		cxx << "Module::" << name << "::" << name << "()";

		bool hasInitizationList = false;
		for (i = 0 ; i < settings->size(); ++i) {
			stringstream tmp;
			(*settings)[i]->generateInitializationList(fwd, hdr, tmp, inl);
			if (!tmp.str().empty()) {
				if (!hasInitizationList)
					cxx << "\n  : " << tmp.str();
				else {
					cxx << ", " << tmp.str();
					hasInitizationList = true;
				}
			}
			tmp << ends;
		}

		if (has_objectSet_setting)
			cxx << ", info(this)";

		cxx  << nl	<< "{" << nl << "}" << nl << nl;
	}
}

bool DefaultObjectDefn::isExtendable() const {
	if (find_if(settings->begin(), settings->end(),
				boost::mem_fn(&FieldSetting::isExtendable))
			!= settings->end())
		return true;

	return false;
}

void DefaultObjectDefn::generateInstanceCode(ostream& cxx) const {
	string nam("  m_");
	nam += makeCppName(name);
	nam += ".";

	if (name.empty())
		return;

	for_each(settings->begin(), settings->end(),
			 boost::bind(&FieldSetting::generateInstanceCode, _1, nam, boost::ref(cxx)) );
}
/////////////////////////////////////////////////////////////////////////////////

ObjectFromObject::ObjectFromObject(InformationObjectPtr referenceObj,  const string& fld)
	: refObj(referenceObj), field(fld) {
}

ObjectFromObject::~ObjectFromObject() {
}

const ObjectClassBase* ObjectFromObject::getObjectClass() const {
	return refObj->getObjectClass();
}


void ObjectFromObject::printOn(ostream& strm) const {
	if (name.size() > 0)
		strm << name << ' ' << refObj->getClassName() << " ::= ";
	strm << *refObj << "." << field;
}

const Setting* ObjectFromObject::getSetting(const string& fieldname) const {
	return refObj->getSetting(fieldname);
}

bool ObjectFromObject::VerifyObjectDefinition() {
	return true;
}

void ObjectFromObject::generateInstanceCode(ostream& ) const {
	// not implemented
}

/////////////////////////////////////////////////////////////////////////////////


FieldSettingPtr SettingToken::MatchSetting(const string& fieldName) {
	return FieldSettingPtr (new FieldSetting(fieldName, setting));
}

/////////////////////////////////////////////////////////////////////////////////

void TypeSetting::generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag) {
	type->setOuterClassName(prefix);
	type->setName(name);
	type->generateCplusplus(fwd, hdr, cxx, inl);
	flag |= has_type_setting;
}

void TypeSetting::generateInfo(const string& name,ostream& hdr) {
	hdr  << "m_" << name << "=&" << name << "::theInfo;" << nl;
}

void TypeSetting::printOn(ostream& strm) const {
	strm << *type;
}


///////////////////////////////////////////////////////////////////////////////

void ValueSetting::generateCplusplus(const string& , const string& name, ostream& fwd, ostream& hdr, ostream& , ostream& , unsigned& flag) {
	if (type->getTypeName() != "ASN1::BOOLEAN")
		hdr  << "const " << type->getTypeName() << ' ' << name << ";" << nl;
	flag |= has_value_setting;
}

void ValueSetting::generateInitializationList(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	if (type->getTypeName() != "ASN1::BOOLEAN")
		value->generateCplusplus(fwd, hdr, cxx, inl);
}


ValueSetting::ValueSetting(TypePtr typeBase, ValuePtr valueBase)
	: type(typeBase), value(valueBase) {
}

ValueSetting::~ValueSetting() {
}

void ValueSetting::printOn(ostream& strm) const {
	if (type.get())
		strm << type->getTypeName() << " : ";
	strm << *value;
}

////////////////////////////////////////////////////////

ValueSetSetting::ValueSetSetting(ValueSetPtr set)
	:valueSet(set) {
}

ValueSetSetting::~ValueSetSetting() {
}

void ValueSetSetting::printOn(ostream& strm) const {
	strm << *valueSet;
}

void ValueSetSetting::generateCplusplus(const string& , const string& , ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag) {
	flag |= has_valueSet_setting;
}

/////////////////////////////////////////////////////////////////////////////////

ObjectSetting::ObjectSetting(InformationObjectPtr obj, ObjectClassBase* objClass)
	: objectClass(objClass), object(obj) {
}

void ObjectSetting::printOn(ostream& strm) const {
	strm << *object;
}

void ObjectSetting::generateCplusplus(const string& , const string& , ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag) {
	flag |= has_object_setting;
}

ObjectSetting::~ObjectSetting() {
}

/////////////////////////////////////////////////////////////////////////////////

ObjectSetSetting::~ObjectSetSetting() {
}

void ObjectSetSetting::printOn(ostream& strm) const {
	strm << *objectSet;
}

void ObjectSetSetting::generateCplusplus(const string& , const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag) {
//  if (!objectSet->isExtendable())
//    hdr << "const ";
	hdr << makeCppName(objectClass->getName()) << " " << name << ";" << nl ;

	flag |= has_objectSet_setting;
}

void ObjectSetSetting::generateInfo(const string& name,ostream& hdr) {
	hdr  << "m_" << name << "=&(parent-> " << name << ");" << nl;
}

void ObjectSetSetting::generateInstanceCode(const string& prefix, ostream& cxx) const {
	objectSet->generateObjectSetInstanceCode(prefix + "insert(", cxx);
}

/////////////////////////////////////////////////////////////////////////////////

InformationObjectSetDefn::InformationObjectSetDefn(const string& nam,
		ObjectClassBasePtr objClass,
		ConstraintPtr set,
		ParameterListPtr list)
	: name(nam),objectClass(objClass), rep(set), parameters(list) {
}

InformationObjectSetDefn::~InformationObjectSetDefn() {
}

const ObjectClassBase* InformationObjectSetDefn::getObjectClass() const {
	return objectClass.get();
}

ValueSetPtr InformationObjectSetDefn::getValueSetFromValueField(const string& field) const {
	return rep->getValueSetFromValueField(field);
}

ValueSetPtr InformationObjectSetDefn::getValueSetFromValueSetField(const string& field) const {
	return rep->getValueSetFromValueSetField(field);
}

ConstraintPtr InformationObjectSetDefn::getObjectSetFromObjectField(const string& field) const {
	return rep->getObjectSetFromObjectField(field);
}

ConstraintPtr InformationObjectSetDefn::getObjectSetFromObjectSetField(const string& field) const {
	return rep->getObjectSetFromObjectSetField(field);
}

void InformationObjectSetDefn::printOn(ostream& strm) const {
	if (name.size()) {
		strm << name << ' ' ;
		if (parameters.get())
			strm << *parameters << ' ';
		strm << *objectClass << " ::= " ;
	}

	strm << '{';
	rep->PrintElements(strm);
	strm << "}" << nl;
}

void InformationObjectSetDefn::generateInstanceCode(ostream& cxx) const {
	if (!hasParameters()) {
		rep->generateObjectSetInstanceCode("  m_" + makeCppName(name)+".insert(",cxx);
	}
}

void InformationObjectSetDefn::generateType(ostream& fwd, ostream& hdr, ostream& cxx, ostream& ) const {
	string typeName = makeCppName(getName());
	string classNameString = typeName;
	string templatePrefix;

	if (parameters.get())
		parameters->generateCplusplus(templatePrefix, classNameString);

	hdr  << nl
		 << "//5" << nl
		 << "// " << classNameString  << nl
		 << "//" << nl << nl;

	string objClassName = makeCppName(getObjectClass()->getReferenceName());

	hdr << templatePrefix
		<< "class " << typeName << " {" << nl
		<< "public:" << nl << tab;
	hdr << "typedef " << objClassName << " element_type;" << nl
		<< typeName << "(ASN1::CoderEnv& env);" << nl;

	cxx  << nl
		 << "//6" << nl
		 << "// " << classNameString  << nl
		 << "//" << nl << nl;

	bool needDeleteObjSet = false;
	cxx << templatePrefix
		<< classNameString << "::" << typeName << "(ASN1::CoderEnv& env)" << " {" << nl;
	needDeleteObjSet = generateTypeConstructor(cxx);
	cxx << bat << "}\n" << nl;
	if (needDeleteObjSet)
		hdr << "    ~" << typeName << "() { delete objSet; }" << nl;

	if (!isExtendable()) hdr << "const ";
	hdr << "element_type* get() const { return objSet; }" << nl;

	if (!isExtendable())
		hdr << "const ";
	hdr << "element_type* operator->() const { return objSet; }" << nl;

	hdr << "bool extensible() const { return ";
	if (isExtendable())
		hdr << "true";
	else
		hdr << "false";
	hdr << "; }" << nl;

	hdr << bat << "private:" << nl << tab;
	if (!isExtendable())
		hdr << "const ";

	hdr << "element_type* objSet;" << nl;
	hdr << bat << "};"  << nl << nl;
}

bool InformationObjectSetDefn::generateTypeConstructor(ostream& cxx) const {
	stringstream tmp;
	rep->generateObjSetAccessCode(tmp);
	if (!tmp.str().empty()) {
		cxx << tmp.str();
		return true;
		tmp << ends;
	}

	cxx << "  ASN1::Module* module = env.find(\"" << contexts.top()->Module->getName() << "\");" << nl
		<< "  if (module)" << nl
		<< "  objSet = &(static_cast<"<< contexts.top()->Module->getCModuleName() << "::Module*>(module)->get_"
		<< makeCppName(getName()) << "());" << nl
		<< "  else" << nl
		<< "    objSet = nullptr;" << nl;
	return false;
}


/////////////////////////////////////////////////////////////////////////////////

DefinedObjectSet::DefinedObjectSet(const string& ref)
	: referenceName(ref)
	, reference(nullptr) {
}

const InformationObjectSet* DefinedObjectSet::getReference() const {
	if (reference == nullptr)
		reference = contexts.top()->Module->findInformationObjectSet(referenceName);
	if (reference == nullptr) {
		cerr << StdError(Fatal) << "Invalid ObjectSet " << getName();
		exit(1);
	}
	return reference;
}


const ObjectClassBase* DefinedObjectSet::getObjectClass() const {
	if (contexts.top()->DummyParameters) {
		Parameter* param = contexts.top()->DummyParameters->getParameter(getName().c_str());
		if (param&& dynamic_cast<ObjectParameter*>(param)) {
			ObjectParameter* p = (ObjectParameter*) param;
			return p->getGovernor();
		}
	}
	return getReference()->getObjectClass();
}

ValueSetPtr DefinedObjectSet::getValueSetFromValueField(const string& field) const {
	return getReference()->getValueSetFromValueField(field);
}

ValueSetPtr DefinedObjectSet::getValueSetFromValueSetField(const string& field) const {
	return getReference()->getValueSetFromValueSetField(field);
}

ConstraintPtr DefinedObjectSet::getObjectSetFromObjectField(const string& field) const {
	return getReference()->getObjectSetFromObjectField(field);
}

ConstraintPtr DefinedObjectSet::getObjectSetFromObjectSetField(const string& field) const {
	return getReference()->getObjectSetFromObjectSetField(field);
}

void DefinedObjectSet::printOn(ostream& strm) const {
	strm << referenceName;
}

bool DefinedObjectSet::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}
/////////////////////////////////////////////////////////////////////////////////

ParameterizedObjectSet::ParameterizedObjectSet(const string& ref,
		ActualParameterListPtr args)
	: DefinedObjectSet(ref), arguments(args) {
}

ParameterizedObjectSet::~ParameterizedObjectSet() {
}

string ParameterizedObjectSet::getName() const {
	stringstream strm;
	strm << referenceName << *arguments;
	return string(strm.str());
}

void ParameterizedObjectSet::printOn(ostream& strm) const {
	strm << referenceName << *arguments;
}

/////////////////////////////////////////////////////////////////////////////////

ObjectSetFromObject::ObjectSetFromObject(InformationObjectPtr obj, const string& fld)
	: refObj(obj), field(fld) {
}

ObjectSetFromObject::~ObjectSetFromObject() {
}

const ObjectClassBase* ObjectSetFromObject::getObjectClass() const {
	return refObj->getObjectClass();
}

string ObjectSetFromObject::getName() const {
	return refObj->getName() + field;
}

ValueSetPtr ObjectSetFromObject::getValueSetFromValueField(const string& fld) const {
	return getRepresentation()->getValueSetFromValueField(fld);
}

ValueSetPtr ObjectSetFromObject::getValueSetFromValueSetField(const string& fld) const {
	return getRepresentation()->getValueSetFromValueSetField(fld);
}

ConstraintPtr ObjectSetFromObject::getObjectSetFromObjectField(const string& fld) const {
	return getRepresentation()->getObjectSetFromObjectField(fld);
}

ConstraintPtr ObjectSetFromObject::getObjectSetFromObjectSetField(const string& fld) const {
	return getRepresentation()->getObjectSetFromObjectSetField(fld);
}

void ObjectSetFromObject::printOn(ostream& strm) const {
	strm << *refObj << '.' << field;
}

Constraint* ObjectSetFromObject::getRepresentation() const {
	if (rep.get() == nullptr) {
		SingleObjectConstraintElement elem(refObj);
		rep = elem.getObjectSetFromObjectSetField(field);
	}
	return rep.get();
}

bool ObjectSetFromObject::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}



/////////////////////////////////////////////////////////////////////////////////

ObjectSetFromObjects::ObjectSetFromObjects(ObjectSetConstraintElementPtr objSet,
		const string& fld)
	: refObjSet(objSet), field(fld) {
}

ObjectSetFromObjects::~ObjectSetFromObjects() {
}

const ObjectClassBase* ObjectSetFromObjects::getObjectClass() const {
	return refObjSet->getObjectClass();
}

string ObjectSetFromObjects::getName() const {
	return refObjSet->getName() + field;
}

ValueSetPtr ObjectSetFromObjects::getValueSetFromValueField(const string& fld) const {
	return getRepresentation()->getValueSetFromValueField(fld);
}

ValueSetPtr ObjectSetFromObjects::getValueSetFromValueSetField(const string& fld) const {
	return getRepresentation()->getValueSetFromValueSetField(fld);
}

ConstraintPtr ObjectSetFromObjects::getObjectSetFromObjectField(const string& fld) const {
	return getRepresentation()->getObjectSetFromObjectField(fld);
}

ConstraintPtr ObjectSetFromObjects::getObjectSetFromObjectSetField(const string& fld) const {
	return getRepresentation()->getObjectSetFromObjectSetField(fld);
}

void ObjectSetFromObjects::printOn(ostream& strm) const {
	strm << *refObjSet << '.' << field;
}

ConstraintPtr ObjectSetFromObjects::getRepresentation() const {
	if (rep.get() == nullptr) {
		rep = refObjSet->getObjectSetFromObjectField(field);
		if (rep.get() == nullptr)
			rep = refObjSet->getObjectSetFromObjectSetField(field);
	}
	return rep;
}

bool ObjectSetFromObjects::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}

void ObjectSetFromObjects::generateObjSetAccessCode(ostream& strm) {
	strm << "  " << makeCppName(refObjSet->getName()) <<  " tmp(env);" << nl
		 << "  if (tmp.get())" << nl
		 << "  {" << nl
		 << "    element_type *tempObjSet = new element_type;" << nl
		 << "    typename " << makeCppName(refObjSet->getName()) << "::element_type::const_iterator first = tmp->begin(), last = tmp->end();" << nl
		 << "    for (; first != last; ++first)" << nl
		 << "    {" << nl
		 << "       const element_type* infoFromObjects = first->get_" << field.substr(1) << "();" << nl
		 << "       if (infoFromObjects)" << nl
		 << "         tempObjSet->insert(infoFromObjects->begin(), infoFromObjects->end());" << nl
		 << "    }" << nl
		 << "    objSet = tempObjSet;" << nl
		 << "  }" << nl;
}



/////////////////////////////////////////////////////////////////////////////////

DefaultSyntaxBuilder::DefaultSyntaxBuilder(TokenGroupPtr tkGrp)
	: tokenGroup(tkGrp)
	, setting(new FieldSettingList) {
}

DefaultSyntaxBuilder::~DefaultSyntaxBuilder() {
}

void DefaultSyntaxBuilder::addToken(DefinedSyntaxToken* token) {
	TokenOrGroupSpec::MakeDefaultSyntaxResult result =
		tokenGroup->MakeDefaultSyntax(token, setting.get());
	if ( result == TokenOrGroupSpec::FAIL || result == TokenOrGroupSpec::NOT_CONSUMED) {
		cerr << StdError(Fatal) << "Invalid Object Definition" << nl;
		exit(1);
	}
	tokenGroup->cancelMakeDefaultSyntax();

}

auto_ptr<FieldSettingList> DefaultSyntaxBuilder::getDefaultSyntax() {
	endSyntaxToken token;
#ifdef FIXME
	// The code below does not work
	if (tokenGroup->MakeDefaultSyntax(&token, setting.get()) == TokenOrGroupSpec::NOT_CONSUMED) {
		tokenGroup->Reset();
		return setting;
	}
	cerr << StdError(Fatal) << "Incomplete Object Definition" << nl;
	return auto_ptr<FieldSettingList>();
#else
	tokenGroup->Reset();
	return setting;
#endif
}

void DefaultSyntaxBuilder::ResetTokenGroup() {
	tokenGroup->Reset();
}

//////////////////////////////////////////////////////////////////////

SingleObjectConstraintElement::SingleObjectConstraintElement(InformationObjectPtr obj)
	: object(obj) {
}

SingleObjectConstraintElement::~SingleObjectConstraintElement() {
}

void SingleObjectConstraintElement::printOn(ostream& strm ) const {
	strm << *object;
}

ValueSetPtr SingleObjectConstraintElement::getValueSetFromValueField(const string& fld) const {
	ValueSetting* setting = (ValueSetting*) object->getSetting(fld);
	if (setting) {
		auto_ptr<ConstraintElementVector> elems ( new ConstraintElementVector );
		elems->push_back(ConstraintElementPtr(new SingleValueConstraintElement(setting->getValue())));
		ConstraintPtr con(new Constraint(elems, false));
		return ValueSetPtr(new ValueSetDefn(TypePtr(new DefinedType(setting->getType(), "") ) , con));
	} else {
		cerr << StdError(Fatal) << "Retrieve non-extisted object field : " <<  fld  << nl;
		exit(1);
	}
	//return ValueSetPtr(new ValueSetDefn);
}

ValueSetPtr SingleObjectConstraintElement::getValueSetFromValueSetField(const string& fld) const {
	ValueSetSetting* setting = (ValueSetSetting*) object->getSetting(fld);
	if (setting)
		return setting->getValueSet();
	else {
		cerr << StdError(Fatal) << "Retrieve non-extisted object field : "<<  fld  << nl;
		exit(1);
	}

	//return ValueSetPtr(new ValueSetDefn);
}

ConstraintPtr SingleObjectConstraintElement::getObjectSetFromObjectField(const string& fld) const {
	ConstraintPtr result;
	ObjectSetting* setting = (ObjectSetting*) object->getSetting(fld);
	if (setting) {
		auto_ptr<ConstraintElementVector> elems ( new ConstraintElementVector );
		elems->push_back(ConstraintElementPtr(
							 new SingleObjectConstraintElement(setting->getObject())));
		result.reset(new Constraint(elems, false));
	}
	return result;
}

ConstraintPtr SingleObjectConstraintElement::getObjectSetFromObjectSetField(const string& fld) const {
	ObjectSetSetting* setting = (ObjectSetSetting*) object->getSetting(fld);
	if (setting)
		return setting->getObjectSet();
	else
		return ConstraintPtr();
}

bool SingleObjectConstraintElement::hasPERInvisibleConstraint(const Parameter& ) const {
	return false;
}

void SingleObjectConstraintElement::generateObjectSetInstanceCode(const string& prefix, ostream& cxx) const {
	cxx << prefix;
	object->generateInstanceCode(cxx);
	cxx << ");" << nl;
}

/////////////////////////////////////////////////////////////////////////

ValueSetDefn::ValueSetDefn()
	: elements(nullptr) {
}

ValueSetDefn::ValueSetDefn(TypePtr base, ConstraintPtr cons)
	: type(base), elements(new ConstraintElementVector) {
	elements->swap(cons->getStandardElements());

	elements->insert(elements->end(),
					 cons->getExtensionElements().begin(),
					 cons->getExtensionElements().end());

	extendable = cons->isExtendable();
}

ValueSetDefn::~ValueSetDefn() {
}

void ValueSetDefn::Union(ValueSetPtr& other) {
	if (type.get() == nullptr)
		type.reset(new DefinedType(other->getType(), ""));
	if (elements.get() == nullptr)
		elements = auto_ptr<ConstraintElementVector>( new ConstraintElementVector);
	elements->insert(elements->end(), other->getElements()->begin(), other->getElements()->end());
}

TypePtr ValueSetDefn::MakeValueSetType() {
	type->addConstraint(ConstraintPtr(new Constraint(elements, extendable)));
	type->setAsValueSetType();
	return type;
}

void ValueSetDefn::Intersect(ValueSetPtr& other) {
	assert(other.get());

	if (elements.get() == nullptr || elements->size() == 0)
		type.reset(new DefinedType(other->getType(),""));

	auto_ptr<ConstraintElementVector> root(new ConstraintElementVector);
	root->swap(*elements);
	root->insert(root->end(), other->getElements()->begin(), other->getElements()->end());
	elements = auto_ptr<ConstraintElementVector>(new ConstraintElementVector);
	elements->push_back(ConstraintElementPtr(new ElementListConstraintElement(root)));
}

void ValueSetDefn::printOn(ostream& strm) const {
	strm << '{';
	PrintVector(strm, *elements, '|');
	strm << '}';
}


void ValueSetDefn::resolveReference() const {
	if (type.get())
		type->resolveReference();
}

////////////////////////////////////////////////////////////////////////////////////////

ValueSetFromObject::ValueSetFromObject(InformationObjectPtr obj
									   , const string& fld)
	: object(obj), field(fld) {
}

ValueSetFromObject::~ValueSetFromObject() {
}

void ValueSetFromObject::Union(ValueSetPtr& other) {
	getRepresentation()->Union(other);
}

void ValueSetFromObject::Intersect(ValueSetPtr& other) {
	getRepresentation()->Intersect(other);
}

TypePtr ValueSetFromObject::MakeValueSetType() {
	return getRepresentation()->MakeValueSetType();
}

void ValueSetFromObject::printOn(ostream& strm) const {
	strm << object->getName() << ".&" << field;
}



TypePtr ValueSetFromObject::getType() {
	return getRepresentation()->getType();
}

ConstraintElementVector* ValueSetFromObject::getElements() {
	return getRepresentation()->getElements();
}

ValueSetPtr ValueSetFromObject::getRepresentation() {
	if (rep.get() == nullptr) {
		SingleObjectConstraintElement cons(object);
		rep = cons.getValueSetFromValueSetField(field);
	}
	return rep;
}

//////////////////////////////////////////////////////////////////


ValueSetFromObjects::ValueSetFromObjects(ObjectSetConstraintElementPtr objSet,
		const string& fld)
	: objectSet(objSet), field(fld) {
}

ValueSetFromObjects::~ValueSetFromObjects() {
}

void ValueSetFromObjects::Union(ValueSetPtr& other) {
	getRepresentation()->Union(other);
}

void ValueSetFromObjects::Intersect(ValueSetPtr& other) {
	getRepresentation()->Intersect(other);
}

TypePtr ValueSetFromObjects::MakeValueSetType() {
	return getRepresentation()->MakeValueSetType();
}

void ValueSetFromObjects::printOn(ostream& strm) const {
	strm << *objectSet << "." << field;
}


TypePtr ValueSetFromObjects::getType() {
	return getRepresentation()->getType();
}

ConstraintElementVector* ValueSetFromObjects::getElements() {
	return getRepresentation()->getElements();
}

ValueSetPtr ValueSetFromObjects::getRepresentation() {
	if (rep.get() == nullptr) {
		if (isupper(field[1]))
			rep = objectSet->getValueSetFromValueSetField(field);
		else
			rep = objectSet->getValueSetFromValueField(field);
	}
	return rep;
}

///////////////////////////////////////////////////////////////////////////////////

Symbol::Symbol(const string& sym, bool param)
	: name(sym), parameterized(param) {
}

void Symbol::printOn(ostream& strm) const {
	strm << name;
	if (parameterized) strm << "{}";
}
///////////////////////////////////////////////////////////////////////////////////

void TypeReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to) {
	assert(from);
	assert(to);
	TypePtr refType = from->findType(name);
	boost::shared_ptr<ImportedType> type;
	if (refType.get())
		type.reset(new ImportedType(refType));
	else
		type.reset(new ImportedType(name, false));
	type->setModuleName(from->getName());
	to->addType(type);
}

void TypeReference::AppendToModule(const string& fromName, ModuleDefinition* to) {
	assert(to);
	boost::shared_ptr<ImportedType> type(new ImportedType(name, parameterized));
	type->setModuleName(fromName);
	to->addType(type);
}

void TypeReference::generateUsingDirective(const string& , ostream& ) const {
	// strm << "using " << moduleName << "::" << name << ";" << nl;
}


///////////////////////////////////////////////////////////////////////////////////

void ValueReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to) {
	assert(from);
	assert(to);
	to->addValue(ValuePtr(new ImportedValue(from->getName(),name,from->findValue(name))));
}

///////////////////////////////////////////////////////////////////////////////////

void ObjectClassReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to) {
	assert(from);
	assert(to);
	ObjectClassBasePtr oc(new ImportedObjectClass(from->getName(),
						  name,
						  from->findObjectClass(name).get() ) );
	to->addObjectClass(oc);
}

void ObjectClassReference::generateUsingDirective(const string& moduleName, ostream& strm) const {
	strm << "using " << moduleName << "::" << makeCppName(name) << ";" << nl;
}

///////////////////////////////////////////////////////////////////////////////////

void ObjectSetReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to) {
	assert(from);
	assert(to);
	InformationObjectSetPtr ios(new ImportedObjectSet(from->getName(),from->findInformationObjectSet(name)));
	to->addInformationObjectSet(ios);
}

void ObjectSetReference::generateUsingDirective(const string& moduleName, ostream& strm) const {
	strm << "using " << moduleName << "::" << makeCppName(name) << ";" << nl;
}

///////////////////////////////////////////////////////////////////////////////////

void ObjectReference::AppendToModule(ModuleDefinition* from, ModuleDefinition* to) {
	assert(from);
	assert(to);
	InformationObjectPtr io(new ImportedObject(from->getName(),name,from->findInformationObject(name)));
	to->addInformationObject(io);
}

///////////////////////////////////////////////////////////////////////////////////
Parameter::Parameter(const string& nam)
	: name(nam), identifierType(TYPEREFERENCE) {
}



Parameter::Parameter(const string& nam, int type)
	: name(nam), identifierType(type) {
}

Parameter::Parameter(const Parameter& other)
	: name(other.name), identifierType(other.identifierType) {
}


int Parameter::getIdentifierType() {
	return identifierType;
}

const string& Parameter::getName() const {
	return name;
}

void Parameter::printOn(ostream& strm) const {
	strm << name;
}

bool Parameter::ReferencedBy(const TypeBase& type) const {
	DefinedType t(name);
	return type.referencesType(t);
}

ActualParameterPtr Parameter::MakeActualParameter() const {
	return ActualParameterPtr(new ActualTypeParameter(TypePtr(new DefinedType(name))));
}

/////////////////////////////////////////////////

ValueParameter::ValueParameter(TypePtr gover, const string& nam)
	: Parameter(nam, isupper(nam[0]) ? TYPEREFERENCE : VALUEREFERENCE)
	, governor(gover) {
}

ValueParameter::ValueParameter(const ValueParameter& other)
	: Parameter(other)
	, governor(other.governor) {
}

ValueParameter::~ValueParameter() {
}

void ValueParameter::printOn(ostream& strm) const {
	strm << governor->getTypeName() << " : " << name;
}

ActualParameterPtr ValueParameter::MakeActualParameter() const {
	if (isupper(name[0]))
		return ActualParameterPtr(new ActualValueSetParameter(TypePtr(new DefinedType(name))));
	else
		return ActualParameterPtr(new ActualValueParameter(ValuePtr(new DefinedValue(name))));
}

///////////////////////////////////////////////////

ObjectParameter::ObjectParameter(DefinedObjectClassPtr gover,
								 const string& nam)
	: Parameter(nam, isupper(nam[0]) ? OBJECTSETREFERENCE : OBJECTREFERENCE)
	, governor(gover) {
}

ObjectParameter::~ObjectParameter() {
}

void ObjectParameter::printOn(ostream& strm) const {
	strm << governor->getName() << " : " << name;
}


bool ObjectParameter::ReferencedBy(const TypeBase&  type) const {
	DefinedType t(name);
	return type.referencesType(t);
}

ActualParameterPtr ObjectParameter::MakeActualParameter() const {
	if (isupper(name[0]))
		return ActualParameterPtr(new ActualObjectSetParameter(
									  boost::shared_ptr<ObjectSetConstraintElement>(new DefinedObjectSet(name))));
	else
		return ActualParameterPtr(new ActualObjectParameter(InformationObjectPtr(new DefinedObject(name))));
}

//////////////////////////////////////////////////////////

void ParameterList::Append(ParameterPtr param) {
	rep.push_back(param);
}

int ParameterList::getIdentifierType(const char* identifier) {
	Parameter* p = getParameter(identifier);
	return (p != nullptr) ? p->getIdentifierType() : -1;
}

Parameter* ParameterList::getParameter(const char* identifier) {
	for (size_t i = 0; i < rep.size() ; ++i) {
		if (rep[i]->getName() == identifier)
			return rep[i].get();
	}
	return nullptr;
}

void ParameterList::printOn(ostream& strm) const {
	if (!rep.empty()) {
		strm << " {";
		for (size_t i = 0; i < rep.size(); ++i) {
			strm << *rep[i];
			if (i != rep.size() - 1)
				strm << ", ";
		}
		strm << " } ";
	}
}


void ParameterList::generateCplusplus(string& templatePrefix, string& classNameString) {
	if (rep.size()) {
		templatePrefix = "template <";
		classNameString += '<';
		bool outputDelimeter = false;
		for (size_t i = 0; i < rep.size(); i++) {
			if (outputDelimeter) {
				templatePrefix += ", ";
				classNameString += ", ";
			}

			string ident = makeCppName(rep[i]->getName());
			templatePrefix += "class " + ident;
			classNameString += ident;
			outputDelimeter = true;
		}
		templatePrefix += ">\n";
		classNameString += '>';
	}
}

boost::shared_ptr<ParameterList> ParameterList::getReferencedParameters(const TypeBase& type) const {
	boost::shared_ptr<ParameterList> result(new ParameterList);
	for (size_t i = 0; i < rep.size(); ++i) {
		if (rep[i]->ReferencedBy(type))
			result->rep.push_back(rep[i]);
	}
	if (result->rep.empty())
		result.reset();
	return result;
}

ActualParameterListPtr ParameterList::MakeActualParameters() const {
	ActualParameterListPtr lst(new ActualParameterList);
	for (size_t i = 0; i < rep.size(); ++i) {
		lst->push_back(rep[i]->MakeActualParameter());
	}
	return lst;
}


/////////////////////////////////////////////////////////////////////

ActualTypeParameter::ActualTypeParameter(TypePtr type)
	: param (type) {
}

ActualTypeParameter::~ActualTypeParameter() {
}

bool ActualTypeParameter::referencesType(const TypeBase& type) const {
	return param->referencesType(type);
}

bool ActualTypeParameter::useType(const TypeBase& type) const {
	return param->useType(type);
}

bool ActualTypeParameter::generateTemplateArgument(string& name) const {
	name += param->getTypeName();
	return true;
}

void ActualTypeParameter::printOn(ostream& strm) const {
	strm << param->getTypeName();
}

///////////////////////////////////////////////////////////

ActualValueParameter::ActualValueParameter(ValuePtr value)
	: param(value) {
}

ActualValueParameter::~ActualValueParameter() {
}

void ActualValueParameter::printOn(ostream& strm) const {
	strm << param->getName();
}

/////////////////////////////////////////////////////////////

ActualValueSetParameter::ActualValueSetParameter(TypePtr valueSet)
	: param(valueSet) {
}

ActualValueSetParameter::~ActualValueSetParameter() {
}

bool ActualValueSetParameter::referencesType(const TypeBase& type) const {
	return param->referencesType(type);
}

bool ActualValueSetParameter::useType(const TypeBase& type) const {
	return param->useType(type);
}

bool ActualValueSetParameter::generateTemplateArgument(string& name) const {
	name += param->getTypeName();
	return true;
}


void ActualValueSetParameter::printOn(ostream& strm) const {
	strm << '{' << param->getTypeName() << '}';
}

/////////////////////////////////////////////////////////////

ActualObjectParameter::ActualObjectParameter(InformationObjectPtr obj)
	: param(obj) {
}

ActualObjectParameter::~ActualObjectParameter() {
}

void ActualObjectParameter::printOn(ostream& strm) const {
	strm << param->getName();
}

bool ActualObjectParameter::generateTemplateArgument(string& name) const {
	name += param->getName();
	return true;
}

bool ActualObjectParameter::useType(const TypeBase& type) const {
	return type.getTypeName() == param->getName();
}

bool ActualObjectParameter::referencesType(const TypeBase& type) const {
	return type.getTypeName() == param->getName();
}

/////////////////////////////////////////////////////////////////////

ActualObjectSetParameter::ActualObjectSetParameter(boost::shared_ptr<ObjectSetConstraintElement> objectSet)
	: param(objectSet) {
}

ActualObjectSetParameter::~ActualObjectSetParameter() {
}

void ActualObjectSetParameter::printOn(ostream& strm) const {
	strm << '{' << param->getName() << '}' ;
}

bool ActualObjectSetParameter::generateTemplateArgument(string& name) const {
	name += param->getName();
	str_replace(name,"{","<");
	str_replace(name,"}","> ");
	return true;
}

bool ActualObjectSetParameter::useType(const TypeBase& type) const {
	return type.getTypeName() == param->getName();
}

bool ActualObjectSetParameter::referencesType(const TypeBase& type) const {
	return type.getTypeName() == param->getName();
}
//////////////////////////////////////////////////////////////

ObjectSetType::ObjectSetType(InformationObjectSetPtr os)
	: TypeBase(0, contexts.top()->Module)
	, objSet(os) {
	name = objSet->getName();
	identifier = makeCppName(name);
}

const char * ObjectSetType::getAncestorClass() const {
	return identifier.c_str();
}

void ObjectSetType::generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {
	objSet->generateType(fwd, hdr, cxx, inl);
}

bool ObjectSetType::hasParameters() const {
	return objSet->hasParameters();
}




ostream& operator<<(ostream& out, const StdError& e) {
	out << fileName << '(' << lineNumber << ") : ";
	switch(e.e) {
	case Fatal		:
		fatals++;
		out << "error";
		break;
	case Warning	:
		warnings++;
		out << "warning";
		break;
	}
	return out << ": ";
}


/////////////////////////////////////////////////////////
//
//  Utility
//

string makeCppName(const string& identifier) {
	string s = identifier;
	if (!s.empty()) {
		str_replace(s, "-", "_");
		static const char** end = CppReserveWords+ARRAY_SIZE(CppReserveWords);
		if (binary_search(CppReserveWords,  end,  s.c_str(),  str_less()))
			s += '_';
	}
	return s;
}

// creates a short name for module names from combining the first character
// of each '-' parted words, eg. H323-MESSAGES => HM,
// Multimedia-System-Control => MSC
static string shortenName(const string& name) {
	string s ;
	s += name[0];

	size_t i = 0;
	while ( (i = name.find('-', i+1)) != -1)
		s += name[i+1];

	return s;
}

string toLower(const string& str) {
	string result;
	result.resize(str.size());
	transform(str.begin(), str.end(), result.begin(), tolower);
	return result;
}

string toUpper(const string& str) {
	string result;
	result.resize(str.size());
	transform(str.begin(), str.end(), result.begin(), toupper);
	return result;
}

string getFileName(const string& fullname) {
	int i = fullname.find_last_of(DIR_SEPARATOR);
	return fullname.substr(i+1);
}

string getFileDirectory(const string& fullname) {
	string::size_type p = fullname.find_last_of(DIR_SEPARATOR);
	if (p!=string::npos)
		return fullname.substr(0, p);

	return string();
}

string getFileTitle(const string& fullname) {
	string result = getFileName(fullname);
	int dotpos = result.find_last_of('.');
	if (dotpos == -1)
		return result.substr(0, dotpos-1);

	return result;
}



void addRemoveItem(const char* item) {
	contexts.top()->RemoveList.push_back(string(item));
}


ModuleDefinition* findModule(const char* name) {
	ModuleDefinitionPtr smd = findWithName(Modules, name);
	return smd.get();
}
ModuleDefinition* CreateModule(const char* name) {
	ModuleDefinitionPtr mdp (new ModuleDefinition(name));
	Modules.push_back(mdp);
	return mdp.get();
}



const char* tokenAsString(int token) {
	switch (token) {
	case MODULEREFERENCE:
		return "MODULEREFERENCE";
	case TYPEREFERENCE:
		return "TYPEREFERENCE";
	case OBJECTCLASSREFERENCE:
		return "OBJECTCLASSREFERENCE";
	case VALUEREFERENCE:
		return "VALUEREFERENCE";
	case OBJECTREFERENCE:
		return "OBJECTREFERENCE";
	case OBJECTSETREFERENCE:
		return "OBJECTSETREFERENCE";
	case PARAMETERIZEDTYPEREFERENCE:
		return "PARAMETERIZEDTYPEREFERENCE";
	case PARAMETERIZEDOBJECTCLASSREFERENCE:
		return "PARAMETERIZEDOBJECTCLASSREFERENCE";
	case PARAMETERIZEDVALUEREFERENCE:
		return "PARAMETERIZEDVALUEREFERENCE";
	case PARAMETERIZEDOBJECTREFERENCE:
		return "PARAMETERIZEDOBJECTREFERENCE";
	case PARAMETERIZEDOBJECTSETREFERENCE:
		return "PARAMETERIZEDOBJECTSETREFERENCE";
	case VALUESET_BRACE:
		return "VALUESET_BRACE";
	case OBJECT_BRACE:
		return "OBJECT_BRACE";
	case OBJECTSET_BRACE:
		return "OBJECTSET_BRACE";
	case IDENTIFIER:
		return "IDENTIFIER";
	case BIT_IDENTIFIER:
		return "BIT_IDENTIFIER";
	case OID_IDENTIFIER:
		return "OID_IDENTIFIER";
	case IMPORT_IDENTIFIER:
		return "IMPORT_IDENTIFIER";
	case fieldreference:
		return "fieldreference";
	case FieldReference:
		return "FieldReference";
	case TYPEFIELDREFERENCE:
		return "TYPEFIELDREFERENCE";
	case FIXEDTYPEVALUEFIELDREFERENCE:
		return "FIXEDTYPEVALUEFIELDREFERENCE";
	case VARIABLETYPEVALUEFIELDREFERENCE:
		return "VARIABLETYPEVALUEFIELDREFERENCE";
	case FIXEDTYPEVALUESETFIELDREFERENCE:
		return "FIXEDTYPEVALUESETFIELDREFERENCE";
	case VARIABLETYPEVALUESETFIELDREFERENCE:
		return "VARIABLETYPEVALUESETFIELDREFERENCE";
	case OBJECTFIELDREFERENCE:
		return "OBJECTFIELDREFERENCE";
	case OBJECTSETFIELDREFERENCE:
		return "OBJECTSETFIELDREFERENCE";
	case REAL:
		return "REAL";
	case INTEGER:
		return "INTEGER";
	case CSTRING:
		return "CSTRING";
	case BSTRING:
		return "BSTRING";
	case HSTRING:
		return "HSTRING";
	case BS_BSTRING:
		return "BS_BSTRING";
	case BS_HSTRING:
		return "BS_HSTRING";
	case ABSENT:
		return "ABSENT";
	case ABSTRACT_SYNTAX:
		return "ABSTRACT_SYNTAX";
	case ALL:
		return "ALL";
	case ANY:
		return "ANY";
	case APPLICATION:
		return "APPLICATION";
	case ASSIGNMENT:
		return "ASSIGNMENT";
	case AUTOMATIC:
		return "AUTOMATIC";
	case BEGIN_t:
		return "BEGIN_t";
	case BIT:
		return "BIT";
	case BMPString:
		return "BMPString";
	case BOOLEAN_t:
		return "BOOLEAN_t";
	case BY:
		return "BY";
	case CHARACTER:
		return "CHARACTER";
	case CHOICE:
		return "CHOICE";
	case CLASS:
		return "CLASS";
	case COMPONENT:
		return "COMPONENT";
	case COMPONENTS:
		return "COMPONENTS";
	case CONSTRAINED:
		return "CONSTRAINED";
	case DEFAULT:
		return "DEFAULT";
	case DEFINED:
		return "DEFINED";
	case DEFINITIONS:
		return "DEFINITIONS";
	case ELLIPSIS:
		return "...";
	case EMBEDDED:
		return "EMBEDDED";
	case END:
		return "END";
	case ENUMERATED:
		return "ENUMERATED";
	case EXCEPT:
		return "EXCEPT";
	case EXPLICIT:
		return "EXPLICIT";
	case EXPORTS:
		return "EXPORTS";
	case EXTERNAL:
		return "EXTERNAL";
	case FALSE_t:
		return "FALSE_t";
	case FROM:
		return "FROM";
	case GeneralString:
		return "GeneralString";
	case GraphicString:
		return "GraphicString";
	case IA5String :
		return "IA5String";
	case TYPE_IDENTIFIER:
		return "TYPE_IDENTIFIER";
	case IDENTIFIER_t:
		return "IDENTIFIER_t";
	case IMPLICIT:
		return "IMPLICIT";
	case IMPORTS:
		return "IMPORTS";
	case INCLUDES:
		return "INCLUDES";
	case INSTANCE:
		return "INSTANCE";
	case INTEGER_t:
		return "INTEGER_t";
	case INTERSECTION:
		return "INTERSECTION";
	case ISO646String:
		return "ISO646String";
	case MACRO:
		return "MACRO";
	case MAX:
		return "MAX";
	case MIN :
		return "MIN";
	case MINUS_INFINITY :
		return "MINUS_INFINITY";
	case NOTATION:
		return "NOTATION";
	case NOT_A_NUMBER:
		return "NOT-A-NUMBER";
	case NULL_VALUE:
		return "NULL_VALUE";
	case NULL_TYPE:
		return "NULL_TYPE";
	case NumericString:
		return "NumericString";
	case OBJECT:
		return "OBJECT";
	case OCTET:
		return "OCTET";
	case OF_t:
		return "OF_t";
	case OPTIONAL_t:
		return "OPTIONAL_t";
	case PDV:
		return "PDV";
	case PLUS_INFINITY:
		return "PLUS_INFINITY";
	case PRESENT:
		return "PRESENT";
	case PrintableString:
		return "PrintableString";
	case PRIVATE:
		return "PRIVATE";
	case SEQUENCE:
		return "SEQUENCE";
	case SET:
		return "SET";
	case SIZE_t:
		return "SIZE_t";
	case STRING:
		return "STRING";
	case SYNTAX:
		return "SYNTAX";
	case T61String:
		return "T61String";
	case TAGS:
		return "TAGS";
	case TeletexString:
		return "TeletexString";
	case TRUE_t:
		return "TRUE_t";
	case TYPE_t:
		return "TYPE_t";
	case UNION:
		return "UNION";
	case UNIQUE:
		return "UNIQUE";
	case UNIVERSAL:
		return "UNIVERSAL";
	case UniversalString:
		return "UniversalString";
	case VideotexString:
		return "VideotexString";
	case VisibleString:
		return "VisibleString";
	case GeneralizedTime:
		return "GeneralizedTime";
	case UTCTime:
		return "UTCTime";
	case VALUE:
		return "VALUE";
	case WITH:
		return "WITH";
	case ObjectDescriptor_t:
		return "ObjectDescriptor_t";
	case WORD_t:
		return "WORD_t";
	case OID_INTEGER:
		return "OID_INTEGER";
	}
	static string result = "???" + to_string(token) + "???";
	return result.c_str();
}

string getPath(const char* name) {
	string path;
	if (!asndir.empty()) {
		path += asndir;
		path += DIR_SEPARATOR;
	}
	if (name)
		path += name;
	return path;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#include <windows.h>
static vector<string> readDirectory(const string &directory, const string &extension) {
	vector<string> result;
	HANDLE hFind;
	WIN32_FIND_DATA data;
	string asnfiles = directory + DIR_SEPARATOR + "*." + extension.c_str();
	hFind = FindFirstFile(asnfiles.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			const char* dot = strrchr(data.cFileName, '.');
			if (strcmp(dot + 1, extension.c_str()) == 0)
				result.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	return result;
}
#else
#include <dirent.h>
#include <vector>
#include <string>
using namespace std;

/**
 *  * Read a directory listing into a vector of strings, filtered by file extension.
 *   * Throws exception on error.
 *    **/
vector<string> readDirectory(const string &directory, const string &extension) {
	vector<string> result;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(directory.c_str())) == nullptr) {
		throw runtime_error("readDirectory() - Unable to open directory.");
	}

	while ((ent = readdir(dir)) != nullptr) {
		string entry( ent->d_name );
		size_t pos = entry.rfind(extension);
		if (pos!=string::npos && pos==entry.length()-extension.length()) {
			result.push_back( entry );
		}
	}

	if (closedir(dir) != 0) {
		throw runtime_error("readDirectory() - Unable to close directory.");
	}

	return result;
}
#endif


/*
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
 * Fixed OutputFile::open() problem in VS.NET 2003
 *
 * Revision 1.17  2003/04/02 10:49:40  btrummer
 * Reimplemented the enum names output in EnumeratedType::generateInfo().
 * Now, ENUMERATED::getName() works correctly, even if the defined enums
 * don't start with 0 or contain holes in the value assignment.
 *
 * Revision 1.16  2003/02/17 12:12:30  btrummer
 * The last fix must be solved using an #ifdef.
 * Otherwise, it won't compile on Win32/VC6.
 *
 * Revision 1.15  2003/02/10 14:12:51  btrummer
 * ObjectClassDefn::generateCplusplus(): added scope to const_iterator(i)
 * to make Solaris' Forte Developer 7 C++ 5.4 happy.
 *
 * Revision 1.14  2003/02/10 08:52:59  btrummer
 * added a missing const in the BMPStringType::generateOperators() output.
 *
 * Revision 1.13  2003/01/27 12:41:29  btrummer
 * added comparision operators for generated ENUMERATED classes.
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
 * in TypeBase::beginGenerateCplusplus().
 *
 * Revision 1.9  2002/10/31 07:36:15  btrummer
 * Aaargh. The new typedef must be defined protected rather than private.
 * (private is only working on g++ 2.95)
 *
 * Revision 1.8  2002/10/31 06:50:26  btrummer
 * added a "typedef Inherited::InfoType InfoType;" to all generated classes
 * to prevent g++ compile errors, when the ASN.1 file contains tags.
 *
 * Revision 1.7  2002/10/03 07:12:14  btrummer
 * Fixed ObjectClassFieldType::generateDecoder():
 * if objSet.get() == nullptr, true should be returned.
 *
 * Revision 1.6  2002/10/03 07:02:57  btrummer
 * Bugfix in InformationObjectSetDefn::generateTypeConstructor():
 * If module == nullptr, objSet must be initialized to nullptr.
 *
 * Revision 1.5  2002/09/14 01:56:32  mangelo
 * Removed the memory leak caused by strstream::str()
 *
 * Revision 1.4  2002/08/20 22:35:54  mangelo
 * add MSVC DLL support
 *
 * Revision 1.3  2002/07/02 02:03:25  mangelo
 * Remove Pwlib dependency
 *
 * Revision 1.2  2001/09/07 22:39:28  mangelo
 * add Log keyword substitution
 *
 *
 * 2001/07/25 Huang-Ming Huang
 *   added code to generate "#undef ERROR"
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
 *   add support for Information Object Class and generate code that follows
 *   X/open ASN.1/C++ interface.
 */
