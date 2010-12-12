/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse igraph_pajek_yyparse
#define yylex   igraph_pajek_yylex
#define yyerror igraph_pajek_yyerror
#define yylval  igraph_pajek_yylval
#define yychar  igraph_pajek_yychar
#define yydebug igraph_pajek_yydebug
#define yynerrs igraph_pajek_yynerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NEWLINE = 258,
     NUM = 259,
     ALNUM = 260,
     QSTR = 261,
     PSTR = 262,
     NETWORKLINE = 263,
     VERTICESLINE = 264,
     ARCSLINE = 265,
     EDGESLINE = 266,
     ARCSLISTLINE = 267,
     EDGESLISTLINE = 268,
     MATRIXLINE = 269,
     VP_X_FACT = 270,
     VP_Y_FACT = 271,
     VP_IC = 272,
     VP_BC = 273,
     VP_LC = 274,
     VP_LR = 275,
     VP_LPHI = 276,
     VP_BW = 277,
     VP_FOS = 278,
     VP_PHI = 279,
     VP_R = 280,
     VP_Q = 281,
     VP_LA = 282,
     VP_FONT = 283,
     VP_URL = 284,
     VP_SIZE = 285,
     EP_C = 286,
     EP_S = 287,
     EP_A = 288,
     EP_W = 289,
     EP_H1 = 290,
     EP_H2 = 291,
     EP_A1 = 292,
     EP_A2 = 293,
     EP_K1 = 294,
     EP_K2 = 295,
     EP_AP = 296,
     EP_P = 297,
     EP_L = 298,
     EP_LP = 299,
     EP_LR = 300,
     EP_LPHI = 301,
     EP_LC = 302,
     EP_LA = 303,
     EP_SIZE = 304,
     EP_FOS = 305
   };
#endif
/* Tokens.  */
#define NEWLINE 258
#define NUM 259
#define ALNUM 260
#define QSTR 261
#define PSTR 262
#define NETWORKLINE 263
#define VERTICESLINE 264
#define ARCSLINE 265
#define EDGESLINE 266
#define ARCSLISTLINE 267
#define EDGESLISTLINE 268
#define MATRIXLINE 269
#define VP_X_FACT 270
#define VP_Y_FACT 271
#define VP_IC 272
#define VP_BC 273
#define VP_LC 274
#define VP_LR 275
#define VP_LPHI 276
#define VP_BW 277
#define VP_FOS 278
#define VP_PHI 279
#define VP_R 280
#define VP_Q 281
#define VP_LA 282
#define VP_FONT 283
#define VP_URL 284
#define VP_SIZE 285
#define EP_C 286
#define EP_S 287
#define EP_A 288
#define EP_W 289
#define EP_H1 290
#define EP_H2 291
#define EP_A1 292
#define EP_A2 293
#define EP_K1 294
#define EP_K2 295
#define EP_AP 296
#define EP_P 297
#define EP_L 298
#define EP_LP 299
#define EP_LR 300
#define EP_LPHI 301
#define EP_LC 302
#define EP_LA 303
#define EP_SIZE 304
#define EP_FOS 305




/* Copy the first part of user declarations.  */
#line 23 "foreign-pajek-parser.y"


/* 
   IGraph library.
   Copyright (C) 2006  Gabor Csardi <csardi@rmki.kfki.hu>
   MTA RMKI, Konkoly-Thege Miklos st. 29-33, Budapest 1121, Hungary
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 
   02110-1301 USA

*/

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "memory.h"
#include "error.h"
#include "attributes.h"
#include "config.h"
#include "igraph_math.h"
#include <math.h>
extern int igraph_pajek_yylex(void);
extern int igraph_pajek_mylineno;
extern char *igraph_pajek_yytext;
extern int igraph_pajek_yyleng;
char *igraph_i_pajek_errmsg;
int igraph_pajek_yyerror(char *s);
int igraph_i_pajek_add_string_vertex_attribute(const char *name, 
					       const char *value,
					       int len);
int igraph_i_pajek_add_string_edge_attribute(const char *name, 
					     const char *value,
					     int len);
int igraph_i_pajek_add_numeric_vertex_attribute(const char *name, 
						igraph_real_t value);
int igraph_i_pajek_add_numeric_edge_attribute(const char *name, 
					      igraph_real_t value);
int igraph_i_pajek_add_numeric_attribute(igraph_trie_t *names,
					 igraph_vector_ptr_t *attrs,
					 long int count,
					 const char *attrname,
					 igraph_integer_t vid,
					 igraph_real_t number);
int igraph_i_pajek_add_string_attribute(igraph_trie_t *names,
					igraph_vector_ptr_t *attrs,
					long int count,
					const char *attrname,
					igraph_integer_t vid,
					const char *str);
void igraph_i_pajek_reset_scanner(void);
extern igraph_vector_t *igraph_pajek_vector;
extern igraph_bool_t igraph_pajek_directed;
extern long int igraph_pajek_vcount;
extern int igraph_pajek_mode;
extern long int igraph_pajek_actfrom, igraph_pajek_actto;
extern igraph_trie_t *igraph_i_pajek_vertex_attribute_names;
extern igraph_vector_t *igraph_i_pajek_vertex_attribute_types;
extern igraph_vector_ptr_t *igraph_i_pajek_vertex_attributes;
extern igraph_trie_t *igraph_i_pajek_edge_attribute_names;
extern igraph_vector_t *igraph_i_pajek_edge_attribute_types;
extern igraph_vector_ptr_t *igraph_i_pajek_edge_attributes;
extern igraph_real_t igraph_pajek_get_number(const char *str, long int len);
extern long int igraph_i_pajek_actvertex;
extern long int igraph_i_pajek_actedge;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 105 "foreign-pajek-parser.y"
{
  long int intnum;
  double   realnum;  
  struct {
    char *str;
    int len;
  } string;  
}
/* Line 187 of yacc.c.  */
#line 291 "foreign-pajek-parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 304 "foreign-pajek-parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   240

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  66
/* YYNRULES -- Number of rules.  */
#define YYNRULES  134
/* YYNRULES -- Number of states.  */
#define YYNSTATES  200

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   305

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,     8,    12,    16,    19,    20,    23,
      25,    28,    29,    37,    39,    41,    42,    45,    49,    50,
      52,    53,    56,    58,    61,    64,    69,    74,    79,    82,
      85,    88,    91,    94,    97,   100,   103,   106,   107,   111,
     112,   116,   117,   121,   122,   126,   127,   131,   133,   134,
     137,   140,   143,   146,   149,   153,   154,   157,   159,   160,
     167,   169,   171,   175,   176,   179,   181,   182,   189,   191,
     193,   194,   196,   197,   200,   202,   207,   210,   213,   216,
     219,   222,   225,   228,   231,   234,   237,   240,   243,   246,
     249,   252,   253,   257,   258,   262,   263,   267,   268,   272,
     273,   277,   279,   283,   284,   287,   289,   293,   294,   297,
     299,   301,   305,   306,   309,   311,   315,   316,   319,   321,
     323,   327,   329,   330,   333,   336,   337,   340,   342,   344,
     346,   347,   350,   352,   354
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      52,     0,    -1,    53,    54,    72,    -1,    -1,     8,   115,
       3,    -1,    55,     3,    56,    -1,     9,   113,    -1,    -1,
      56,    57,    -1,     3,    -1,    59,     3,    -1,    -1,    59,
      58,    60,    61,    62,    63,     3,    -1,   113,    -1,   116,
      -1,    -1,   114,   114,    -1,   114,   114,   114,    -1,    -1,
     116,    -1,    -1,    63,    64,    -1,    65,    -1,    15,   114,
      -1,    16,   114,    -1,    17,   114,   114,   114,    -1,    18,
     114,   114,   114,    -1,    19,   114,   114,   114,    -1,    20,
     114,    -1,    21,   114,    -1,    22,   114,    -1,    23,   114,
      -1,    24,   114,    -1,    25,   114,    -1,    26,   114,    -1,
      27,   114,    -1,    30,   114,    -1,    -1,    28,    66,    71,
      -1,    -1,    29,    67,    71,    -1,    -1,    17,    68,    71,
      -1,    -1,    18,    69,    71,    -1,    -1,    19,    70,    71,
      -1,   116,    -1,    -1,    72,    73,    -1,    72,    79,    -1,
      72,    95,    -1,    72,   101,    -1,    72,   107,    -1,    10,
       3,    74,    -1,    -1,    74,    75,    -1,     3,    -1,    -1,
      77,    78,    76,    85,    86,     3,    -1,   113,    -1,   113,
      -1,    11,     3,    80,    -1,    -1,    80,    81,    -1,     3,
      -1,    -1,    83,    84,    82,    85,    86,     3,    -1,   113,
      -1,   113,    -1,    -1,   114,    -1,    -1,    86,    87,    -1,
      88,    -1,    31,   114,   114,   114,    -1,    32,   114,    -1,
      34,   114,    -1,    35,   114,    -1,    36,   114,    -1,    37,
     114,    -1,    38,   114,    -1,    39,   114,    -1,    40,   114,
      -1,    41,   114,    -1,    44,   114,    -1,    45,   114,    -1,
      46,   114,    -1,    48,   114,    -1,    49,   114,    -1,    50,
     114,    -1,    -1,    33,    89,    94,    -1,    -1,    42,    90,
      94,    -1,    -1,    43,    91,    94,    -1,    -1,    47,    92,
      94,    -1,    -1,    31,    93,    94,    -1,   116,    -1,    12,
       3,    96,    -1,    -1,    96,    97,    -1,     3,    -1,    99,
      98,     3,    -1,    -1,    98,   100,    -1,   113,    -1,   113,
      -1,    13,     3,   102,    -1,    -1,   102,   103,    -1,     3,
      -1,   105,   104,     3,    -1,    -1,   104,   106,    -1,   113,
      -1,   113,    -1,   108,     3,   109,    -1,    14,    -1,    -1,
     109,   110,    -1,   111,     3,    -1,    -1,   112,   111,    -1,
     113,    -1,     4,    -1,     4,    -1,    -1,   115,   116,    -1,
       5,    -1,     4,    -1,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   179,   179,   181,   181,   183,   185,   189,   189,   191,
     192,   193,   193,   196,   198,   202,   203,   207,   213,   213,
     217,   217,   220,   221,   224,   227,   232,   237,   242,   245,
     248,   251,   254,   257,   260,   263,   266,   271,   271,   275,
     275,   279,   279,   283,   283,   288,   288,   295,   297,   297,
     297,   297,   297,   297,   299,   301,   301,   303,   304,   304,
     310,   312,   314,   316,   316,   318,   319,   319,   325,   327,
     329,   329,   333,   333,   336,   337,   342,   345,   348,   351,
     354,   357,   360,   363,   366,   369,   372,   375,   378,   381,
     384,   389,   389,   393,   393,   397,   397,   401,   401,   405,
     405,   411,   413,   415,   415,   417,   417,   419,   419,   421,
     423,   428,   430,   430,   432,   432,   434,   434,   436,   438,
     445,   447,   449,   449,   451,   453,   453,   455,   465,   468,
     471,   471,   473,   474,   475
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NEWLINE", "NUM", "ALNUM", "QSTR",
  "PSTR", "NETWORKLINE", "VERTICESLINE", "ARCSLINE", "EDGESLINE",
  "ARCSLISTLINE", "EDGESLISTLINE", "MATRIXLINE", "VP_X_FACT", "VP_Y_FACT",
  "VP_IC", "VP_BC", "VP_LC", "VP_LR", "VP_LPHI", "VP_BW", "VP_FOS",
  "VP_PHI", "VP_R", "VP_Q", "VP_LA", "VP_FONT", "VP_URL", "VP_SIZE",
  "EP_C", "EP_S", "EP_A", "EP_W", "EP_H1", "EP_H2", "EP_A1", "EP_A2",
  "EP_K1", "EP_K2", "EP_AP", "EP_P", "EP_L", "EP_LP", "EP_LR", "EP_LPHI",
  "EP_LC", "EP_LA", "EP_SIZE", "EP_FOS", "$accept", "input", "nethead",
  "vertices", "verticeshead", "vertdefs", "vertexline", "@1", "vertex",
  "vertexid", "vertexcoords", "shape", "params", "param", "vpword", "@2",
  "@3", "@4", "@5", "@6", "vpwordpar", "edgeblock", "arcs", "arcsdefs",
  "arcsline", "@7", "arcfrom", "arcto", "edges", "edgesdefs", "edgesline",
  "@8", "edgefrom", "edgeto", "weight", "edgeparams", "edgeparam",
  "epword", "@9", "@10", "@11", "@12", "@13", "epwordpar", "arcslist",
  "arcslistlines", "arclistline", "arctolist", "arclistfrom", "arclistto",
  "edgeslist", "edgelistlines", "edgelistline", "edgetolist",
  "edgelistfrom", "edgelistto", "adjmatrix", "matrixline",
  "adjmatrixlines", "adjmatrixline", "adjmatrixnumbers", "adjmatrixentry",
  "longint", "number", "words", "word", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    51,    52,    53,    53,    54,    55,    56,    56,    57,
      57,    58,    57,    59,    60,    61,    61,    61,    62,    62,
      63,    63,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    66,    65,    67,
      65,    68,    65,    69,    65,    70,    65,    71,    72,    72,
      72,    72,    72,    72,    73,    74,    74,    75,    76,    75,
      77,    78,    79,    80,    80,    81,    82,    81,    83,    84,
      85,    85,    86,    86,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    89,    88,    90,    88,    91,    88,    92,    88,    93,
      88,    94,    95,    96,    96,    97,    97,    98,    98,    99,
     100,   101,   102,   102,   103,   103,   104,   104,   105,   106,
     107,   108,   109,   109,   110,   111,   111,   112,   113,   114,
     115,   115,   116,   116,   116
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     0,     3,     3,     2,     0,     2,     1,
       2,     0,     7,     1,     1,     0,     2,     3,     0,     1,
       0,     2,     1,     2,     2,     4,     4,     4,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     0,     3,     0,
       3,     0,     3,     0,     3,     0,     3,     1,     0,     2,
       2,     2,     2,     2,     3,     0,     2,     1,     0,     6,
       1,     1,     3,     0,     2,     1,     0,     6,     1,     1,
       0,     1,     0,     2,     1,     4,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     0,     3,     0,     3,     0,     3,     0,     3,     0,
       3,     1,     3,     0,     2,     1,     3,     0,     2,     1,
       1,     3,     0,     2,     1,     3,     0,     2,     1,     1,
       3,     1,     0,     2,     2,     0,     2,     1,     1,     1,
       0,     2,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,   130,     0,     0,     0,     1,     0,    48,     0,     4,
     133,   132,   134,   131,   128,     6,     2,     7,     0,     0,
       0,     0,   121,    49,    50,    51,    52,    53,     0,     5,
      55,    63,   103,   112,   122,     9,     8,    11,    13,    54,
      62,   102,   111,   120,    10,     0,    57,    56,     0,    60,
      65,    64,     0,    68,   105,   104,   107,   109,   114,   113,
     116,   118,   123,     0,   125,   127,    15,    14,    58,    61,
      66,    69,     0,     0,   124,   126,   129,    18,     0,    70,
      70,   106,   108,   110,   115,   117,   119,    20,    19,    16,
      72,    71,    72,     0,    17,     0,     0,    12,     0,     0,
      41,    43,    45,     0,     0,     0,     0,     0,     0,     0,
       0,    37,    39,     0,    21,    22,    59,    99,     0,    91,
       0,     0,     0,     0,     0,     0,     0,     0,    93,    95,
       0,     0,     0,    97,     0,     0,     0,    73,    74,    67,
      23,    24,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,     0,     0,    36,     0,
       0,    76,     0,    77,    78,    79,    80,    81,    82,    83,
      84,     0,     0,    85,    86,    87,     0,    88,    89,    90,
      42,    47,     0,    44,     0,    46,     0,    38,    40,   100,
     101,     0,    92,    94,    96,    98,    25,    26,    27,    75
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     7,     8,    29,    36,    45,    37,    66,
      77,    87,    93,   114,   115,   156,   157,   142,   144,   146,
     180,    16,    23,    39,    47,    79,    48,    68,    24,    40,
      51,    80,    52,    70,    90,    95,   137,   138,   162,   171,
     172,   176,   159,   189,    25,    41,    55,    72,    56,    82,
      26,    42,    59,    73,    60,    85,    27,    28,    43,    62,
      63,    64,    65,    91,     4,   181
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -161
static const yytype_int16 yypact[] =
{
       1,  -161,    15,    12,    14,  -161,    28,  -161,    37,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,    -6,  -161,    57,    60,
      74,    78,  -161,  -161,  -161,  -161,  -161,  -161,    79,    23,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,    80,  -161,    25,
      27,    31,    33,    35,  -161,    19,  -161,  -161,    28,  -161,
    -161,  -161,    28,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,    81,    28,  -161,    82,  -161,  -161,  -161,
    -161,  -161,    52,    55,  -161,  -161,  -161,    19,    82,    82,
      82,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,    82,
    -161,  -161,  -161,   210,  -161,   142,   162,  -161,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,  -161,  -161,    82,  -161,  -161,  -161,    82,    82,  -161,
      82,    82,    82,    82,    82,    82,    82,    82,  -161,  -161,
      82,    82,    82,  -161,    82,    82,    82,  -161,  -161,  -161,
    -161,  -161,    19,    82,    19,    82,    19,    82,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,    19,    19,  -161,    19,
      82,  -161,    19,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,    19,    19,  -161,  -161,  -161,    19,  -161,  -161,  -161,
    -161,  -161,    82,  -161,    82,  -161,    82,  -161,  -161,  -161,
    -161,    82,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -143,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,     5,    -2,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -160,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
      24,  -161,    66,   -56,  -161,    -4
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -126
static const yytype_int16 yytable[] =
{
      13,   183,   192,   185,    18,    19,    20,    21,    22,     1,
      78,   193,   194,   187,   188,     5,   195,     9,    10,    11,
      12,     6,    89,    10,    11,    12,    35,    14,    46,    14,
      50,    14,    14,    94,    54,    14,    58,    14,  -125,    14,
      17,    67,   140,   141,   143,   145,   147,   148,   149,   150,
     151,   152,   153,   154,   155,    81,    14,   158,    84,    14,
      30,   160,   161,    31,   163,   164,   165,   166,   167,   168,
     169,   170,    15,    88,   173,   174,   175,    32,   177,   178,
     179,    33,    34,    44,    74,    92,    76,   182,    75,   184,
      96,   186,     0,     0,     0,    38,     0,     0,     0,     0,
       0,     0,     0,     0,   191,    49,    53,    57,    61,     0,
       0,     0,     0,     0,    69,     0,     0,     0,    71,     0,
       0,     0,     0,     0,     0,     0,   196,     0,   197,     0,
     198,     0,     0,     0,     0,   199,     0,     0,    83,    86,
       0,     0,     0,     0,     0,   116,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   190,     0,     0,   190,     0,
       0,     0,     0,     0,     0,   139,     0,   190,   190,     0,
       0,     0,   190,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    97,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113
};

static const yytype_int16 yycheck[] =
{
       4,   144,   162,   146,    10,    11,    12,    13,    14,     8,
      66,   171,   172,   156,   157,     0,   176,     3,     4,     5,
       6,     9,    78,     4,     5,     6,     3,     4,     3,     4,
       3,     4,     4,    89,     3,     4,     3,     4,     3,     4,
       3,    45,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     3,     4,   113,     3,     4,
       3,   117,   118,     3,   120,   121,   122,   123,   124,   125,
     126,   127,     6,    77,   130,   131,   132,     3,   134,   135,
     136,     3,     3,     3,     3,    80,     4,   143,    64,   145,
      92,   147,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   160,    39,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   182,    -1,   184,    -1,
     186,    -1,    -1,    -1,    -1,   191,    -1,    -1,    72,    73,
      -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   159,    -1,    -1,   162,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    -1,   171,   172,    -1,
      -1,    -1,   176,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,    52,    53,   115,     0,     9,    54,    55,     3,
       4,     5,     6,   116,     4,   113,    72,     3,    10,    11,
      12,    13,    14,    73,    79,    95,   101,   107,   108,    56,
       3,     3,     3,     3,     3,     3,    57,    59,   113,    74,
      80,    96,   102,   109,     3,    58,     3,    75,    77,   113,
       3,    81,    83,   113,     3,    97,    99,   113,     3,   103,
     105,   113,   110,   111,   112,   113,    60,   116,    78,   113,
      84,   113,    98,   104,     3,   111,     4,    61,   114,    76,
      82,     3,   100,   113,     3,   106,   113,    62,   116,   114,
      85,   114,    85,    63,   114,    86,    86,     3,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    64,    65,     3,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    87,    88,     3,
     114,   114,    68,   114,    69,   114,    70,   114,   114,   114,
     114,   114,   114,   114,   114,   114,    66,    67,   114,    93,
     114,   114,    89,   114,   114,   114,   114,   114,   114,   114,
     114,    90,    91,   114,   114,   114,    92,   114,   114,   114,
      71,   116,   114,    71,   114,    71,   114,    71,    71,    94,
     116,   114,    94,    94,    94,    94,   114,   114,   114,   114
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 185 "foreign-pajek-parser.y"
    {
  igraph_pajek_vcount=(yyvsp[(2) - (2)].intnum);
}
    break;

  case 11:
#line 193 "foreign-pajek-parser.y"
    { igraph_i_pajek_actvertex=(yyvsp[(1) - (1)].intnum); }
    break;

  case 12:
#line 193 "foreign-pajek-parser.y"
    { }
    break;

  case 13:
#line 196 "foreign-pajek-parser.y"
    { (yyval.intnum)=(yyvsp[(1) - (1)].intnum); igraph_pajek_mode=1; }
    break;

  case 14:
#line 198 "foreign-pajek-parser.y"
    {
  igraph_i_pajek_add_string_vertex_attribute("id", (yyvsp[(1) - (1)].string).str, (yyvsp[(1) - (1)].string).len);
}
    break;

  case 16:
#line 203 "foreign-pajek-parser.y"
    { 
  igraph_i_pajek_add_numeric_vertex_attribute("x", (yyvsp[(1) - (2)].realnum));
  igraph_i_pajek_add_numeric_vertex_attribute("y", (yyvsp[(2) - (2)].realnum));
	    }
    break;

  case 17:
#line 207 "foreign-pajek-parser.y"
    { 
  igraph_i_pajek_add_numeric_vertex_attribute("x", (yyvsp[(1) - (3)].realnum));
  igraph_i_pajek_add_numeric_vertex_attribute("y", (yyvsp[(2) - (3)].realnum));
  igraph_i_pajek_add_numeric_vertex_attribute("z", (yyvsp[(3) - (3)].realnum));
	    }
    break;

  case 19:
#line 213 "foreign-pajek-parser.y"
    { 
  igraph_i_pajek_add_string_vertex_attribute("shape", (yyvsp[(1) - (1)].string).str, (yyvsp[(1) - (1)].string).len);	      
}
    break;

  case 23:
#line 221 "foreign-pajek-parser.y"
    {
	 igraph_i_pajek_add_numeric_vertex_attribute("xfact", (yyvsp[(2) - (2)].realnum));
       }
    break;

  case 24:
#line 224 "foreign-pajek-parser.y"
    {
	 igraph_i_pajek_add_numeric_vertex_attribute("yfact", (yyvsp[(2) - (2)].realnum));
       }
    break;

  case 25:
#line 227 "foreign-pajek-parser.y"
    { /* RGB color */
         igraph_i_pajek_add_numeric_vertex_attribute("color-red", (yyvsp[(2) - (4)].realnum));
	 igraph_i_pajek_add_numeric_vertex_attribute("color-green", (yyvsp[(3) - (4)].realnum));
	 igraph_i_pajek_add_numeric_vertex_attribute("color-blue", (yyvsp[(4) - (4)].realnum));
       }
    break;

  case 26:
#line 232 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("framecolor-red", (yyvsp[(2) - (4)].realnum));
	 igraph_i_pajek_add_numeric_vertex_attribute("framecolor-green", (yyvsp[(3) - (4)].realnum));
	 igraph_i_pajek_add_numeric_vertex_attribute("framecolor-blue", (yyvsp[(4) - (4)].realnum));
       }
    break;

  case 27:
#line 237 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("labelcolor-red", (yyvsp[(2) - (4)].realnum));
	 igraph_i_pajek_add_numeric_vertex_attribute("labelcolor-green", (yyvsp[(3) - (4)].realnum));
	 igraph_i_pajek_add_numeric_vertex_attribute("labelcolor-blue", (yyvsp[(4) - (4)].realnum));
       }
    break;

  case 28:
#line 242 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("labeldist", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 29:
#line 245 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("labeldegree2", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 30:
#line 248 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("framewidth", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 31:
#line 251 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("fontsize", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 32:
#line 254 "foreign-pajek-parser.y"
    {       
         igraph_i_pajek_add_numeric_vertex_attribute("rotation", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 33:
#line 257 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("radius", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 34:
#line 260 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("diamondratio", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 35:
#line 263 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("labeldegree", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 36:
#line 266 "foreign-pajek-parser.y"
    {
         igraph_i_pajek_add_numeric_vertex_attribute("vertexsize", (yyvsp[(2) - (2)].realnum));
     }
    break;

  case 37:
#line 271 "foreign-pajek-parser.y"
    { igraph_pajek_mode=3; }
    break;

  case 38:
#line 271 "foreign-pajek-parser.y"
    { 
         igraph_pajek_mode=1;
	 igraph_i_pajek_add_string_vertex_attribute("font", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
     }
    break;

  case 39:
#line 275 "foreign-pajek-parser.y"
    { igraph_pajek_mode=3; }
    break;

  case 40:
#line 275 "foreign-pajek-parser.y"
    {
         igraph_pajek_mode=1;
	 igraph_i_pajek_add_string_vertex_attribute("url", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
     }
    break;

  case 41:
#line 279 "foreign-pajek-parser.y"
    { igraph_pajek_mode=3; }
    break;

  case 42:
#line 279 "foreign-pajek-parser.y"
    {
         igraph_pajek_mode=1;
	 igraph_i_pajek_add_string_vertex_attribute("color", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
     }
    break;

  case 43:
#line 283 "foreign-pajek-parser.y"
    { igraph_pajek_mode=3; }
    break;

  case 44:
#line 283 "foreign-pajek-parser.y"
    {
         igraph_pajek_mode=1;
	 igraph_i_pajek_add_string_vertex_attribute("framecolor", 
						    (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
     }
    break;

  case 45:
#line 288 "foreign-pajek-parser.y"
    { igraph_pajek_mode=3; }
    break;

  case 46:
#line 288 "foreign-pajek-parser.y"
    {
         igraph_pajek_mode=1;
	 igraph_i_pajek_add_string_vertex_attribute("labelcolor", 
						    (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
     }
    break;

  case 47:
#line 295 "foreign-pajek-parser.y"
    { (yyval.string)=(yyvsp[(1) - (1)].string); }
    break;

  case 54:
#line 299 "foreign-pajek-parser.y"
    { igraph_pajek_directed=1; }
    break;

  case 58:
#line 304 "foreign-pajek-parser.y"
    { igraph_i_pajek_actedge++;
	                  igraph_pajek_mode=2; }
    break;

  case 59:
#line 305 "foreign-pajek-parser.y"
    { 
  igraph_vector_push_back(igraph_pajek_vector, (yyvsp[(1) - (6)].intnum)-1);
  igraph_vector_push_back(igraph_pajek_vector, (yyvsp[(2) - (6)].intnum)-1); }
    break;

  case 62:
#line 314 "foreign-pajek-parser.y"
    { igraph_pajek_directed=0; }
    break;

  case 66:
#line 319 "foreign-pajek-parser.y"
    { igraph_i_pajek_actedge++; 
	                    igraph_pajek_mode=2; }
    break;

  case 67:
#line 320 "foreign-pajek-parser.y"
    { 
  igraph_vector_push_back(igraph_pajek_vector, (yyvsp[(1) - (6)].intnum)-1);
  igraph_vector_push_back(igraph_pajek_vector, (yyvsp[(2) - (6)].intnum)-1); }
    break;

  case 71:
#line 329 "foreign-pajek-parser.y"
    {
  igraph_i_pajek_add_numeric_edge_attribute("weight", (yyvsp[(1) - (1)].realnum));
}
    break;

  case 75:
#line 337 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("color-red", (yyvsp[(2) - (4)].realnum));
       igraph_i_pajek_add_numeric_edge_attribute("color-green", (yyvsp[(3) - (4)].realnum));
       igraph_i_pajek_add_numeric_edge_attribute("color-blue", (yyvsp[(4) - (4)].realnum));
   }
    break;

  case 76:
#line 342 "foreign-pajek-parser.y"
    { 
       igraph_i_pajek_add_numeric_edge_attribute("arrowsize", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 77:
#line 345 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("edgewidth", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 78:
#line 348 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("hook1", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 79:
#line 351 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("hook2", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 80:
#line 354 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("angle1", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 81:
#line 357 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("angle2", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 82:
#line 360 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("velocity1", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 83:
#line 363 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("velocity2", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 84:
#line 366 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("arrowpos", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 85:
#line 369 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("labelpos", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 86:
#line 372 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("labelangle", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 87:
#line 375 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("labelangle2", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 88:
#line 378 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("labeldegree", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 89:
#line 381 "foreign-pajek-parser.y"
    {		/* what is this??? */
       igraph_i_pajek_add_numeric_edge_attribute("arrowsize", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 90:
#line 384 "foreign-pajek-parser.y"
    {
       igraph_i_pajek_add_numeric_edge_attribute("fontsize", (yyvsp[(2) - (2)].realnum));
   }
    break;

  case 91:
#line 389 "foreign-pajek-parser.y"
    { igraph_pajek_mode=4; }
    break;

  case 92:
#line 389 "foreign-pajek-parser.y"
    {
      igraph_pajek_mode=2;
      igraph_i_pajek_add_string_edge_attribute("arrowtype", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
    }
    break;

  case 93:
#line 393 "foreign-pajek-parser.y"
    { igraph_pajek_mode=4; }
    break;

  case 94:
#line 393 "foreign-pajek-parser.y"
    {
      igraph_pajek_mode=2;
      igraph_i_pajek_add_string_edge_attribute("linepattern", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
    }
    break;

  case 95:
#line 397 "foreign-pajek-parser.y"
    { igraph_pajek_mode=4; }
    break;

  case 96:
#line 397 "foreign-pajek-parser.y"
    {
      igraph_pajek_mode=2;
      igraph_i_pajek_add_string_edge_attribute("label", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
    }
    break;

  case 97:
#line 401 "foreign-pajek-parser.y"
    { igraph_pajek_mode=4; }
    break;

  case 98:
#line 401 "foreign-pajek-parser.y"
    {
      igraph_pajek_mode=2;
      igraph_i_pajek_add_string_edge_attribute("labelcolor", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
    }
    break;

  case 99:
#line 405 "foreign-pajek-parser.y"
    { igraph_pajek_mode=4; }
    break;

  case 100:
#line 405 "foreign-pajek-parser.y"
    {
      igraph_pajek_mode=2;
      igraph_i_pajek_add_string_edge_attribute("color", (yyvsp[(3) - (3)].string).str, (yyvsp[(3) - (3)].string).len);
    }
    break;

  case 101:
#line 411 "foreign-pajek-parser.y"
    { igraph_pajek_mode=2; (yyval.string)=(yyvsp[(1) - (1)].string); }
    break;

  case 102:
#line 413 "foreign-pajek-parser.y"
    { igraph_pajek_directed=1; }
    break;

  case 109:
#line 421 "foreign-pajek-parser.y"
    { igraph_pajek_mode=0; igraph_pajek_actfrom=fabs((yyvsp[(1) - (1)].intnum))-1; }
    break;

  case 110:
#line 423 "foreign-pajek-parser.y"
    { 
  igraph_vector_push_back(igraph_pajek_vector, igraph_pajek_actfrom); 
  igraph_vector_push_back(igraph_pajek_vector, fabs((yyvsp[(1) - (1)].intnum))-1); 
}
    break;

  case 111:
#line 428 "foreign-pajek-parser.y"
    { igraph_pajek_directed=0; }
    break;

  case 118:
#line 436 "foreign-pajek-parser.y"
    { igraph_pajek_mode=0; igraph_pajek_actfrom=fabs((yyvsp[(1) - (1)].intnum))-1; }
    break;

  case 119:
#line 438 "foreign-pajek-parser.y"
    { 
  igraph_vector_push_back(igraph_pajek_vector, igraph_pajek_actfrom); 
  igraph_vector_push_back(igraph_pajek_vector, fabs((yyvsp[(1) - (1)].intnum))-1); 
}
    break;

  case 121:
#line 447 "foreign-pajek-parser.y"
    { igraph_pajek_actfrom=0; igraph_pajek_actto=0; }
    break;

  case 124:
#line 451 "foreign-pajek-parser.y"
    { igraph_pajek_actfrom++; igraph_pajek_actto=0; }
    break;

  case 127:
#line 455 "foreign-pajek-parser.y"
    {
  if ((yyvsp[(1) - (1)].intnum)>0) { 
    igraph_vector_push_back(igraph_pajek_vector, igraph_pajek_actfrom);
    igraph_vector_push_back(igraph_pajek_vector, igraph_pajek_actto);
  }
  igraph_pajek_actto++;
}
    break;

  case 128:
#line 465 "foreign-pajek-parser.y"
    { (yyval.intnum)=igraph_pajek_get_number(igraph_pajek_yytext,
					  igraph_pajek_yyleng); }
    break;

  case 129:
#line 468 "foreign-pajek-parser.y"
    { (yyval.realnum)=igraph_pajek_get_number(igraph_pajek_yytext,
					  igraph_pajek_yyleng); }
    break;

  case 132:
#line 473 "foreign-pajek-parser.y"
    { (yyval.string).str=igraph_pajek_yytext; (yyval.string).len=igraph_pajek_yyleng; }
    break;

  case 133:
#line 474 "foreign-pajek-parser.y"
    { (yyval.string).str=igraph_pajek_yytext; (yyval.string).len=igraph_pajek_yyleng; }
    break;

  case 134:
#line 475 "foreign-pajek-parser.y"
    { (yyval.string).str=igraph_pajek_yytext+1; (yyval.string).len=igraph_pajek_yyleng-2; }
    break;


/* Line 1267 of yacc.c.  */
#line 2254 "foreign-pajek-parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 477 "foreign-pajek-parser.y"


int igraph_pajek_yyerror(char *s)
{
  static char str[300];  
  igraph_i_pajek_reset_scanner();
  snprintf(str, sizeof(str), "Parse error in Pajek file, line %li (%s)", 
	   (long)igraph_pajek_mylineno, s);
  igraph_i_pajek_errmsg = str;
  return 0;
}

igraph_real_t igraph_pajek_get_number(const char *str, long int length) {
  igraph_real_t num;
  char *tmp=igraph_Calloc(length+1, char);
  
  strncpy(tmp, str, length);
  tmp[length]='\0';
  sscanf(tmp, "%lf", &num);
  igraph_Free(tmp);
  return num;
} 

/* TODO: NA's */

int igraph_i_pajek_add_numeric_attribute(igraph_trie_t *names,
					 igraph_vector_ptr_t *attrs,
					 long int count,
					 const char *attrname,
					 igraph_integer_t vid,
					 igraph_real_t number) {
  long int attrsize=igraph_trie_size(names);
  long int id;
  igraph_vector_t *na;
  igraph_i_attribute_record_t *rec;

  igraph_trie_get(names, attrname, &id);
  if (id == attrsize) {
    /* add a new attribute */
    rec=igraph_Calloc(1, igraph_i_attribute_record_t);
    na=igraph_Calloc(1, igraph_vector_t);
    igraph_vector_init(na, count);
    rec->name=strdup(attrname);
    rec->type=IGRAPH_ATTRIBUTE_NUMERIC;
    rec->value=na;
    igraph_vector_ptr_push_back(attrs, rec);
  }
  rec=VECTOR(*attrs)[id];
  na=(igraph_vector_t*)rec->value;
  if (igraph_vector_size(na) == vid) {
    IGRAPH_CHECK(igraph_vector_push_back(na, number));
  } else if (igraph_vector_size(na) < vid) {
    long int origsize=igraph_vector_size(na);
    IGRAPH_CHECK(igraph_vector_resize(na, (long int)vid+1));
    for (;origsize<count; origsize++) {
      VECTOR(*na)[origsize] = IGRAPH_NAN;
    }
    VECTOR(*na)[(long int) vid] = number;
  } else { 
    VECTOR(*na)[(long int) vid] = number;
  }    

  return 0;
}

/* TODO: NA's */

int igraph_i_pajek_add_string_attribute(igraph_trie_t *names,
					igraph_vector_ptr_t *attrs,
					long int count,
					const char *attrname,
					igraph_integer_t vid,
					const char *str) {
  long int attrsize=igraph_trie_size(names);
  long int id;
  igraph_strvector_t *na;
  igraph_i_attribute_record_t *rec;
  long int i;

  igraph_trie_get(names, attrname, &id);
  if (id == attrsize) {
    /* add a new attribute */
    rec=igraph_Calloc(1, igraph_i_attribute_record_t);
    na=igraph_Calloc(1, igraph_strvector_t);
    igraph_strvector_init(na, count);
    for (i=0; i<count; i++) {
      igraph_strvector_set(na, i, "");
    }
    rec->name=strdup(attrname);
    rec->type=IGRAPH_ATTRIBUTE_STRING;
    rec->value=na;
    igraph_vector_ptr_push_back(attrs, rec);
  }
  rec=VECTOR(*attrs)[id];
  na=(igraph_strvector_t*)rec->value;
  if (igraph_strvector_size(na) <= vid) { 
    long int origsize=igraph_strvector_size(na);
    IGRAPH_CHECK(igraph_strvector_resize(na, vid+1));
    for (;origsize<count; origsize++) {
      igraph_strvector_set(na, origsize, "");
    }
  }
  igraph_strvector_set(na, vid, str);

  return 0;
}

int igraph_i_pajek_add_string_vertex_attribute(const char *name, 
					       const char *value,
					       int len) {
  char *tmp;
  int ret;

  tmp=igraph_Calloc(len+1, char);
  if (tmp==0) {
    IGRAPH_ERROR("cannot add element to hash table", IGRAPH_ENOMEM);
  }
  IGRAPH_FINALLY(free, tmp);
  strncpy(tmp, value, len);
  tmp[len]='\0';

  ret=igraph_i_pajek_add_string_attribute(igraph_i_pajek_vertex_attribute_names,
					  igraph_i_pajek_vertex_attributes,
					  igraph_pajek_vcount,
					  name, igraph_i_pajek_actvertex-1,
					  tmp);
  
  igraph_Free(tmp);
  IGRAPH_FINALLY_CLEAN(1);
  
  return ret;
}

int igraph_i_pajek_add_string_edge_attribute(const char *name, 
					     const char *value,
					     int len) {
  char *tmp;
  int ret;

  tmp=igraph_Calloc(len+1, char);
  if (tmp==0) {
    IGRAPH_ERROR("cannot add element to hash table", IGRAPH_ENOMEM);
  }
  IGRAPH_FINALLY(free, tmp);
  strncpy(tmp, value, len);
  tmp[len]='\0';
  
  ret=igraph_i_pajek_add_string_attribute(igraph_i_pajek_edge_attribute_names,
					  igraph_i_pajek_edge_attributes,
					  igraph_i_pajek_actedge,
					  name, igraph_i_pajek_actedge-1,
					  tmp);

  igraph_Free(tmp);
  IGRAPH_FINALLY_CLEAN(1);
  
  return ret;
}

int igraph_i_pajek_add_numeric_vertex_attribute(const char *name, 
						igraph_real_t value) {
  
  return
    igraph_i_pajek_add_numeric_attribute(igraph_i_pajek_vertex_attribute_names,
					 igraph_i_pajek_vertex_attributes,
					 igraph_pajek_vcount,
					 name, igraph_i_pajek_actvertex-1,
					 value);
}

int igraph_i_pajek_add_numeric_edge_attribute(const char *name, 
					      igraph_real_t value) {

  return
    igraph_i_pajek_add_numeric_attribute(igraph_i_pajek_edge_attribute_names,
					 igraph_i_pajek_edge_attributes,
					 igraph_i_pajek_actedge,
					 name, igraph_i_pajek_actedge-1,
					 value);
}

