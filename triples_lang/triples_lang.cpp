/*
 CPP Triples-Lang 

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 g++ -o triples_lang triples_lang.cpp
 ./triples_lang 

*/


#include <cstdio>
#include <iostream>
#include <numeric> 
#include <set>

#include <map>
#include "struct.h"  
#include <stdexcept> 
#include "trpl_graph.cpp"  

#include "io.c"    
#include "ml/dec_tree.cpp"    

using namespace std;

int choice, keyId ,  valueID ;
string key , value; 



Node* LoadedDecTreeModel;
  

HashTableProp archive_db  ;
HashTableEdge archive_edges  ;    
 
HashTableProp    signals_db  ;
HashTableSig     signals_flt_vect  ;
HashTableSigStr  signals_str_vect  ; 

HashTableProp svo_db  ; 
HashTableAdjacency svo_links  ;    
HashTableAdjacency s_v  ;    
HashTableAdjacency v_o  ;    
HashTableAdjacency o_s  ;    
HashTableAdjacency s_o  ;    
HashTableAdjacency v_s  ;    
HashTableAdjacency o_v  ; 
/**/

  
const int MAX_CMD_LINES = 30;
const int MAX_VAR_NAME  = 6; 
const int MAX_BLOCKS    = 2;
const int MAX_CMDS      = 4;   
const int MAX_ROUTINES  = 3;
const int MAX_F_CMDS    = 8; 
const int MAX_MEM       = 10; 

char ROUTINESSNAMES[MAX_ROUTINES][MAX_VAR_NAME] ;
struct triplesspace::cmdsSVO BLOCKS[MAX_BLOCKS][MAX_CMDS];
struct triplesspace::cmdsSVO ROUTINES[MAX_ROUTINES][MAX_F_CMDS] ;

char MEMSTR[MAX_MEM][MAX_CMD_LINES] ;
char MEMSTRNAME[MAX_MEM][MAX_VAR_NAME] ; 

float MEMFLT[MAX_MEM] ;
char MEMFLTNAME[MAX_MEM][MAX_VAR_NAME] ; 

char current_name[ MAX_VAR_NAME] ; 
   
int current_blocks   = -1;
int current_str_mem  = -1;
int current_flt_mem  = -1;
int loaded_routine   = -1;



void initRoutine(int routineId) {
  int _max = MAX_CMDS - 1;
  for (int i = 0; i < _max; i++) { 
       strcpy(ROUTINES[routineId][i].s, "");
       strcpy(ROUTINES[routineId][i].o, "");
       strcpy(ROUTINES[routineId][i].v, ""); 
  }
}

void initBlock(int blockId) {
  int _max = MAX_CMDS - 1;
  for (int i = 0; i < _max; i++) { 
       strcpy(BLOCKS[blockId][i].s, "");
       strcpy(BLOCKS[blockId][i].o, "");
       strcpy(BLOCKS[blockId][i].v, "");
  //  BLOCKS[block][i].v[0] =  {0};
  //  BLOCKS[block][i].s[0] =  {0};
    //BLOCKS[block][i].o[0] =  {0};
  }
}

void setup() {

  // Serial.begin(115200);
  // Serial.println("RF.Triples-Lang");  
  int _max = MAX_BLOCKS - 1;
  for (int i = 0; i < _max; i++) {
    initBlock(i);
  }  

  int _max_r = MAX_ROUTINES - 1;
  for (int i = 0; i < _max_r; i++) {
    initRoutine(i);
  }  
} 

int nextBlock() {

  int _max = MAX_BLOCKS - 1;
  for (int i = 0; i < _max; i++) {
    if (strcmp(BLOCKS[i][0].s, "")  == 0)  {  
   // if (BLOCKS[i][0].s[0] == '\0') {
      return i;
    }
  }
  return 0;
}


int nextCmd(int block_id) {

  int _max = MAX_CMDS;  
  for (int i = 0; i < _max; i++) {  
    if (strcmp( BLOCKS[block_id][i].v , "")  == 0) {   
      return i;
      } 
  } 

  return 0;
 }


int nextRoutine() {

  int _max = MAX_ROUTINES - 1;
  for (int i = 0; i < _max; i++) {
    if (strcmp(ROUTINES[i][0].s, "")  == 0)  {  
   // if (FUNCTS[i][0].v[0] == '\0') {
      return i;
    }
  }
  return 0;
}

int getRoutineID(char name[] ) {
 
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) {  
      if ( strcmp(ROUTINESSNAMES[i], name)  ==0 ) {
        return  i; 
    }
  }
  return  -1;
}  

int getMemoryID(char name[]) {
 
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) { 
      if  (strcmp(MEMSTRNAME[i], name)  == 0 ) {
 
        return  i; 
    }
  }
  return  -1;
}  

char* getMemory(char name[]) { 
  int _max = MAX_MEM -1; 
 
  for (int i = 0; i < _max; i++) {  
      if (strcmp(MEMSTRNAME[i], name)  ==0 ) {   
         return MEMSTR[i]; 
    }
  }
  char* res;
  return res;
}  
char setMemory(char name[], char value[]  ) { 
  
  int _id = getMemoryID(name);
  if (_id == -1)
  {  
      current_str_mem ++ ;
      _id = current_str_mem;
  }  
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) {  
 
      MEMSTRNAME[_id][i] = name[i];
      MEMSTR[_id][i] = value[i];
  }  
  return ' ';
}  

int getFltMemoryID(char name[]) {
 
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) { 
      if  (strcmp(MEMFLTNAME[i], name)  == 0 ) { 
        return  i; 
    }
  }
  return  -1;
}  

float getFltMemory(char name[]) {  
  int _id = getFltMemoryID(name);
  if (_id != -1)
  {   
         return MEMFLT[_id];  
  }
  return -1;
}  
char setFltMemory(char name[], float value   ) { 
  
  int _id = getFltMemoryID(name);
  if (_id == -1)
  {  
      current_flt_mem ++ ;
      _id = current_flt_mem;
  }  
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) {   
      MEMFLTNAME[_id][i] = name[i];
  }  

   MEMFLT[_id] = value;
  return ' ';
}  
 
void copy(char source[], char target[]  ) { 
   
  int _max = strlen(source) ;
  for (int i = 0; i < _max; i++) {     
      target[i] = source[i];  
  }   
 
 
}  

char clearMemory( ) {
 
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) { 
      MEMSTRNAME[i][0]= {0};
      MEMSTR[i][0] = {0}; 
   }
  current_str_mem = -1;
  current_flt_mem = -1;
  return ' ';
}  


struct triplesspace::SVO runIfBlock(int blockId) {

  struct triplesspace::SVO output;
  //strcpy(output.s, "");  
 // strcpy(output.v, "");   
  int _max = MAX_CMDS - 1;
  int run = 1; 

  for (int i = 0; i < _max; i++) { 
    if ( (strcmp( BLOCKS[blockId][i].v, " ")  != 0) && (strcmp(BLOCKS[blockId][i].v , "")  != 0) && run == 1) { 
        if (i == 0) {
    
          char*  v =  BLOCKS[blockId][i].v; 
          char*  s =  BLOCKS[blockId][i].s;   
          char*  o =  BLOCKS[blockId][i].o ;   
   
          struct triplesspace::SVO condition = core(v,s,o); 
          
          if (condition.sigs.front() == 0) { 
            run = 0;   
          }
        } else { 
          char*  v = BLOCKS[blockId][i].v ;  
          char*  s = BLOCKS[blockId][i].s;  
          char*  o = BLOCKS[blockId][i].o;      
          output = core(v,s,o);    
        } 
    }
  }
  return output;
}
 

struct triplesspace::SVO execute( char* o) {
  struct triplesspace::SVO output;
  //strcpy(output.s, ""); 
 // strcpy(output.v, "");  

  int _max = MAX_CMDS - 1;
  int run = 1;
  int routine_id = getRoutineID(o);     
  for (int i = 0; i < _max; i++) {
 

    if ( (strcmp( ROUTINES[routine_id][i].v , " ")  != 0) &&   (strcmp( ROUTINES[routine_id][i].v , "")  != 0) && run == 1) { 
          char*  v =  ROUTINES[routine_id][i].v; 
          char*  s =  ROUTINES[routine_id][i].s;   
          char*  o =  ROUTINES[routine_id][i].o ;     
           output = core(v , s, o);      
      }
    }  
  return output;
}


struct triplesspace::SVO dispatcher(char *v, char *s, char *o) {
 
  struct triplesspace::SVO output; 
    

  if (current_blocks != -1) {
 
     if (strcmp(v, "flow")  == 0  && strcmp(s, "if")  == 0  &&  strcmp(o, "end")  == 0) { 
 
      // // Serial.println(current_blocks);
      if (current_blocks <= 0) {  
         output =  runIfBlock(current_blocks);
         BLOCKS[current_blocks][0].s[0] = {0};
         BLOCKS[current_blocks][0].v[0] = {0};
         BLOCKS[current_blocks][0].o[0] = {0};
         current_blocks = current_blocks - 1;  
         return  output;
       }
      else
        { 
         int parent_block = current_blocks - 1; 
         int next_cnd = nextCmd(parent_block);
         
         int _max = MAX_MEM -1;
         for (int i = 0; i < _max; i++) { 
            int j = next_cnd + i;
            copy(BLOCKS[current_blocks][i].v, BLOCKS[parent_block][j].v );
            copy(BLOCKS[current_blocks][i].s, BLOCKS[parent_block][j].s );
            copy(BLOCKS[current_blocks][i].o, BLOCKS[parent_block][j].o ) ;
         }
         //BLOCKS[current_blocks][0].s[0] = {0};
         //BLOCKS[current_blocks][0].v[0] = {0};
         //BLOCKS[current_blocks][0].o[0] = {0};

         initBlock(current_blocks) ;
         current_blocks = current_blocks - 1; 

         return  output;


        }
    } else if (strcmp(v, "routine")  == 0  && strcmp(s, "create")  == 0  &&  strcmp(o, "end")  == 0) {  

        int cmds_cnt = MAX_CMDS -1;
        int routine_id = nextRoutine();
 
 
         int _max = MAX_VAR_NAME -1;
         for (int i = 0; i < _max; i++) {   
              ROUTINESSNAMES[routine_id][i] = current_name[i]; 
           }  
  
        for (int i = 0; i <= cmds_cnt; i++) {     
          copy(BLOCKS[current_blocks][i].v, ROUTINES[routine_id][i].v );
          copy(BLOCKS[current_blocks][i].s, ROUTINES[routine_id][i].s );
          copy(BLOCKS[current_blocks][i].o, ROUTINES[routine_id][i].o ); 
         }   
 

         initBlock(current_blocks) ;
         current_blocks = current_blocks - 1;  
         strcpy(current_name, ""); 
         return output;

    } else {
      int current_code = nextCmd(current_blocks);    
      copy( v, BLOCKS[current_blocks][current_code].v );
      copy( o, BLOCKS[current_blocks][current_code].o );
      copy( s, BLOCKS[current_blocks][current_code].s ); 
    //  // Serial.println (current_blocks); 
      return output;
    }

  } else {
     // try {
        return core(v, s, o);
     //   }
    // catch (const std::runtime_error& e) {
      //  std::cerr << "Caught runtime_error: " << e.what() << std::endl;
        
      //   return  output; 
     //  } 
    }
  
}  
   

/* 
// TODO- short hand to min char size in memeory
// replace all svo that are directives to  pointer in memory when storig routine and flow
 
int (*getOperation(char op))(int, int) {

   if (op == 'add') {
       return &add;  
   } else if (op == 'subract') {
       return &subtract;  
   }
   } else if (op == 'subract') {
       return &subtract;  
   }
   return NULL; // Return NULL if no match
}
int main() { 
   int (*operation)(int, int) = getOperation('+');
   if (operation) {
       printf("Result: %d\n", operation(5, 3));  
   } else {
       printf("Invalid operation\n");
   }
   return 0;
 
//date time funct
  char date[] = "2026,08,05";  
  char *token;   
  token = strtok(date, ",");  
  int year = atoi(token);   
}*/

struct triplesspace::SVO core(char *v , char *s , char *o ) {
 
  struct triplesspace::SVO output;
 
 

  if (strcmp(v, "help")   == 0  ) {  
      // Serial.println("set,get,echo,add,subtract,divide,multiple,increment,decrement,more,less,equal,different,flow,routine,help");
  } 
  else if (strcmp(v, "annotation") ==0){
      
  }
  else if ( strcmp(v, "echo")   == 0) { 
      output.recs.push_back(s);   

  } else if (strcmp(v, "flow")   == 0  ) {
    if ( strcmp(s, "if")   == 0  ) { 
      if (strcmp(o, "start")   == 0  ) {
          current_blocks = nextBlock();  
       }
     }
   }
 
  else if (strcmp(v, "routine")   == 0  ) { 
    if (strcmp(s, "create")   == 0  ) {
  
         current_blocks = nextBlock();   
         strcpy(current_name, o);     
    }
    else if (strcmp(s, "execute")   == 0  ) { 
         return  execute(  o);
    }  

  } else if (strcmp(v, "express")   == 0 ) {
    if (strcmp(v, "lights")   == 0) {
      // PLace Holder  
    }
  } else if (strcmp(v, "movement")   == 0 ) {
    if (strcmp(s, "torso")   == 0 ) {
      // Place Holder
    } else if (strcmp(s, "legs")   == 0) {
      //Place Holder
    }
  } else if (strcmp(v, "set")   == 0 ){ 
   
    char t =  setMemory(s,  o);  
 
  } else if (strcmp(v, "get")  ==0){
 
 
    char* _t = getMemory(s);    
    char res[MAX_MEM-1];
    strcpy(res, _t);  
    output.recs.push_back(res); 

  } else if (strcmp(v, "put")   == 0 ){ 
  
    float fo =  atof(o);  
    float t =  setFltMemory(s,  fo);
 
  } else if (strcmp(v, "pull")  ==0){
   
    float val =  getFltMemory(s);    
    output.sigs.push_back(val); 

  } else if (strcmp(v, "clear")  ==0){
    clearMemory();  
    output.sigs.push_back(1); 
    
 
  }   else if ( strcmp(v, "upper")   == 0) { 
     
      int _max = strlen(s) -1 ; 
      char out[_max] ;
      for (int i = 0; i <= _max; i++) {     
         out[i] =  toupper(s[i]); 
      }
      output.recs.push_back(out);  

  }   else if ( strcmp(v, "lower")   == 0) { 
 
      int _max = strlen(s) -1 ; 
      char out[_max] ;
      for (int i = 0; i <= _max; i++) {     
         out[i] =  tolower(s[i]); 
      }
      output.recs.push_back(out);  

  }   else if ( strcmp(v, "replace")   == 0) { 

       char* v1   =  getMemory(s);
       char* _frm = std::strtok(o  , "->");  
       char* _to  = std::strtok(NULL,"->");  

       char frm[6];  
       strcpy(frm, _frm);

       char to[6];  
       strcpy(to, _to);

       //replace_char(v1, frm, to); 
       output.recs.push_back(v1);  

  }  else if (strcmp(v, "split")  ==0){ 

       char* v1   =  getMemory(s); 
       char* fin  = std::strtok(v1  , o); 

       output.recs.push_back(fin);  
       fin = std::strtok(NULL, " ");
       while (fin != NULL)
       { 
           output.recs.push_back(fin);  
           fin = std::strtok(NULL, " ");
       }  

  }   else if ( strcmp(v, "join")   == 0) { 
       char* v1 =  getMemory(s);
       char* v2 =  getMemory(o);

       int _max1 = strlen(s) ; 
       int _max2 = strlen(o) ; 
       int _max = _max1 + _max2;
       char out[_max];

       for (int i = 0; i <= _max1; i++) {     
         out[i] =  s[i]; 
       }

       for (int i = 0; i <= _max2; i++) {     
         out[i+_max1] =  o[i]; 
       }

       output.recs.push_back(out);  

  }  else if ( strcmp(v, "element")   == 0) { 
       char* v1 =  getMemory(s); 
       char* fin = std::strtok(v1 , "|");  
       output.recs.push_back(fin);    

  }  else if (strcmp(v, "add")  ==0){
  
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  
    float value = f1 + f2;    
    output.sigs.push_back(value);  
    // dtostrf(value, 4, 3, output.o);  


  } else if (strcmp(v, "increment")  ==0)  {
    char* v1 =  getMemory(s); 
    float f1 =  atof(v1);   
    float value = f1 + 1;
    output.sigs.push_back(value);  
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "decrement")  ==0)  {
    char* v1 =  getMemory(s); 
    float f1 =  atof(v1);   
    float value = f1 - 1; 
    output.sigs.push_back(value);   
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "subtract")  ==0)  {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  
    float value = f1 - f2;
    output.sigs.push_back(value);  
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "divide")  ==0)   {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);   
    float value = f1 / f2;
    output.sigs.push_back(value);    
    // dtostrf(value, 4, 3, output.o);  

  } else if  (strcmp(v, "multiply")  ==0)  {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);   
    float value = f1 * f2;
    output.sigs.push_back(value);  
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "more")  ==0){
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 > f2) { 
      output.sigs.push_back(1);    
    } else {     
      output.sigs.push_back(0);    
    }

  } else if (strcmp(v, "less")  ==0)   {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 < f2) { 
      output.sigs.push_back(1);    
    } else {     
      output.sigs.push_back(0);    
    }

  } else if (strcmp(v, "equal")  ==0)  {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 == f2) {
      output.sigs.push_back(1);    
    } else {     
      output.sigs.push_back(0);    
    }

  } else if (strcmp(v, "different")  ==0)   {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 != f2) {
      output.sigs.push_back(1);    
    } else {     
      output.sigs.push_back(0);    
    }

   } else if (strcmp(v, "encoding")  ==0)     {

      if (strcmp(s, "bin")  ==0)   {
      
     } else if (strcmp(s, "ohe")  == 0)   {
 
     } else if (strcmp(s, "fuzzy")  == 0)   {

     } else if (strcmp(s, "image")  == 0)   {

     } else if (strcmp(s, "sound")  == 0)   {

     }
     
   } else if (strcmp(v, "normalizing") ==0) {

     if (strcmp(s, "extremes")  ==0)   {
      
     } else if (strcmp(s, "euclidian")  == 0)   {
 
     } else if (strcmp(s, "fuzzy")  == 0)   {

     }

   } else if (strcmp(v, "learn")  ==0)   {


     if (strcmp(s, "display")  ==0)   {
 
        int trees = 0;
        string model = dectree_repr(LoadedDecTreeModel); 
        ofstream outFile("tree.txt"); 
         
        outFile << model;  
        outFile.close();

     } else if (strcmp(s, "save")  ==0)   {
  
        string FileName = o;
      //  bool res = dectree_save(FileName, LoadedDecTreeModel);  

        int trees = 0;
        string model = dectree_repr(LoadedDecTreeModel); 
        ofstream outFile("tree.txt"); 
         
        outFile << model;  
        outFile.close();



     } else if (strcmp(s, "load")  ==0)   {

        string FileName = o;

       // Node* LoadedDecTreeModel;
        LoadedDecTreeModel= load_tree("tree.txt"); 
        

     } else if (strcmp(s, "dectree")  ==0)   {

        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");   


        vector<map<string, string>> data;
        vector<string> labels;  
        set<string> features;
        std::map<string, string> vecMap;
        
        char *buffer ;
        buffer = strtok(_features, "|");    
        int keyId = signals_db.exists(buffer)  ; 

        vector<string>  vals = signals_str_vect.retrieve(keyId) ;  
        for (auto& val:vals) {
            vecMap[buffer] = val;
            data.emplace_back(vecMap); 
         } 
 
        features.insert(buffer) ;  
        buffer = strtok(NULL,"|");

        //stack<vector<int>> myStack; // stack of vectors
        int i_rec = -1;
        while (buffer !=NULL)
           {    
             keyId = signals_db.exists(buffer)  ;  
             vals = signals_str_vect.retrieve(keyId) ; 
             i_rec = 0;
             for (auto& val:vals) {
                vecMap[buffer] = val;
               // data..insert(data.begin() + i_rec, vecMap);
                data[i_rec].insert({buffer, val});
                i_rec++;
             }
  
             features.insert(buffer) ;  
             buffer = strtok(NULL,"|");
           
           }
 

        keyId = signals_db.exists(_label)  ;   
        labels = signals_str_vect.retrieve(keyId)  ;    
        LoadedDecTreeModel = dectree_fit(data, labels, features);  

     } else if (strcmp(s, "decent")  == 0)   {


     } else if (strcmp(s, "neural")  == 0)   {

 
     } else if (strcmp(s, "deep")  == 0)   {


     }
     
   } else if (strcmp(v, "reason")  ==0)   {

     if (strcmp(s, "present")  ==0)   {

     } else if (strcmp(s, "future")  == 0)   {

     } else if (strcmp(s, "past")  == 0)   {

     } else if (strcmp(s, "dectree")  == 0)   {  

         std::map<string, string> data; 
         FILE *filePtr;
         cout << "file: " << o  << endl;
         filePtr = fopen(o, "r"); 
         char currentline[1000];
         int lcnt = 0;
         const char *delim = "," ;
         char *buffer ;

         int cols = 0;
         string header[100];
         string colname ;
         string soutput;

         while (fgets(currentline , 1000, filePtr))
          {      
            /*Increment line count*/   
            size_t len = strlen(currentline);
            if (len > 0 && currentline[len - 1] == '\n') {
                      currentline[len - 1] = '\0';
            }
           if (lcnt  ==0)
           {
                buffer = strtok(currentline, delim);     
                header[cols] = buffer; 
                cols ++;

                buffer = strtok(NULL, delim);
                while (buffer !=NULL)
                 {      
                    header[cols] = buffer;   
                    cols ++;  
                    buffer = strtok(NULL,delim);
                 } 
           }
           else
           {
              cols = 0;
              buffer = strtok(currentline, delim); 
              colname = header[cols];     
              keyId = signals_db.exists(colname)  ;   
              data.insert({colname,buffer});  
              buffer = strtok(NULL,delim);
              while (buffer !=NULL)
                 {   
 
                    cols ++;
                    colname = header[cols];  
                    data.insert({colname,buffer});     
                    buffer = strtok(NULL,delim); 
                 }  

           }

           string results = dectree_inference(LoadedDecTreeModel, data);  
           soutput.append(results);
           soutput.append("|");
           cout << results  << endl;  
           data.clear();
           lcnt++;

          }
         
      output.sigs.push_back(lcnt);      

     }


   } else if (strcmp(v, "plans")  ==0)   {
      // Rules engine 
      //define 
      //layout
      //alter
      //execute
      //activate
      //implement
    

   } else if (strcmp(v, "file")  == 0)   {

    if (strcmp(s, "read")  ==0)   {

        FILE *filePtr;      
        filePtr = fopen(o, "r");  
        select(filePtr);

     } else if (strcmp(s, "sort")  == 0)   {

        FILE *filePtr;     
        filePtr = fopen(o, "r");   
        sort(filePtr); 

     } else if (strcmp(s, "delete")  == 0)   {

        FILE *filePtr;     
        filePtr = fopen(o, "r");     
      //  remove_rec(filePtr, '1'); 

     }

   } else if (strcmp(v, "archive")  == 0)   {

      /*update remove query access, define */
 
      if (strcmp(s, "add")  ==0)   {

          char* _frm = std::strtok(o  , "->");  
          char* _to  = std::strtok(NULL,"->");  
          char* key   =  getMemory(_frm);
          char* value =  getMemory(_to);  

          keyId   = archive_edges.nextID();
          valueID = keyId + 1;
          archive_edges.insert(keyId, valueID);
          archive_edges.insert(valueID, keyId);
          archive_db.insert(keyId, key);
          archive_db.insert(valueID, value); 
 
     } else if (strcmp(s, "exists")  == 0)   {
          std::string key =  o;  
          int keyId =  archive_db.exists(key);   
           output.sigs.push_back(keyId);      

     } else if (strcmp(s, "retrieve")  == 0)   { 
          int keyId =  atoi(o);   
          std::string resp  = archive_db.retrieve(keyId);   
          char *out;
          strcpy(out, resp.c_str());
          output.recs.push_back( out);      
 
      } else if (strcmp(s, "definition")  == 0)   {   
          std::string key =  o;  
          int keyId =  archive_db.exists(key) ;   
          int respId = archive_edges.retrieve(keyId) ;  
          std::string resp = archive_db.retrieve(respId);   
          char *out;
          strcpy(out, resp.c_str());
          output.recs.push_back( out);      

      } else if (strcmp(s, "similar")  == 0)   { 
 
          std::string key =  o;   
          int keyId =  archive_db.similar(key) ;    
          int respId = archive_edges.retrieve(keyId) ;   
          std::string resp = archive_db.retrieve(respId);  
          char *out;
          strcpy(out, resp.c_str());
          output.recs.push_back( out);      

      }else if (strcmp(s, "linked")  == 0)   { 
          std::string key =  o;  
          int keyId =  archive_db.similar(key) ;  
          int respId = archive_edges.retrieve(keyId) ;  
      }
 
    } else if (strcmp(v, "records")  == 0)   {

      int keyId  ;    
      if (strcmp(s, "add")  ==0)   {

          char* _frm = std::strtok(o  , "->");  
          char* _to  = std::strtok(NULL,"->");    
 
          keyId   = signals_db.nextID();   
          signals_db.insert(keyId,  _frm )  ;
          signals_str_vect.insert(keyId, _to );    
       

      } else if (strcmp(s, "load")  == 0)  { 

         FILE *filePtr;
         cout << "file: " << o  << endl;
         filePtr = fopen(o, "r"); 
         char currentline[1000];
         int lcnt = 0;
         const char *delim = "," ;
         char *buffer ;

         int cols = 0;
         string header[100];
         string colname ;

         while (fgets(currentline , 1000, filePtr))
          {      
            /*Increment line count*/   
            size_t len = strlen(currentline);
            if (len > 0 && currentline[len - 1] == '\n') {
                currentline[len - 1] = '\0';
            }
           if (lcnt  ==0)
           {
                buffer = strtok(currentline, delim);    
               // cout << buffer  << endl;

                keyId   = signals_db.nextID();   
                signals_db.insert(keyId,  buffer )  ;
                header[cols] = buffer; 
                cols ++;

                buffer = strtok(NULL, delim);
                while (buffer !=NULL)
                 {      
                    header[cols] = buffer;    
                    keyId   = signals_db.nextID();   
                    signals_db.insert(keyId,  buffer )  ;
                    cols ++;  
                    buffer = strtok(NULL,delim);
                 } 
           }
           else
           {
              cols = 0;
              buffer = strtok(currentline, delim); 
              colname = header[cols];    
              keyId = signals_db.exists(colname)  ;  
              signals_str_vect.insert(keyId, buffer );    

              buffer = strtok(NULL,delim); 
              while (buffer !=NULL)
                 {   
 
                    cols ++;
                    colname = header[cols];
                    keyId = signals_db.exists(colname)  ;  
                    signals_str_vect.insert(keyId, buffer );      
                    buffer = strtok(NULL,delim); 
                 }  

           }
           lcnt++;

          }
          
       output.sigs.push_back( keyId);    
     //  strcpy(output.o, "1");

 
     } else if (strcmp(s, "exists")  == 0)   {  

          keyId = signals_db.exists(o)  ;  
        //  string key  =  signals_flt_vect.exists(o);  
        //   snprintf(output.o, sizeof(output.o), "%s", key.c_str());    
          output.sigs.push_back( keyId);    

     } else if (strcmp(s, "last")  == 0)   {  
 
             keyId = signals_db.exists(o)  ;  
             vector<string> resp  = signals_str_vect.retrieve(keyId);  
             string val =  resp.back(); 
             char *out;
             strcpy(out, val.c_str());
             output.recs.push_back( out);   

     } else if (strcmp(s, "first")  == 0)    { 
          

             keyId = signals_db.exists(o)  ;  
             vector<string> resp  = signals_str_vect.retrieve(keyId);  
             string val =  resp.back();  

             char *out;
             strcpy(out, val.c_str());
             output.recs.push_back( out);      
             

     } else if (strcmp(s, "distance")  == 0)   { 
          int keyId =  atoi(o);   
          // int index = std::distance(v.begin(), it) 
          // snprintf(output.o, sizeof(output.o), "%f", resp.back());   
    
     } else if (strcmp(s, "count")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);    
          output.sigs.push_back( resp.size());    
 
 
      } else if (strcmp(s, "fusison")  == 0)   {   
           //should be nested array for TS

      } else if (strcmp(s, "compress")  == 0)   {   
           //should be nested array for TS
      } else if (strcmp(s, "similar")  == 0)   {  

      }else if (strcmp(s, "linked")  == 0)   {  

      }


    } else if (strcmp(v, "signals")  == 0)   {

      int keyId  ;    
      if (strcmp(s, "add")  ==0)    {

          char* _frm = std::strtok(o  , "->");  
          char* _to  = std::strtok(NULL,"->");    
 
          keyId   = signals_db.nextID();   
          signals_db.insert(keyId,  _frm )  ;
           float fvalue =  atof(_to);    

      } else if (strcmp(s, "load")  == 0)   { 

         FILE *filePtr;
         cout << "file: " << o  << endl;
         filePtr = fopen(o, "r"); 
         char currentline[1000];
         int lcnt = 0;
         const char *delim = "," ;
         char *buffer ;

         int cols = 0;
         string header[100];
         string colname ;

         while (fgets(currentline , 1000, filePtr))
          {      
            /*Increment line count*/   
            size_t len = strlen(currentline);
            if (len > 0 && currentline[len - 1] == '\n') {
                      currentline[len - 1] = '\0';
            }
           if (lcnt  ==0)
           {
                buffer = strtok(currentline, delim);   

               // cout << buffer  << endl;

                keyId   = signals_db.nextID();   
                signals_db.insert(keyId,  buffer )  ;
                header[cols] = buffer; 
                cols ++;

                buffer = strtok(NULL, delim);
                while (buffer !=NULL)
                 {      
                    header[cols] = buffer;  

                    keyId   = signals_db.nextID();   
                    signals_db.insert(keyId,  buffer )  ;
                    cols ++;  
                    buffer = strtok(NULL,delim);
                 } 
           }
           else
           {
             cols = 0;
             buffer = strtok(currentline, delim); 
             colname = header[cols];    
             float fvalue =  atof(buffer);   
             keyId = signals_db.exists(colname)  ;  
             signals_flt_vect.insert(keyId, fvalue );   

              buffer = strtok(NULL,delim);
              cout << buffer  << endl;
              while (buffer !=NULL)
                 {   
 
                    cols ++;
                    colname = header[cols]; 
                    float fvalue =  atof(buffer);   
                    keyId = signals_db.exists(colname)  ;  
                    signals_flt_vect.insert(keyId, fvalue );   
                    buffer = strtok(NULL,delim); 
                 }  

           }
           lcnt++;

          }
         
           
      output.sigs.push_back(lcnt);       
     //  strcpy(output.o, "1");

 
     } else if (strcmp(s, "exists")  == 0)   {  

          keyId = signals_db.exists(o)  ;     
          output.sigs.push_back(keyId);    

     } else if ((strcmp(s, "last")  == 0)   )  {  
 
             keyId = signals_db.exists(o)  ;  
             vector<float> resp  = signals_flt_vect.retrieve(keyId);    
             output.sigs.push_back(resp.back());    

     } else if ((strcmp(s, "first")  == 0)    )  { 
         
 
             keyId = signals_db.exists(o)  ;  
             vector<float> resp  = signals_flt_vect.retrieve(keyId);     
             output.sigs.push_back(resp.front());     
          
     } else if (strcmp(s, "sum")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);   
          int sum = std::accumulate(resp.begin(), resp.end(), 0); 
          output.sigs.push_back(sum);     
 
     } else if (strcmp(s, "mean")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);    
          int sum = std::accumulate(resp.begin(), resp.end(), 0);

          float mean =  sum / resp.size();
          output.sigs.push_back(mean);     
 
 
     } else if (strcmp(s, "minimum")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);  
          auto it = std::min_element(resp.begin(), resp.end()); 
          output.sigs.push_back(*it);      

     } else if (strcmp(s, "maximum")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);   
          auto it = std::max_element(resp.begin(), resp.end());   
          output.sigs.push_back(*it);      

     } else if (strcmp(s, "distance")  == 0)   { 
          int keyId =  atoi(o);   
          // int index = std::distance(v.begin(), it) 
          // snprintf(output.o, sizeof(output.o), "%f", resp.back());   
  

     } else if (strcmp(s, "varience")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);    
          output.sigs.push_back(resp.back());      

     } else if (strcmp(s, "count")  == 0)   {  
          keyId = signals_db.exists(o)  ;  
          vector<float> resp  = signals_flt_vect.retrieve(keyId);    
          output.sigs.push_back(resp.size());      
 
 
      } else if (strcmp(s, "fusison")  == 0)   {   
           //should be nested array for TS

      } else if (strcmp(s, "compress")  == 0)   {   
           //should be nested array for TS
      } else if (strcmp(s, "similar")  == 0)   {  

      }else if (strcmp(s, "linked")  == 0)   {  

      }
 
     
 
     
    } else if (strcmp(v, "knowledge")  == 0)   {
 
      if (strcmp(s, "add")  ==0)   {

           // char* key   =  getMemory(_frm);
           // char* value =  getMemory(_to);  

           string _svo  =std::string(o  );  

           char* _s  = std::strtok(o  , "|");  
           char* _v  = std::strtok(NULL,"|");  
           char* _o  = std::strtok(NULL,"|");  

           char* _output  = std::strtok(NULL,"->");  

           int _s_id;
           int _v_id;
           int _o_id;
           int _link_id;

           _s_id  = svo_db.nextID();
           _v_id  = _s_id +1;
           _o_id  = _v_id +1;
           _link_id  = _o_id +1;
    
           svo_db.insert(_s_id, _s);   
           svo_db.insert(_v_id, _v); 
           svo_db.insert(_o_id, _o);    
           svo_db.insert(_link_id, _svo);    

           s_v.insert(_s_id, _v_id); 
           v_o.insert(_v_id, _o_id);      
           o_s.insert(_o_id, _s_id);      
           s_o.insert(_s_id, _o_id);     
           v_s.insert(_v_id, _s_id);    
           o_v.insert(_o_id, _v_id);   

      } else if (strcmp(s, "remove")  ==0)   {

      } else if (strcmp(s, "exists")  ==0)   {
 
           std::string in_o  = o;   
           int _s_id;  
           _s_id = svo_db.exists(in_o);  
          output.sigs.push_back(_s_id);      


      } else if (strcmp(s, "link")  ==0)   {

           char* _to    = std::strtok(o  , "->");  
           char* _from  = std::strtok(NULL,"->");  
           int i_to    =  atoi(_to);  
           int i_from  =  atoi(_from);  
           svo_links.insert(i_to, i_from);
           svo_links.insert(i_from, i_to); 
           output.sigs.push_back(1);      

 
      } else if (strcmp(s, "similar")  == 0)   { 
        ///all entries similarto to svo
 
      } else if (strcmp(s, "closest")  == 0)   { 
          // one object bext match 

           char* _s  = std::strtok(o  , "|");  
           char* _v  = std::strtok(NULL,"|");  
           char* _o  = std::strtok(NULL,"|");  

           std::string in_s  =  _s;  
           std::string in_v  =  _v;  
           std::string in_o  =  _o;   
          // char* _output  = std::strtok(NULL,"->");  

           int _s_id;  
            try 
            {
              _s_id = svo_db.exists(in_s); 
            }
            catch (const std::runtime_error& e) {
 
                  std::cerr << "Error: " << e.what() << std::endl;
              
            }

            vector<int> _v_ids ;

            try 
            { 
                _v_ids = s_v.retrieve(_s_id); 
            }
            catch (const std::runtime_error& e) { 
                  std::cerr << "Error 2: " << e.what() << std::endl;
                 
            }



           int _v_id = _v_ids.front(); 

           string v = svo_db.retrieve(_v_id);
           if (in_v == v)
           {  
               vector<int> _o_ids;

               try {
                    _o_ids = v_o.retrieve(_v_id); 
                   }
                catch (const std::runtime_error& e) { 
 
                   std::cerr << "Error 3: " << e.what() << std::endl; 
               }

               int _o_id = _o_ids.front(); 
               string o = svo_db.retrieve(_o_id);
 
               char *out_s =  _s; //.c_str();
               output.recs.push_back(out_s);   

               char *out_v ;

               strcpy(out_v, v.c_str());
               output.recs.push_back(out_v); 

               char *out_o  ;
               strcpy(out_o, o.c_str());
               output.recs.push_back(out_o);  
 /*
 
               strcpy(output.s, in_s.c_str());
               strcpy(output.v, v.c_str());
               strcpy(output.o, o.c_str());   
 */
              }
          
      } else if (strcmp(s, "related")  == 0)   { 
         //one objevt lin to best match for svo
         // std::string key =  o;  
         // int keyId =  props.similar(key) ;  
         // int respId = edges.search(keyId) ; 
         // std::string resp = props.search(respId);     
         // strcpy(output.o, resp.c_str());   

      } else if (strcmp(s, "relatives")  == 0)   { 
         //alll object lin to best match for svo
         // std::string key =  o;  
         // int keyId =  props.similar(key) ;  
         // int respId = edges.search(keyId) ; 
         // std::string resp = props.search(respId);     
         // strcpy(output.o, resp.c_str());  
      }
 
    
     } 

  return output;
 
}

void replace_char(char *str, char find, char replace) {
    while (*str) {
        if (*str == find) {
            *str = replace;
        }
        str++;
    }
}
 
/*
  g++ -o triples_lang triples_lang.cpp
./triples_lang 2>err.log

put t 6.77;
pull t;

set r hi;
set p  meow;
archive add r->p;
archive definition hi;

signals add 1->9;
signals add 1->4;
signals count 1;
signals sum 1;
signals last 1;

knowledge add me|love|cat;
knowledge exists me|love|cat;
knowledge closest me|love|dog;
 extern "C" {
    Geek* Geek_new(){ return new Geek(); }
    void Geek_myFunction(Geek* geek){ geek -> myFunction(); }
}


*/
extern "C" { 
    void dispatch(char *v, char *s, char *o){  dispatcher(v,s,o); }
} 
int main(int argc, char* argv[]) {
    
 
    if (argc > 1)
    { 
        FILE *filePtr;     
        filePtr = stdin;  
        sort(filePtr);
        //select(filePtr);
    } 
    else
    {
    
    std::string input; 
    std::cout << "Triples-Lang (type 'help;' for commands, 'exit;' to quit)\n";
    while (true) {
        std::cout << "> "; // Prompt
        if (!std::getline(std::cin, input)) {
            std::cout << "\nEnd of input detected. Exiting.\n";
            break;
        }
 

        // Convert to lowercase for case-insensitive commands
        std::string cmd = input;
        //std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "exit;") {
            std::cout << "Goodbye!\n";
            break;
        } 
        else if (cmd == "help;") {
            std::cout << "Available commands:\n"
                      << "  help  - Show this help message\n"
                      << "  set,get,echo,add,subtract,divide,multiple,increment,decrement,more,less,equal,different,flow,routine \n"
                      << "  exit  - Quit the program\n";
        }  
        else if (cmd.empty()) {
            // Ignore empty input
            continue;
        } 
        else {
         
             char *s;   
             char *v;      
             char *o;    

             char* cstr = new char[cmd.length() + 1];
             strcpy(cstr, cmd.c_str());

             v = std::strtok(cstr, " ");  
             s = std::strtok(NULL, " ");  
             try
             {
                 o = std::strtok(NULL, " ");   
             }
             catch(const std::exception& e)
             { 
                o = new char[cmd.length() + 1];
             }
               
     
             replace_char(v,';', '\0'); 
             replace_char(s,';', '\0'); 
             try
             {
                 replace_char(o,';', '\0');  
             }
             catch(const std::exception& e)
             {  
                o = new char[cmd.length() + 1];
             }
               
             struct triplesspace::SVO results;

             try {
                  results =dispatcher(v,s,o); 
             }
             catch(const std::exception& e)
             {  
                  std::cout << "Error occured " << e.what() << std::endl;
             }
               

             // std::cout << results.o;
             for (auto i: results.sigs) {
              	std::cout << i << " ";
             }

             for (auto i: results.recs) {
              	std::cout << i << " ";
             }

             std::cout << std::endl;
        }
    }

    return 0;
  }
} 