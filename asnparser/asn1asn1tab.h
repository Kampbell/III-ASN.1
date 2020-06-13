#ifndef _yy_defines_h_
#define _yy_defines_h_


#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
/* Default: YYLTYPE is the text position type. */
typedef struct YYLTYPE
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1
#endif

#define MODULEREFERENCE 257
#define TYPEREFERENCE 258
#define OBJECTCLASSREFERENCE 259
#define VALUEREFERENCE 260
#define OBJECTREFERENCE 261
#define OBJECTSETREFERENCE 262
#define PARAMETERIZEDTYPEREFERENCE 263
#define PARAMETERIZEDOBJECTCLASSREFERENCE 264
#define PARAMETERIZEDVALUEREFERENCE 265
#define PARAMETERIZEDOBJECTREFERENCE 266
#define PARAMETERIZEDOBJECTSETREFERENCE 267
#define VALUESET_BRACE 268
#define OBJECT_BRACE 269
#define OBJECTSET_BRACE 270
#define OID_BRACE 271
#define IDENTIFIER 272
#define BIT_IDENTIFIER 273
#define OID_IDENTIFIER 274
#define OID_INTEGER 275
#define IMPORT_IDENTIFIER 276
#define fieldreference 277
#define FieldReference 278
#define TYPEFIELDREFERENCE 279
#define FIXEDTYPEVALUEFIELDREFERENCE 280
#define VARIABLETYPEVALUEFIELDREFERENCE 281
#define FIXEDTYPEVALUESETFIELDREFERENCE 282
#define VARIABLETYPEVALUESETFIELDREFERENCE 283
#define OBJECTFIELDREFERENCE 284
#define OBJECTSETFIELDREFERENCE 285
#define INTEGER 286
#define REAL 287
#define CSTRING 288
#define BSTRING 289
#define HSTRING 290
#define BS_BSTRING 291
#define BS_HSTRING 292
#define ABSENT 293
#define ABSTRACT_SYNTAX 294
#define ALL 295
#define ANY 296
#define APPLICATION 297
#define ASSIGNMENT 298
#define AUTOMATIC 299
#define BEGIN_t 300
#define BIT 301
#define BMPString 302
#define BOOLEAN_t 303
#define BY 304
#define CHARACTER 305
#define CHOICE 306
#define CLASS 307
#define COMPONENT 308
#define COMPONENTS 309
#define CONSTRAINED 310
#define CONTAINING 311
#define DEFAULT 312
#define DATE 313
#define DATE_TIME 314
#define DEFINED 315
#define DEFINITIONS 316
#define DURATION 317
#define ELLIPSIS 318
#define EMBEDDED 319
#define END 320
#define ENUMERATED 321
#define EXCEPT 322
#define EXPLICIT 323
#define EXPORTS 324
#define EXTENSIBILITY 325
#define EXTERNAL 326
#define FALSE_t 327
#define FROM 328
#define GeneralString 329
#define GeneralizedTime 330
#define GraphicString 331
#define IA5String 332
#define IDENTIFIER_t 333
#define IMPLICIT 334
#define IMPLIED 335
#define IMPORTS 336
#define INCLUDES 337
#define INSTANCE 338
#define INSTRUCTIONS 339
#define INTEGER_t 340
#define INTERSECTION 341
#define ISO646String 342
#define MACRO 343
#define MAX 344
#define MIN 345
#define MINUS_INFINITY 346
#define NOTATION 347
#define NOT_A_NUMBER 348
#define NULL_TYPE 349
#define NULL_VALUE 350
#define NumericString 351
#define OBJECT 352
#define OCTET 353
#define OID_IRI 354
#define OF_t 355
#define OPTIONAL_t 356
#define PATTERN 357
#define PDV 358
#define PLUS_INFINITY 359
#define PRESENT 360
#define PRIVATE 361
#define PrintableString 362
#define RANGE 363
#define REAL_t 364
#define RELATIVE_OID 365
#define RELATIVE_OID_IRI 366
#define SEQUENCE 367
#define SET 368
#define SETTINGS 369
#define SIZE_t 370
#define STRING 371
#define SYNTAX 372
#define T61String 373
#define TAGS 374
#define TIME 375
#define TIME_OF_DAY 376
#define TRUE_t 377
#define TYPE_IDENTIFIER 378
#define TYPE_t 379
#define TeletexString 380
#define UNION 381
#define UNIQUE 382
#define UNIVERSAL 383
#define UTCTime 384
#define UTF8String 385
#define UniversalString 386
#define VALUE 387
#define VideotexString 388
#define VisibleString 389
#define WITH 390
#define ObjectDescriptor_t 391
#define WORD_t 392

#define YYERRCODE 256
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
  boost::int64_t					ival;
  std::string*						sval;
  StringList* 						slst;
  NumberForm*	 					nf;
  ObjIdComponent* 					oic;
  ObjIdComponentList* 				oiclst;
  TypeBase* 						tval;
  TypePtr* 							tptr;
  TypesVector*						tlst;
  ValueBase*						vval;
  ValuesList*						vlst;
  NamedNumber*						nval;
  NamedNumberList*					nlst;
  Constraint*						cons;
  ConstraintElementVector*			elst;
  ConstraintElementBase*            elmt;
  FieldSpec*						fspc;
  FieldSpecsList*                   flst;
  ObjectClassBase*                  objc;
  DefinedObjectClass*               dobj;
  TokenGroup*                       tgrp;
  TokenOrGroupSpec*                 togs;
  Setting*							sett;
  InformationObject*                objt;
  InformationObjectSet*             objs;
  FieldSettingList*                 fldl;
  DefaultSyntaxBuilder*             bldr;
  DefinedSyntaxToken*               tken;
  ValueSet*							vset;
  ObjectSetConstraintElement*		osce;
  Symbol*							symb;
  SymbolList*						syml;
  Parameter*						para;
  ParameterList*					plst;
  ActualParameter*					apar;
  ActualParameterList*				aplt;
  TableConstraint*					tcons;
  ObjectClassFieldType*				ocft;
  DefinedObjectSet*					dos;
  ModuleDefinition*					modd;

  struct {
    Tag::Type tagClass;
    unsigned tagNumber;
  } tagv;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
