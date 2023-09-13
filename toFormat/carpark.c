#include<stdio.h>
#include<stdbool.h>
#include<assert.h>
#include<stdlib.h>
#include<string.h>
#define MAX_DIM 20
#define VHCL_MODELS 'Z'-'A'
#define FRST_VHCL 'A'
#define LAST_VHCL 'Z'
#define MAX_NO 200
#define BLRD '#'
#define SPC '.'
#define POS_ENTRYS 1000
#define EMPTY -1
#define UNUSED '\0'

struct vehicle{
  char label;
  int coords[2];
  bool hrz;
  int length;
  bool exited;
};
typedef struct vehicle vehicle;

struct car_park{
  int height;
  int width;
  char layout[MAX_DIM][MAX_DIM];
  int numOvs;
};
typedef struct car_park car_park;

struct state{
  car_park cp;
  int parent;
};
typedef struct state state;


void test_simple(void);
void test_moves_and_exits(void);
void test_v_moves(void);
void test_children_gen(void);
void copy_layout(char arr[MAX_DIM][MAX_DIM], car_park* cp);
bool dup_layout(car_park cp1, car_park cp2);
void wipe_layout(car_park* cp);
void clear_list(state list[MAX_NO]);
void clear_cp(car_park* cp);
void copy_cp(car_park cp1, car_park* cp2);
void print_cp(car_park cp);
void print_states(state list[POS_ENTRYS], int list_len);
void print_vehicle(vehicle v);
bool is_empty(car_park cp);
bool log_vehicle(car_park cp, vehicle* v);
void log_all_vehicles(car_park* cp, vehicle cp_vs[MAX_NO]);
void insert_cp_vehicles(car_park* cp, vehicle cp_vs[MAX_NO]);
bool check_vehicle_dims(int v_layout[MAX_DIM][2], vehicle* v);
void valid_car_park(car_park cp);
int find_vehicle_coords(car_park cp, vehicle v, int v_layout[MAX_DIM][2]);
int search4vehicle(car_park cp, vehicle v, int v_layout[MAX_DIM][2]);
int get_vehicle_coords(vehicle v, int v_layout[MAX_DIM][2]);
bool can_move(car_park cp, vehicle* v, bool dir);
bool at_exit(car_park cp, vehicle* v);
bool children_moves(car_park parent, state list[POS_ENTRYS], int current, int* list_len);
void scan_file(car_park* cp_root, FILE* fpin);
int find_vehicle_types(car_park cp);
bool log_move(state list[POS_ENTRYS], vehicle cp_vs[MAX_NO], int current, int* list_len, int parent_no);
void log_parent_vehicles(car_park cp, vehicle cp_vs[MAX_NO]);
state run_through_parents(car_park root_cp, state list[POS_ENTRYS], int* list_len);
bool vehicle_moves(car_park cp, vehicle* v, bool dir);
int count_parents(state list[POS_ENTRYS], int list_len);
void print_moves(state list[POS_ENTRYS], int list_len);


int main(int argc, char* argv[])
{
  test_simple();
  test_moves_and_exits();
  test_children_gen();
  int list_len=0;
  car_park cp;
  state static list[POS_ENTRYS];
  vehicle cp_vs[MAX_NO];

  if (argc==1){
    printf("No car park file has been declared.\n");
    exit(EXIT_FAILURE);
  }
  if (argc==2){
    FILE* fpin = fopen(argv[1], "r");
    if(!fpin){
      printf("Could not read file!\n");
      exit(EXIT_FAILURE);
    }
    scan_file(&cp, fpin);
    log_all_vehicles(&cp, cp_vs);
    run_through_parents(cp, list, &list_len);

    int cnt= count_parents(list, list_len);
    printf("%i moves\n", cnt);
  }
  if (argc==3){
    char str[MAX_DIM];
    sscanf(argv[1], "%s", str);
    printf("%s\n", str);
    if (strcmp(str, "-show")==0){
      FILE* fpin = fopen(argv[2], "r");
      if(!fpin){
        printf("Could not read file!\n");
        exit(EXIT_FAILURE);
      }

      scan_file(&cp, fpin);
      log_all_vehicles(&cp, cp_vs);
      run_through_parents(cp, list, &list_len);
      print_moves(list, list_len);
      int cnt= count_parents(list, list_len);
      printf("%i moves\n", cnt);
     }
  }
}

void wipe_layout(car_park* cp)
{
  if(cp!=NULL){
    for(int j=0; j<MAX_DIM; j++){
      for (int i=0; i<MAX_DIM; i++){
        cp->layout[j][i]=UNUSED;
      }
    }
  }
}

void clear_cp(car_park* cp)
{
  if(cp!=NULL){
    for(int j=0; j<cp->height; j++){
      for (int i=0; i<cp->width; i++){
        if (cp->layout[j][i]!=BLRD){
          cp->layout[j][i]=SPC;
        }
      }
    }
  }
}


bool is_empty(car_park cp)
{
  for(int j=0; j<cp.height; j++){
    for (int i=0; i<cp.width; i++){
      if (cp.layout[j][i]!=BLRD && cp.layout[j][i]!=SPC){
        return false;
      }
    }
  }
  return true;
}

void copy_layout(char arr[MAX_DIM][MAX_DIM], car_park* cp)
{
  for(int j=0; j<cp->height; j++){
    for (int i=0; i<cp->width; i++){
      cp->layout[j][i]=arr[j][i];
    }
  }
}

void copy_cp(car_park cp1, car_park* cp2)
{
  for(int j=0; j<cp1.height; j++){
    for (int i=0; i<cp1.width; i++){
      cp2->layout[j][i]=cp1.layout[j][i];
    }
  }
}

bool dup_layout(car_park cp1, car_park cp2)
{
  int height =cp1.height;
  int width=cp1.width;
  int i; int j;
  for(j=0; j<height; j++){
    for (i=0; i<width; i++){
      if (cp1.layout[j][i]!=cp2.layout[j][i]){
        return false;
      }
    }
  }
  return true;
}

void clear_list(state list[MAX_NO])
{
  for (int i=0; i<POS_ENTRYS; i++){
    state s;
    wipe_layout(&s.cp);
    s.parent=0;
    list[i]=s;
  }
}

void print_cp(car_park cp)
{
  printf("\n");
  for(int j=0; j<cp.height; j++){
    for (int i=0; i<cp.width; i++){
      printf("%c",cp.layout[j][i]);
    }
    printf("\n");
  }
}

void print_states(state list[POS_ENTRYS], int list_len)
{
  for (int i=0; i<list_len; i++){
    state s=list[i];
    print_cp(s.cp);
  }
}

void copy_vehicles(vehicle cp_vs1[MAX_NO], vehicle cp_vs2[MAX_NO], int x)
{
  for(int i=0; i<x; i++){
    cp_vs2[i]=cp_vs1[i];
  }
}

void print_vehicle(vehicle v)
{
  printf("%c coordinates %i, %i, hoirzontal? %i, length %i, exited? %i\n\n", v.label, v.coords[0], v.coords[1], v.hrz, v.length, v.exited);
}

void valid_car_park(car_park cp)
{
  for(int j=0; j<cp.height; j++){
    for (int i=0; i<cp.width; i++){
      if (cp.layout[j][i]==BLRD || cp.layout[j][i]==SPC || (cp.layout[j][i]>=FRST_VHCL && cp.layout[j][i]<=LAST_VHCL)){
      }
      else{
        printf("One or more unexpected characters found in the car park file.\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}


void scan_file(car_park* cp_root, FILE* fpin)
{
  int height=0; int width=0;
  char str[MAX_DIM];
  memset(str, UNUSED, sizeof(char)*MAX_DIM);
  char arr[MAX_DIM][MAX_DIM];
  memset(arr, UNUSED, sizeof(char)*MAX_DIM*MAX_DIM);

  if (fgets(str, MAX_DIM, fpin)!=NULL){
    sscanf(str,"%dx%d", &height, &width);
  }
  int h=0;
  char row[MAX_DIM];
  memset(row, UNUSED, sizeof(char)*MAX_DIM);
  wipe_layout(cp_root);

  while(fgets(str, MAX_DIM, fpin)!=NULL && h<MAX_DIM){
    if ((int)strlen(str)!=width+1 || h>height){
      printf("Car park height does not match the declared dimensions.\n");
      exit(EXIT_FAILURE);
    }
    if((int)strlen(str)!=width+1){
      printf("Car park width does not match the declared dimensions.\n");
      exit(EXIT_FAILURE);
    }
    sscanf(str, "%s", cp_root->layout[h]);
    h++;
  }
  cp_root->height=height;
  cp_root->width=width;
  valid_car_park(*cp_root);
  //printf("height=%i, width=%i\n", cp_root->height, cp_root->width);
  //print_cp(*cp_root);
  return ;
}

bool check_vehicle_dims(int v_layout[MAX_DIM][2], vehicle* v)
{
  int l=v->length;
  if(l==0){
    return false;
  }
  int x=0;
  if(l<=1){
    return false;
  }
  for(x=0; x<l-1; x++){

    if(v->hrz==true && v_layout[x][0]!=v_layout[x+1][0]){
      return false;
    }
    if(v->hrz==false && v_layout[x][1]!=v_layout[x+1][1]){
      return false;
    }
  }
  return true;
}


int get_vehicle_coords(vehicle v, int v_layout[MAX_DIM][2])
{
  int row=0;
  bool hrz=v.hrz;
  for(int i=0; i<v.length; i++){
    if(i==0){
      v_layout[row][0]=v.coords[0];
      v_layout[row][1]=v.coords[1];
    }
    if(i!=0 && hrz==true){
      v_layout[row][0]=v.coords[0];
      v_layout[row][1]=v.coords[1]+i;
    }
    if(i!=0 && hrz==false){
      v_layout[row][0]=v.coords[0]+i;
      v_layout[row][1]=v.coords[1];
    }
    row++;
  }
  return row;
}


void log_all_vehicles(car_park* cp, vehicle cp_vs[MAX_NO])
{
  bool found=true;
  char nth_v = FRST_VHCL;

  while(found==true){
    vehicle v;
    v.label=nth_v;
    int x= nth_v-FRST_VHCL;
    found = log_vehicle(*cp, &v);
    cp_vs[x]=v;
    nth_v++;
  }
  cp->numOvs=nth_v-FRST_VHCL-1;
}


void log_parent_vehicles(car_park cp, vehicle cp_vs[MAX_NO])
{
  char nth_v = FRST_VHCL;
  int x=0;
  for(int i=0; i<LAST_VHCL-FRST_VHCL; i++){
    vehicle v;
    v.label=nth_v+i;
    bool found = log_vehicle(cp, &v);
    if (found==true){
      cp_vs[x]=v;
      x++;
    }
  }
}

bool log_vehicle(car_park cp, vehicle* v)
{
  int v_layout[MAX_DIM][2]={{UNUSED}};
  int l = find_vehicle_coords(cp, *v, v_layout);
  if(l==1){
    printf("At least one vehicle lengths is not greater than 1.\n");
    exit(EXIT_FAILURE);
    return false;
  }
  v->length=l;
  v->coords[1]=v_layout[0][1];
  v->coords[0]=v_layout[0][0];
  v->exited=false;

  if (v->coords[1]==v_layout[1][1]){
    v->hrz=false;
  }
  if(v->coords[0]==v_layout[1][0]){
    v->hrz=true;
  }
  if (check_vehicle_dims(v_layout, v)==false){
    return false;
  }
  return true;
}

int find_vehicle_coords(car_park cp, vehicle v, int v_layout[MAX_DIM][2])
{
  int l=0;
  for(int j=0; j<cp.height; j++){
    for (int i=0; i<cp.width; i++){
      if (cp.layout[j][i]==v.label){
        v_layout[l][0]=j;
        v_layout[l][1]=i;
        l++;
      }
    }
  }
  return l;
}

void insert_cp_vehicles(car_park* cp, vehicle cp_vs[MAX_NO])
{
  clear_cp(cp);
  vehicle v;
  int j; int i;
  for(int x=0; x<cp->numOvs; x++){
    v=cp_vs[x];
    if (v.exited==false){
      j=v.coords[0];
      i=v.coords[1];
      cp->layout[j][i]=v.label;
      for(int l=1; l<v.length; l++){
        if (v.hrz==true){
          i++;
        }
        else if (v.hrz==false){
          j++;
        }
        cp->layout[j][i]=v.label;
      }
    }
  }
  return ;
}


bool can_move(car_park cp, vehicle* v, bool dir)
{
  int j=v->coords[0];
  int i=v->coords[1];
  if (v->hrz==true){
    if(dir==true && i<((cp.width)-(v->length))){
      if (cp.layout[j][i+v->length]==SPC){
        v->coords[1]=i+1;
        return true;
      }
    }
    if(dir!=true && i>0){
      if (cp.layout[j][i-1]==SPC){
        v->coords[1]=i-1;
        return true;
      }
    }
  }
  else if (v->hrz==false){
    if(dir!=true && j>0){
      if (cp.layout[j-1][i]==SPC){
        v->coords[0]=j-1;
        return true;
      }
    }
    if(dir==true && j<=((cp.height)-(v->length))){
      if (cp.layout[j+v->length][i]==SPC){
        v->coords[0]=j+1;
        return true;
      }
    }
  }
  return false;
}


bool at_exit(car_park cp, vehicle* v)
{
  int v_layout[MAX_DIM][2];
  memset(v_layout, EMPTY, sizeof(int)*MAX_DIM*2);
  get_vehicle_coords(*v, v_layout);

  int row=0;
  while(v_layout[row][0]!=EMPTY){

    if (v_layout[row][0]==0 || v_layout[row][1]==0){
      v->exited=true;
      return true;
    }
    else if (v_layout[row][0]==cp.height-1 || v_layout[row][1]==cp.width-1){
      v->exited=true;
      return true;
    }
  row++;
  }
  return false;
}


bool log_move(state list[POS_ENTRYS], vehicle cp_vs[MAX_NO], int current, int* list_len, int parent_no)
{
  car_park cp;
  cp=list[current].cp;
  clear_cp(&cp);
  insert_cp_vehicles(&cp, cp_vs);
  bool dup;
  for(int i=0; i<*list_len; i++){
    dup = dup_layout(cp, list[i].cp);
    if(dup==true){
      return false;
    }
  }
  list[*list_len].cp=cp;
  list[*list_len].parent=parent_no;
  (*list_len)++;
  //print_cp(cp);

  return true;
}


bool vehicle_moves(car_park cp, vehicle* v, bool dir)
{

  bool moved=can_move(cp, v, dir);
  if (moved==true){

    //exit flag is turned on in the "at_exit" function
    at_exit(cp, v);
  }
  return moved;
}


bool children_moves(car_park parent, state list[POS_ENTRYS], int current, int* list_len)
{
  vehicle cp_vs[MAX_NO];
  car_park test_cp;
  test_cp.height=list[current].cp.height;
  test_cp.width=list[current].cp.width;
  test_cp.numOvs=list[current].cp.numOvs;
  int parent_no =current;
  for(int i=0; i<parent.numOvs; i++){
    test_cp=parent;
    log_parent_vehicles(parent, cp_vs);
    insert_cp_vehicles(&test_cp,cp_vs);
    vehicle v =cp_vs[i];
    int cnt=0;
    bool dir=true;  bool moved=true;  bool end=false;
    while(end!=true){
      moved=vehicle_moves(test_cp, &v, dir);
      if (moved==true){
        if (cnt>0){
          parent_no++;
        }
        cnt++;
        cp_vs[i]=v;
        log_move(list, cp_vs, current, list_len, parent_no);
        insert_cp_vehicles(&test_cp,cp_vs);
        if(is_empty(test_cp)==true){
          return true;
        }
      }
      else if (moved==false){
        if (dir==false){
          end=true;
        }
        if (dir==true){
          dir=false;
        }
      }
    }
  }
  return false;
}


state run_through_parents(car_park root_cp, state list[POS_ENTRYS], int* list_len)
{
  bool solved;
  int current=0;
  state s;
  s.cp=root_cp;
  list[current]=s;
  (*list_len)++;
  car_park parent=root_cp;
  while(current<*list_len){
    solved=children_moves(parent, list, current, list_len);

    if (solved==true){
      return list[*list_len-1];
    }
    if (current<*list_len-1){
      current++;
      parent=list[current].cp;
    }
  }
  return list[*list_len-1];
}

int count_parents(state list[POS_ENTRYS], int list_len)
{
  int cnt=1;
  state s=list[list_len-1];
  int p = s.parent;
  while (p>0){
    p=s.parent;
    s=list[p];
    cnt++;
  }
  return cnt-1;
}

void print_moves(state list[POS_ENTRYS], int list_len)
{
  state to_print[POS_ENTRYS];
  state s=list[list_len-1];
  int p = s.parent;
  int i=0;
  while (p>0){
    to_print[i]=s;
    print_cp(s.cp);
    p=s.parent;
    s=list[p];
    i++;
  }
  print_cp(s.cp);
  return ;
}

/*
void print_parents(list[POS_ENTRYS], int list_len);
{

}

how to use the 'show' flags
remember not testing scanf functions will result in warning message
checxk pointer !=NULL???

*/
//make test for stange characters


//simple function testing

void test_cp_and_vehicle_valid(void)
{
  //make sure this doesn't happen
  car_park cp1;
  cp1.height=10;
  cp1.width=6;
  char arr1[MAX_DIM][MAX_DIM]={"#.####",".....#", "#CC..#", "#..BB#", "#..AA#", "#....#", "#DDD.#", "######"};
  vehicle vs1[MAX_NO];
  copy_layout(arr1, &cp1);
  log_all_vehicles(&cp1, vs1);
}

void test_simple(void)
{
  car_park cpx;
  cpx.height=6;
  cpx.width=6;
  car_park cpy;
  cpy.height=6;
  cpy.width=6;
  char arr_l[MAX_DIM][MAX_DIM]={"#.####",".....#", "#CC..#", "#..BB#", "#..AA#", "######"};
  copy_layout(arr_l, &cpx);
  copy_layout(arr_l, &cpy);
  assert(dup_layout(cpx, cpy)==true);

  char arr_m[MAX_DIM][MAX_DIM]={"#.####",".....#", "#....#", "#..BB#", "#..AA#", "######"};
  copy_layout(arr_m, &cpx);
  assert(dup_layout(cpx, cpy)==false);

  char arr_n[MAX_DIM][MAX_DIM]={"#.####",".....#", "#CC..#", "#..BB#", "#..AA#", "######"};
  copy_layout(arr_n, &cpx);
  copy_layout(arr_n, &cpy);
  assert(dup_layout(cpx, cpy)==true);

  char arr_o[MAX_DIM][MAX_DIM]={"#.####",".BBB.#", "#AAA.#", "#C...#", "#C...#", "######"};
  copy_layout(arr_o, &cpx);
  copy_layout(arr_o, &cpy);
  assert(dup_layout(cpx, cpy)==true);
}

void test_moves_and_exits(void)
{
  //set up testing, documenting vehicles and vehicles ability to move
  car_park cp1;
  cp1.height=6;
  cp1.width=6;
  char arr[MAX_DIM][MAX_DIM]={"# ####"," BB  #", "#A  C#", "#A  C#", "#A  C#", "######"};
  copy_layout(arr, &cp1);
  vehicle vs1[MAX_NO];
  log_all_vehicles(&cp1, vs1);
  assert(cp1.numOvs==3);
  clear_cp(&cp1);
  assert(is_empty(cp1)==true);
  insert_cp_vehicles(&cp1, vs1);

  vehicle* v1=&vs1[0];
  vehicle* v2=&vs1[1];
  vehicle* v3=&vs1[2];
  int v_layout1[MAX_DIM][2];

  get_vehicle_coords(*v1, v_layout1);
  //print_vehicle(*v1);
  assert(v_layout1[0][0]==2);
  assert(v_layout1[0][1]==1);
  assert(v_layout1[1][0]==3);
  assert(v_layout1[1][1]==1);
  assert(v_layout1[2][0]==4);
  assert(v_layout1[2][1]==1);
  assert(v1->length==3);
  assert(v1->hrz==false);
  assert(v1->label=='A');
  assert(v2->length==2);
  assert(v2->hrz==true);
  assert(v2->label=='B');
  assert(v3->length==3);
  assert(v3->hrz==false);
  assert(cp1.numOvs==3);

  assert(can_move(cp1, v1, true)==false);
  assert(at_exit(cp1, v1)==false);
  assert(can_move(cp1, v2, false)==true);
  assert(at_exit(cp1, v2)==true);
  assert(can_move(cp1, v3, true)==false);
  assert(at_exit(cp1, v3)==false);
  assert(can_move(cp1, v3, false)==1);


  //testing abiltity to exit and remove vehicle from the car park
  car_park cp_e1;
  cp_e1.height=6;
  cp_e1.width=6;
  char arr_e1[MAX_DIM][MAX_DIM]={"#.####","..AAA#", "#B...#", "#B...#", "#B...#", "######"};
  copy_layout(arr_e1, &cp_e1);
  vehicle vs_e1[MAX_NO];
  log_all_vehicles(&cp_e1, vs_e1);
  assert(at_exit(cp_e1, &vs_e1[0])==false);
  assert(can_move(cp_e1, &vs_e1[0],true)==false);
  assert(can_move(cp_e1, &vs_e1[0],false)==true);
  insert_cp_vehicles(&cp_e1, vs_e1);
  //print_cp(cp_e1);
  assert(can_move(cp_e1, &vs_e1[0],false)==true);
  insert_cp_vehicles(&cp_e1, vs_e1);
  //print_cp(cp_e1);
  assert(at_exit(cp_e1, &vs_e1[0])==true);
  assert(cp_e1.numOvs==2);
  insert_cp_vehicles(&cp_e1, vs_e1);

  //6x6 file given
  car_park cp2;
  cp2.height=6;
  cp2.width=6;
  char arr2[MAX_DIM][MAX_DIM]={"#.####",".BBB.#", "#A...#", "#A...#", "#A...#", "######"};
  copy_layout(arr2, &cp2);
  vehicle vs2[MAX_NO];
  log_all_vehicles(&cp2, vs2);
  //round 1
  assert(can_move(cp2, &vs2[0],true)==false);
  assert(can_move(cp2, &vs2[0],false)==false);
  assert(can_move(cp2, &vs2[1],true)==true);
  insert_cp_vehicles(&cp2, vs2);
  //print_cp(cp2);
  assert(can_move(cp2, &vs2[1],false)==true);
  insert_cp_vehicles(&cp2, vs2);
  //print_cp(cp2);
  assert(can_move(cp2, &vs2[1],false)==true);
  insert_cp_vehicles(&cp2, vs2);
  assert(at_exit(cp2, &vs2[1])==true);
  insert_cp_vehicles(&cp2, vs2);
  //print_cp(cp2);

  //round 2
  assert(can_move(cp2, &vs2[0],true)==false);
  assert(can_move(cp2, &vs2[0],false)==true);
  assert(at_exit(cp2, &vs2[0])==false);
  insert_cp_vehicles(&cp2, vs2);
  //print_cp(cp2);
  assert(can_move(cp2, &vs2[0],false)==true);
  assert(at_exit(cp2, &vs2[0])==true);
  insert_cp_vehicles(&cp2, vs2);
  //print_cp(cp2);


  //print_cp(cp_e1);
/*
  char arr1[MAX_DIM][MAX_DIM]={"####","#..BB#", "#A..C#", "#A..C#", "#A..C#", "#.####"};
  car_park cp1;
  cp1.width=6;
  cp1.height=6;
  copy_layout(arr1, &cp1);
*/
}

/*
void test_children_gen(void)
{
  char arr1[MAX_DIM][MAX_DIM]={"###..#","#..BB#", "#A..C#", "#A..C#", "#A..C#", "#.####"};
  car_park cp1;
  cp1.width=6;
  cp1.height=6;
  copy_layout(arr1, &cp1);
  vehicle vs1[MAX_NO];
  car_park cp_blank;
  char blank[MAX_DIM][MAX_DIM]={"###..#","#....#", "#....#", "#....#", "#....#", "#.####"};
  copy_layout(blank, &cp_blank);
  state s;
  s.cp=&cp1;
  //print_cp(*s.cp);
  log_all_vehicles(&cp1, vs1);
  s.cp->numOvs=cp1.numOvs;
  //printf("%i\n", s.cp->numOvs);
  state list1[POS_ENTRYS];
  list1[0]=s;
  //int len=1;
  int* child_added=0;
  children_moves(cp1, list1, 0, 1, child_added);
  //print_states(list1, 2);

}
*/
/*
void test_v_moves(void)
{
  char arr1[MAX_DIM][MAX_DIM]={"####.#","...BB#", "#AA.C.", "#...C#", "#...C#", "######"};
  car_park cp1;
  cp1.width=6;
  cp1.height=6;
  copy_layout(arr1, &cp1);
  vehicle vs1[MAX_NO];
  log_all_vehicles(&cp1, vs1);
  assert(cp1.numOvs==3);
  //print_cp(cp1);
  state s;
  s.cp=cp1;
  state list[POS_ENTRYS];
  list[0]=s;
  //int l=1;
  vehicle v1=vs1[0];
  vehicle v2=vs1[1];
  vehicle v3=vs1[2];

  assert(vehicle_moves(cp1, &v1, false)==false);
  insert_cp_vehicles(&cp1, v1);
  //print_vehicle(v1);
  assert(vehicle_moves(cp1, &v1, true)==true);
  insert_cp_vehicles(&cp1, v1);
  //print_vehicle(v1);
  //assert(l==2);
  //printf("%i\n", l);
  assert(vehicle_moves(cp1, &v2, false)==true);
  //print_vehicle(v2);
  vs1[0]=v1;  vs1[1]=v2;  vs1[2]=v3;
  insert_cp_vehicles(&cp1, vs1);
  //print_cp(cp1);
  assert(vehicle_moves(list, &v2, false)==true);
  //print_vehicle(v2);
  vs1[0]=v1;  vs1[1]=v2;  vs1[2]=v3;
  insert_cp_vehicles(&cp1, vs1);
  //print_cp(cp1);
  assert(vehicle_moves(list, &v2, false)==true);
  //print_vehicle(vs1[1]);
  vs1[0]=v1;  vs1[1]=v2;  vs1[2]=v3;
  insert_cp_vehicles(&cp1, vs1);
  //print_cp(cp1);
  assert(v2.exited==true);
  //print_cp(cp1);

  assert(vehicle_moves(list, &v3, false)==true);
  //print_vehicle(v3);
  vs1[0]=v1;  vs1[1]=v2;  vs1[2]=v3;
  insert_cp_vehicles(&cp1, vs1);
  //print_cp(cp1);
  assert(vehicle_moves(list, &v3, false)==true);
  //print_vehicle(v3);
  vs1[0]=v1;  vs1[1]=v2;  vs1[2]=v3;
  insert_cp_vehicles(&cp1, vs1);
  //print_cp(cp1);
  assert(v3.exited==true);
  //print_cp(cp1);
}
*/

void test_children_gen(void)
{
  char arr1[MAX_DIM][MAX_DIM]={"#.####",".BBB.#", "#A...#", "#A...#", "#A...#", "######"};
  car_park cp1;
  cp1.width=6;
  cp1.height=6;
  copy_layout(arr1, &cp1);
  vehicle vs1[MAX_NO];
  log_all_vehicles(&cp1, vs1);
  //print_cp(cp1);
  //printf("%i\n", cp1.numOvs);
  //assert(cp1.numOvs==2);
  //print_cp(cp1);
  state s;
  s.cp=cp1;
  state list[POS_ENTRYS];
  clear_list(list);
  list[0]=s;
  int l=1;
  assert(children_moves(cp1, list, 0, &l)==false);
  int l0=0;
  run_through_parents(cp1, list, &l0);

}
