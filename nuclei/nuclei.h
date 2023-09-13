#pragma once
#include "general.h"
#include "lisp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#define MAX_CHAR 1024
#define MAX_TKNS 100
#define WRD 30
#define NEW_LN '\n'
#define SPACE ' '
#define EMPTY '\0'
#define TAB '\t'
#define NUMOTKNS START-CAR+1
#define FAIL -1
#define OPENBR_C '('
#define CLOSEBR_C ')'
#define DBL_QTE_C '"'
#define SNGL_QTE_C (char)39 //ACSII code for single quote
//#define DEBUG
#define VARS 'Z'-'A'+1
#define START 'A'
#define FIRST 1
#define SECOND 2
#define ZERO "0"
#define NO_TWO 2
#define TRUE "1"
#define FALSE "0"
#define HASH '#'
#define MAX_LOOP 10000
#define POINTER '^'
#define SET_RED "\033[0;31m"
#define SET_GRN "\033[0;32m"
#define CLEAR_CLR "\033[0m"
#define ERROR(PHRASE, FUNC) {fprintf (stderr, \
  "Fatal Error: %s in %s\n", PHRASE, FUNC); \
  exit(EXIT_FAILURE);}

typedef enum {
  RESET, NEXT, CURRENT, LOOP, LOOP_END, CODE_POS
}mode;

typedef enum {
  /* 0 */ CAR, CDR, CONS, PLUS,
  /*4*/ LENGTH, LESS, GREATER, EQUAL, SET,
  /*9*/ PRINT, IF, WHILE, NIL, VAR,
  /*14*/ STRING, LITERAL, OPENBR, CLOSEBR,
  /*18*/ DBL_QTE, SNGL_QTE, END, ERROR, START_
} token_type;


const char token_string[NUMOTKNS][WRD]={
  "CAR", "CDR", "CONS", "PLUS",
  "LENGTH", "LESS", "GREATER", "EQUAL", "SET",
  "PRINT", "IF", "WHILE", "NIL", "VAR",
  "STRING", "LITERAL","(", ")",
  "\"", "'", "END", "ERROR", "START", "LIST"
};



typedef struct node node;
struct node{
  token_type type;
  char value[MAX_CHAR];
  node* next;
};

typedef struct{
  char arr[VARS][MAX_CHAR];
  char result[MAX_CHAR];
}ic;


typedef struct heap_node heap_node;
struct heap_node{
  node* node;
  heap_node* prev;
};


typedef struct{
  char* file_name;
  int line;
  int offset;
  char* code_ln;
}err_dtls;



//lexicon and tokeniser functions
void neucli_test(void);
void test_next_token(void);
void test_parser(void);
void test_interpreter(void);
void test_lexicon(void);
void test_file_reset(char* str);
int get_line(char* code_ln, FILE* fpin);
node* next_token(mode mod);
node* token_reset(int* offset, int* ln_num, char cd_ln[MAX_CHAR], node** head);
node* next_token_helper(int* offset, char code_ln[MAX_CHAR], node* next);
node* next_token_mode(node* curr, node* head, mode mod);
node* offset_fail(int* offset, int* ln_num, char cd_ln[MAX_CHAR], node* next);
char* token_strings(int* x, token_type t);
token_type string_to_token(char* str);
const char* token_to_string(token_type t);
bool is_symbol(char c, char* wrd);
void litORstr_token(char* cd_ln, int* off, node* next);
void varORinstruct_token(char* cd_ln, int* off, node* next);
bool equal_brackets(const char* str);
bool unexpected_character(const char* str);
node * next_token_next(int* offset, int* ln_num, char code_ln[MAX_CHAR], node* current);

//parser functions
bool accept(token_type t);
bool expect(token_type t);
bool set_(ic* cntxt);
bool set_var(ic* cntxt, node* err);
bool loop_(ic* cntxt);
bool iofunc_(ic* cntxt);
bool list_(ic* cntxt);
bool retfunc_(ic* cntxt);
bool boolfunc_(ic* cntxt);
bool intfunc_(ic* cntxt);
bool intfunc_plus(ic* cntxt, node* err);
bool intfunc_length(ic* cntxt, node* err);
bool listfunc_(ic* cntxt);
bool listfunc_cons(ic* cntxt, node* n);
bool func_(ic* cntxt);
bool if_(ic* cntxt);
bool instruct_(ic* cntxt);
bool instructs_(ic* cntxt);
bool prog_(ic* cntxt);
void run_prog(FILE* fpin);
bool print_(ic* cntxt);
bool var_(void);
bool string_(ic* cntxt);
bool literal_(ic* cntxt);
void list_free(node** head);

//interp functions
char var_interp(void);
void list_interp(ic* cntxt);
void set_interp(ic* cntxt, char v, node* n);
void print_interp(ic* cntxt, node* n);
void literalORstring_interp(ic* cntxt);
void boolfunc_interp(ic* cntxt, node* s, node* a, node* b);
bool if_instructs(ic* cntxt);
bool if_result(ic* cntxt, char res[MAX_CHAR], bool ret1, bool ret2);
void intfunc_interp_plus(ic* cntxt, node* a, int* num);
//void intfunc_interp_plus(ic* cntxt, node* a, node* b, node *n);
void car_cdr_interp(ic* cntxt, node* n, node* a);
void cons_interp(ic* cntxt, node* n, node* a, node* b);
void set_result(ic* cntxt, char* str);
void list_sum_val(lisp* l, atomtype* accum);
int list_to_val(node* n);
void intfunc_length_interp(ic* cntxt, node* a);
void intfunc_length_var_interp(ic* cntxt, node* a);
char* list_to_string(ic* cntxt, node* a);
int intboolfunc_find_val(ic* cntxt, node* a);
bool loop_bool(ic* cntxt);
bool loop_instructs(ic* cntxt, char bool_res);

//other functions
node* loop_heap(node* n, bool pop);
void find_atomORvar_value(ic* cntxt, node* a, int* x);
void skip_brackets(void);
bool testing_mode(int* test);
void decide_if_error(char* str, node* n, bool ret);

//extension
void error_message(char* message, token_type t);
void line_and_offset(err_dtls* dtls, int line, int offset, bool retrive);
void file_name(err_dtls* dtls, char* f_name, bool retrive);
void get_code_ln(err_dtls* dtls, char* code_ln, bool retrive);
