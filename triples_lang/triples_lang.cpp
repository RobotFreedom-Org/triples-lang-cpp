/*
 Arduino Triples-Lang 

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
#include "struct.h"  
    
  
const int MAX_CMD_LINES = 30;
const int MAX_VAR_NAME  = 6; 
const int MAX_BLOCKS    = 2;
const int MAX_CMDS      = 4;  
struct triplesspace::cmdsSVO BLOCKS[MAX_BLOCKS][MAX_CMDS];

const int MAX_ROUTINES   = 3;
const int MAX_F_CMDS   = 8; 
char ROUTINESSNAMES[MAX_ROUTINES][MAX_VAR_NAME] ;
struct triplesspace::cmdsSVO ROUTINES[MAX_ROUTINES][MAX_F_CMDS] ;

const int MAX_MEM     = 10; 
char MEMSTR[MAX_MEM][MAX_CMD_LINES] ;
char MEMSTRNAME[MAX_MEM][MAX_VAR_NAME] ; 

float  MEMFLT[MAX_MEM] ;
char MEMFLTNAME[MAX_MEM][MAX_VAR_NAME] ; 

char current_name[ MAX_VAR_NAME] ; 
  

int current_blocks = -1;
int current_mem    = -1;
int loaded_routine    = -1;
  

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
      current_mem ++ ;
      _id = current_mem;
  }  
  int _max = MAX_MEM -1;
  for (int i = 0; i < _max; i++) {  
 
      MEMSTRNAME[_id][i] = name[i];
      MEMSTR[_id][i] = value[i];
  }  
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
  current_mem = -1;
  return ' ';
}  


struct triplesspace::SVO runIfBlock(int blockId) {

  struct triplesspace::SVO output;
  //strcpy(output.s, ""); 
 // strcpy(output.v, ""); 
  strcpy(output.o, " ");  

  int _max = MAX_CMDS - 1;
  int run = 1; 

  for (int i = 0; i < _max; i++) { 
    if ( (strcmp( BLOCKS[blockId][i].v, " ")  != 0) && (strcmp(BLOCKS[blockId][i].v , "")  != 0) && run == 1) { 
        if (i == 0) {
    
          char*  v =  BLOCKS[blockId][i].v; 
          char*  s =  BLOCKS[blockId][i].s;   
          char*  o =  BLOCKS[blockId][i].o ;   
   
          struct triplesspace::SVO condition = core(v,s,o); 
  
          if (strcmp(  condition.o, "0")  == 0) { 
            run = 0;   
          }
        } else { 
          char*  v = BLOCKS[blockId][i].v ;  
          char*  s = BLOCKS[blockId][i].s;  
          char*  o = BLOCKS[blockId][i].o;      
          struct triplesspace::SVO  _output = core(v,s,o);   
          strcpy(output.o, _output.o);   
        } 
    }
  }
  return output;
}
 

struct triplesspace::SVO execute( char* o) {
  struct triplesspace::SVO output;
  //strcpy(output.s, ""); 
 // strcpy(output.v, ""); 
  strcpy(output.o, " ");  

  int _max = MAX_CMDS - 1;
  int run = 1;
  int routine_id = getRoutineID(o);     
  for (int i = 0; i < _max; i++) {
 

    if ( (strcmp( ROUTINES[routine_id][i].v , " ")  != 0) &&   (strcmp( ROUTINES[routine_id][i].v , "")  != 0) && run == 1) { 
          char*  v =  ROUTINES[routine_id][i].v; 
          char*  s =  ROUTINES[routine_id][i].s;   
          char*  o =  ROUTINES[routine_id][i].o ;     
          struct triplesspace::SVO  _output = core(v , s, o);     
          strcpy(output.o, _output.o);  
      }
    }  
  return output;
}


struct triplesspace::SVO dispatcher(char *v, char *s, char *o) {
 
  struct triplesspace::SVO output; 
 
  strcpy(output.s, ""); 
  strcpy(output.v, ""); 
  strcpy(output.o, "");  


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
    return core(v, s, o);
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
 

  strcpy(output.s, ""); 
  strcpy(output.v, ""); 
  strcpy(output.o, "");  

  if (strcmp(v, "help")   == 0  ) {  
      // Serial.println("set,get,echo,add,subtract,divide,multiple,increment,decrement,more,less,equal,different,flow,routine,help");
  } 
  else if (strcmp(v, "annotation") ==0){
      
  }
  else if ( strcmp(v, "echo")   == 0) { 
      strcpy(output.o, s);  

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
  } else if (strcmp(v, "movmeent")   == 0 ) {
    if (strcmp(s, "torso")   == 0 ) {
      // Place Holder
    } else if (strcmp(s, "legs")   == 0) {
      //Place Holder
    }
  } else if (strcmp(v, "set")   == 0 ){ 
  
    char t =  setMemory(s,  o); 
    strcpy(output.o, " "); 
 
  } else if (strcmp(v, "get")  ==0){
 
    char* _t = getMemory(s);   

    strcpy(output.o, _t); 

  } else if (strcmp(v, "clear")  ==0){
    clearMemory(); 
    strcpy(output.o, "1");
 
  } else if (strcmp(v, "add")  ==0){
  
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  
    float value = f1 + f2;    
    snprintf(output.o, sizeof(output.o), "%f", value);   
    // dtostrf(value, 4, 3, output.o);  


  } else if (strcmp(v, "increment")  ==0)  {
    char* v1 =  getMemory(s); 
    float f1 =  atof(v1);   
    float value = f1 + 1;
    snprintf(output.o, sizeof(output.o), "%f", value);   
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "decrement")  ==0)  {
    char* v1 =  getMemory(s); 
    float f1 =  atof(v1);   
    float value = f1 - 1; 
    snprintf(output.o, sizeof(output.o), "%f", value);    
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "subtract")  ==0)  {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  
    float value = f1 - f2;
    snprintf(output.o, sizeof(output.o), "%f", value);   
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "divide")  ==0)   {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);   
    float value = f1 / f2;
    snprintf(output.o, sizeof(output.o), "%f", value);     
    // dtostrf(value, 4, 3, output.o);  

  } else if  (strcmp(v, "multiply")  ==0)  {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);   
    float value = f1 * f2;
    snprintf(output.o, sizeof(output.o), "%f", value);   
    // dtostrf(value, 4, 3, output.o);  

  } else if (strcmp(v, "more")  ==0){
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 > f2) {
      strcpy(output.o, "1");
    } else { 
      strcpy(output.o, "0");
    }

  } else if (strcmp(v, "less")  ==0)   {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 < f2) { 
      strcpy(output.o, "1");
    } else { 
      strcpy(output.o, "0");
    }

  } else if (strcmp(v, "equal")  ==0)  {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 == f2) {
      strcpy(output.o, "1");
    } else { 
      strcpy(output.o, "0");
    }

  } else if (strcmp(v, "different")  ==0)   {
    char* v1 =  getMemory(s);
    char* v2 =  getMemory(o);  
    float f1 =  atof(v1);  
    float f2 =  atof(v2);  

    if (f1 != f2) {
      strcpy(output.o, "1");
    } else { 
      strcpy(output.o, "0");
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
 


int main(int argc, char* argv[]) {
    
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
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

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

             v = std::strtok(cstr  , " ");  
             s = std::strtok(NULL, " ");  
             o = std::strtok(NULL, " ");     
     
             replace_char(v,';', '\0'); 
             replace_char(s,';', '\0'); 
             replace_char(o,';', '\0');  

             struct triplesspace::SVO results =dispatcher(v,s,o); 
             // std::cout << results.o;
             std::cout <<  results.o << std::endl;
        }
    }

    return 0;
} 