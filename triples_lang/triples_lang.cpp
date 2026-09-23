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
#include "ml/dec_tree_continuious.cpp"  
#include "ml/nn.cpp"   
#include "ml/gd.cpp"     
#include "ml/numap.cpp"    
#include "ml/nhdbscan.cpp"    
 
#include "encoding/run_length_binary.cpp"     
#include "encoding/binning.cpp"    
 
#include "normalization/min_max.cpp"    
 
using namespace std;

int choice, keyId ,  valueID ;
string key , value; 
 
Node* LoadedDecTreeModel;
NeuralNetwork LoadedNeuralNetwork;
DecisionTreeContinuous LoadedDecTreeContModel;
Vector LoadedGradientDescent;
NUMAP LoadedUMap;
NHDBSCAN LoadedNHDBScan;

triplesspace::Situation CurrentSituation;
    
Archives archive;
Memory memory_sig_rec ;  
Graph  svo_graph(100000)  ;     

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

map<string ,string> cmdLookup;



std::vector<std::string> splitIgnoringQuotes(const std::string& text) {
    std::vector<std::string> tokens;
    bool inQuotes = false;
    std::string currentToken;

    for (char c : text) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ') {
            if (!inQuotes) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
            } else {
                currentToken += c;  
            }
        } else {
            if (!inQuotes) {
                currentToken += c;
            } else {
                currentToken += c;  
            }
        }
    }
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    return tokens;
}

void initCommands( ) 

{

    char *buffer ;
    FILE *filePtr;  
    char currentline[1000];  
    string sfile = "./docs/docs.txt";
    filePtr = fopen(sfile.c_str(), "r");  
    vector<string> doc;

    while (fgets(currentline , 1000, filePtr))
    {       
      doc = splitIgnoringQuotes(currentline);

      cmdLookup[doc[0]] =  doc[1] + " " + doc[2];  
      
    }

//   cmdLookup["routine"] = ""; 
//   cmdLookup["archive"] = "";
 //  cmdLookup["signals"] = "";
  // cmdLookup["knowledge"] = "";
 //  cmdLookup["learner"] = "";
 //  cmdLookup["reason"] = ""; 

}
 
string findCommand(string value)
{
   for (const auto &kv : cmdLookup) { 
   
       double sim = stringSimilarity(value, kv.first);
   
       if (sim > .5){
           return kv.first;
      }
     } 
  return " ";
}

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
 
size_t countCharOccurrences(const char* str, char target) {
    if (str == nullptr) {
        return 0; // Null pointer safety
    }

    size_t count = 0;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] == target) {
            ++count;
        }
    }
    return count;
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

         auto it = cmdLookup.find(s);
         
         if (it != cmdLookup.end()) { 
             output.recs.push_back(it->second);   
         }
         else
         {
             output.recs.push_back("Not found");   

         }
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


  } else if (strcmp(v, "situation")  ==0)   {


    if (strcmp(s, "react")  ==0)   {
         // situation react stimuli;å
        // CurrentSituation 
    }

  } else if (strcmp(v, "strategy")  ==0)   {

    if (strcmp(s, "identfy")  ==0)   {
    // strategy identify optional; 
    }

  } else if (strcmp(v, "response")  ==0)   {

    if (strcmp(s, "select")  ==0)   {
    
    }
   // response select vocal; 
   //response select movemenet;

   } else if (strcmp(v, "encoding")  ==0)     {

     if (strcmp(s, "bin")  ==0)   {
      
     } else if (strcmp(s, "ohe")  == 0)   {
 
     } else if (strcmp(s, "fuzzy")  == 0)   {

     } else if (strcmp(s, "image")  == 0)   {

     } else if (strcmp(s, "sound")  == 0)   {

     } else if (strcmp(s, "runlength")  == 0)   {
  
        vector<float>  vals =  memory_sig_rec.get_signals(o);
        std::vector<double> doubleVec(vals.begin(), vals.end());
        std::vector<std::pair<char, int>> res =  compressBinarySignal(doubleVec);
       // output.sigs = res;

     } 

   } else if (strcmp(v, "normalizing") ==0) {

     if (strcmp(s, "minmax")  ==0)   { 
 
        
        vector<float>  vals = memory_sig_rec.get_signals(o) ;   
        std::vector<double> doubleVec(vals.begin(), vals.end());
        MinMaxEnc mdl =  minMaxFit(doubleVec); 

        vector<double> res =  minMaxEncode(doubleVec, mdl);
        std::vector<float> fltVec(res.begin(), res.end());
        output.sigs = fltVec;
      
     } else if (strcmp(s, "euclidian")  == 0)   {
 
     } else if (strcmp(s, "fuzzy")  == 0)   {

     }

   } else if (strcmp(v, "learner")  ==0)   {


     if (strcmp(s, "display")  ==0)   {
 
        int trees = 0;
        string model = dectree_repr(LoadedDecTreeModel); 
        ofstream outFile("tree.txt"); 
         
        outFile << model;  
        outFile.close();

     } else if (strcmp(s, "save")  ==0)   {
   
        
        char* mldType = std::strtok(o  , "->");  
        char* _file  = std::strtok(NULL,"->");  
        string FileName = _file;

        if (strcmp(mldType, "tree")  ==0)   {   

            string model = dectree_repr(LoadedDecTreeModel); 
            ofstream outFile(FileName);  
            outFile << model;  
            outFile.close();

        } else if (strcmp(mldType, "neuralnetwork")  ==0)   {  

             LoadedNeuralNetwork.save(FileName);
        } else if (strcmp(mldType, "dectree")  ==0)   {  

             LoadedDecTreeContModel.save(FileName);   

        } else if (strcmp(mldType, "umap")  ==0)   {  

            LoadedUMap.save(FileName);

        } else if (strcmp(mldType, "hdbscan")  ==0)   {  

            LoadedNHDBScan.save(FileName);
        }

     } else if (strcmp(s, "load")  ==0)   {

        char* mldType = std::strtok(o  , "->");  
        char* _file  = std::strtok(NULL,"->");  
        string FileName = _file;

        if (strcmp(mldType, "tree")  ==0)   {   

           LoadedDecTreeModel = load_tree(FileName);

        } else if (strcmp(mldType, "neuralnetwork")  ==0)   {  

            LoadedNeuralNetwork.load(FileName);
        }  else if (strcmp(mldType, "dectree")  ==0)   {  

            LoadedDecTreeContModel.load(FileName);

        } else if (strcmp(mldType, "umap")  ==0)   {  

            LoadedUMap.load(FileName); 

        } else if (strcmp(mldType, "hdbscan")  ==0)   {  

            LoadedNHDBScan.load(FileName); 

        }
 

     } else if (strcmp(s, "tree")  ==0)   {

        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");   
  

        vector<map<string, string>> data;
        vector<string> labels;  
        set<string> features;
        std::map<string, string> vecMap;
        
        char *buffer ;
        buffer = strtok(_features, "|");      

        vector<string>  vals =  memory_sig_rec.get_records(buffer) ;   
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
             vals = memory_sig_rec.get_records(buffer); 
             i_rec = 0;
             for (auto& val:vals) { 
                data[i_rec].insert({buffer, val});
                i_rec++;
  
                 }
  
             // cout << "feat add: " << buffer  << endl;
             features.insert(buffer) ;  
             buffer = strtok(NULL,"|");
           
             }
 
  
        labels = memory_sig_rec.get_records(_label) ;    
        //cout << "labels: " << labels.front()  << endl;

        LoadedDecTreeModel = dectree_fit(data, labels, features);  

     } else if (strcmp(s, "neuralnetwork")  ==0)   {

        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");   
 
        vector<vector<double>> features;
        vector<vector<double>> labels;    
        char *buffer ;
        buffer = strtok(_features, "|");     

        vector<float>  vals = memory_sig_rec.get_signals(buffer);    
        std::vector<double> doubleVec(vals.begin(), vals.end());
        features.push_back(doubleVec) ;  

        buffer = strtok(NULL,"|");

        //stack<vector<int>> myStack; // stack of vectors
        int i_rec = -1;
        while (buffer !=NULL)
           {     
             vals = memory_sig_rec.get_signals(buffer);  
             doubleVec = std::vector<double>(vals.begin(), vals.end());
             features.push_back(doubleVec) ;  
             buffer = strtok(NULL,"|");
           
           }
   
        vector<float> sigs =   memory_sig_rec.get_signals(_label);   
        std::vector<double> doubleVec2(sigs.begin(), sigs.end());
        labels.push_back(doubleVec2) ;    
        
        NeuralNetwork nn(0.5);
        for (int epoch = 0; epoch < 5000; epoch++) {
            for (size_t i = 0; i < features.size(); i++) {
               nn.train(features[i], labels[i]);
          }
        }
       LoadedNeuralNetwork = nn;

     } else if (strcmp(s, "gradientdecent")  ==0)   {

        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");   
     
        char *buffer ;
        buffer = strtok(_features, "|");     

        vector<float>  vals =   memory_sig_rec.get_signals(buffer);     
        std::vector<double> features(vals.begin(), vals.end()); 
  
 
        vector<float> sigs = memory_sig_rec.get_signals(_label);      
        std::vector<double>  labels(sigs.begin(), sigs.end()); 
         
        double learningRate = 0.1;
        int maxIters = 1000;
        double tolerance = 1e-6;
        //LoadedGradientDescent = nn;
        Vector result = gradientDescent(features, labels, learningRate, maxIters, tolerance);

     } else if (strcmp(s, "decisiontree")  == 0)   {
     
        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");    
        std::vector<Sample> data;

        char *buffer ;
        buffer = strtok(_features, "|");     

        Sample vecMap;
        vector<float>  vals =  memory_sig_rec.get_signals(buffer);     
        for (auto& val:vals) {  
            vecMap.features.push_back(val);
            data.emplace_back(vecMap); 
         } 
  
        buffer = strtok(NULL,"|");
 
        int i_rec = -1;
        while (buffer !=NULL)
           {     
             vals =  memory_sig_rec.get_signals(buffer); 
             i_rec = 0;
             for (auto& val:vals) { 
                data[i_rec].features.push_back(val); 
                i_rec++;
             } 
             buffer = strtok(NULL,"|");
           
           }
 
   
        vals =  memory_sig_rec.get_signals(_label); 
        i_rec = 0;
        for (auto& val:vals) { 
                data[i_rec].label = val; 
                i_rec++;
          }   
        LoadedDecTreeContModel.fit(data);   
    

     } else if (strcmp(s, "umap")  ==0)   {

        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");   
 
        vector<vector<double>> features; 
        char *buffer ;
        buffer = strtok(_features, "|");     

        vector<float>  vals = memory_sig_rec.get_signals(buffer);    
        std::vector<double> doubleVec(vals.begin(), vals.end());
        features.push_back(doubleVec) ;  

        buffer = strtok(NULL,"|");

        //stack<vector<int>> myStack; // stack of vectors
        int i_rec = -1;
        while (buffer !=NULL)
           {     
             vals = memory_sig_rec.get_signals(buffer);  
             doubleVec = std::vector<double>(vals.begin(), vals.end());
             features.push_back(doubleVec) ;  
             buffer = strtok(NULL,"|");
           
           } 

       LoadedUMap.train(features);

     } else if (strcmp(s, "hdbscan")  ==0)   {

        char* _features = std::strtok(o  , "->");  
        char* _label    = std::strtok(NULL,"->");   
  

        char *buffer ;
        buffer = strtok(_features, "|");      
        vector<float>  valsX = memory_sig_rec.get_signals(buffer); 
        buffer = strtok(NULL,"|");
        vector<float>  valsY = memory_sig_rec.get_signals(buffer); 
        buffer = strtok(NULL,"|");
        vector<float>  valsZ = memory_sig_rec.get_signals(buffer); 
        int p = valsX.size();

        std::vector<HDBSPoint> points;
        for (int i = 0; i < p; ++i) {
          
            HDBSPoint pt;
            pt.x = valsX[i];
            pt.y = valsY[i];
            pt.z = valsZ[i];
            points.push_back(pt) ;  
        }

       std::vector<int> res = LoadedNHDBScan.fit(points, 3);

     } else if (strcmp(s, "simulatedanneal")  == 0)   {  
     } else if (strcmp(s, "kmeans")  == 0)   {  
     } else if (strcmp(s, "ltsm")  == 0)   {  
     } else if (strcmp(s, "cnn")  == 0)   {  
     } 

   } else if (strcmp(v, "stimuli")  ==0)   {

     if (strcmp(s, "sensor")  ==0)   {
     }

   } else if (strcmp(v, "recalculate")  ==0)   {

     if (strcmp(s, "goals")  ==0)   {
     }
     else if (strcmp(s, "success")  ==0)   {
     }

   } else if (strcmp(v, "assess")  ==0)   {

     if (strcmp(s, "emotion")  ==0)   {
     }

     else if (strcmp(s, "response")  ==0)   {
     }
     
   } else if (strcmp(v, "formulate")  ==0)   {

     if (strcmp(s, "stratgey")  ==0)   {
     }

     else if (strcmp(s, "response")  ==0)   {
     }

   } else if (strcmp(v, "express")  ==0)   {

     if (strcmp(s, "vocal")  ==0)   {
     }

     else if (strcmp(s, "movement")  ==0)   {
     }


   } else if (strcmp(v, "reason")  ==0)   {

     if (strcmp(s, "present")  ==0)   {

     } else if (strcmp(s, "link")  == 0)   {
          // 
          // umap 
          //

          // LoadedUMap

     } else if (strcmp(s, "classify")  == 0)   {
          // 
          // hdbscan
          // 

          // LoadedNHDBScan


     } else if (strcmp(s, "respond")  == 0)   {
          // 
          // hdbscan
          // 

          // LoadedNHDBScan

     } else if (strcmp(s, "decide")  == 0)   {   

        int lcnt = 0;
        char *buffer ;
        string soutput;
        size_t cnt = countCharOccurrences(o,'|'); 
        std::map<string, string> data;  

        if (cnt > 1)
        {
            buffer = strtok(o, "|");    
            string _s = buffer  ;
            buffer = strtok(NULL, "|");   
            string _v = buffer  ;
            buffer = strtok(NULL, "|");   
            string _o = buffer  ; 
 
           // char* l1 = 's';
           // char* l2 = "v";
           // char* l3 = "o";
            data.insert({"s",_s});  
            data.insert({"v",_v});  
            data.insert({"o",_o});  

            string results = dectree_inference(LoadedDecTreeModel, data); 
          // std::cout << results << std::endl;     
           // char *out; 
            output.recs.push_back(results);   

         
         }
         else
         { 
         FILE *filePtr;
         cout << "file: " << o  << endl;
         filePtr = fopen(o, "r"); 
         char currentline[1000];
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
           lcnt++;


           string results = dectree_inference(LoadedDecTreeModel, data);   

           output.recs.push_back(results);  
           data.clear();

          } 

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
 
      if (strcmp(s, "load")  ==0)   {
 
          string key   =  o; //getMemory(o);  
          vector<string> _key ; 
          //todo add loop bqsed on split
          _key.push_back(key); 
          
          int to = archive.add(_key);  
          output.sigs.push_back(to);

      } else if (strcmp(s, "add")  ==0)   {
 
          string key   =  o; //getMemory(o);  
          vector<string> _key ; 
          //todo add loop bqsed on split
          _key.push_back(key); 
          
          int to = archive.add(_key);  
          output.sigs.push_back(to);

      } else if  (strcmp(s, "duology")  ==0)   {

          char* _frm = std::strtok(o  , "->");  
          char* _to  = std::strtok(NULL,"->");  
          string key   = _frm ; // getMemory(_frm);
          string  value = _to; // getMemory(_to);  
  
          vector<string> _key ;
          vector<string>  _value ;
          //todo add loop bqsed on split
          _key.push_back(key);
          _value.push_back(value);
          
          int to = archive.add(_key);
          int frm = archive.add(_value);  

          archive.link(to, frm, "response"); 
 
     } else if (strcmp(s, "exists")  == 0)   {
          std::string key =  o;  
          int keyId =  archive.exists(o);   
          output.sigs.push_back(keyId);      

     } else if (strcmp(s, "retrieve")  == 0)   { 
 
          int keyid =  atoi(o);     
          vector<std::string> resp  = archive.retrieve(keyid);  
            
          output.recs.reserve(resp.size());  
          for (auto& s : resp) { 
              output.recs.push_back(s);
          }   
 
      } else if (strcmp(s, "response")  == 0)   {    //defition

          vector<string> _mess ; 
          string _o = o;
          _mess.push_back(_o); 

          vector<string> resp = archive.definition(_mess) ;    
          output.recs.reserve(resp.size());  
          for (auto& s : resp) { 
              output.recs.push_back(s );
          }    
      } else if (strcmp(s, "match")  == 0)   { 
  
          vector<string> _mess ; 
          string _o = o;
          _mess.push_back(_o); 
          vector<string> resp = archive.match(_mess) ;      

          string _t;
          for (const auto &s : resp) {    
             char out[s.size() +1];   

             for (size_t i = 0; i < s.length(); ++i) {
                   out[i] = s[i];
              }
             out[s.size()] = '\0';
             output.recs.push_back(out);  
           }   
 

      }else if (strcmp(s, "linked")  == 0)   { 
          int keyid =  atoi(o);    
          vector<int> resp  = archive.linked(keyId) ;  

          for (auto& s : resp) { 
              output.sigs.push_back(float(s));
          }    
      }
 
    } else if (strcmp(v, "records")  == 0)   {

      int keyId  ;    
      if (strcmp(s, "add")  ==0)   {

          char* _frm = std::strtok(o  , "->");  
          char* _to  = std::strtok(NULL,"->");     
          memory_sig_rec.add_records(_frm, _to );    
       

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
              memory_sig_rec.add_records( colname , buffer)  ;   

           //    cout << colname  << buffer << endl;

              buffer = strtok(NULL, delim); 
              while (buffer !=NULL)
                 {   
 
                    cols ++;
                    colname = header[cols];
                    memory_sig_rec.add_records( colname , buffer)  ; 
                    buffer = strtok(NULL, delim); 
                 }  

           }
           lcnt++;

          }
          
      // cout << "file: " << memory_sig_rec.get_records(colname).front()  << endl;
       output.sigs.push_back( cols);    
     //  strcpy(output.o, "1");

 
     } else if (strcmp(s, "exists")  == 0)   {  

          keyId = memory_sig_rec.exists( o );      
          output.sigs.push_back( keyId);    

     } else if (strcmp(s, "last")  == 0)   {  
  
             vector<string> resp  =  memory_sig_rec.get_records( o );  
             string val =  resp.back();  
             output.recs.push_back( val);   

     } else if (strcmp(s, "first")  == 0)    { 
           
             vector<string> resp  = memory_sig_rec.get_records( o );  
             string val =  resp.back();   
             output.recs.push_back( val);      
             

     } else if (strcmp(s, "distance")  == 0)   { 
          int keyId =  atoi(o);   
          // int index = std::distance(v.begin(), it) 
          // snprintf(output.o, sizeof(output.o), "%f", resp.back());   
    
     } else if (strcmp(s, "count")  == 0)   {   
          vector<string> resp  = memory_sig_rec.get_records( o );  
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
          float fvalue =  atof(_to);    
          memory_sig_rec.add_signals( _frm , fvalue)  ;

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
             float fvalue =  atof(buffer);    
             memory_sig_rec.add_signals( colname , fvalue)  ; 

              buffer = strtok(NULL,delim);
              cout << buffer  << endl;
              while (buffer !=NULL)
                 {   
 
                    cols ++;
                    colname = header[cols]; 
                    float fvalue =  atof(buffer);   
                    memory_sig_rec.add_signals( colname , fvalue)  ;  
                    buffer = strtok(NULL,delim); 
                 }  

           }
           lcnt++;

          }
         
           
      output.sigs.push_back(lcnt);       
     //  strcpy(output.o, "1");

 
     } else if (strcmp(s, "exists")  == 0)   {  

          keyId = memory_sig_rec.exists(o)  ;     
          output.sigs.push_back(keyId);    

     } else if ((strcmp(s, "last")  == 0)   )  {  
  
             vector<float> resp  =  memory_sig_rec.get_signals(o);  
             output.sigs.push_back(resp.back());    

     } else if ((strcmp(s, "first")  == 0)    )  {  
             vector<float> resp  = memory_sig_rec.get_signals(o);    
             output.sigs.push_back(resp.front());     
          
     } else if (strcmp(s, "sum")  == 0)   {   
          vector<float> resp  =  memory_sig_rec.get_signals(o);  
          int sum = std::accumulate(resp.begin(), resp.end(), 0); 
          output.sigs.push_back(sum);     
 
     } else if (strcmp(s, "mean")  == 0)   {   
          vector<float> resp  =  memory_sig_rec.get_signals(o);   
          int sum = std::accumulate(resp.begin(), resp.end(), 0);

          float mean =  sum / resp.size();
          output.sigs.push_back(mean);     
 
 
     } else if (strcmp(s, "minimum")  == 0)   {   
          vector<float> resp  = memory_sig_rec.get_signals(o);   
          auto it = std::min_element(resp.begin(), resp.end()); 
          output.sigs.push_back(*it);      

     } else if (strcmp(s, "maximum")  == 0)   {   
          vector<float> resp  = memory_sig_rec.get_signals(o);     
          auto it = std::max_element(resp.begin(), resp.end());   
          output.sigs.push_back(*it);      

     } else if (strcmp(s, "distance")  == 0)   { 
          int keyId =  atoi(o);   
          // int index = std::distance(v.begin(), it) 
          // snprintf(output.o, sizeof(output.o), "%f", resp.back());   
  

     } else if (strcmp(s, "varience")  == 0)   {   
          vector<float> resp  = memory_sig_rec.get_signals(o);   
          output.sigs.push_back(resp.back());      

     } else if (strcmp(s, "count")  == 0)   {   
          vector<float> resp  = memory_sig_rec.get_signals(o);    
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
  

           string label = _s;
           int _s_id  = svo_graph.addVertex(label, "subject") ; 
 
           label = _v;
           int _v_id  = svo_graph.addVertex(label, "verb") ;
 
           label = _o;
           int _o_id  = svo_graph.addVertex(label, "object") ;
      
           svo_graph.addEdge(_s_id, _v_id, 1, "s_v"); 
           svo_graph.addEdge(_v_id, _o_id, 1, "v_o");      
           svo_graph.addEdge(_o_id, _s_id, 1, "o_s");  


      } else if  (strcmp(s, "duology")  ==0)   {


           char* part1  = std::strtok(o  , "->");  
           char* part2  = std::strtok(NULL,"->"); 

           char* _s1  = std::strtok(part1  , "|");  
           char* _v1  = std::strtok(NULL,"|");  
           char* _o1  = std::strtok(NULL,"|");   

           char* _s2  = std::strtok(part2  , "|");  
           char* _v2  = std::strtok(NULL,"|");  
           char* _o3  = std::strtok(NULL,"|");  

           string label = _s1;
           int _s_id  = svo_graph.addVertex(label, "subject") ; 
 
           label = _v1;
           int _v_id  = svo_graph.addVertex(label, "verb") ;
 
           label = _o1;
           int _o_id  = svo_graph.addVertex(label, "object") ;
      
           svo_graph.addEdge(_s_id, _v_id, 1, "s_v"); 
           svo_graph.addEdge(_v_id, _o_id, 1, "v_o");      
           svo_graph.addEdge(_o_id, _s_id, 1, "o_s");  




      } else if (strcmp(s, "remove")  ==0)   {

      } else if (strcmp(s, "exists")  ==0)   {
 
           std::string in_o  = o;   
           int _s_id;  
           _s_id = svo_graph.exists(in_o);  
           output.sigs.push_back(_s_id);      


      } else if (strcmp(s, "link")  ==0)   {

           char* _to    = std::strtok(o  , "->");  
           char* _from  = std::strtok(NULL,"->");  
           int i_to    =  atoi(_to);  
           int i_from  =  atoi(_from);  
           svo_graph.addEdge(i_to, i_from);
           svo_graph.addEdge(i_from, i_to); 
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
 

           int _s_id;  
            try 
            {
              _s_id = svo_graph.exists(in_s); 
            }

            catch (const std::runtime_error& e) {

                  std::cout <<  "Vert not found"  << std::endl; 
                  std::cerr << "Error: " << e.what() << std::endl;
              
            }

            vector<int> _v_ids ;

            try 
            { 
                _v_ids = svo_graph.egoNet(_s_id, "s_v"); 
            }
            catch (const std::runtime_error& e) { 
                  std::cout <<  "Vert not found"  << std::endl;
                  std::cerr << "Error 2: " << e.what() << std::endl;
                 
            }
 

           int _v_id = _v_ids.front();  
           string v = svo_graph.property(_v_id, "label");
  

           if (in_v == v)
           {  

                vector<int> _o_ids; 

               try {
                     _o_ids = svo_graph.egoNet(_v_id, "v_o");  
                   }
                catch (const std::runtime_error& e) {  
                   std::cout <<  "eroor 3"  << std::endl;
                   std::cerr << "Error 3: " << e.what() << std::endl; 
               }
               

               int _o_id = _o_ids.front();   
               string out_o = svo_graph.property(_o_id, "label");    

               output.recs.push_back(o);    
               output.recs.push_back(v);  
               output.recs.push_back(out_o);   
              }
          
      } else if (strcmp(s, "related")  == 0)   { 
         //one objevt lin to best match for svo
         // std::string key =  o;  
         // int keyId =  props.similar(key) ;  
         // int respId = edges.search(keyId) ; 
         // std::string resp = props.search(respId);     
         // strcpy(output.o, resp );   

      } else if (strcmp(s, "relatives")  == 0)   { 
         //alll object lin to best match for svo
         // std::string key =  o;  
         // int keyId =  props.similar(key) ;  
         // int respId = edges.search(keyId) ; 
         // std::string resp = props.search(respId);     
         // strcpy(output.o, resp );  
      }
     }
     else
     {

               output.recs.push_back("command ");   
               output.recs.push_back(v);    
               output.recs.push_back(" not found.\n");    

               string res =  findCommand(v);
               if ( res != " ")  { 
                    output.recs.push_back("Did you mean ");   
                    output.recs.push_back(res ); 
                    output.recs.push_back("?\n" ); 
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
 
}


*/

std::string clean_cmd(std::string str)
{
    if (str.back() == ';')
    {
        int i = str.size() -1;
        return str.substr(0, i);
    }
    return str;
}


extern "C" { 
    void dispatch(char *v, char *s, char *o){  dispatcher(v,s,o); }
} 
int main(int argc, char* argv[]) {
    
    initCommands();
    if (argc > 1)
    { 
       // FILE *filePtr;     
       // filePtr = stdin;  
      //  sort(filePtr);
        //select(filePtr);
        string s_arg = argv[1];

        char *buffer ;
        FILE *filePtr;  
        char currentline[1000];  
     
        filePtr = fopen(s_arg.c_str(), "r");  
        vector<string> doc;
    
        while (fgets(currentline , 1000, filePtr))
        {        

             string incmd = currentline;
             vector <string> ary_cmds = splitIgnoringQuotes(incmd); 
             char* v = new char[ary_cmds[0].size() + 1];
             std::strcpy(v, ary_cmds[0].c_str());   
    
             string instr;

             if (ary_cmds.size() > 1)
             {
                 instr = clean_cmd(ary_cmds[1]) ; 
              }
             else
             {
                 instr = "";
             } 

             char* s = new char[instr.size() + 1];
             std::strcpy(s, instr.c_str()); 

             if (ary_cmds.size() > 2)
             {
                 instr = clean_cmd(ary_cmds[2]) ; 
              }
             else
             {
                 instr = "";
             }
   
             char* o = new char[instr.size() + 1];
             std::strcpy(o, instr.c_str()); 

           
          struct triplesspace::SVO results; 


          try 
             {
                  results = dispatcher(v, s, o); 
             }
          catch(const std::exception& e)
             {  
                  std::cout << "Error occured " << e.what() << std::endl;
             } 

          for (auto i: results.sigs) {
              	std::cout << i << " ";
             } 

          for (auto word : results.recs) { 
                std::cout <<  word << " "; 
             }

          std::cout << std::endl; 
        } 

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
 
 
        std::string cmd = input;
        //std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "exit;") {
            std::cout << "Goodbye!\n";
            break;
        } 
        else if (cmd == "help;") {
 
            std::cout << "help  - Show this help message\n"  ;
            std::cout << "exit  - Quit the program\n";
            std::cout << "Keywords\n";
            int i = 0;
            int totalWidth = 12;
            string cmd ;
            for (const auto &kv : cmdLookup) {  

                 cmd = kv.first ;
                 if (cmd.size() < totalWidth) {
                   cmd.insert(0, totalWidth - cmd.size(), ' '); // left pad
                 }
                std::cout <<  cmd << "    "; 
                i++;
                if (i ==4)
                {
                  i =0;
                   std::cout << "\n";
                }
              }  
            std::cout << "\n";  
        }  
        else if (cmd.empty()) {
            // Ignore empty input
            continue;
        } 
        else 
        {
            
             string incmd = cmd;
             vector <string> ary_cmds = splitIgnoringQuotes(incmd); 
             char* v = new char[ary_cmds[0].size() + 1];
             std::strcpy(v, ary_cmds[0].c_str());  
             
             string instr;

             if (ary_cmds.size() > 1)
             {
                 instr = clean_cmd(ary_cmds[1]) ; 
              }
             else
             {
                 instr = "";
             } 

             char* s = new char[instr.size() + 1];
             std::strcpy(s, instr.c_str()); 

             if (ary_cmds.size() > 2)
             {
                 instr = clean_cmd(ary_cmds[2]) ; 
              }
             else
             {
                 instr = "";
             }
   
             char* o = new char[instr.size() + 1];
             std::strcpy(o, instr.c_str()); 

             struct triplesspace::SVO results;
  

             try 
             {
                  results = dispatcher(v, s, o); 
             }
             catch(const std::exception& e)
             {  
                  std::cout << "Error occured " << e.what() << std::endl;
             } 

             for (auto i: results.sigs) {
              	std::cout << i << " ";
             } 

             for (auto word : results.recs) { 
                std::cout <<  word << " "; 
             }

             std::cout << std::endl;
 
              //for (auto s : results.recs) {
               //   free(s);

              // } 
        }
    }

    return 0;
  }
} 