#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <bits/stdc++.h>
#include <sstream> 
#define WIDTH 1280 //image height and width constants 
#define HEIGHT 720
#define MASK_SIZE 100
/*
 This class describes the  coordinats that are available in the mask file  
 the mask file contains several lines, each line describes a rectangle such that : 
 <start_i> <start_j> <stop_i> <stop_j>

*/
class Rectangle { 
    public:
      int start_i ; 
      int start_j; 
      int end_i; 
      int end_j; 
      Rectangle(int start_i_ , int start_j_, int end_i_, int end_j_){ 
          start_i = start_i_; 
          start_j = start_j_ ; 
          end_i = end_i_; 
          end_j = end_j_; 
      }
        
};
/* 
This class handles the folwing functionalities : 
   - raw file reading and extracting its content 
   - extracting  the values from the mask file 
   - raw file writing from a 2D array 
  
*/
class FileHandler {
	public:
	  char* input_file;  
      char* mask_file;
	  unsigned char image[WIDTH] [HEIGHT];
      std::vector<Rectangle> rectangles;  // Rectangle List
	  FileHandler( char* input_file_ , char* mask_file_) {
      input_file = input_file_ ; 
	  mask_file = mask_file_; 

	};
public: void  readImage() {
	    FILE* file = fopen(input_file, "rb");
        if(!file ) { 
            printf("no file found");
            return ; 
        }else {
            fread(image, 1, WIDTH*HEIGHT, file);
            fclose(file); 
           
        }
};
/* 
 In order to extract the values from the mask file given, we need first to
  get the lines,  then we get the values that are sparated by space

   */
public: int  readMask() { 
	std::ifstream  mask(mask_file); 
    std::string line ; 
    if(! mask ) { 
        printf("no file mask found");
        return 0 ; 
    }else {
        // Extracting the lines
        while(std::getline(mask, line)){  
            // Getting the values that are separated by space
            std::vector<std::string> result ; 
            std::istringstream iss(line);
            for(std::string line ; iss >> line; ) { 
                result.push_back(line);
            } 
            // if there are less than 4 values then the process would be aborted 

            if ( result.size() == 4 ){                
                Rectangle rectangle(toInt(result[0]),
                                        toInt(result[1]),
                                        toInt(result[2]),
                                        toInt(result[3])) ; 
                rectangles.push_back(rectangle); 

            }else {
                printf("Malformed mask file, exactly 4 values are accepted per line");
                return 0 ; 
            }
         }
    }
    std::cout <<  rectangles.size() << "\n";
    // returns 1 in the case when the whole process succeeds
    return 1 ;
    
}
/* This method is used to convert string value to int  */ 
private: int toInt(std::string str ){ 
    std::stringstream intValue(str); 
    int number = 0 ; 
    intValue >> number ; 
    return number; 

}
public: void  columnAvg(int n) {  // avergaing the image column with space n (i-n, j-n) to (i+n, j+n) 

 };
};