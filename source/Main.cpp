#include <iostream>
#include<filesystem>
#include<unistd.h>
#include "Blurrer.cpp"
#include<string>
#include <mpi.h>

int main()
{    
  
     std::string dir= get_current_dir_name();  // this command may only work on a linux env
     char path[dir.length()];
     char  mask[]=  "datas/mask1"; // mask file 
     char file_name [] ="datas/police1.raw"; // image file
     Blurrer blurrer(file_name, mask); 
     blurrer.readImage(); 
     blurrer.readMask(); 
     blurrer.getCoords(); 
     printf("Hello World!\n") ;

};
