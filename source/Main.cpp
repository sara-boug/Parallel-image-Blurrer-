#include <iostream>
#include<filesystem>
#include<unistd.h>
#include "FileHandler.cpp"
#include<string>
#include <mpi.h>

int main()
{    
  
     std::string dir= get_current_dir_name();  // this command may only work on a linux env
     char path[dir.length()];
     char  mask[]=  "datas/mask1"; // mask file 
     char file_name [] ="datas/police1.raw"; // image file
     FileHandler fileHandler(file_name, mask); 
     fileHandler.readImage(); 
     fileHandler.readMask(); 
     printf("Hello World!\n") ;

};
