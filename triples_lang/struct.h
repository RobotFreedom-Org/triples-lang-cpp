
#include  <string.h>

namespace triplesspace {

struct SVO { 
  std::vector<float> sigs; 
  std::vector<std::string> recs;   
}; 


struct Event{
  int cnt;
  int level;
  std::string  code; 
};

 
struct Situation{
   std::vector<std::string> prior_stimuli; 

   std::string stimuli;
   //move to a map

   Event lifeforms  ;
   Event humans ;
   Event threat;
   Event weather;
   Event moving;
   Event activity;
   Event ambiance;
   Event user_present;  

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