
#include "bingrid.h"
#define BOARDSTR (MAX*MAX+1)

void printboard(board* brd);
bool board_dim_check(int x);
char next_entry(char c1, char c2);
void solve_pairs(board* brd);
void solve_pair_dir(board* brd, bool dir, int i);
void solve_oxo(board* brd);
void solve_oxo_dir(board* brd, bool dir, int i);
void solve_cnt(board* brd);
void solve_count_dir(board* brd, bool dir, int i);
void solve_count_entry(board* brd, bool dir, int i, int cnt0, int cnt1);


void blank_array(board* brd)
{
  for (int i=0; i<MAX; i++){
    for (int j=0; j<MAX; j++){
      brd->b2d[j][i]='\0';
    }
  }
}

void printboard(board* brd)
{
  for (int i=0; i<MAX; i++){
    for (int j=0; j<MAX; j++){
      if (brd->b2d[i][j]=='\0' && j==0){
        i=MAX-1;
        j=MAX-1;
      }
      if (brd->b2d[i][j]!='\0'){
        printf("%c", brd->b2d[i][j]);
      }
    }
    printf("\n");
  }
}

bool board_dim_check(int x)
{
  double y = sqrt(x);
  int squ_check = pow(floor(y),2);

  if (squ_check==x && (int)y%2==0 && x!=0 && (int)y<=MAX){
    return true;
  }
  else{
    return false;
  }
}

bool str2board(board* brd, char* str)
{
  if(brd==NULL || str==NULL){
    exit(EXIT_FAILURE);
  }
  int str_cnt = strlen(str);

  if (board_dim_check(str_cnt)==true){
    brd->sz=sqrt(str_cnt);
    blank_array(brd);

    int cnt=0;
    for(int i=0; i<brd->sz; i++){
      for(int j=0; j<brd->sz; j++){
        brd->b2d[i][j]=str[cnt];
        cnt++;
      }
    }
    return true;
  }
  else{
    return false;
  }
}

void board2str(char* str, board* brd)
{
  if(str==NULL){
    exit(EXIT_FAILURE);
  }
  for(int i=0; i<BOARDSTR; i++){
    str[i]='\0';
  }
  int cnt=0;
  for(int i=0; i<brd->sz; i++){
    for(int j=0; j<brd->sz; j++){
      str[cnt]=brd->b2d[i][j];
      cnt++;
    }
  }
}

void solve_pairs(board* brd)
{
  char str1[BOARDSTR], str2[BOARDSTR];
  bool x_dir=true, y_dir=false;

  do{
    board2str(str1,brd);
    //solve grid for pairs , 1 row/column at a time
    for(int i=0; i<brd->sz; i++){
      solve_pair_dir(brd, x_dir, i);
      solve_pair_dir(brd, y_dir, i);
    }
    board2str(str2,brd);
  }while(strcmp(str1,str2)!=0);
}

void solve_pair_dir(board* brd, bool dir, int i)
{
  int* v;  int* h;
  int vert=0, hrz=0, j=0;

  if (dir==true){
    v=&i;  vert=0;
    h=&j;  hrz=1;
  }
  if (dir==false){
    v=&j;  vert=1;
    h=&i;  hrz=0;
  }
  for (j=0; j<brd->sz-1; j++){
    //does the curent and next cell contain a pair?
    char c1 = brd->b2d[*v][*h];
    char c2 = brd->b2d[*v+vert][*h+hrz];
    if (next_entry(c1, c2)!='\0'){
      //fill cell after the found pair
      if (j<=brd->sz-2 && brd->b2d[*v+(2*vert)][*h+(2*hrz)]==UNK){
        brd->b2d[*v+(2*vert)][*h+(2*hrz)]=next_entry(c1,c2);
      }
      //fill cell before the found pair
      if (j>0 && brd->b2d[*v-vert][*h-hrz]==UNK){
        brd->b2d[*v-vert][*h-hrz]=next_entry(c1,c2);
      }
    }
  }
}

char next_entry(char c1, char c2)
{
  if(c1==c2 && c1!=UNK){
    if (c1==ZERO){
      return ONE;
    }
    if (c1==ONE){
      return ZERO;
    }
    else{
      return '\0';
    }
  }
  else{
    return '\0';
  }
}

void solve_oxo(board* brd)
{
  char str1[BOARDSTR], str2[BOARDSTR];
  bool x_dir=true, y_dir=false;
  do{
    board2str(str1,brd);
    //solve grid for pairs , 1 row/column at a time
    for(int i=0; i<brd->sz; i++){
      solve_oxo_dir(brd, x_dir, i);
      solve_oxo_dir(brd, y_dir, i);
    }
    board2str(str2,brd);
  }while (strcmp(str1,str2)!=0);
}

void solve_oxo_dir(board* brd, bool dir, int i)
{
  int* v;  int* h;
  int vert=0, hrz=0, j=0;

  if (dir==true){
    v=&i;  vert=0;
    h=&j;  hrz=1;
  }
  if (dir==false){
    v=&j;  vert=1;
    h=&i;  hrz=0;
  }
  for (j=1; j<brd->sz-1; j++){
    char c1=brd->b2d[*v+vert][*h+hrz];
    char c2=brd->b2d[*v][*h];
    char c3=brd->b2d[*v-vert][*h-hrz];
    if (next_entry(c1, c3)!='\0' && c2==UNK){
      brd->b2d[*v][*h]=next_entry(c1, c3);
    }
  }
}


void solve_count(board* brd)
{
  char str1[BOARDSTR], str2[BOARDSTR];
  bool x_dir=true, y_dir=false;
  do{
    board2str(str1,brd);
    //solve grid for pairs , 1 row/column at a time
    for(int i=0; i<brd->sz; i++){
      solve_count_dir(brd, x_dir, i);
      solve_count_dir(brd, y_dir, i);
    }
    board2str(str2,brd);
  }while (strcmp(str1,str2)!=0);
}

void solve_count_dir(board* brd, bool dir, int i)
{
  int* v;  int* h;
  int j=0;
  if (dir==true){
    v=&i; h=&j;
  }
  if (dir==false){
    v=&j; h=&i;
  }
  int cnt0=0, cnt1=0;
  for(j=0; j<brd->sz; j++){
    if (brd->b2d[*v][*h]==ZERO){
      cnt0++;
    }
    if (brd->b2d[*v][*h]==ONE){
      cnt1++;
    }
    solve_count_entry(brd, dir, i, cnt0, cnt1);
  }
}

void solve_count_entry(board* brd, bool dir, int i, int cnt0, int cnt1)
{
  int j=0;
  int* v; int* h;
  if (dir==true){
    v=&i;  h=&j;
  }
  if (dir==false){
    v=&j;  h=&i;
  }
  char next_mv = '\0';
  if (cnt0==(brd->sz/2) || cnt1==(brd->sz/2)){
    if (cnt1==(brd->sz/2)){
      next_mv = ZERO;
    }
    if (cnt0==(brd->sz/2)){
      next_mv = ONE;
    }
    for(j=0; j<brd->sz; j++){
      if(brd->b2d[*v][*h]==UNK){
        brd->b2d[*v][*h]=next_mv;
      }
    }
  }
}

bool complete_board(board* brd)
{
  int cnt=0;
   for (int i=0; i<brd->sz; i++){
     for (int j=0; j<brd->sz; j++){
       if (brd->b2d[i][j]==UNK){
         cnt++;
       }
     }
   }
   if (cnt>0){
     return false;
   }
   else{
     return true;
   }
}

bool solve_board(board* brd)
{
  if(brd==NULL){
    exit(EXIT_FAILURE);
  }
  bool ans=false;
  int cnt=0;

  char str1[BOARDSTR], str2[BOARDSTR];
  do{
    board2str(str1, brd);
    solve_pairs(brd);
    solve_oxo(brd);
    solve_count(brd);
    ans=complete_board(brd);
    board2str(str2, brd);
    cnt++;
  }while (strcmp(str1,str2)!=0);
  return ans;
}

void test(void)
{
  board b;
  char str[BOARDSTR];

  //string 2 board testing
  assert(str2board(&b, "101001")==0);
  assert(str2board(&b, ".............")==0);
  assert(str2board(&b, "1110110110001111")==1);

  //board dim squ_check
  assert(board_dim_check(4)==1);
  assert(board_dim_check(3)==0);
  assert(board_dim_check(9)==0);
  assert(board_dim_check(12)==0);
  assert(board_dim_check(16)==1);
  assert(board_dim_check(0)==0);
  //test 16*16
  assert(board_dim_check(256)==1);
  //test 18*18
  assert(board_dim_check(324)==0);

  //board to string testing
  assert(str2board(&b, "0000000000000000")==1);
  board2str(str,&b);
  assert(strcmp(str,"0000000000000000")==0);

  //next entry testing
  assert(next_entry('1', '0')=='\0');
  assert(next_entry('0', '0')==ONE);
  assert(next_entry('1', '1')==ZERO);
  assert(next_entry('.', '.')=='\0');
  assert(next_entry('1', '.')=='\0');

  //solve pair dir testing
  assert(str2board(&b, "..00..........11")==1);
  solve_pair_dir(&b, true, 0);
  board2str(str,&b);
  assert(strcmp(str,".100..........11")==0);

  assert(str2board(&b, "..00..........11")==1);
  solve_pair_dir(&b, true, 1);
  board2str(str,&b);
  assert(strcmp(str,"..00..........11")==0);

  assert(str2board(&b, "..00..........11")==1);
  solve_pair_dir(&b, false, 1);
  board2str(str,&b);
  assert(strcmp(str,"..00..........11")==0);

  //solve pairs testing
  assert(str2board(&b, "..00..........11")==1);
  solve_pairs(&b);
  board2str(str,&b);
  assert(strcmp(str,".100.........011")==0);

  assert(str2board(&b, "..0..1...1..0..0")==1);
  solve_pairs(&b);
  board2str(str,&b);
  assert(strcmp(str,"1001.1...1..0010")==0);

  assert(str2board(&b, ".00..1...1..00.0")==1);
  solve_pairs(&b);
  board2str(str,&b);
  assert(strcmp(str,"1001.1...1..0010")==0);

  assert(str2board(&b, "..11.....0..1..1")==1);
  solve_pairs(&b);
  board2str(str,&b);
  assert(strcmp(str,".011.....0..1..1")==0);

  assert(str2board(&b, ".......0...0....")==1);
  solve_pairs(&b);
  board2str(str,&b);
  assert(strcmp(str,"...1...0...0...1")==0);

  assert(str2board(&b, "1010101001010100110110..1011000101..")==1);
  solve_pairs(&b);
  board2str(str,&b);
  assert(strcmp(str,"1010101001010100110110.01011000101.1")==0);

  //oxo testing
  assert(str2board(&b, "1.10.....0..10.0")==1);
  solve_oxo(&b);
  board2str(str,&b);
  assert(strcmp(str,"1010.1...0..1010")==0);

  //oxo direction
  assert(str2board(&b, ".1.1.........0.0")==1);
  solve_oxo_dir(&b, true, 0);
  board2str(str,&b);
  assert(strcmp(str,".101.........0.0")==0);

  assert(str2board(&b, ".1.1.........0.0")==1);
  solve_oxo_dir(&b, true, 1);
  board2str(str,&b);
  assert(strcmp(str,".1.1.........0.0")==0);

  //solve count testing
  assert(str2board(&b, ".1.1.1..............0.00.......11.1.")==1);
  solve_count(&b);
  board2str(str,&b);
  assert(strcmp(str,"010101.0.....0....110100.0....011010")==0);

  assert(str2board(&b, ".0..")==1);
  solve_count(&b);
  board2str(str, &b);
  assert(strcmp(str, "1001")==0);

  //count direction
  assert(str2board(&b, "...1...0...0....")==1);
  solve_count_dir(&b, false, 3);
  board2str(str,&b);
  assert(strcmp(str,"...1...0...0...1")==0);

  assert(str2board(&b, "001.............")==1);
  solve_count_dir(&b, false, 0);
  board2str(str,&b);
  assert(strcmp(str,"001.............")==0);

  //complete board testing
  assert(str2board(&b, "..0.0.01.11.0.........0.0.11.......1...01.....1..0...0....1..0.0")==1);
  assert(complete_board(&b)==false);
  assert(str2board(&b, "0110")==1);
  assert(complete_board(&b)==true);

  //solve board testing
  assert(str2board(&b, "..0.0.01.11.0.........0.0.11.......1...01.....1..0...0....1..0.0")==1);
  assert(solve_board(&b)==true);
  board2str(str,&b);
  assert(strcmp(str,"1001010101100110110010010011010101011010101001101001100101101010")==0);
}
