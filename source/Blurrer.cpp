#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <bits/stdc++.h>
#include <sstream> 
#define WIDTH 1280 //image height and width constants 
#define HEIGHT 720
#define MASK_SIZE 100


class Blurrer {
	public:
	  char* input_file;  
      char* mask_file;
	  unsigned char image[WIDTH] [HEIGHT];
	  Blurrer( char* input_file_ , char* mask_file_) {
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

public: void readMask() { 
	    std::ifstream  mask(mask_file); 
        std::string line ; 
        if(! mask ) { 
            printf("no file mask found");
            return ; 
        }else {
         while(std::getline(mask, line)){  
             std::vector<std::string> result ; 
             std::istringstream iss(line);
             for(std::string line ; iss >> line; ) { 
                 result.push_back(line);
             } 
             std::stringstream intValue(result[0]); 
             int number = 0 ; 
             intValue >> number ; 
             std::cout << number << "\n";
         }
        }
}

public: void  columnAvg(int n) {  // avergaing the image column with space n (i-n, j-n) to (i+n, j+n)
	//printf( "%u", image[0][0]); 
     

};

public: void getCoords() { 
      printf( "%u" , image[250][325]) ;   
};
private: void master() {

	};

private:void slave() {

	};

};