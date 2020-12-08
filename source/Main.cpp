#include <iostream>
#include<filesystem>
#include<unistd.h>
#include "FileHandler.cpp"
#include "Blurrer.cpp"
#include<string>
#include <mpi.h>

int main()
{    
  
     std::string dir= get_current_dir_name();  // this command may only work on a linux env
     char path[dir.length()];
     char  mask_file[]=  "datas/mask1"; // mask file 
     char input_file [] ="datas/police1.raw"; // image file
     char output_file_name[] = "datas/output.raw";
     FileHandler fileHandler;
     fileHandler.input_file = input_file; 
     fileHandler.mask_file = mask_file; 
     fileHandler.readImage(); 
     fileHandler.readMask(); 
     Blurrer blurrer(fileHandler,10); 
     blurrer.setImage();
     blurrer.parallelBlurr(); 
     blurrer.writeToFile(output_file_name);


};
