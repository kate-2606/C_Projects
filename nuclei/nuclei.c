
#include "nuclei.h"


int main(int argc, char* argv[])
{
  neucli_test();

  if (argc==1){
    printf("No code file has been declared.\n");
    exit(EXIT_FAILURE);
  }
  if (argc==NO_TWO){
    FILE* fpin = fopen(argv[1], "r");
    if(!fpin){
      printf("Could not read file!\n");
      exit(EXIT_FAILURE);
    }
    #ifdef EXTEN
      file_name(NULL, argv[1], false);
    #endif
    run_prog(fpin);
  }
}


void run_prog(FILE* fpin)
{
  ic* cntxt=NULL;
  #ifdef INTERP
    ic c;
    cntxt=&c;
  #endif

  char code_ln[MAX_CHAR]={EMPTY};
  memset(code_ln, EMPTY, sizeof(char)*MAX_CHAR);
  get_line(code_ln, fpin);
  next_token(RESET);
  next_token(NEXT);
  if(prog_(cntxt) && cntxt==NULL){
    printf("Parsed OK\n");
  }
}


bool testing_mode(int* test)
{
  static int x=0;

  if(test!=NULL){
    x=*test;
  }
  if(test==NULL){
    if (x==0){
      return false;
    }
    else{
      return true;
    }
  }
  return true;
}


int get_line(char* code_ln, FILE* fpin)
{
  static int ln_num=0;
  static FILE* file=NULL;

  if (fpin!=NULL){
    file=fpin;
    return 0;
  }

  char str[MAX_CHAR];
  if(fgets(str, MAX_CHAR, file)==NULL){
    return FAIL;
  }
  strcpy(code_ln, str);
  ln_num++;
  return ln_num;
}


node* next_token(mode mod)
{
  static int offset=FAIL;  static char code_ln[MAX_CHAR]={EMPTY};
  static node* head=NULL;  static node* current=NULL;  static int ln_num=0;
  if(mod==RESET){
    return token_reset(&offset, &ln_num, code_ln, &head);
  }
  if(head==NULL){
    current  = (node*) calloc(1, sizeof(node));
    head=current;
  }
  if(mod==CURRENT){
    return current;
  }
  if(mod==LOOP){
    current=loop_heap(NULL, false);
    return current;
  }
  #ifdef EXTEN
    if(mod==CODE_POS){
      line_and_offset(NULL, ln_num, offset, false);
      get_code_ln(NULL, code_ln, false);
    }
  #endif
  if(mod==NEXT){
    if (current->next!=NULL){
      current=current->next;
      return current;
    }
    else{
      node* next  = (node*) calloc(1, sizeof(node));
      current->next=next;

      if(offset==FAIL){
        next=offset_fail(&offset, &ln_num, code_ln, next);
        if(next->type==END){
          return next;
        }
      }
      next=next_token_helper(&offset, code_ln, next);
      current=next;
    }
  }
  return current;
}


node* token_reset(int* offset, int* ln_num, char cd_ln[MAX_CHAR], node** head)
{
  *offset=FAIL;  *ln_num=0;
  memset(cd_ln, EMPTY, sizeof(char)*MAX_CHAR);
  list_free(head);
  *head=NULL;
  return NULL;
}


node* offset_fail(int* offset, int* ln_num, char cd_ln[MAX_CHAR], node* next)
{
  while(*offset==FAIL){
    if(get_line(cd_ln, NULL)==FAIL){
      next->type=END;
      return next;
    }
    (*offset)=0;  *ln_num+=1;
    if(cd_ln[*offset]==HASH){
      *offset=FAIL;
    }
  }
  return next;
}


node* next_token_helper(int* offset, char code_ln[MAX_CHAR], node* next)
{
  char sym=code_ln[*offset];
  while(sym==SPACE || sym==TAB || sym==NEW_LN ){
    if(sym==NEW_LN){
      if(get_line(code_ln, NULL)==FAIL){
        next->type=END;
        return next;
      }
      (*offset)=0;
    }
    else{
      (*offset)++;
    }
    sym=code_ln[*offset];
  }
  if (sym==SNGL_QTE_C || sym==DBL_QTE_C){
    litORstr_token(code_ln, offset, next);
  }
  else{
    varORinstruct_token(code_ln, offset, next);
  }
  return next;
}


bool is_symbol(char c, char* wrd)
{
  switch(c){
    case OPENBR_C:
    case CLOSEBR_C:
    case DBL_QTE_C:
    case SNGL_QTE_C:
    break;
    default:
    return false;
  }
  wrd[strlen(wrd)]=c;
  return true;
}


void litORstr_token(char* cd_ln, int* off, node* next)
{
  char sym=cd_ln[*off];  bool is_str = false;
  is_str=sym==DBL_QTE_C? true:false;
  char wrd[MAX_CHAR];
  memset(wrd, EMPTY, sizeof(char)*MAX_CHAR);

  *off+=1;
  sym=cd_ln[*off];
  char end=is_str==true ? DBL_QTE_C : SNGL_QTE_C;

  next->type=is_str==true? STRING : LITERAL;
  do{
    wrd[strlen(wrd)]=sym;
    *off+=1;
    sym=cd_ln[*off];
  }while(sym!=end && *off!=(int) strlen(cd_ln)-1);
    next->type = sym ==end? next->type : ERROR;
    *off+=1;
    if (strcmp(wrd, "NIL")==0){
      next->type=NIL;
    }
    else{
      strcpy(next->value,wrd);
    }
}


void varORinstruct_token(char* cd_ln, int* off, node* next)
{
  char sym=cd_ln[*off];
  char wrd[WRD];
  memset(wrd, EMPTY, sizeof(char)*WRD);
  memset(next->value, EMPTY, sizeof(char)*EMPTY);

  if(is_symbol(sym, wrd)==true){
    *off+=1;
    next->type=string_to_token(wrd);
  }
  else{
    while (isalpha(sym)!=0 ){
      wrd[strlen(wrd)]=sym;
      *off+=1;
      sym=cd_ln[*off];
    }
    if(strlen(wrd)!=1){
      next->type=string_to_token(wrd);
    }
    else{
      strcpy(next->value,wrd);
      next->type=VAR;
    }
  }
  return ;
}


token_type string_to_token(char* str)
{
  for (int i=0; i<NUMOTKNS; i++){
    if (strcmp(token_string[i], str)==0){
      return i;
    }
  }
  return ERROR;
}


const char* token_to_string(token_type t)
{
    return token_string[t];
}


bool accept(token_type t)
{
  node* n=next_token(CURRENT);
  if(n->type==t){
    n=next_token(NEXT);
    return true;
  }
  return false;
}


bool expect(token_type t)
{
  if(accept(t)){
    return true;
  }
  return false;
}


bool set_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  node* err=n;
  #ifdef DEBUG
    printf("in set, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;

  if (expect(SET)){

    n=next_token(CURRENT);
    if(n->type==VAR){
      return set_var(cntxt, err);
    }
    decide_if_error("expected VAR as first argument", err, ret);
  }
  return ret;
}


bool set_var(ic* cntxt, node* err)
{
  bool ret=false;  char v;

  if(cntxt!=NULL){
    v=var_interp();
  }
  node* n=next_token(NEXT);

  if(list_(cntxt)){
    if(cntxt!=NULL){
      set_interp(cntxt, v, n);
    }
    return true;
  }
  decide_if_error("expected LIST as second argument", err, ret);
  return false;
}


void set_interp(ic* cntxt, char v, node* n)
{
  if(n->type==LITERAL){
    strcpy(cntxt->arr[v-START], cntxt->result);
  }
  else if (n->type==VAR){
    char l =n->value[0];
    strcpy(cntxt->arr[v-START], cntxt->arr[l-START]);
  }
  else if (n->type==NIL){
    strcpy(cntxt->arr[v-START], ZERO);
  }
  else{
    strcpy(cntxt->arr[v-START], cntxt->result);
  }
  #ifdef INTERP
    lisp* l =lisp_fromstring(cntxt->result);
    lisp_free(&l);
  #endif
  return ;
}


bool list_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  #ifdef DEBUG
    printf("in list, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;
  if (var_()){
    ret=true;
  }
  else if (literal_(cntxt)){
    ret=true;
  }
  else if (n->type==NIL){
    n=next_token(NEXT);
    ret=true;
  }
  else if (expect(OPENBR)){
    retfunc_(cntxt);
    if (expect(CLOSEBR)){
      ret=true;
    }
  }
  n=next_token(CURRENT);
  if(cntxt!=NULL){
  }
  return ret;
}


bool retfunc_(ic* cntxt)
{
  #ifdef DEBUG
    node* n=next_token(CURRENT);
    printf("in retfunc, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;
  if(listfunc_(cntxt) || intfunc_(cntxt) || boolfunc_(cntxt)){
    ret=true;
  }
  return ret;
}


bool boolfunc_(ic* cntxt)
{
  node* err=next_token(CURRENT);
  #ifdef DEBUG
    printf("in boolfunc, token=%s\n", token_to_string(err->type));
  #endif
  bool ret=false;
  if (err->type==LESS || err->type==GREATER || err->type==EQUAL){
    node* a=next_token(NEXT);
    if(list_(cntxt)){
      if(a->type==OPENBR && cntxt!=NULL){
        strcpy(a->value,cntxt->result);
      }
      node* b=next_token(CURRENT);
      if(list_(cntxt)){
        if(cntxt!=NULL){
          if(b->type==OPENBR && cntxt!=NULL){
            strcpy(b->value,cntxt->result);
          }
          boolfunc_interp(cntxt, err, a, b);
        }
        ret=true;
      }
    }
    decide_if_error("expected two LIST arguments", err, ret);
  }
  return ret;
}


void boolfunc_interp(ic* cntxt, node* s, node* a, node* b)
{
  char* res=FALSE;

  int a_num=intboolfunc_find_val(cntxt, a);
  int b_num=intboolfunc_find_val(cntxt, b);

 if(s->type==LESS && a_num<b_num){
    res=TRUE;
  }
  else if(s->type==GREATER && a_num>b_num){
    res=TRUE;
  }
  else if(s->type==EQUAL && a_num==b_num){
    res=TRUE;
  }
  strcpy(cntxt->result, res);
}


void list_sum_val(lisp* l, atomtype* accum)
{
  #ifdef INTERP
   *accum = *accum + lisp_getval(l);
   return ;
  #endif
  *accum = *accum + 0;
  l=NULL;
  return ;
}


int list_to_val(node* n)
{
  #ifdef INTERP
    int acc=0;
    lisp* l=lisp_fromstring(n->value);
    lisp_reduce(list_sum_val, l, &acc);
    lisp_free(&l);

    return acc;
  #endif
  n=NULL;
  return 0;
}


bool intfunc_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  node* err=n;
  #ifdef DEBUG
    printf("in intfunc, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;
  if (expect(PLUS)){
    ret=intfunc_plus(cntxt, err);
  }
  if (expect(LENGTH)){
    ret=intfunc_length(cntxt, err);
  }
  return ret;
}


bool intfunc_plus(ic* cntxt, node* err)
{
  bool ret=false;
  int a_num=0;  int b_num=0;
  node* a=next_token(CURRENT);
  if (list_(cntxt)){
    intfunc_interp_plus(cntxt, a, &a_num);
    node* b=next_token(CURRENT);
    if(list_(cntxt)){
        intfunc_interp_plus(cntxt, b, &b_num);
      if(cntxt!=NULL){
        int res=a_num+b_num;
        sprintf(cntxt->result, "%d", res);
      }
      ret=true;
    }
  }
  decide_if_error("expected two LIST arguments", err, ret);
  return ret;
}


char* list_to_string(ic* cntxt, node* a)
{
  if(a->type==VAR){
    return cntxt->arr[a->value[0]-START];
  }
  else if(a->type==LITERAL){
    return a->value;
  }
  else if(a->type==NIL){
    return NULL;
  }
  else{
    return cntxt->result;
  }
  return NULL;
}


bool intfunc_length(ic* cntxt, node* err)
{
  bool ret=false;
  node* a=next_token(CURRENT);
  if(list_(cntxt)){
    if(cntxt!=NULL){
      intfunc_length_interp(cntxt, a);
    }
    ret=true;
  }
  decide_if_error("expected LIST argument", err, ret);
  return ret;

}


void intfunc_length_interp(ic* cntxt, node* a)
{
  #ifdef INTERP
  if(cntxt!=NULL){
    if (a!=NULL){
      lisp* l=NULL;
      if(a->type==NIL){
        sprintf(cntxt->result, "%d", 0);
        return ;
      }
      char* s =NULL;
      s=list_to_string(cntxt, a);
      if((s!=NULL && s[0]==EMPTY) || s==NULL){
        sprintf(cntxt->result, "%d", 0);
        return ;
      }
      l=lisp_fromstring(s);
      int x = lisp_length(l);
      sprintf(cntxt->result, "%d", x);
      lisp_free(&l);
      return ;
    }
  }
  #endif
  cntxt=NULL;  a=NULL;
}


void find_atomORvar_value(ic* cntxt, node* a, int* x)
{
  int err=0;
  if(a!=NULL &&cntxt!=NULL){
    if(a->type==VAR){
      err=sscanf(cntxt->arr[a->value[0]-START], "%d", x);
    }
    else{
      err=sscanf(a->value, "%d", x);
    }
  }
  else{
    *x=0;
  }
  return ;
}


void intfunc_interp_plus(ic* cntxt, node* a, int* num)
{
  #ifdef DEBUG
    printf("in intfunc_interp, token=%s\n", token_to_string(n->type));
  #endif
    if(cntxt!=NULL){
      char* s=NULL;
      s=list_to_string(cntxt, a);
      if((s!=NULL && s[0]==EMPTY) || s==NULL){
        *num=0;
      }
      if(s!=NULL && s[0]==OPENBR){
        if(s[1]==CLOSEBR){
          *num=0;
         }
         else{
          *num=list_to_val(a);
         }
         return ;
      }
      else{
        sscanf(s, "%d", num);
      }
      return ;
    }
}


int intboolfunc_find_val(ic* cntxt, node* a)
{
  int num;
  if(a!=NULL && a->value[0]==OPENBR_C){
    num=list_to_val(a);
  }
  else{
    find_atomORvar_value(cntxt, a, &num);
  }
  return num;
}


bool listfunc_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  node* err=n;
  #ifdef DEBUG
    printf("in listfunc, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;
  if (expect(CAR) || expect(CDR)){
    node* a=next_token(CURRENT);
    if(list_(cntxt)){
      ret=true;
      if(cntxt!=NULL){
        car_cdr_interp(cntxt, n, a);
      }
    }
    decide_if_error("expected LIST argument", err, ret);
  }
  if (expect(CONS)){
    ret=listfunc_cons(cntxt, n);
    decide_if_error("expected two LIST arguments", err, ret);
  }
  return ret;
}


bool listfunc_cons(ic* cntxt, node* n)
{
  bool ret=false;
  node* a=next_token(CURRENT);
  if(list_(cntxt)){
    node* b=next_token(CURRENT);
    if(list_(cntxt)){
      ret=true;
      if(cntxt!=NULL){
        cons_interp(cntxt, n, a, b);
      }
    }
  }
  return ret;
}


void car_cdr_interp(ic* cntxt, node* n, node* a)
{
  #ifdef INTERP
  char* s=NULL;  lisp* l1=NULL;  lisp* l2=NULL;
    s=list_to_string(cntxt, a);
    if (s!=NULL && s[0]!=EMPTY){
      l1=lisp_fromstring(s);
      if(n->type==CAR){
        l2=lisp_car(l1);
      }
      else if(n->type==CDR){
        l2=lisp_cdr(l1);
      }
      lisp_tostring(l2, cntxt->result);
      lisp_free(&l1);

    }
    else{
      memset(cntxt->result, EMPTY, sizeof(char)*MAX_CHAR);
      decide_if_error("expected Non-NIL LIST", n, false);
    }
    return ;
  #endif
  n=NULL;  a=NULL;  cntxt=NULL;
  return ;
}


void cons_interp(ic* cntxt, node* n, node* a, node* b)
{
  #ifdef INTERP
  lisp* l1=NULL;  lisp* l2=NULL;  lisp* l3=NULL;
  char* s=list_to_string(cntxt, a);
  if(s!=NULL){
    l1=lisp_fromstring(s);
  }
  s=list_to_string(cntxt, b);
  if(s!=NULL){
    l2=lisp_fromstring(s);
  }

  if(n->type==CONS){
    l3=lisp_cons(l1, l2);
  }
  lisp_tostring(l3, cntxt->result);
  lisp_free(&l3);
  return ;
  #endif
  n=NULL;  a=NULL;  b=NULL;  cntxt=NULL;
  return ;
}


bool loop_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  #ifdef DEBUG
    printf("in loop, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;
  if (n->type==WHILE){
    if(cntxt!=NULL){
      loop_heap(n, false);
      next_token(LOOP);
    }
    next_token(NEXT);
    if(loop_bool(cntxt)){
      ret=true;
    }
  }
  return ret;
}


bool loop_bool(ic* cntxt)
{
  bool ret=false;  int bool_res=0;
  if(expect(OPENBR)){
    if(boolfunc_(cntxt)){
      if(expect(CLOSEBR)){
        if(cntxt!=NULL){
        bool_res=cntxt->result[0];
        }
        ret=loop_instructs(cntxt, bool_res);
      }
    }
  }
  return ret;
}


bool loop_instructs(ic* cntxt, char bool_res)
{
  static int cnt=0;
  bool ret=false;  node* n=NULL;
  if(expect(OPENBR)){
    if(cntxt!=NULL && bool_res==FALSE[0]){
      loop_heap(NULL, true);
      skip_brackets();
      ret= true;
    }
    else{
      if(instructs_(cntxt)){
        ret=true;
      }
    }
  }
  if (cntxt!=NULL && bool_res==TRUE[0]){
    cnt++;
    next_token(LOOP);
    loop_heap(NULL, true);
    n=next_token(CURRENT);
    if(cnt==MAX_LOOP){
      decide_if_error("Infinate loop detected", n, false);
    }
    n=next_token(CURRENT);
    loop_(cntxt);
    ret=true;
  }
  return ret;
}


node* loop_heap(node* n, bool pop)
{
  static heap_node* head=NULL;  node* ret=NULL;

  if (n!=NULL && pop==false){
    heap_node* new_node = (heap_node*) calloc(1, sizeof(heap_node));
    new_node->node=n;
    new_node->prev=head;
    head = new_node;
  }
  else if (head!=NULL) {
    ret=head->node;
    if (pop) {
      heap_node* temp=head;
      head=head->prev;
      free(temp);
      }
  }
  else if(head==NULL && pop==true){
    ret=NULL;
  }
  return ret;
}


bool iofunc_(ic* cntxt)
{
  #ifdef DEBUG
    node* n=next_token(CURRENT);
    printf("in iofunc, token=%s\n", token_to_string(n->type));
  #endif

  return set_(cntxt) || print_(cntxt);
}


bool instruct_(ic* cntxt)
{
  #ifdef DEBUG
  node* n=next_token(CURRENT);
    printf("in instruct, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;

  if (expect(OPENBR)){
    if(func_(cntxt)){
        ret=expect(CLOSEBR);
    }
  }
  return ret;
}


bool instructs_(ic* cntxt)
{
  //<INSTRCTS> ::= <INSTRCT> <INSTRCTS> | ")"
  node* n=next_token(CURRENT);
  #ifdef DEBUG
    printf("in instructs, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;
  if (instruct_(cntxt)){
    if (instructs_(cntxt)){
      return true;
    }
  }
  else {
    ret=accept(CLOSEBR);
    return ret;
  }
  if(!ret){
    n->type=ERROR;
    decide_if_error("INSTRUCTS", n, ret);
  }
  return ret;
}


bool prog_(ic* cntxt)
{
  #ifdef DEBUG
    node* n=next_token(CURRENT);
    printf("in prog, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;
  if (expect(OPENBR)){
    if(instructs_(cntxt)){
      ret=true;
    }
  }
  if(!ret){
    node* a=next_token(CURRENT);
    if(cntxt!=NULL){
      memset(cntxt->result, EMPTY, sizeof(char)*MAX_CHAR);
      a->type=ERROR;
    }
    decide_if_error("PROGRAM", a, ret);
  }
  return ret;
}


bool func_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  #ifdef DEBUG
    printf("in func, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;
  if(retfunc_(cntxt) || iofunc_(cntxt) || if_(cntxt) || loop_(cntxt)){
    ret=true;
  }
  decide_if_error("Unexpected INPUT(s)", n, ret);
  return ret;
}


bool print_(ic* cntxt)
{
  node* n=next_token(CURRENT); node* err=n;
  #ifdef DEBUG
    printf("in print, token=%s\n", token_to_string(n->type));
  #endif
  bool ret=false;
  if(accept(PRINT)){
    n=next_token(CURRENT);
    if(list_(cntxt)){
      ret=true;
    }
    if(string_(cntxt)){
      ret=true;
    }
    decide_if_error("expected LIST or STRING", err, ret);
    if(cntxt!=NULL){
      print_interp(cntxt, n);
    }
  }
  return ret;
}


void print_interp(ic* cntxt, node* n)
{
  if (n->type==VAR){
    char l =n->value[0];
    strcpy(cntxt->result, cntxt->arr[l-START]);
  }
  else if (n->type==NIL){
    strcpy(cntxt->result, ZERO);
  }
  if(!testing_mode(NULL)){
    printf("%s\n", cntxt->result);
  }
  return ;
}


bool var_(void)
{
  #ifdef DEBUG
    node* n=next_token(CURRENT);
    printf("in print, token=%s\n", token_to_string(n->type));
  #endif

  return accept(VAR);
}


char var_interp(void)
{
  //makes a mark of the variabelk to be assigned
  node* n=next_token(CURRENT);
  return n->value[0];
}


bool string_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  #ifdef DEBUG
    printf("in string, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;
  if (n->type==STRING){
    if(cntxt!=NULL){
      literalORstring_interp(cntxt);
    }
    accept(STRING);
     ret=true;
  }
  return ret;
}


bool literal_(ic* cntxt)
{
  node* n=next_token(CURRENT);
  #ifdef DEBUG
    printf("in literal, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;
  if (n->type==LITERAL){
    if(cntxt!=NULL){
      literalORstring_interp(cntxt);
    }
    accept(LITERAL);
     ret=true;
  }
  return ret;
}


void literalORstring_interp(ic* cntxt)
{
  if(cntxt!=NULL){
  node* n=next_token(CURRENT);
  strcpy(cntxt->result, n->value);
  }
  return ;
}


bool if_(ic* cntxt)
{
  #ifdef DEBUG
    node* n=next_token(CURRENT);
    printf("in if, token=%s\n", token_to_string(n->type));
  #endif

  bool ret=false;
  if (accept(IF)){
    if (accept(OPENBR)){
      if(boolfunc_(cntxt)){

        if (accept(CLOSEBR)){
          ret= if_instructs(cntxt);
        }
      }
    }
  }
  return ret;
}


bool if_instructs(ic* cntxt)
{
  bool ret1=false;  bool ret2=false;
  char res[MAX_CHAR];
  if(cntxt!=NULL){
    strcpy(res,cntxt->result);
  }
  if (accept(OPENBR)){
    if (cntxt!=NULL && res[0]==FALSE[0]){
      skip_brackets();
    }
    else{
      ret1=instructs_(cntxt);
    }
    if (accept(OPENBR)){
      if (cntxt!=NULL && res[0]==TRUE[0]){
        skip_brackets();
      }
      else{
        ret2=instructs_(cntxt);
      }
    }
  }
  return if_result(cntxt, res, ret1, ret2);
}


bool if_result(ic* cntxt, char res[MAX_CHAR], bool ret1, bool ret2)
{
  if (cntxt!=NULL && strcmp(res, TRUE)==0){
    return ret1;
  }
  if (cntxt!=NULL && strcmp(res, FALSE)==0){
    return ret2;
  }
  if (cntxt==NULL && ret1==true && ret2==true){
    return true;
  }
  else if (cntxt==NULL){
    return false;
  }
  return false;
}


void skip_brackets(void)
{
  int cnt=1;
  node* n=next_token(CURRENT);
  do{
    if (n->type==OPENBR){
      cnt++;
    }
    if (n->type==CLOSEBR){
      cnt--;
    }
    n=next_token(NEXT);
  }while(cnt!=0);
}


void list_free(node** head)
{
  if(*head!=NULL){
    list_free(&((*head)->next));
  }
  free(*head);
  *head=NULL;
}


void test_file_reset(char* code)
{
  FILE* f=NULL;
  f=fopen("testfile.txt", "w+");
  fputs(code,f);
  fseek(f,0,SEEK_SET);

  //resetting static variables in get_line and next_token
  char code_ln[MAX_CHAR]={EMPTY};
  memset(code_ln, EMPTY, sizeof(char)*MAX_CHAR);
  get_line(code_ln, f);
  next_token(RESET);
  return ;
  //token* tok=next_token(false);
}


//used before changing to the next peice of testing code.
//re-populates the test file and resets static vaiables in next-token function
void decide_if_error(char* str, node* n, bool ret)
{

  if(ret==false && !testing_mode(NULL)){
    #ifdef EXTEN
      error_message(str, n->type);
    #endif
    ERROR(str, \
    token_to_string(n->type));
  }
}


void error_message(char* message, token_type t)
{
  err_dtls d;
  next_token(CODE_POS);
  line_and_offset(&d, 0, 0, true);
  file_name(&d, NULL, true);
  get_code_ln(&d, NULL, true);

  char* str = "NUCLEI error";
  char pointer = POINTER;

  printf("%s:%d:%d", d.file_name, d.line, d.offset);
  printf("%s", SET_RED);
  printf(" %s: ", str);
  printf("%s", CLEAR_CLR);
  printf("%s in %s\n", message, token_to_string(t));
  printf("%s", d.code_ln);
  for(int i=0; i<d.offset; i++){
    printf(" ");
  }
  printf("%s", SET_GRN);
  printf("%c", pointer);
  printf("%s\n", CLEAR_CLR);

  exit(EXIT_FAILURE);
}


void line_and_offset(err_dtls* dtls, int line, int offset, bool retrive)
{
  static int ln=0;  static int off=0;
  if(!retrive){
    ln=line;
    off=offset;
  }
  if(retrive){
    dtls->line=ln+1;
    dtls->offset=off;
  }
}


void file_name(err_dtls* dtls, char* f_name, bool retrive)
{
    static char* str;
    if(!retrive){
      str=f_name;
    }
    if(retrive){
      dtls->file_name=str;
    }
}


void get_code_ln(err_dtls* dtls, char* code_ln, bool retrive)
{
  static char* str;
  if(!retrive){
    str=code_ln;
  }
  if(retrive){
    dtls->code_ln=str;
  }
}


void neucli_test(void)
{
  int test=1;
  bool mod=testing_mode(&test);

  test_lexicon();
  test_next_token();
  test_parser();
  test_interpreter();

  test=0;
  mod=testing_mode(&test);
}


void test_lexicon(void)
{
  //test token_to and string_to
  assert(string_to_token("TEST")==ERROR);
  assert(string_to_token("CAR")==0);
  assert(string_to_token("CDR")==1);
  assert(string_to_token("CONS")==2);
  assert(string_to_token("(")==16);
  assert(string_to_token(")")==17);
  assert(string_to_token("\"")==18);
  assert(string_to_token("'")==19);

  assert(strcmp(token_to_string(19),"'")==0);
  assert(strcmp(token_to_string(21),"ERROR")==0);
  assert(strcmp(token_to_string(0),"CAR")==0);
  assert(strcmp(token_to_string(5),"LESS")==0);
  assert(strcmp(token_to_string(9),"PRINT")==0);
  assert(strcmp(token_to_string(12),"NIL")==0);
  assert(strcmp(token_to_string(15),"LITERAL")==0);
  assert(strcmp(token_to_string(6),"GREATER")==0);

//test symbol_token
  char wrd[WRD]={EMPTY};
  assert(is_symbol('(', wrd)==true);
  memset(wrd, EMPTY, sizeof(char)*WRD);
  assert(is_symbol('*', wrd)==false);
  memset(wrd, EMPTY, sizeof(char)*WRD);
  assert(is_symbol('\'', wrd)==true);

//test varORinstruct
  int offset=0;
  node next;

  varORinstruct_token("(IF WHILE TEST LESS EQUAL)",&offset, &next);
  assert(next.type==OPENBR);
  varORinstruct_token("(IF WHILE TEST LESS EQUAL)",&offset, &next);
  assert(next.type==IF);
  offset=4;
  varORinstruct_token("(IF WHILE TEST LESS EQUAL)",&offset, &next);
  assert(next.type==WHILE);
  offset=10;
  varORinstruct_token("(IF WHILE TEST LESS EQUAL)",&offset, &next);
  assert(next.type==ERROR);
  offset=1;
  varORinstruct_token("(FAIL)",&offset, &next);
  assert(next.type==ERROR);
  offset=1;
  varORinstruct_token("(PRINT)",&offset, &next);
  assert(next.type==PRINT);

//test litORstr
  memset(wrd, EMPTY, sizeof(char)*WRD);
  offset=3;
  litORstr_token("---'(1 2 3 66 (7)'", &offset, &next);
  assert(next.type==LITERAL);

  offset=6;
  litORstr_token("SET C '5'", &offset, &next);
  assert(next.type==LITERAL);
}


void test_next_token(void)
{
  FILE* f=NULL;
  f=fopen("testfile.txt", "w+");
  fputs("(\n(PRINT \"FIRST TEST\")\n)",f);
  fseek(f,0,SEEK_SET);
  char code_ln[MAX_CHAR]={EMPTY};
  get_line(code_ln, f);
  node* n=next_token(NEXT);
  assert(n->type==OPENBR);

  n=next_token(NEXT);
  assert(n->type==OPENBR);

  n=next_token(NEXT);
  assert(n->type==PRINT);

  n=next_token(NEXT);
  assert(n->type==STRING);

  fclose(f);
}


void test_parser(void)
{
  ic* cntxt=NULL;
  //-------------------------------------TESTING EMPTY SPACE
  test_file_reset("()");
  node* n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("\n()");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("\n\n \n()");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("  \n\n  ()");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("  \n\n  ()\n");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("  \n\n  (\n\n\n  )\n   \n");
  n= next_token(NEXT);
  assert(prog_(cntxt));
  //-------------------------------------COMMENT TESTING
  test_file_reset("#typing notes\n((SET C '5'))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("#typing notes\n((SET C '5'))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((SET C '5')#typing notes\n)");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("((SET C '5'))\n#typing notes\n");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  //-------------------------------------TESTING LIST TYPES
  test_file_reset("((SET C '5'))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((SET X Y))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("SET X ");
  n= next_token(NEXT);
  assert(!func_(cntxt));

  test_file_reset("SET X Y Z");
  n= next_token(NEXT);
  assert(!instruct_(cntxt));

  test_file_reset("((SET X XW))");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("((SET Z \"NIL\"))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((SET Z \"NUL\"))");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("(\n(SET K '(5 (1 2 3))')\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("(\n(SET K (CONS '5' NIL))\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("(\n(SET K (LESS '5' '1'))\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("(\n(SET K (PLUS '5' '5'))\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("(\n(SET K '***********')\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));


  //-------------------------------------TESTING FUNCTIONS
  test_file_reset("((PRINT \"TESTING\"))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((PRINT '5'))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((PRINT '5' \"testing\"))");
  n= next_token(NEXT);
  assert(!func_(cntxt));

  test_file_reset("((PRINT (PLUS F '1')))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("     (    (  PRINT \"TESTING\"\n\n\n)\n\n\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((PRINT NULL))");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("SET C ");
  n= next_token(NEXT);
  assert(!set_(cntxt));

  test_file_reset("((SET C B))\n");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("(\n(SET C '5' '1')\n)");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("(\n(SET C '5')(WHILE (LESS '0' C)(\n(PRINT C)\n(SET A (PLUS '-1' C))\n(SET G Y))))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  //-------------------------------------TESTING BOOLFUNCs
  test_file_reset("((EQUAL F       '9'))\n");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((EQUAL F \"STRING\"))\n");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  //-------------------------------------TESTING RETFUNCs
  test_file_reset("(\n(PLUS F '123')\n)");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("(\n(PLUS F PRINT)\n)");
  n= next_token(NEXT);
  assert(!intfunc_(cntxt));

  test_file_reset("(\n(LENGTH NULL)\n)");
  n= next_token(NEXT);
  assert(!intfunc_(cntxt));

  //-------------------------------------TESTING LISTFUNCs
  test_file_reset("((CAR '2'))");
  n= next_token(NEXT);
  assert(prog_(cntxt));
  //-----------------------------(missing quote)
  test_file_reset("((CAR '(1 2 3 (4))))");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("((PRINT (CAR A \"string\")))");
  n= next_token(NEXT);
  assert(!func_(cntxt));

  test_file_reset("((CAR \"STRING\"");
  n= next_token(NEXT);
  assert(!prog_(cntxt));

  test_file_reset("((CDR '&&&&'))");
  n= next_token(NEXT);
  assert(prog_(cntxt));

  test_file_reset("((CONS '5' )");
  n= next_token(NEXT);
  assert(!listfunc_(cntxt));

  //-------------------------------------TESTING COMPLEX-FUNCs
  test_file_reset("     ((IF(LESS '2' '100')((SET P '72'))((SET P '50'))))");
  n = next_token(NEXT);
  assert(prog_(NULL));

  test_file_reset("((SET C '0')(WHILE (LESS C '5')((PLUS C '1'))))");
  n= next_token(NEXT);
  assert(prog_(NULL));

  test_file_reset("((SET P '0')(WHILE (LESS P '2')((SET C '0')\
  (WHILE (LESS C '5')((PLUS C '1')))  (SET P '27')))))");
  n= next_token(NEXT);
  assert(prog_(NULL));

  assert(remove("testfile.txt")==0);
}


void test_interpreter(void)
{
  #ifdef INTERP
  ic c;

  test_file_reset("(\n(SET C '5')\n)");
  node* n= next_token(NEXT);
  assert(c.arr['C'-START]!=NULL);
  assert(prog_(&c));
  assert(strcmp(c.arr['C'-START],"5")==0);

  test_file_reset("(\n(SET M '5')  (SET N '-78')\n)");
  n= next_token(NEXT);
  assert(c.arr['C'-START]!=NULL);
  assert(prog_(&c));
  assert(strcmp(c.arr['M'-START],"5")==0);
  assert(strcmp(c.arr['N'-START],"-78")==0);

  n->value[0]='M';
  n->type=VAR;
  int x=0;
  find_atomORvar_value(&c, n, &x);
  assert(x==5);

  test_file_reset("(\n(SET M '5')  (SET N '-78')  (SET M N)\n)");
  n= next_token(NEXT);
  assert(c.arr['C'-START]!=NULL);
  assert(prog_(&c));
  assert(strcmp(c.arr['M'-START],"-78")==0);
  assert(strcmp(c.arr['N'-START],"-78")==0);

  test_file_reset("(SET M )");
  n= next_token(NEXT);
  assert(c.arr['C'-START]!=NULL);
  assert(!instruct_(&c));

  test_file_reset("(\n(SET Z '(123 (4 5) 7)')\n)");
  n= next_token(NEXT);
  assert(c.arr['C'-START]!=NULL);
  assert(prog_(&c));
  assert(strcmp(c.arr['Z'-START],"(123 (4 5) 7)")==0);

  test_file_reset("(\n(SET Z (CAR'(123 (4 5) 7)'))\n)");
  n= next_token(NEXT);
  assert(c.arr['C'-START]!=NULL);
  assert(prog_(&c));
  assert(strcmp(c.arr['Z'-START],"123")==0);

  test_file_reset("( (SET M '(123 (4 5) 7)') ( PRINT M))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "(123 (4 5) 7)")==0);

  test_file_reset(" ( ( PRINT \"TESTING\"))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "TESTING")==0);

  test_file_reset("((CAR '5'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "()")==0);

  test_file_reset("((CAR NIL))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "")==0);

  test_file_reset("     ((LESS '2' '5'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, TRUE)==0);

  test_file_reset("     ((LESS '2' '(100 7 8)'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, TRUE)==0);

  test_file_reset("((CDR '(5 (1 2 3) 7 8)'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "((1 2 3) 7 8)")==0);

  test_file_reset("((CDR '10'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "()")==0);

  test_file_reset("((CDR '10' Z))");
  n= next_token(NEXT);
  assert(!func_(&c));

  test_file_reset("((CONS '5' '(15)'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "(5 15)")==0);

  test_file_reset("((SET L '(8 7)') (SET L (CAR (CDR L))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "7")==0);

   test_file_reset("((SET P (PLUS (CAR L) '6')))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "6")==0);

  test_file_reset("((SET L '(8 7)') (PLUS (CAR L) '6'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "14")==0);


  test_file_reset("((IF(LESS '2' '100')((SET P '72'))((SET P '50'))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['P'-START],"72")==0);

  test_file_reset("((IF(LESS '2' '100')((IF(LESS '100' '5')((SET P '72'))\
  ((SET P '99'))))((SET P '50'))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['P'-START],"99")==0);

  test_file_reset("((IF(EQUAL '1' '1')((PRINT \"YES\"))((GARBAGE))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result,"YES")==0);

  test_file_reset("((SET C '5')(WHILE (LESS '0' C)((PRINT C)\
  (SET C (PLUS '-1' C)))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['C'-START],"0")==0);

  test_file_reset("((SET T '10')(WHILE (GREATER T '2')((SET T (PLUS T '-1'))))\
  (SET P '26'))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['T'-START],"2")==0);
  assert(strcmp(c.arr['P'-START],"26")==0);

  test_file_reset("((SET X '10') (SET Y '26')  (PLUS X Y)  )))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result,"36")==0);


/*PASSED BUT DOES AND EXITFAIL IN LISP FUNCTIONS
  printf("-----------------------------CAR test 3\n");
  test_file_reset("((CAR '****'))");
  n= next_token(NEXT);
  assert(prog_(&c));
  printf("result---------------------%s\n",c.result);
  assert(strcmp(c.result, "()")==0);
*/

  test_file_reset(" ( (SET M '5') (SET N '3')(LESS M N))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, FALSE)==0);

  test_file_reset(" ( (SET M '5') (SET N '3')(PLUS M N))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.result, "8")==0);

  test_file_reset("  ((SET A '1')(PRINT A)(SET B '2')(PRINT B)(SET C B)\
    (PRINT C)(SET D(LESS B C))(PRINT D)(SET E (PLUS B '5'))(PRINT E)\
    (SET F '(2 3)')(PRINT F)(SET G (LENGTH F))(PRINT G)(SET H (CONS A F))\
    (PRINT H)(SET I (CONS '5' '(6 7)'))(PRINT I)\
    (SET J (CONS '1' (CONS '2' NIL)))(PRINT J)\
    (SET K '(1 2 3 (4 5) 6)')(PRINT K))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['A'-START], "1")==0);
  assert(strcmp(c.arr['B'-START], "2")==0);
  assert(strcmp(c.arr['C'-START], "2")==0);
  assert(strcmp(c.arr['D'-START], "0")==0);
  assert(strcmp(c.arr['E'-START], "7")==0);
  assert(strcmp(c.arr['F'-START], "(2 3)")==0);
  assert(strcmp(c.arr['G'-START], "2")==0);
  assert(strcmp(c.arr['H'-START], "(1 2 3)")==0);
  assert(strcmp(c.arr['I'-START], "(5 6 7)")==0);
  assert(strcmp(c.arr['J'-START], "(1 2)")==0);
  assert(strcmp(c.arr['K'-START], "(1 2 3 (4 5) 6)")==0);

  test_file_reset(" ((SET L (CONS '2' NIL))\
    (SET M (CONS '3' (CONS '4' (CONS '5' NIL)))) \
    (SET N (CONS '1' L))(SET P (CONS N M)) )");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['L'-START], "(2)")==0);
  assert(strcmp(c.arr['M'-START], "(3 4 5)")==0);
  assert(strcmp(c.arr['N'-START], "(1 2)")==0);
  assert(strcmp(c.arr['P'-START], "((1 2) 3 4 5)")==0);

  test_file_reset(" ( (SET P '4')(SET P(PLUS P '-1')))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['P'-START], "3")==0);

  test_file_reset("");

  test_file_reset("((SET P '4')(SET C '0')(WHILE (GREATER P '2')(\
  (WHILE (LESS C '5')((SET C(PLUS C '1'))(PRINT \"IN INNER LOOP!!!!!!!!!\")\
  ))(PRINT \"IN OUTTER LOOP!!!!!!!!!\")(PRINT P)(SET P (PLUS P '-1')))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['P'-START],"2")==0);

/*
  printf("-----------------------------WHILE test 2\n");
  test_file_reset("((WHILE (EQUAL '1' '1')((PRINT \"OUTTER\")\
  (WHILE (EQUAL '1' '1')((PRINT \"INNER\"))))");
  n= next_token(NEXT);
  assert(prog_(&c));
  */

  test_file_reset("((SET L '(1 0)')(SET C '2')(WHILE (LESS C '20') (\
  (SET N (PLUS (CAR L)  (CAR (CDR L)))) (SET M (CONS N L)) (SET L M)\
  (SET B (PLUS '1' C))  (SET C B)))(PRINT M))");
  n= next_token(NEXT);
  assert(prog_(&c));
  assert(strcmp(c.arr['M'-START],"(4181 2584 1597 987 610 377 233 144 89 \
55 34 21 13 8 5 3 2 1 1 0)")==0);

  assert(remove("testfile.txt")==0);
  #endif
}
