%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {DWISLPY}
%define api.parser.class {Parser}
%define parse.error verbose
    
%code requires{
    
    #include "dwislpy-ast.hh"
    #include "dwislpy-check.hh"

    namespace DWISLPY {
        class Driver;
        class Lexer;
    }

    #define YY_NULLPTR nullptr

}

%parse-param { Lexer  &lexer }
%parse-param { Driver &main  }
    
%code{

    #include <sstream>
    #include "dwislpy-util.hh"    
    #include "dwislpy-main.hh"

    #undef yylex
    #define yylex lexer.yylex
    
}


%define api.value.type variant
%define parse.assert
%define api.token.prefix {Token_}

%locations

%token               EOFL  0  
%token               EOLN
%token               INDT
%token               DEDT
%token               COMA ","
%token               DEFN "def"
%token               PASS "pass"
%token               WHLE "while"
%token               RETN "return"
%token               IFTN "if"
%token               ELSE "else"
%token               PRNT "print"
%token               INPT "input"
%token               ASGN "="
%token               PLEQ "+="
%token               MIEQ "-="
%token               TIEQ "*="
%token               PLUS "+"
%token               MNUS "-"
%token               TMES "*"
%token               IDIV "//"
%token               IMOD "%"
%token               LPAR "(" 
%token               RPAR ")"
%token               LESS "<"
%token               LTEQ "<="
%token               CONJ "and"
%token               DISJ "or"
%token               EQAL "=="
%token               NEGT "not"
%token               NONE "None"
%token               TRUE "True"
%token               FALS "False"
%token               ARRW "->"    // for types!
%token               BOOL "bool"
%token               INTC "int"
%token               STRC "str"
%token               COLN ":"
%token <int>         NMBR
%token <std::string> NAME
%token <std::string> STRG

%type <Prgm_ptr> prgm
%type <Expn_vec> prms
%type <Blck_ptr> nest
%type <Type>     type
%type <Defs>     defs
%type <Defn_ptr> defn
%type <Blck_ptr> blck
%type <Stmt_vec> stms
%type <Stmt_ptr> stmt
%type <Expn_ptr> expn
%type <SymT>     fmls

%%

%start main;

%left PLUS MNUS;
%left TMES IMOD IDIV;
    
main:
  prgm {
      main.set($1);
  }
;

prgm:
  defs blck {
      Defs ds = $1;
      Blck_ptr b = $2; 
      $$ = Prgm_ptr { new Prgm {ds, b, b->where()} };
  }
| blck {
      Defs ds { };
      Blck_ptr b = $1; 
      $$ = Prgm_ptr { new Prgm {ds, b, b->where()} };
  }   
;

defs:
  defs defn {
      Defs ds = $1;
      Defn_ptr d = $2;
      ds[d->name] = d;
      $$ = ds;
  }
| defn {
      Defs ds { };
      Defn_ptr d = $1;
      ds[d->name] = d;
      $$ = ds;
  }
;

type:
  INTC {
    $$ = IntTy {};
  }
| STRC {
    $$ = StrTy {};
  }
| BOOL {
    $$ = BoolTy {};
  }
| NONE {
    $$ = NoneTy {};
  }
;

defn:
  DEFN NAME LPAR fmls RPAR ARRW type COLN EOLN nest {
      $$ = Defn_ptr { new Defn {$2, $4, $7, $10, lexer.locate(@1)} };
  }
| DEFN NAME LPAR RPAR ARRW type COLN EOLN nest {
      SymT frmls = { };
      $$ = Defn_ptr { new Defn {$2, frmls, $6, $9, lexer.locate(@1)} };
  }
;

nest:
  INDT blck DEDT {
      $$ = $2;
  }
| INDT blck EOFL {
      $$ = $2;
  }
;

blck:
  stms {
      Stmt_vec ss = $1;
      $$ = Blck_ptr { new Blck {ss, ss[0]->where()} };
  }
;

prms: 
  prms COMA expn  {
      Expn_vec ps = $1;
      ps.push_back($3);
      $$ = ps;
  }
| expn {
      Expn_vec ps { };
      ps.push_back($1);
      $$ = ps;
  }
;

fmls:
  NAME COLN type {
    SymT st { };
    st.add_frml($1,$3);
    $$ = st;
  }
| fmls COMA NAME COLN type {
    SymT st = $1;
    st.add_frml($3,$5);
    $$ = st;
  }
;

stms:
  stms stmt {
      Stmt_vec ss = $1;
      ss.push_back($2);
      $$ = ss;
  }
| stmt {
      Stmt_vec ss { };
      ss.push_back($1);
      $$ = ss;
  }
;

stmt: 
  NAME COLN type ASGN expn EOLN {
      $$ = Ntro_ptr { new Ntro {$1,$3,$5,lexer.locate(@2)} };
  }
| NAME ASGN expn EOLN {
      $$ = Asgn_ptr { new Asgn {$1,$3,lexer.locate(@2)} };
  }
| NAME PLEQ expn EOLN {
      $$ = PlEq_ptr { new PlEq {$1,$3,lexer.locate(@2)} };
  }
| NAME MIEQ expn EOLN {
      $$ = MiEq_ptr { new MiEq {$1,$3,lexer.locate(@2)} };
  }
| NAME TIEQ expn EOLN {
      $$ = TiEq_ptr { new TiEq {$1,$3,lexer.locate(@2)} };
  }
| NAME LPAR prms RPAR EOLN {
      $$ = Proc_ptr { new Proc {$1,$3,lexer.locate(@2)} };
  }
| NAME LPAR RPAR EOLN {
      Expn_vec a = { };
      $$ = Proc_ptr { new Proc {$1,a,lexer.locate(@2)} };
  }
| PASS EOLN {
      $$ = Pass_ptr { new Pass {lexer.locate(@1)} };
  }
| PRNT LPAR prms RPAR EOLN {
      $$ = Prnt_ptr { new Prnt {$3,lexer.locate(@1)} };
  }
| PRNT LPAR RPAR EOLN {
      Expn_vec p = { };
      $$ = Prnt_ptr { new Prnt {p,lexer.locate(@1)} };
  }
| WHLE expn COLN EOLN nest {
      $$ = Whle_ptr { new Whle {$2,$5,lexer.locate(@2)} };
  }
| IFTN expn COLN EOLN nest ELSE COLN EOLN nest {
      $$ = Tern_ptr { new Tern {$2,$5,$9,lexer.locate(@2)} };
  }
| RETN expn EOLN {
      $$ = RetE_ptr { new RetE {$2,lexer.locate(@2)} };
  }
| RETN EOLN {
      $$ = Retn_ptr { new Retn {lexer.locate(@2)} };
  }
;

expn:
  NAME LPAR prms RPAR {
      $$ = Func_ptr { new Func {$1,$3,lexer.locate(@2)} };
  }
| NAME LPAR RPAR {
      Expn_vec a = { };
      $$ = Func_ptr { new Func {$1,a,lexer.locate(@2)} };
  }
|  expn PLUS expn {
      $$ = Plus_ptr { new Plus {$1,$3,lexer.locate(@2)} };
  }
| expn MNUS expn {
      $$ = Mnus_ptr { new Mnus {$1,$3,lexer.locate(@2)} };
  }
| expn TMES expn {
      $$ = Tmes_ptr { new Tmes {$1,$3,lexer.locate(@2)} };
  }
| expn IDIV expn {
      $$ = IDiv_ptr { new IDiv {$1,$3,lexer.locate(@2)} };
  }
| expn IMOD expn {
      $$ = IMod_ptr { new IMod {$1,$3,lexer.locate(@2)} };
  }
| expn CONJ expn {
      $$ = Conj_ptr { new Conj {$1,$3,lexer.locate(@2)} };
  }
| expn DISJ expn {
      $$ = Disj_ptr { new Disj {$1,$3,lexer.locate(@2)} };
  }
| expn LESS expn {
      $$ = Less_ptr { new Less {$1,$3,lexer.locate(@2)} };
  }
| expn LTEQ expn {
      $$ = LtEq_ptr { new LtEq {$1,$3,lexer.locate(@2)} };
  }
| expn EQAL expn {
      $$ = Eqal_ptr { new Eqal {$1,$3,lexer.locate(@2)} };
  }
| NEGT expn {
      $$ = Negt_ptr { new Negt {$2,lexer.locate(@1)} };
  }
| NMBR {
      $$ = Ltrl_ptr { new Ltrl {Valu {$1},lexer.locate(@1)} };
  }
| STRG {
      $$ = Ltrl_ptr { new Ltrl {Valu {de_escape($1)},lexer.locate(@1)} };
  }
| TRUE {
      $$ = Ltrl_ptr { new Ltrl {Valu {true},lexer.locate(@1)} };
  }
| FALS {
      $$ = Ltrl_ptr { new Ltrl {Valu {false},lexer.locate(@1)} };
  }
| NONE {
      $$ = Ltrl_ptr { new Ltrl {Valu {None},lexer.locate(@1)} };
  }    
| INPT LPAR expn RPAR {
      $$ = Inpt_ptr { new Inpt {$3,lexer.locate(@1)} };
  }
| INTC LPAR expn RPAR {
      $$ = IntC_ptr { new IntC {$3,lexer.locate(@1)} };
  }
| STRC LPAR expn RPAR {
      $$ = StrC_ptr { new StrC {$3,lexer.locate(@1)} };
  }
| NAME {
      $$ = Lkup_ptr { new Lkup {$1,lexer.locate(@1)} };
  }
| LPAR expn RPAR {
      $$ = $2;
  }
;

%%
       
void DWISLPY::Parser::error(const location_type &loc, const std::string &msg) {
    throw DwislpyError { lexer.locate(loc), msg };
}
