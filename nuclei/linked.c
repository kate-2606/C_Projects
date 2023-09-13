#include "specific.h"
#define LIST 23

void error_message(char* message, int t);


lisp* lisp_atom(const atomtype a)
{
  lisp* l = (lisp*)ncalloc(1, sizeof(lisp));
  l->val=a;
  l->car=NULL;
  l->cdr=NULL;
  return l;
}


lisp* lisp_cons(const lisp* l1,  const lisp* l2)
{
  lisp* l = (lisp*)ncalloc(1, sizeof(lisp));
  l->car=(lisp*)l1;
  l->cdr=(lisp*)l2;
  return l;
}


lisp* lisp_car(const lisp* l)
{
  if (l==NULL){
    return NULL;
  }
  return l->car;
}


lisp* lisp_cdr(const lisp* l)
{
  if (l==NULL){
    return NULL;
  }
  return l->cdr;
}


atomtype lisp_getval(const lisp* l)
{
  if (lisp_isatomic(l)==false){
    #ifdef EXTEN
      error_message("Cannot find the value of a non-atomic node", LIST);
    #endif
    on_error("Cannot find the value of a non-atomic node.");
  }
  return l->val;
}


bool lisp_isatomic(const lisp* l)
{
  if (l!=NULL && l->car==NULL && l->cdr==NULL){
    return true;
  }
  return false;
}


lisp* lisp_copy(const lisp* l)
{
  if(l!=NULL){
    lisp* c = (lisp*)ncalloc(1, sizeof(lisp));
    c->car=lisp_copy(l->car);
    c->cdr=lisp_copy(l->cdr);
    c->val=l->val;
    return c;
  }

  return NULL;
}


int lisp_length(const lisp* l)
{
  if (l==NULL || lisp_isatomic(l)){
    return 0;
  }
  int cnt=0;
  while (l!=NULL){
    l=l->cdr;
    cnt++;
  }
  return cnt;
}


void lisp_tostring(const lisp* l, char* str)
{
  memset(str, EMPTY, sizeof(char)*LISTSTRLEN);
  tostring_helper(l, str, true);
}


void tostring_helper(const lisp* l, char* str, bool new)
{
  if(l!=NULL){
    bool new_level=new;

    tostr_car_helper(l, str, new_level);

    new_level=true;
    if(l->car!=NULL){
      tostring_helper(l->car, str, new_level);
    }
    new_level=false;

    tostr_cdr_helper(l, str);

    if(l->cdr!=NULL){
      tostring_helper(l->cdr, str, new_level);
    }
  }
  if(l==NULL){
    strcat(str, "()");
  }
}


void tostr_car_helper(const lisp* l, char* str, bool level)
{
  if(l->car==NULL && l->cdr==NULL){
    char str_int[MAX_DGTS];
    sprintf(str_int, "%d", l->val);
    str=strcat(str,str_int);
  }
  if(l->car!=NULL && level==true){
    str=strcat(str,"(");
  }
}


void tostr_cdr_helper(const lisp* l, char* str)
{
  if(l->car!=NULL && l->cdr!=NULL){
    str=strcat(str," ");
  }
  if(l->car!=NULL && l->cdr==NULL){
    str=strcat(str,")");
  }
}


void lisp_free(lisp** l)
{
  if (*l!=NULL){
    lisp_free(&((*l)->car));
    lisp_free(&((*l)->cdr));
    free(*l);
    *l=NULL;
  }
}


lisp* lisp_fromstring(const char* str)
{
  int x=0;
  if (string_checker(str)==true){
    return fromstring_helper(str, &x);
  }
  return NULL;
}


char next_char(const char* str, int* x)
{
  char c=str[*x];
  *x+=1;
  return c;
}


lisp* fromstring_helper(const char* str, int* x)
{
  lisp* top=NULL;
  char c=next_char(str, x);
  if (c=='('){
    top=fromstr_sub_helper(str, x, top);
  }
  else if(isdigit(c)!=0 || c=='-'){
    *x-=1;
    return lisp_atom(multi_digit(str,x));
  }
  else{
       #ifdef EXTEN
      error_message("First character of the string not bracket or number", LIST);
    #endif
    on_error("First character of the string not bracket or number.");
  }
  return top;
}


lisp* fromstr_sub_helper(const char* s, int* x, lisp* top)
{
  lisp* last=NULL;
  char c=next_char(s, x);
  while(c!=')'){
    if(c!=' '){
      *x-=1;
      lisp * l=lisp_cons(fromstring_helper(s, x), NULL);
      if (top==NULL){
        top=l;
      }
      if(last!=NULL){
        last->cdr=l;
      }
      last=l;
    }
    c=next_char(s,x);
  }
  return top;
}


int multi_digit(const char* str, int* x)
{
  int d = atoi(&str[*x]);
  if(str[*x]=='-' || str[*x]=='+'){
    (*x)+=1;
  }
  while (isdigit(str[*x])!=0){
    (*x)+=1;
  }
  return d;
}


lisp* lisp_list(const int n, ...)
{
  va_list ap;
  va_start(ap,n);
  lisp* last=NULL;
  lisp* top=NULL;
  for (int i=0; i<n; i++){
    lisp* a= va_arg(ap,lisp*);
    lisp* l=lisp_cons(a, NULL);
    if (top==NULL){
      top=l;
    }
    if(last!=NULL){
      last->cdr=l;
    }
    last=l;
  }
  return top;
}


void lisp_reduce(void (*func)(lisp* l, atomtype* n), lisp* l, atomtype* acc)
{
  if (lisp_isatomic(l)){
    func(l, acc);
  }
  if(l->car!=NULL){
    lisp_reduce((*func), l->car, acc);
  }
  if(l->cdr!=NULL){
    lisp_reduce((*func), l->cdr, acc);
  }
}


bool string_checker(const char* str)
{
  if(equal_brackets(str)==false){
       #ifdef EXTEN
      error_message("Number of open and closed brackets do not match", LIST);
    #endif
    on_error("Number of open and closed brackets in LIST do not match.");
  }
  if(empty_sublist(str)==true){
         #ifdef EXTEN
      error_message("Empty sub list", LIST);
    #endif
    on_error("LIST contains an empty sub list.");
  }
  if (unexpected_character(str)==true){
     #ifdef EXTEN
      error_message("Unexpected characters", LIST);
    #endif
    on_error("LIST contains unexpected characters.");
  }
  return true;
}


bool equal_brackets(const char* str)
{
  int close=0;  int open=0;
  int x=0;
  while (str[x]!=EMPTY){
    char c=str[x];
    if(c=='('){
      open++;
    }
    if(c==')'){
      close++;
    }
    x++;
  }
  if(open!=close){
    return false;
  }
  return true;
}


bool empty_sublist(const char* str)
{
  int x=0;
  char temp1=EMPTY;  char temp2=EMPTY;
  while (str[x]!=EMPTY){
    if(x>1){
      temp1=str[x-1];
      temp2=str[x];
      if(temp1=='(' && temp2==')'){
        return true;
      }
    }
    x++;
  }
  return false;
}


bool unexpected_character(const char* str)
{
  int x=0;
  while (str[x]!=EMPTY){
    char c=str[x];
    if(c=='(' || c==')' || c==' ' || c=='-' || (c>='0' && c<='9')){
    }
    else{
      return true;
    }
    x++;
  }
  return false;
}


void test(void)
{
  test_simple();
  test_tostring();
  test_fromstring();
  test_copy();
  test_strcheck();
  test_extensions();
}


void test_simple(void)
{
  lisp* t1=lisp_atom(4);
  assert(lisp_isatomic(t1)==true);
  assert(lisp_getval(t1)==4);
  assert(t1->val==4);
  assert(t1->car==NULL);
  assert(t1->cdr==NULL);

  lisp* t2=lisp_atom(6);
  assert(lisp_isatomic(t2)==true);
  assert(lisp_getval(t2)==6);
  assert(t2->val==6);

  lisp* t3=lisp_cons(t1,t2);
  assert(lisp_isatomic(t3)==false);
  assert(t3->car==t1);
  assert(t3->cdr==t2);

  assert(lisp_car(t3)==t1);
  assert(lisp_cdr(t3)==t2);
  assert(lisp_car(t1)==NULL);
  assert(lisp_car(t2)==NULL);
  assert(lisp_cdr(t1)==NULL);
  assert(lisp_cdr(t2)==NULL);

  assert(lisp_length(t3)==2);

  char arr[LISTSTRLEN];
  memset(arr, '\0', sizeof(char)*LISTSTRLEN);
  lisp_tostring(t3, arr);
  //printf("%s\n", arr);

  memset(arr, '\0', sizeof(char)*LISTSTRLEN);
  strcat(arr, "(((7) 8) 98 -1500)");
  int x=3;
  assert(multi_digit(arr, &x)==7);
  x=9;
  assert(multi_digit(arr, &x)==98);
  x=12;
  assert(multi_digit(arr, &x)==-1500);
  lisp_free(&(t3));
}


void test_strcheck(void)
{
   //string_checker("(())");
   //string_checker("(%%)");
   //string_checker("((())");
   assert(equal_brackets("((())))")==false);
   assert(equal_brackets("(8 (7))")==true);
   assert(empty_sublist("(())")==true);
   assert(empty_sublist("((0))")==false);
   assert(unexpected_character("(&)")==true);
   assert(unexpected_character("(-17000)")==false);
}


void test_copy(void)
{
  lisp* l1 = lisp_fromstring("((1) 4 5 (7 8 9) 3000)");
  lisp* l2=lisp_copy(l1);
  char arr1[LISTSTRLEN];
  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(l2, arr1);
  //printf("%s\n", arr1);
  assert(strcmp(arr1,"((1) 4 5 (7 8 9) 3000)")==0);

  char arr2[LISTSTRLEN];
  memset(arr2, EMPTY, sizeof(char)*LISTSTRLEN);
  l1->car->car->val=3000;
  lisp_tostring(l1, arr2);
  assert(strcmp(arr1, arr2)!=0);
  //printf("%s\n%s\n",arr2, arr1);

  lisp_free(&l1);
  assert(l1==NULL);
  lisp_free(&l2);
  assert(l2==NULL);

/*
STRING CONTAIN AN EMPTY SUB LIST
  lisp* l3 = lisp_fromstring("(() 1 2)");
  lisp* l4=lisp_copy(l3);
  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(l4, arr1);
  printf("%s\n", arr1);
  assert(strcmp(arr1,"(() 1 2)")==0);
*/

  lisp* l5 = lisp_fromstring("1");
  lisp* l6=lisp_copy(l5);
  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(l6, arr1);
  //printf("%s\n", arr1);
  assert(strcmp(arr1,"1")==0);
  lisp_free(&l5);
  lisp_free(&l6);
}


void test_tostring(void)
{
  lisp* t3=lisp_atom((atomtype) 3);
  lisp* t4=lisp_atom(4);
  lisp* t2=lisp_cons(t4,NULL);
  lisp* t1=lisp_cons(t3,t2);
  char arr[LISTSTRLEN];
  memset(arr, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(t1, arr);
  lisp_free(&t1);
  //printf("%s\n", arr);

  lisp* n4=lisp_atom(3);
  lisp* n6=lisp_atom(3);
  lisp* n8=lisp_atom(5);
  lisp* n10=lisp_atom(1);
  lisp* n12=lisp_atom(2);
  lisp* n15=lisp_atom(-9768);
  lisp* n17=lisp_atom(8);
  lisp* n19=lisp_atom(5);
  lisp* n7=lisp_cons(n8,NULL);
  lisp* n5=lisp_cons(n6, n7);
  lisp* n3=lisp_cons(n4, n5);
  lisp* n9=lisp_cons(n10, NULL);
  lisp* n2=lisp_cons(n3, n9);
  lisp* n16=lisp_cons(n17, NULL);
  lisp* n14=lisp_cons(n15, n16);
  lisp* n18=lisp_cons(n19, NULL);
  lisp* n13=lisp_cons(n14, n18);
  lisp* n11=lisp_cons(n12, n13);
  lisp* n1=lisp_cons(n2, n11);

  assert(n1->car==n2);

  memset(arr, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(n1, arr);
  //printf("%s\n", arr1);
  lisp_free(&n1);

  lisp* m2=lisp_cons(NULL,NULL);
  lisp* m4=lisp_atom(1);
  lisp* m6=lisp_atom(2);
  lisp* m5=lisp_cons(m6,NULL);
  lisp* m3=lisp_cons(m4,m5);
  lisp* m1=lisp_cons(m2,m3);
  memset(arr, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(m1, arr);
  lisp_free(&m1);
  assert(m1==NULL);

//----------------------------------big test-----------------------
/*
  X----------------X----X--------------------------------X
  |                |    |                                |
  |                0    X---------------------------X   78345
  X----------X          |                           |
  |          |          X-----------X---X---X---X   0
  |          2          |           |   |   |   |
  X---X---X             X-----X    -2  -4  -6  -8
  |   |   |             |     |
  7  58  -3             X    108
                        |
                        15
*/
  lisp* o4=lisp_atom(7);
  lisp* o6=lisp_atom(58);
  lisp* o8=lisp_atom(-3);
  lisp* o10=lisp_atom(2);
  lisp* o12=lisp_atom(0);
  lisp* o18=lisp_atom(-15);
  lisp* o20=lisp_atom(108);
  lisp* o22=lisp_atom(-2);
  lisp* o24=lisp_atom(-4);
  lisp* o26=lisp_atom(-6);
  lisp* o28=lisp_atom(-8);
  lisp* o30=lisp_atom(0);
  lisp* o32=lisp_atom(78543);
  lisp* o7=lisp_cons(o8,NULL);
  lisp* o5=lisp_cons(o6,o7);
  lisp* o3=lisp_cons(o4,o5);
  lisp* o9=lisp_cons(o10,NULL);
  lisp* o2=lisp_cons(o3,o9);
  lisp* o17=lisp_cons(o18,NULL);
  lisp* o19=lisp_cons(o20,NULL);
  lisp* o16=lisp_cons(o17,o19);
  lisp* o27=lisp_cons(o28,NULL);
  lisp* o25=lisp_cons(o26,o27);
  lisp* o23=lisp_cons(o24,o25);
  lisp* o21=lisp_cons(o22,o23);
  lisp* o29=lisp_cons(o30,NULL);
  lisp* o31=lisp_cons(o32,NULL);
  lisp* o15=lisp_cons(o16,o21);
  lisp* o14=lisp_cons(o15,o29);
  lisp* o13=lisp_cons(o14,o31);
  lisp* o11=lisp_cons(o12,o13);
  lisp* o1=lisp_cons(o2,o11);

  memset(arr, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(o1, arr);
  //printf("%s\n", arr);
  assert(strcmp(arr, "(((7 58 -3) 2) 0 ((((-15) 108) -2 -4 -6 -8) 0) 78543)")==0);
  lisp_free(&o1);

  memset(arr, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp* h=lisp_fromstring("(8 6 (1))");
  tostring_helper(h, arr, true);
  assert(strcmp(arr,"(8 6 (1))")==0);

  memset(arr, EMPTY, sizeof(char)*LISTSTRLEN);
  tostr_car_helper(h, arr, true);
  assert(strcmp(arr, "(")==0);
  tostr_car_helper(h->car, arr, true);
  assert(strcmp(arr, "(8")==0);
  tostr_cdr_helper(h->cdr, arr);
  assert(strcmp(arr, "(8 ")==0);
  tostr_car_helper(h->cdr->car, arr, false);
  assert(strcmp(arr, "(8 6")==0);
  lisp_free(&h);
}


void test_fromstring(void)
{
  int c=1; int d=5;
  assert(multi_digit("(1 2 -56)", &c)==1);
  assert(multi_digit("(1 2 -56)", &d)==-56);
  assert(c==2);
  assert(d==8);

  int a=0; int b=1;
  assert(next_char("(789 6 7)", &a)=='(');
  assert(next_char("(789 6 7)", &b)=='7');
  assert(a==1);
  assert(b==2);

  char arr1[LISTSTRLEN];
  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  strcat(arr1, "((2 1 3) 7)");
  lisp* l=lisp_fromstring(arr1);

  char arr2[LISTSTRLEN];
  memset(arr2, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(l, arr2);
  //printf("CURRENT TEST \n%s\n \n%s\n", arr1, arr2);
  assert(strcmp(arr1, arr2)==0);
  lisp_free(&l);

  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  strcat(arr1, "(((9 7 6) 7 8) 3)");
  lisp* n=lisp_fromstring(arr1);
  memset(arr2, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(n, arr2);
  assert(strcmp(arr1, arr2)==0);
  //printf("CURRENT TEST \n%s\n", arr2);
  lisp_free(&n);

  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  strcat(arr1, "-700");
  lisp* m=lisp_fromstring(arr1);
  memset(arr2, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(m, arr2);
  //printf("CURRENT TEST \n%s\n", arr2);
  assert(m->car==NULL);
  assert(strcmp(arr1, arr2)==0);
  lisp_free(&m);

  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  //strcat(arr1, "(0 (1 -2) 3 4 50)");
  strcat(arr1, "(123)");
  lisp* o=lisp_fromstring(arr1);
  memset(arr2, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(o, arr2);
  //printf("CURRENT TEST \n%s\n", arr2);
  assert(strcmp(arr1, arr2)==0);
  lisp_free(&o);

  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  strcat(arr1, "(0 1 2)");
  lisp* p=lisp_fromstring(arr1);
  memset(arr2, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp_tostring(p, arr2);

  char arrp[LISTSTRLEN];
  memset(arrp, EMPTY, sizeof(char)*LISTSTRLEN);
  char arrq[LISTSTRLEN];
  memset(arrq, EMPTY, sizeof(char)*LISTSTRLEN);
  int x=0;
  lisp* q=fromstring_helper(arr1,&x);
  lisp_tostring(p,arrp);
  lisp_tostring(q,arrq);
  assert(strcmp(arrp, arrq)==0);
  //printf("%s\n", arr2);
  //assert(strcmp(arr1, arr2)==0);
  lisp_free(&p);
  lisp_free(&q);

  int y=1;
  memset(arr1, EMPTY, sizeof(char)*LISTSTRLEN);
  strcat(arr1, "((8 9 ) 7)");
  lisp* r=fromstr_sub_helper(arr1, &y, NULL);
  assert(r->car!=NULL);
  assert(r->car->car!=NULL);
  assert(r->car->car->car==NULL);
  lisp_free(&r);
}


void test_extensions(void)
{
  char str[LISTSTRLEN];
  memset(str, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp* g1 = lisp_list(1, lisp_atom(0));
  lisp_tostring(g1, str);
  assert(strcmp(str, "(0)")==0);

  memset(str, EMPTY, sizeof(char)*LISTSTRLEN);
  lisp* g0=lisp_fromstring("(1 6 ((7) 8) -5000)");
  lisp* g2 = lisp_list(4, g0, lisp_atom(-123456), lisp_copy(g1), lisp_atom(25));
  lisp_tostring(g2, str);
  //printf("%s\n", str);
  assert(strcmp(str, "((1 6 ((7) 8) -5000) -123456 (0) 25)")==0);

  int acc=0;
  lisp_reduce(sub, g0, &acc);
  assert(acc==4978);

  acc=0;
  lisp_reduce(sub, g2, &acc);
  assert(acc==128409);
  lisp_free(&g2);
  lisp_free(&g1);
}

void sub(lisp* l, atomtype* accum)
{
   *accum = *accum - lisp_getval(l);
}
