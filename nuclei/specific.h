#pragma once
#include "lisp.h"

#include <ctype.h>
#include <stdlib.h>

#define NIL NULL
#define LISTSTRLEN 1024
#define MAX_DGTS 50
#define EMPTY '\0'
#define LISPIMPL "Linked"

struct lisp {
  lisp* car;
  lisp* cdr;
  atomtype val;
};

void test_tostring(void);
void test_simple(void);
void test_fromstring(void);
void test_copy(void);
void test_strcheck(void);
void test_extensions(void);
void tostring_helper(const lisp* l, char* str, bool new);
void tostr_car_helper(const lisp* l, char* str, bool level);
void tostr_cdr_helper(const lisp* l, char* str);
lisp* fromstring_helper(const char* str, int* x);
lisp* fromstr_sub_helper(const char* s, int* x, lisp* top);
int multi_digit(const char* str, int* x);
bool string_checker(const char* str);
bool equal_brackets(const char* str);
bool empty_sublist(const char* str);
bool unexpected_character(const char* str);
void sub(lisp* l, atomtype* accum);
