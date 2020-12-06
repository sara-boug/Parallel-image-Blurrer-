#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <sstream> 
#include "Rectangle.cpp"
#include "Dimension.cpp"
using namespace Dimension; 

/* 
This class handles the folwing functionalities : 
   - raw file reading and extracting its content 
   - extracting  the values from the mask file 
   - raw file writing from a 2D array 
  
*/
class FileHandler {
	  public : char* input_file;  
      public:  char* mask_file;
	  public: unsigned char image[HEIGHT][WIDTH];
      public: std::vector<Rectangle> rectangles;  // Rectangle List

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
            // If there are less than 4 values then the process would be aborted 

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
    
};
/* This method is used to convert string value to int  */ 
private: int toInt(std::string str ){ 
    std::stringstream intValue(str); 
    int number = 0 ; 
    intValue >> number ; 
    return number; 
};

// simple getters
public: std::vector<Rectangle> getMaskArray() { 
    return rectangles ; 
};

public: int setImage(int i , int j , int value ) { 
   return   image[j][i] = 0; 
}
};