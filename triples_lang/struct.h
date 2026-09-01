
#include  <string.h>

namespace triplesspace {

struct SVO { 
  char s[12];  
  char v[12];  
  char o[12];  
}; 

struct cmdsSVO { 
  char s[8];  
  char v[8];  
  char o[8];  
}; 

struct iSVO { 
  int s;  
  int v;  
  int o;  
}; 


struct fSVO { 
  float s;  
  float v;  
  float o;  
}; 
}
struct triplesspace::SVO core(char *v , char *s , char *o ) ;