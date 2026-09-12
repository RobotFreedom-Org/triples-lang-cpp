
#include  <string.h>

namespace triplesspace {

struct SVO { 
  std::vector<float> sigs; 
  std::vector<char*> recs;   
}; 
 
  

struct cmdsSVO { 
  char s[12];  
  char v[12];  
  char o[12];  
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