
#include <stdio.h>
#include "triples_lang.cpp"

extern "C"  //Tells the compile to use C-linkage for the next scope.
{
     // Simple function to add two integers
    char* run(char *v, char *s, char *o) { 

             struct triplesspace::SVO results = dispatcher(v,s,o); 
             char* final;
             char buffer[32];   
             // std::cout << results.o;
             for (auto i: results.sigs) {  
 
                 int len = snprintf(buffer, sizeof(buffer), "%.2f", i);
                 final = (char*)malloc(len); 
                 strcpy(final, buffer); 
              } 

             for (auto i: results.recs) {  
                final = i;
             }

             return final;

        }
} 