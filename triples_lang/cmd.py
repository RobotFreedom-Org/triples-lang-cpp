"""

 
 g++ -shared -fPIC triples_lang_lib.cpp -o triples_lang_lib.so
 g++ -shared -fPIC  -o triples_lang_lib.so triples_lang_lib.cpp
"""
import sys 
import ctypes

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-f", "--file", default=None)  
parser.add_argument("-l", "--library", default=None)   
parser.add_argument("-p", "--parameters", default=None)   
parser.add_argument("-r", "--routine", default=None)   
parser.add_argument("-t", "--type", default="jsonl")   

lib = ctypes.CDLL("triples_lang_lib.so") 

#mylib = ctypes.CDLL(os.path.abspath(libname))

# Determine library name based on OS
if sys.platform.startswith("win"):
    libname = "mylib.dll"
elif sys.platform == "darwin":
    libname = "libmylib.dylib"
else:
    libname = "libmylib.so"
 
lib.run.argtypes = (ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p )
lib.run.restype = ctypes.c_char_p 
#result = lib.run( ctypes.c_char_p('echo'.encode('utf-8')), ctypes.c_char_p('hello'.encode('utf-8')) ,ctypes.c_char_p(''.encode('utf-8')))
#print(result) 

if __name__ == '__main__': 
     """
     cat simple_data.jsonl | python triples-lang.py -f stdin -l simple_engine.trpl -r test -p prompt,response
     python triples-lang.py -f simple_data.jsonl  -l simple_engine.trpl -r test -p prompt,response
     """

     args          = parser.parse_args()  
     in_file       = args.file  
     in_libray     = args.library  
     in_routine    = args.routine  
     in_params     = args.parameters  
     in_file_type  = args.type   
   
     if in_file is not None:
          """ 
          if in_libray is not None:

               triples.process("library"     , in_libray)
               triples.process("routine"     , in_routine)
               triples.process("map"         , in_params.split(",")) 
               triples.process("format"      , in_file_type) 

               if  in_file.startswith("stdin"): 
                 for line in sys.stdin.read():
                     resp  =  triples.process("row", line.strip())
                     print(resp)
               else: 
                 for line in open("simple_data.jsonl"):  
                     resp  =  triples.process("row", line.strip())
                     print(resp)

          else: 
              resp  =  triples.run(in_file) 
              for line in resp:
                  print(line)
         """
     else:     
        while True:
          
            cmds = input(">")
            if cmds.endswith(";"):
                cmds = cmds[:-1]

            if cmds == "bye": 
                 sys.exit()
                 
            obj, prep = "",""
  
            if cmds.find(" ") > -1: 
                cmd, params =  cmds.split(" ",1) #split_ignoring_quotes(cmds,1)  
                cmd = cmd.strip()
  
                if params.find(" ") > -1: 
                    obj, prep = params.split(" ",1) # split_ignoring_quotes(params , 1) 
                else:
                    obj =  params.strip()
            else:
                cmd, obj, prep = cmds , "", ""
  
            resp =   cmd , obj, prep

            result = lib.run( ctypes.c_char_p(cmd.encode('utf-8')), 
                              ctypes.c_char_p(obj.encode('utf-8')) ,
                              ctypes.c_char_p(prep.encode('utf-8')))
            print( result)
