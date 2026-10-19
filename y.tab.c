/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "gramatica.y"

#include <iostream>
#include "main.h"
int yylex();
void yyerror(const char *s);

static Tipo tipo_actual   = Tipo::NINGUNO;
static Uso  uso_actual    = Uso::NINGUNO;
static std::string ambito_actual = "global";
static std::vector<EntradaTS*> funciones_abiertas; // pila de funciones en curso (hay funciones anidadas)

static std::string valor_por_defecto(Tipo tipo) {
    switch (tipo) {
        case Tipo::USHORTINT: return "0";
        case Tipo::DOUBLEF:   return "0.0";
        default:              return "0";
    }
}

static void declarar(EntradaTS *e, Uso uso, Tipo tipo) {
    if (e->uso != Uso::NINGUNO)
        std::cerr << "Linea " << LINEA_ACTUAL << ": '" << e->lexema
                  << "' ya declarado" << std::endl;
    e->uso = uso;
    e->tipo = tipo;
    e->ambito = ambito_actual;
}

#line 100 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    CTE_INT = 259,                 /* CTE_INT  */
    CTE_FLOAT = 260,               /* CTE_FLOAT  */
    CTE_STR = 261,                 /* CTE_STR  */
    PR_IF = 262,                   /* PR_IF  */
    PR_ELSE = 263,                 /* PR_ELSE  */
    PR_END_IF = 264,               /* PR_END_IF  */
    PR_BEGIN = 265,                /* PR_BEGIN  */
    PR_END = 266,                  /* PR_END  */
    PR_POUT = 267,                 /* PR_POUT  */
    PR_RET = 268,                  /* PR_RET  */
    PR_CLASS = 269,                /* PR_CLASS  */
    PR_FUNCTION = 270,             /* PR_FUNCTION  */
    PR_USHORTINT = 271,            /* PR_USHORTINT  */
    PR_DOUBLEF = 272,              /* PR_DOUBLEF  */
    PR_EXTENDS = 273,              /* PR_EXTENDS  */
    PR_TYPEDEF = 274,              /* PR_TYPEDEF  */
    PR_REPEAT = 275,               /* PR_REPEAT  */
    PR_WHILE = 276,                /* PR_WHILE  */
    OP_ASIGNACION = 277,           /* OP_ASIGNACION  */
    OP_IGUAL_IGUAL = 278,          /* OP_IGUAL_IGUAL  */
    OP_MENOR_IGUAL = 279,          /* OP_MENOR_IGUAL  */
    OP_MAYOR_IGUAL = 280,          /* OP_MAYOR_IGUAL  */
    OP_DISTINTO = 281              /* OP_DISTINTO  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ID 258
#define CTE_INT 259
#define CTE_FLOAT 260
#define CTE_STR 261
#define PR_IF 262
#define PR_ELSE 263
#define PR_END_IF 264
#define PR_BEGIN 265
#define PR_END 266
#define PR_POUT 267
#define PR_RET 268
#define PR_CLASS 269
#define PR_FUNCTION 270
#define PR_USHORTINT 271
#define PR_DOUBLEF 272
#define PR_EXTENDS 273
#define PR_TYPEDEF 274
#define PR_REPEAT 275
#define PR_WHILE 276
#define OP_ASIGNACION 277
#define OP_IGUAL_IGUAL 278
#define OP_MENOR_IGUAL 279
#define OP_MAYOR_IGUAL 280
#define OP_DISTINTO 281

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 31 "gramatica.y"

    EntradaTS *ts_ref;
    Tipo tipo_val;

#line 210 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_CTE_INT = 4,                    /* CTE_INT  */
  YYSYMBOL_CTE_FLOAT = 5,                  /* CTE_FLOAT  */
  YYSYMBOL_CTE_STR = 6,                    /* CTE_STR  */
  YYSYMBOL_PR_IF = 7,                      /* PR_IF  */
  YYSYMBOL_PR_ELSE = 8,                    /* PR_ELSE  */
  YYSYMBOL_PR_END_IF = 9,                  /* PR_END_IF  */
  YYSYMBOL_PR_BEGIN = 10,                  /* PR_BEGIN  */
  YYSYMBOL_PR_END = 11,                    /* PR_END  */
  YYSYMBOL_PR_POUT = 12,                   /* PR_POUT  */
  YYSYMBOL_PR_RET = 13,                    /* PR_RET  */
  YYSYMBOL_PR_CLASS = 14,                  /* PR_CLASS  */
  YYSYMBOL_PR_FUNCTION = 15,               /* PR_FUNCTION  */
  YYSYMBOL_PR_USHORTINT = 16,              /* PR_USHORTINT  */
  YYSYMBOL_PR_DOUBLEF = 17,                /* PR_DOUBLEF  */
  YYSYMBOL_PR_EXTENDS = 18,                /* PR_EXTENDS  */
  YYSYMBOL_PR_TYPEDEF = 19,                /* PR_TYPEDEF  */
  YYSYMBOL_PR_REPEAT = 20,                 /* PR_REPEAT  */
  YYSYMBOL_PR_WHILE = 21,                  /* PR_WHILE  */
  YYSYMBOL_OP_ASIGNACION = 22,             /* OP_ASIGNACION  */
  YYSYMBOL_OP_IGUAL_IGUAL = 23,            /* OP_IGUAL_IGUAL  */
  YYSYMBOL_OP_MENOR_IGUAL = 24,            /* OP_MENOR_IGUAL  */
  YYSYMBOL_OP_MAYOR_IGUAL = 25,            /* OP_MAYOR_IGUAL  */
  YYSYMBOL_OP_DISTINTO = 26,               /* OP_DISTINTO  */
  YYSYMBOL_27_ = 27,                       /* '+'  */
  YYSYMBOL_28_ = 28,                       /* '-'  */
  YYSYMBOL_29_ = 29,                       /* '*'  */
  YYSYMBOL_30_ = 30,                       /* '/'  */
  YYSYMBOL_31_ = 31,                       /* ';'  */
  YYSYMBOL_32_ = 32,                       /* ','  */
  YYSYMBOL_33_ = 33,                       /* '('  */
  YYSYMBOL_34_ = 34,                       /* ')'  */
  YYSYMBOL_35_ = 35,                       /* '='  */
  YYSYMBOL_36_ = 36,                       /* '['  */
  YYSYMBOL_37_ = 37,                       /* ']'  */
  YYSYMBOL_38_ = 38,                       /* '.'  */
  YYSYMBOL_39_ = 39,                       /* '<'  */
  YYSYMBOL_40_ = 40,                       /* '>'  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_prog = 42,                      /* prog  */
  YYSYMBOL_sent_decl_lista = 43,           /* sent_decl_lista  */
  YYSYMBOL_sent_decl = 44,                 /* sent_decl  */
  YYSYMBOL_tipo = 45,                      /* tipo  */
  YYSYMBOL_lista_ids = 46,                 /* lista_ids  */
  YYSYMBOL_decl_variables = 47,            /* decl_variables  */
  YYSYMBOL_lista_params_formales = 48,     /* lista_params_formales  */
  YYSYMBOL_decl_funcion = 49,              /* decl_funcion  */
  YYSYMBOL_50_1 = 50,                      /* $@1  */
  YYSYMBOL_decl_clase = 51,                /* decl_clase  */
  YYSYMBOL_clase_item_lista = 52,          /* clase_item_lista  */
  YYSYMBOL_clase_item = 53,                /* clase_item  */
  YYSYMBOL_54_2 = 54,                      /* $@2  */
  YYSYMBOL_extends_clause = 55,            /* extends_clause  */
  YYSYMBOL_decl_typedef = 56,              /* decl_typedef  */
  YYSYMBOL_lista_valores = 57,             /* lista_valores  */
  YYSYMBOL_sent_ejec_lista = 58,           /* sent_ejec_lista  */
  YYSYMBOL_sent_ejec = 59,                 /* sent_ejec  */
  YYSYMBOL_asignacion = 60,                /* asignacion  */
  YYSYMBOL_asignacion_atributo = 61,       /* asignacion_atributo  */
  YYSYMBOL_indice = 62,                    /* indice  */
  YYSYMBOL_retorno = 63,                   /* retorno  */
  YYSYMBOL_pout = 64,                      /* pout  */
  YYSYMBOL_bloque_sent_ejec = 65,          /* bloque_sent_ejec  */
  YYSYMBOL_condicion = 66,                 /* condicion  */
  YYSYMBOL_comparador = 67,                /* comparador  */
  YYSYMBOL_seleccion = 68,                 /* seleccion  */
  YYSYMBOL_repeat_while = 69,              /* repeat_while  */
  YYSYMBOL_expresion = 70,                 /* expresion  */
  YYSYMBOL_termino = 71,                   /* termino  */
  YYSYMBOL_factor = 72,                    /* factor  */
  YYSYMBOL_expresion_restr = 73,           /* expresion_restr  */
  YYSYMBOL_termino_restr = 74,             /* termino_restr  */
  YYSYMBOL_factor_restr = 75,              /* factor_restr  */
  YYSYMBOL_cte = 76,                       /* cte  */
  YYSYMBOL_invocacion = 77,                /* invocacion  */
  YYSYMBOL_lista_params_reales = 78,       /* lista_params_reales  */
  YYSYMBOL_lista_ctes_opcional = 79,       /* lista_ctes_opcional  */
  YYSYMBOL_lista_constantes = 80,          /* lista_constantes  */
  YYSYMBOL_acceso_objeto = 81,             /* acceso_objeto  */
  YYSYMBOL_llamada_opcional = 82           /* llamada_opcional  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   322

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  248

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   281


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      33,    34,    29,    27,    32,    28,    38,    30,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    31,
      39,    35,    40,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    36,     2,    37,     2,     2,     2,     2,     2,     2,
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
      25,    26
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    51,    51,    55,    62,    74,    75,    83,    87,    88,
      89,    90,    94,    95,    96,   100,   101,   105,   110,   120,
     121,   125,   124,   147,   157,   162,   172,   173,   177,   179,
     178,   198,   199,   209,   214,   224,   229,   239,   240,   246,
     247,   255,   259,   260,   261,   262,   263,   264,   265,   269,
     274,   284,   289,   294,   301,   311,   312,   316,   324,   334,
     339,   344,   354,   355,   359,   363,   364,   365,   366,   367,
     368,   372,   377,   383,   394,   399,   411,   412,   413,   417,
     418,   419,   423,   424,   425,   436,   453,   454,   455,   459,
     460,   461,   465,   466,   467,   471,   472,   473,   484,   501,
     502,   506,   507,   511,   512,   522,   523,   527,   528,   535,
     539,   540,   544,   545,   549,   550
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "CTE_INT",
  "CTE_FLOAT", "CTE_STR", "PR_IF", "PR_ELSE", "PR_END_IF", "PR_BEGIN",
  "PR_END", "PR_POUT", "PR_RET", "PR_CLASS", "PR_FUNCTION", "PR_USHORTINT",
  "PR_DOUBLEF", "PR_EXTENDS", "PR_TYPEDEF", "PR_REPEAT", "PR_WHILE",
  "OP_ASIGNACION", "OP_IGUAL_IGUAL", "OP_MENOR_IGUAL", "OP_MAYOR_IGUAL",
  "OP_DISTINTO", "'+'", "'-'", "'*'", "'/'", "';'", "','", "'('", "')'",
  "'='", "'['", "']'", "'.'", "'<'", "'>'", "$accept", "prog",
  "sent_decl_lista", "sent_decl", "tipo", "lista_ids", "decl_variables",
  "lista_params_formales", "decl_funcion", "$@1", "decl_clase",
  "clase_item_lista", "clase_item", "$@2", "extends_clause",
  "decl_typedef", "lista_valores", "sent_ejec_lista", "sent_ejec",
  "asignacion", "asignacion_atributo", "indice", "retorno", "pout",
  "bloque_sent_ejec", "condicion", "comparador", "seleccion",
  "repeat_while", "expresion", "termino", "factor", "expresion_restr",
  "termino_restr", "factor_restr", "cte", "invocacion",
  "lista_params_reales", "lista_ctes_opcional", "lista_constantes",
  "acceso_objeto", "llamada_opcional", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-184)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-22)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      24,  -184,    13,   201,  -184,    44,  -184,  -184,    38,  -184,
    -184,    73,  -184,    36,  -184,  -184,  -184,  -184,  -184,   103,
     125,    34,    57,    49,    64,  -184,    52,   110,    90,    10,
      14,   116,    17,    19,    70,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,   120,     9,   123,    83,  -184,   119,  -184,    20,
    -184,   156,  -184,   146,    26,    21,   233,   179,   154,    26,
    -184,   157,    59,   161,    26,   168,  -184,  -184,   185,  -184,
    -184,  -184,   178,   126,   132,   102,  -184,  -184,   158,   202,
     186,  -184,  -184,   172,  -184,  -184,   250,   112,   203,  -184,
    -184,  -184,  -184,   182,   229,   135,   192,  -184,  -184,   206,
       7,  -184,   212,   225,  -184,   224,    51,  -184,    69,  -184,
     153,   227,  -184,   209,   230,   236,   257,   258,  -184,    -9,
    -184,   259,    53,   254,   267,   288,  -184,  -184,    26,    26,
    -184,    26,    26,   260,    26,   260,  -184,   262,   261,    26,
     256,  -184,  -184,  -184,  -184,  -184,  -184,    26,   263,   268,
     269,   271,    26,  -184,  -184,  -184,  -184,   265,  -184,   158,
     272,  -184,   290,   149,    63,   273,   274,   203,   203,  -184,
    -184,   107,  -184,   229,  -184,    26,  -184,   130,   264,   229,
    -184,  -184,  -184,  -184,   275,    53,  -184,  -184,  -184,    53,
    -184,     0,   270,    97,   248,  -184,  -184,  -184,  -184,  -184,
      26,  -184,   276,  -184,    80,   144,  -184,   256,   277,   280,
     159,   292,   211,  -184,  -184,    63,    63,  -184,    63,    63,
     207,  -184,   297,  -184,  -184,   295,  -184,  -184,  -184,  -184,
      44,  -184,   248,   248,  -184,  -184,  -184,  -184,   281,   228,
     167,  -184,  -184,   283,   183,  -184,   284,  -184
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     7,     0,     0,     1,    41,    14,    41,     0,    12,
      13,     0,     5,     0,     8,     9,    10,    11,     6,     0,
       0,     0,     0,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,    42,    43,    47,    46,
      45,    48,     0,     0,     0,     0,    36,     0,    18,     0,
      17,     0,    40,     0,     0,     0,     0,     0,     0,     0,
       3,     0,     0,     0,     0,     0,    41,    62,     0,    44,
       4,     2,     0,     0,     0,     0,    27,    31,     0,     0,
       0,    15,    50,    82,   101,   102,     0,     0,    78,    81,
      83,    86,    87,     0,   106,     0,     0,    56,    55,     0,
       0,    73,     0,     0,    61,     0,     0,    58,     0,    75,
       0,     0,    25,     0,     0,     0,    29,     0,    26,     0,
      38,     0,     0,     0,     0,     0,    84,    85,     0,     0,
      49,     0,     0,   109,     0,   109,    54,     0,     0,     0,
       0,    65,    66,    67,    68,    69,    70,     0,     0,     0,
       0,     0,     0,    34,    33,    32,    28,     0,    24,     0,
       0,    23,     0,     0,     0,     0,   115,    76,    77,    79,
      80,     0,   104,   105,   103,     0,    53,     0,     0,    64,
      59,    60,    57,    63,     0,     0,    37,    35,    20,     0,
       7,    95,     0,     0,    91,    94,    96,    99,   100,   113,
       0,   112,     0,   111,     0,     0,    51,     0,     0,     0,
       0,     0,     0,    97,    98,     0,     0,    88,     0,     0,
       0,   108,     0,   107,    52,     0,    71,    74,     7,    19,
       0,    41,    89,    90,    92,    93,   114,   110,     0,     0,
       0,    72,    41,     0,     0,    22,     0,    30
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -184,  -184,  -183,  -184,   -41,   232,  -184,   121,  -184,  -184,
    -184,  -184,   241,  -184,  -184,  -184,  -184,    -7,   -31,  -184,
    -184,   193,  -184,  -184,  -135,   166,  -184,  -184,  -184,   -45,
     151,   150,  -184,    68,    67,   -66,   -18,   122,   184,  -184,
    -158,  -184
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,    12,    13,    27,    14,   163,    15,    80,
      16,    75,    76,   157,    77,    17,   119,    19,    35,    36,
      37,    99,    38,    39,    68,   102,   147,    40,    41,    94,
      88,    89,   193,   194,   195,    90,    91,    95,   172,   204,
      92,   201
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      20,    42,    42,    67,    74,   178,   198,   212,   138,    87,
      70,    53,   120,     4,   103,    58,    42,   106,    61,   108,
      63,    79,    93,   159,    83,    84,    85,     1,   160,    83,
      84,    85,    54,    55,    74,    44,   124,    24,   125,    25,
      71,    21,   139,    55,    45,   239,    56,    59,    57,    86,
      62,    26,    64,   -21,    86,    49,     6,   198,   198,   110,
     198,   198,    83,    84,    85,   105,   191,    84,    85,     9,
      10,    65,   225,    29,    22,    18,    23,    30,   128,   129,
      66,   162,    32,    33,    47,   149,     6,    86,    46,   173,
      34,   192,    42,   186,   177,    48,   128,   129,   196,     9,
      10,    73,   179,   150,    28,     6,    29,   103,   202,    67,
      30,   203,   222,   117,    31,    32,    33,   223,     9,    10,
      73,    52,    42,    34,   215,   216,    28,   113,    29,    25,
     205,   217,    30,   115,    72,   116,    43,    32,    33,   128,
     129,    50,    51,   130,   162,    34,   197,    60,   211,   196,
     196,    69,   196,   196,    28,    78,    29,   128,   129,    81,
      30,   206,    84,    85,   151,    32,    33,   134,    28,   135,
      29,   128,   129,    34,    30,   224,    67,    82,   243,    32,
      33,   189,   100,   190,    28,   101,    29,    34,   104,    42,
      30,   189,   107,   228,   246,    32,    33,   197,   197,   109,
     197,   197,     5,    34,     6,    55,   111,   123,   124,   112,
     125,     7,   230,   121,     6,     8,   133,     9,    10,   122,
      11,   231,    42,   136,   240,     8,    42,     9,    10,   230,
      11,     6,   131,   132,    96,   244,    97,    98,   242,   134,
     153,   236,     8,   137,     9,    10,   140,    11,   141,   142,
     143,   144,   128,   129,   126,   127,   128,   129,   148,    29,
     152,   154,    51,    30,   145,   146,    66,   155,    32,    33,
      97,    98,   207,   208,   213,   214,    34,   218,   219,   167,
     168,   169,   170,   232,   233,   234,   235,   164,   156,   158,
     161,   166,   176,   188,   180,   229,   171,   175,   185,   181,
     182,   237,   183,   187,   238,   114,   210,   200,   226,   209,
     199,   227,   241,   221,   245,   247,   118,   165,   184,   174,
       0,     0,   220
};

static const yytype_int16 yycheck[] =
{
       7,    19,    20,    34,    45,   140,   164,   190,     1,    54,
       1,     1,    78,     0,    59,     1,    34,    62,     1,    64,
       1,     1,     1,    32,     3,     4,     5,     3,    37,     3,
       4,     5,    22,    33,    75,     1,    36,     1,    38,     3,
      31,     3,    35,    33,    10,   228,    36,    33,    38,    28,
      33,    15,    33,    33,    28,     3,     3,   215,   216,    66,
     218,   219,     3,     4,     5,     6,     3,     4,     5,    16,
      17,     1,   207,     3,     1,    31,     3,     7,    27,    28,
      10,   122,    12,    13,    35,    34,     3,    28,    31,   134,
      20,    28,   110,   159,   139,    31,    27,    28,   164,    16,
      17,    18,   147,    34,     1,     3,     3,   152,     1,   140,
       7,     4,    32,    11,    11,    12,    13,    37,    16,    17,
      18,    31,   140,    20,    27,    28,     1,     1,     3,     3,
     175,    34,     7,     1,    11,     3,    11,    12,    13,    27,
      28,    31,    32,    31,   185,    20,   164,    31,   189,   215,
     216,    31,   218,   219,     1,    36,     3,    27,    28,     3,
       7,    31,     4,     5,    11,    12,    13,    32,     1,    34,
       3,    27,    28,    20,     7,    31,   207,    31,    11,    12,
      13,    32,     3,    34,     1,    31,     3,    20,    31,   207,
       7,    32,    31,    34,    11,    12,    13,   215,   216,    31,
     218,   219,     1,    20,     3,    33,    21,    35,    36,    31,
      38,    10,     1,    11,     3,    14,    34,    16,    17,    33,
      19,    10,   240,    31,   231,    14,   244,    16,    17,     1,
      19,     3,    29,    30,     1,   242,     3,     4,    10,    32,
      31,    34,    14,    37,    16,    17,    34,    19,    23,    24,
      25,    26,    27,    28,     4,     5,    27,    28,    34,     3,
      33,    31,    32,     7,    39,    40,    10,    31,    12,    13,
       3,     4,     8,     9,     4,     5,    20,    29,    30,   128,
     129,   131,   132,   215,   216,   218,   219,    33,    31,    31,
      31,     3,    31,     3,    31,     3,    36,    35,    33,    31,
      31,     4,    31,    31,     9,    73,   185,    33,    31,    34,
      37,    31,    31,    37,    31,    31,    75,   124,   152,   135,
      -1,    -1,   200
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    42,    43,     0,     1,     3,    10,    14,    16,
      17,    19,    44,    45,    47,    49,    51,    56,    31,    58,
      58,     3,     1,     3,     1,     3,    15,    46,     1,     3,
       7,    11,    12,    13,    20,    59,    60,    61,    63,    64,
      68,    69,    77,    11,     1,    10,    31,    35,    31,     3,
      31,    32,    31,     1,    22,    33,    36,    38,     1,    33,
      31,     1,    33,     1,    33,     1,    10,    59,    65,    31,
       1,    31,    11,    18,    45,    52,    53,    55,    36,     1,
      50,     3,    31,     3,     4,     5,    28,    70,    71,    72,
      76,    77,    81,     1,    70,    78,     1,     3,     4,    62,
       3,    31,    66,    70,    31,     6,    70,    31,    70,    31,
      58,    21,    31,     1,    46,     1,     3,    11,    53,    57,
      76,    11,    33,    35,    36,    38,     4,     5,    27,    28,
      31,    29,    30,    34,    32,    34,    31,    37,     1,    35,
      34,    23,    24,    25,    26,    39,    40,    67,    34,    34,
      34,    11,    33,    31,    31,    31,    31,    54,    31,    32,
      37,    31,    45,    48,    33,    62,     3,    71,    71,    72,
      72,    36,    79,    70,    79,    35,    31,    70,    65,    70,
      31,    31,    31,    31,    66,    33,    76,    31,     3,    32,
      34,     3,    28,    73,    74,    75,    76,    77,    81,    37,
      33,    82,     1,     4,    80,    70,    31,     8,     9,    34,
      48,    45,    43,     4,     5,    27,    28,    34,    29,    30,
      78,    37,    32,    37,    31,    65,    31,    31,    34,     3,
       1,    10,    74,    74,    75,    75,    34,     4,     9,    43,
      58,    31,    10,    11,    58,    31,    11,    31
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    42,    42,    43,    43,    43,    44,    44,
      44,    44,    45,    45,    45,    46,    46,    47,    47,    48,
      48,    50,    49,    49,    51,    51,    52,    52,    53,    54,
      53,    53,    53,    55,    55,    56,    56,    57,    57,    58,
      58,    58,    59,    59,    59,    59,    59,    59,    59,    60,
      60,    61,    61,    61,    61,    62,    62,    63,    63,    64,
      64,    64,    65,    65,    66,    67,    67,    67,    67,    67,
      67,    68,    68,    68,    69,    69,    70,    70,    70,    71,
      71,    71,    72,    72,    72,    72,    72,    72,    72,    73,
      73,    73,    74,    74,    74,    75,    75,    75,    75,    75,
      75,    76,    76,    77,    77,    78,    78,    79,    79,    79,
      80,    80,    81,    81,    82,    82
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     6,     6,     6,     2,     3,     0,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     3,     4,
       2,     0,    12,     6,     6,     5,     2,     1,     3,     0,
      11,     1,     3,     3,     3,     7,     3,     3,     1,     2,
       3,     0,     1,     1,     2,     1,     1,     1,     1,     4,
       3,     6,     7,     5,     4,     1,     1,     5,     3,     5,
       5,     3,     1,     4,     3,     1,     1,     1,     1,     1,
       1,     7,     9,     3,     7,     3,     3,     3,     1,     3,
       3,     1,     1,     1,     2,     2,     1,     1,     5,     3,
       3,     1,     3,     3,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     5,     5,     3,     1,     3,     3,     0,
       3,     1,     4,     4,     3,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* prog: ID sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'  */
#line 52 "gramatica.y"
      {
          std::cout << "Se reconocio un programa" << std::endl;
      }
#line 1724 "y.tab.c"
    break;

  case 3: /* prog: ID sent_decl_lista error sent_ejec_lista PR_END ';'  */
#line 56 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: falta 'begin' o esta mal formada la seccion declarativa del programa."
                    << std::endl;
          yyerrok;
      }
#line 1735 "y.tab.c"
    break;

  case 4: /* prog: ID sent_decl_lista PR_BEGIN sent_ejec_lista PR_END error  */
#line 63 "gramatica.y"
      {
      	  std::cerr << "Linea " << LINEA_ACTUAL
      	            << ": Error: falta ';' despues del 'end' del programa."
      	            << std::endl;
      	  yyerrok;
      }
#line 1746 "y.tab.c"
    break;

  case 6: /* sent_decl_lista: sent_decl_lista error ';'  */
#line 76 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia declarativa mal formada. "
                    << "Se descartan tokens hasta ';'."
                    << std::endl;
          yyerrok;
      }
#line 1758 "y.tab.c"
    break;

  case 12: /* tipo: PR_USHORTINT  */
#line 94 "gramatica.y"
                   { (yyval.tipo_val) = Tipo::USHORTINT; }
#line 1764 "y.tab.c"
    break;

  case 13: /* tipo: PR_DOUBLEF  */
#line 95 "gramatica.y"
                   { (yyval.tipo_val) = Tipo::DOUBLEF; }
#line 1770 "y.tab.c"
    break;

  case 14: /* tipo: ID  */
#line 96 "gramatica.y"
                   { (yyval.tipo_val) = Tipo::NINGUNO; }
#line 1776 "y.tab.c"
    break;

  case 17: /* decl_variables: tipo lista_ids ';'  */
#line 106 "gramatica.y"
      {
          std::cout << "Declaracion de variables (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1785 "y.tab.c"
    break;

  case 18: /* decl_variables: tipo error ';'  */
#line 111 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de variables mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1796 "y.tab.c"
    break;

  case 21: /* $@1: %empty  */
#line 125 "gramatica.y"
       { 
           (yyvsp[0].ts_ref)->uso = Uso::FUNCION;
           (yyvsp[0].ts_ref)->tipo = (yyvsp[-2].tipo_val);
           (yyvsp[0].ts_ref)->tiene_ret = false;
           funciones_abiertas.push_back((yyvsp[0].ts_ref)); 
       }
#line 1807 "y.tab.c"
    break;

  case 22: /* decl_funcion: tipo PR_FUNCTION ID $@1 '(' lista_params_formales ')' sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'  */
#line 133 "gramatica.y"
       {
           EntradaTS *func = funciones_abiertas.back();
           funciones_abiertas.pop_back();

           if (!func->tiene_ret) {
               std::cout << "Linea " << LINEA_ACTUAL 
                         << ": Aviso: la funcion '" << func->lexema 
                         << "' no tiene sentencia 'ret'. Se asume valor de retorno por defecto ("
                         << valor_por_defecto(func->tipo) << ")." << std::endl;
           }
           std::cout << "Declaracion de funcion (linea "
                     << LINEA_ACTUAL << ")" << std::endl;
       }
#line 1825 "y.tab.c"
    break;

  case 23: /* decl_funcion: tipo PR_FUNCTION ID error PR_END ';'  */
#line 148 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de funcion mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1836 "y.tab.c"
    break;

  case 24: /* decl_clase: PR_CLASS ID PR_BEGIN clase_item_lista PR_END ';'  */
#line 158 "gramatica.y"
      {
          std::cout << "Declaracion de clase (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1845 "y.tab.c"
    break;

  case 25: /* decl_clase: PR_CLASS ID error PR_END ';'  */
#line 163 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de clase mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1856 "y.tab.c"
    break;

  case 29: /* $@2: %empty  */
#line 179 "gramatica.y"
      {
          (yyvsp[0].ts_ref)->uso = Uso::METODO;
          (yyvsp[0].ts_ref)->tipo = (yyvsp[-1].tipo_val);
          (yyvsp[0].ts_ref)->tiene_ret = false;
          funciones_abiertas.push_back((yyvsp[0].ts_ref));
      }
#line 1867 "y.tab.c"
    break;

  case 30: /* clase_item: tipo ID $@2 '(' lista_params_formales ')' sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'  */
#line 187 "gramatica.y"
      {
          EntradaTS *metodo = funciones_abiertas.back();
          funciones_abiertas.pop_back();

          if (!metodo->tiene_ret) {
              std::cout << "Linea " << LINEA_ACTUAL 
                        << ": Aviso: el metodo '" << metodo->lexema 
                        << "' no tiene sentencia 'ret'. Se asume valor de retorno por defecto ("
                        << valor_por_defecto(metodo->tipo) << ")." << std::endl;
          }
      }
#line 1883 "y.tab.c"
    break;

  case 32: /* clase_item: tipo error ';'  */
#line 200 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de atributo o metodo mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1894 "y.tab.c"
    break;

  case 33: /* extends_clause: PR_EXTENDS lista_ids ';'  */
#line 210 "gramatica.y"
      {
          std::cout << "Clausula EXTENDS (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1903 "y.tab.c"
    break;

  case 34: /* extends_clause: PR_EXTENDS error ';'  */
#line 215 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: clausula 'extends' mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1914 "y.tab.c"
    break;

  case 35: /* decl_typedef: PR_TYPEDEF ID '=' '[' lista_valores ']' ';'  */
#line 225 "gramatica.y"
      {
          std::cout << "Declaracion de typedef (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1923 "y.tab.c"
    break;

  case 36: /* decl_typedef: PR_TYPEDEF error ';'  */
#line 230 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de typedef mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1934 "y.tab.c"
    break;

  case 40: /* sent_ejec_lista: sent_ejec_lista error ';'  */
#line 248 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia ejecutable mal formada. "
                    << "Se descartan tokens hasta ';'."
                    << std::endl;
          yyerrok;
      }
#line 1946 "y.tab.c"
    break;

  case 49: /* asignacion: ID OP_ASIGNACION expresion ';'  */
#line 270 "gramatica.y"
      {
          std::cout << "Asignacion (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1955 "y.tab.c"
    break;

  case 50: /* asignacion: ID error ';'  */
#line 275 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1966 "y.tab.c"
    break;

  case 51: /* asignacion_atributo: ID '.' ID '=' expresion ';'  */
#line 285 "gramatica.y"
      {
          std::cout << "Asignacion de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1975 "y.tab.c"
    break;

  case 52: /* asignacion_atributo: ID '[' indice ']' '=' expresion ';'  */
#line 290 "gramatica.y"
      {
          std::cout << "Asignacion posicional de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 1984 "y.tab.c"
    break;

  case 53: /* asignacion_atributo: ID '.' ID error ';'  */
#line 295 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion de atributo mal formada."
                    << std::endl;
          yyerrok;
      }
#line 1995 "y.tab.c"
    break;

  case 54: /* asignacion_atributo: ID '[' error ';'  */
#line 302 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion posicional mal formada."
                    << std::endl;
          yyerrok;
      }
#line 2006 "y.tab.c"
    break;

  case 57: /* retorno: PR_RET '(' expresion ')' ';'  */
#line 317 "gramatica.y"
      {
          if (!funciones_abiertas.empty()) {
              funciones_abiertas.back()->tiene_ret = true;
          }
          std::cout << "Sentencia RET (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 2018 "y.tab.c"
    break;

  case 58: /* retorno: PR_RET error ';'  */
#line 325 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'ret' mal formada."
                    << std::endl;
          yyerrok;
      }
#line 2029 "y.tab.c"
    break;

  case 59: /* pout: PR_POUT '(' CTE_STR ')' ';'  */
#line 335 "gramatica.y"
      {
          std::cout << "Sentencia POUT (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 2038 "y.tab.c"
    break;

  case 60: /* pout: PR_POUT '(' expresion ')' ';'  */
#line 340 "gramatica.y"
      {
          std::cout << "Sentencia POUT (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 2047 "y.tab.c"
    break;

  case 61: /* pout: PR_POUT error ';'  */
#line 345 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'pout' mal formada."
                    << std::endl;
          yyerrok;
      }
#line 2058 "y.tab.c"
    break;

  case 71: /* seleccion: PR_IF '(' condicion ')' bloque_sent_ejec PR_END_IF ';'  */
#line 373 "gramatica.y"
      {
          std::cout << "Sentencia IF (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 2067 "y.tab.c"
    break;

  case 72: /* seleccion: PR_IF '(' condicion ')' bloque_sent_ejec PR_ELSE bloque_sent_ejec PR_END_IF ';'  */
#line 379 "gramatica.y"
      {
          std::cout << "Sentencia IF-ELSE (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 2076 "y.tab.c"
    break;

  case 73: /* seleccion: PR_IF error ';'  */
#line 384 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'if' mal formada, incompleta, "
                    << "o le falta 'end_if'."
                    << std::endl;
          yyerrok;
      }
#line 2088 "y.tab.c"
    break;

  case 74: /* repeat_while: PR_REPEAT bloque_sent_ejec PR_WHILE '(' condicion ')' ';'  */
#line 395 "gramatica.y"
      {
          std::cout << "Sentencia REPEAT-WHILE (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
#line 2097 "y.tab.c"
    break;

  case 75: /* repeat_while: PR_REPEAT error ';'  */
#line 400 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'repeat-while' mal formada."
                    << std::endl;
          yyerrok;
      }
#line 2108 "y.tab.c"
    break;

  case 84: /* factor: '-' CTE_INT  */
#line 426 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error semantico: la constante '"
                    << (yyvsp[0].ts_ref)->lexema
                    << "' es de tipo USHORTINT (sin signo), "
                    << "no admite el signo '-'."
                    << std::endl;

          (yyval.ts_ref) = (yyvsp[0].ts_ref);
      }
#line 2123 "y.tab.c"
    break;

  case 85: /* factor: '-' CTE_FLOAT  */
#line 437 "gramatica.y"
        {
            std::string lexema_neg = "-" + (yyvsp[0].ts_ref)->lexema;
      
            auto it = tabla_simbolos.find(lexema_neg);
      
            if (it == tabla_simbolos.end()) {
                EntradaTS nueva_entrada;
                nueva_entrada.lexema = lexema_neg;
      
                it = tabla_simbolos.insert(
                    {lexema_neg, nueva_entrada}
                ).first;
            }
      
            (yyval.ts_ref) = &(it->second);
        }
#line 2144 "y.tab.c"
    break;

  case 97: /* factor_restr: '-' CTE_INT  */
#line 474 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error semantico: la constante '"
                    << (yyvsp[0].ts_ref)->lexema
                    << "' es de tipo USHORTINT (sin signo), "
                    << "no admite el signo '-'."
                    << std::endl;

          (yyval.ts_ref) = (yyvsp[0].ts_ref);
      }
#line 2159 "y.tab.c"
    break;

  case 98: /* factor_restr: '-' CTE_FLOAT  */
#line 485 "gramatica.y"
      {
          std::string lexema_neg = "-" + (yyvsp[0].ts_ref)->lexema;

          auto it = tabla_simbolos.find(lexema_neg);

          if (it == tabla_simbolos.end()) {
              EntradaTS nueva_entrada;
              nueva_entrada.lexema = lexema_neg;

              it = tabla_simbolos.insert(
                  {lexema_neg, nueva_entrada}
              ).first;
          }

          (yyval.ts_ref) = &(it->second);
      }
#line 2180 "y.tab.c"
    break;

  case 104: /* invocacion: ID '(' error ')' lista_ctes_opcional  */
#line 513 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: invocacion a funcion mal formada."
                    << std::endl;
          yyerrok;
      }
#line 2191 "y.tab.c"
    break;

  case 108: /* lista_ctes_opcional: '[' error ']'  */
#line 529 "gramatica.y"
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: lista de orden de evaluacion mal formada."
                    << std::endl;
          yyerrok;
      }
#line 2202 "y.tab.c"
    break;


#line 2206 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 553 "gramatica.y"


void yyerror(const char *s) {
	if (std::string(s).find("unexpected end of file") != std::string::npos)
		std::cerr << "Linea " << LINEA_ACTUAL
		          << ": Error: fin de archivo inesperado. Verificar que el programa termine con "
		          << "'end;' y que no haya sentencias o bloques sin cerrar"
		          << std::endl;
	else
    	std::cerr << "Linea " << LINEA_ACTUAL << ": Error: " << s << std::endl;
}
