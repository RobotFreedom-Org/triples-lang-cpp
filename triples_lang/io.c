/*
 Comparision fo C and SQL

 Orgional Copyright (c) 2001 AnalysticalWay.com 
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
*/

#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

void quickSort(void *array, unsigned n);
static int compareRec(const void *a, const void *b);

void create(string FileName)
{               
   ofstream outFile(FileName +".pipe"); 
   outFile << "sub|verb|obj\n"; 
   outFile.close();
}

void select( FILE *filePtr)
{               
  char currentline[100];  
  /*Init variable with blank characters.*/
  memset(currentline, ' ', 99);
  
  /*loop through and print*/
  while (fgets(currentline, 100, filePtr))
  {
    /*Write output to terminal*/
    fputs(currentline, stdout);

  }
}


void select_by_id(FILE *filePtr, char *argv)
{       
  char currentline[100];
  char _custid[2];
  char custid[2];
                      
  /*Set file point to piped input*/
  filePtr = stdin;   
  
  /*Get passed parameter*/
  strncpy(_custid, argv, 1);
  
  /*Set last character to \0 (make it a C string).*/
  _custid[1] ='\0';    
           
  /*Init variable with blank characters.*/
  memset(currentline, ' ', 99);

  while (fgets(currentline, 100, filePtr))
  {               
      memset(custid, ' ', 1);

      /*Get the first character of the line(the custid)*/
      strncpy(custid, currentline, 1);   
      custid[1]='\0';

     if(strcmp(custid,_custid) == 0 )
       {       
           /*Write output to terminal*/                   
           fputs(currentline, stdout);
       }
   }
}

void remove_rec(FILE *filePtr, char* _id )
{    
  char currentline[100]; 
  char id[2];
  
  /*Set last character to \0 (make it a C string).*/
  _id[1] ='\0';
          
  /*Init variable with blank characters.*/
  memset(currentline, ' ', 99);
  
  while (fgets(currentline, 100, filePtr))
  {
      memset(id, ' ', 1);
      
      /*Get the first character of the line(the custid)*/
      strncpy(id, currentline, 1);
      //id[1]='\0';

     if(strcmp(id,_id) != 0 )
       {    
           /*Write output to terminal*/                    
           fputs(currentline, stdout);
       }
   }
}



void sort(FILE *filePtr) { 
  
  char currentline[100];
  char *records[1000]; 
  int lcnt = 0;
  int lcnt_in = 0;  
  int lcnt_out = 0; 

  /*Init variable with blank characters.*/
  memset(currentline, ' ', 100);        
  memset(records, ' ', 1000); 
               
  while (fgets(currentline, 100, filePtr))
  { 
    lcnt++ ; 
     /*Skip the first line (the header).*/
   if (lcnt >1)  
   {
      records[lcnt_in++ ] = strdup(currentline);
   }
   
  } 
  /*Run quick sort*/     
 quickSort(records, lcnt_in);
                          
 while(lcnt_out < lcnt_in)
 {        
  /*Write output to terminal*/  
  printf("%s", records[lcnt_out++]);
  } 
}

static int compareRec(const void *a, const void *b) {   
 
  char _byA[4];
  char _byB[4];
  char lineA[100];
  char lineB[100];
  char *parsedBuffer;         
  strncpy(lineA,*(char **)a,100);   
  strncpy(lineB,*(char **)b,100); 
  
  parsedBuffer=strtok(lineA, ","); 
 // parsedBuffer=strtok(NULL, ",");   
  strncpy(_byA,parsedBuffer,3);

  parsedBuffer=strtok(lineB, ","); 
  strncpy(_byB,parsedBuffer,3);
         
  _byA[3] ='\0';
  _byB[3] ='\0';        
 return strcmp(_byA, _byB);   
}

void quickSort(void *records, unsigned lcnt) 
{                   
 qsort(records, lcnt, sizeof(char *), compareRec);
}
 

void append( char *filename, char *values )
{                               
                        
  FILE *filePtr;
  filePtr = fopen(filename, "a"); 
  char newrecord[100];  
                             
  /*Init variable with blank characters.*/
   memset(newrecord, ' ', 100); 
    
   /*Copy cust id to newline*/  
   strncpy(newrecord,values,2);    

   /*Output new record to file*/
   fputs (newrecord ,filePtr);
   
   /*Add a new line indicator*/
   fputs("\r\n",filePtr);

   /*Close the file*/
   fclose(filePtr);
    
}


void merge(char *filenameA, char *filenameB)
{
  FILE *PtrFileA;
  FILE *PtrFileB;  
  char currentlineA[20];
  char currentlineB[20];
  char *parsedBuffer;
  char _custidA[2];  
  char _custidB[2];  
  char _poId[2]; 
  char _name[20];
  char _age[4];
  char _delim[2];  
  char _record[50];
  char newrecord[100];
  int  lcnt = 0; 
                               
   /*Set file to phyical file with read only option(rather than piped input as with the other examples)*/
   PtrFileA = fopen(filenameA, "r");
   PtrFileB = fopen(filenameB, "r");
                   
  _delim[0] =',';
  _delim[1] ='\0';
   
  while (fgets(currentlineA, 20, PtrFileA))
  {             
     /*Increment line count*/
      lcnt++;
                   
     /*Skip the first line (the header).*/
      if (lcnt>1)
	{                      
      /*Init variable with blank characters.*/
      memset(_record, ' ', 50);
      memset(newrecord, ' ', 100);
                    
      /*Split line by delim ','*/
      parsedBuffer=strtok(currentlineA, ",");         
                                    
      /*Get the first field(the custid)*/
      strncpy(_custidA,parsedBuffer,2); 
       _custidA[1] ='\0';

      strncpy(_record,_custidA,2);
      strcat(_record,_delim);
                                     
      /*Increment the parser forward one field*/
      parsedBuffer=strtok(NULL, ",");              
      strcat(_record,parsedBuffer);
      strcat(_record,_delim);

      parsedBuffer=strtok(NULL, ",");    
               
      strncpy(_age,parsedBuffer,2);
       _age[2] ='\0';
      strcat(_record,_age);
      strcat(_record,_delim); 
                         
      /*Rewind orders file.*/
      rewind(PtrFileB);         

  while (fgets(currentlineB, 20, PtrFileB))
  {            
     parsedBuffer=strtok(currentlineB, ",");          
     strncpy(_poId,parsedBuffer,2);   
     _poId[1] ='\0';
       
      parsedBuffer=strtok(NULL, ","); 
     strncpy(_custidB,parsedBuffer,2);   
     _custidB[1] ='\0';
      
      /*If id's match merge in second file to first*/
     if(strcmp(_custidA,_custidB) == 0 )
      {         
      strncpy(newrecord,_record,20);                      
      strcat(newrecord,_poId);
      strcat(newrecord,_delim);   
      parsedBuffer=strtok(NULL, ",");              
      strcat(newrecord,parsedBuffer);        
      /*Write output to terminal*/
      printf("%s",newrecord  );   
       }
    }   
  }    
}                                
   /*Close the files*/
   fclose(PtrFileA);
   fclose(PtrFileB); 
}


void merge_outer(char *filenameA, char *filenameB)
{   
  FILE *PtrFileA;
  FILE *PtrFileB;     
  char currentlineA[20];
  char currentlineB[20];
  char *parsedBuffer;
  char _custidA[2];  
  char _custidB[2];  
  char _poId[2]; 
  char _name[20];
  char _age[4];
  char _delim[2];  
  char _record[50];
  char newrecord[100];
  int  lcnt = 0;
  int found =0; 
                                     
   /*Set file to phyical file with read only option(rather than piped input as with the other examples)*/
   PtrFileA = fopen(filenameA, "r");
   PtrFileB = fopen(filenameB, "r");       
               
  _delim[0] =',';
  _delim[1] ='\0';
  
  while (fgets(currentlineA, 20, PtrFileA))
  {          
     /*Increment line count*/
      lcnt++;
                    
     /*Skip the first line (the header).*/
      if (lcnt>1)
	{                             
      /*Init variable with blank characters.*/
      memset(_record, ' ', 50);
      memset(newrecord, ' ', 100);
                                     
      /*Get the first field(the custid)*/
      parsedBuffer=strtok(currentlineA, ",");         

      strncpy(_custidA,parsedBuffer,2); 
      // _custidA[1] ='\0';
                                      
      strncpy(_record,_custidA,2);
      strcat(_record,_delim);        
      
      /*Increment the parser forward one field*/                                     
      parsedBuffer=strtok(NULL, ",");              
      strcat(_record,parsedBuffer);
      strcat(_record,_delim);
                                  
      parsedBuffer=strtok(NULL, ",");    
               
      strncpy(_age,parsedBuffer,2);
     //  _age[2] ='\0';
      strcat(_record,_age);
      strcat(_record,_delim);
     // _record[49] = '\0';
                     
      /*Rewind orders file.*/
      rewind(PtrFileB); 
      
      /*Reset the found indicator*/
      found =0;

  while (fgets(currentlineB, 20, PtrFileB))
  {    
     parsedBuffer=strtok(currentlineB, ",");          
     strncpy(_poId,parsedBuffer,2);   
     _poId[1] ='\0';
       
      parsedBuffer=strtok(NULL, ","); 
     strncpy(_custidB,parsedBuffer,2);   
     _custidB[1] ='\0';
                       
      /*If id's match merge in second file to first*/
     if(strcmp(_custidA,_custidB) == 0 )
      {                            
      /*Set found indicator*/      
      found =1;
      
      strncpy(newrecord,_record,20);                      
      strcat(newrecord,_poId);
      strcat(newrecord,_delim);   
      parsedBuffer=strtok(NULL, ",");              
      strcat(newrecord,parsedBuffer);       
      /*Write output to terminal*/
      printf("%s",newrecord  );  
      }
    }
                   
      /*If no match was found write output to terminal*/
  if (found ==0)
  {                                    
      strcat(_record,_delim);
      printf("%s",_record  );   
      
      /*Put a newline because not copied from newrecord*/
      printf("\r\n");
   }
 }
}
                        
   /*Close the files*/ 
   fclose(PtrFileA);
   fclose(PtrFileB); 
}


void groupby(char *filenameA, char *filenameB)
{           
  FILE *PtrFileA;
  FILE *PtrFileB;  
  char currentlineA[20];
  char currentlineB[20];
  char *parsedBuffer;
  char _custidA[2];  
  char _custidB[2];  
  char _name[20];
  char _delim[2];  
  char _record[50];
  char _txtorders[10];
  int lcnt = 0;
  int _orders =0;                                 
                        
                   
   _delim[0] =',';
   _delim[1] ='\0';        
   _orders =0;      
    
   /*Set file to phyical file with append option(rather than piped input as with the other examples)*/
   PtrFileA = fopen(filenameA, "r");
   PtrFileB = fopen(filenameB, "r");  
   
  while (fgets(currentlineA, 20, PtrFileA))
  {           
     /*Increment line count*/
      lcnt++;
                     
     /*Skip the first line (the header).*/
 if (lcnt>1)
	{                 
      /*Init variable with blank characters.*/
      memset(_record, ' ', 50);
                                  
      /*Split line by delim ','*/
      parsedBuffer=strtok(currentlineA, ",");         
                                    
      /*Get the first field (the custid)*/
      strncpy(_custidA,parsedBuffer,2); 
       _custidA[1] ='\0';

      strncpy(_record,_custidA,2);
      strcat(_record,_delim);
      
      /*Increment the parser forward one field*/
      parsedBuffer=strtok(NULL, ",");    
      strncpy(_name,parsedBuffer,20);      
      strncpy(_record,_name,20);
      strcat(_record,_delim);

      /*Rewind orders file.*/
      rewind(PtrFileB);

      /*Reset orders counts*/
      _orders =0;

  while (fgets(currentlineB, 20, PtrFileB))
  { 
                        
      /*Split line by delim ','*/
      parsedBuffer=strtok(currentlineB, ",");  
      
      /*Move to next field defined by delim*/      
      parsedBuffer=strtok(NULL, ","); 
      
      strncpy(_custidB,parsedBuffer,2);   
      _custidB[1] ='\0';

     if(strcmp(_custidA,_custidB) == 0 )
      {       
      /*Increment number of order for matched records*/
        _orders++;
      }
   }               
       /*Convert int to string */ 
      strcat(_record,_txtorders);    
      
      /*Write output to terminal*/
      printf("%s\n",_record  ); 
  }
} 
                   
   /*Close the files*/
   fclose(PtrFileA);
   fclose(PtrFileB); 
}


void update(char *filenameA , char *key, char *vars)
{              
  FILE *filePtr;
  char currentline[100]; 
  char outbuffer[100];
  char _custid[2];
  char _age[4];
  char name[20];
  char custid[2];
  char _delim[2];
  char *parsedBuffer;      
  int lcnt;
                
  /*Set file point to piped input*/
  filePtr = stdin;   
  
  /*Get passed parameters*/
  strncpy(_custid, key, 1);
  strncpy(_age, vars, 3);
   
  _delim[0] =',';
  _delim[1] ='\0';
  
  /*Init variable with blank characters.*/
  memset(currentline, ' ', 99);

  while (fgets(currentline, 100, filePtr))
  {
     /*Increment line count*/
     lcnt++;

     /*Skip the first line (the header).*/
     if (lcnt >1)
       {               
      memset(custid, ' ', 1);
      memset(outbuffer, ' ', 99);
      
      /*Get the first character of the line(the custid)*/
      strncpy(custid, currentline, 1);
      custid[1]='\0';

      /*If custId of current line equals passed custid do this...*/
     if(strcmp(custid,_custid) == 0 )
       {                  
           /*Split line by delim ','*/
           parsedBuffer=strtok(currentline, ",");
           parsedBuffer=strtok(NULL, ",");          

           strncpy(outbuffer,_custid,2);
           strcat(outbuffer,_delim);
           strcat(outbuffer,parsedBuffer);
           strcat(outbuffer,_delim);
           strcat(outbuffer,_age);
                                        
           /*Write output to terminal*/   
           fputs(outbuffer, stdout);
           /*Put a newline because not copied from currentline*/
           fputs("\r\n",stdout);
       }
     else
       {                   
           /*Write output to terminal*/        
           fputs(currentline, stdout);
       }
     }
  }
}
