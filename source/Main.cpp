#include <iostream>
#include<filesystem>
#include<unistd.h>
#include "FileHandler.cpp"
#include "Blurrer.cpp"
#include<string>
#include <mpi.h>
/** 
 * @author Bouglam sara 
 *
 * */
int main(int argc , char * argv[])
{    
     // extracting arguments from the command line 
     if (argc ==  1 )  {
          printf("NO ARGUMENT PROVIDED !!\n                                                                                                                                                           ");
     }else {
           char*  mask_file; // mask file 
           char* input_file;// image file
           char* output_file;
           bool parallel = false , mask = false , input = false , output = false ; 
           int neighbours = 10 ; 
           // Handlnig the command Line arguments
           for (int i=1; i < argc; i++ ) { 
                std::string arg = argv[i];
                if( arg == "-input") { 
                        input_file = argv[i+1] ; 
                        input = true ; 
                        i ++ ; 
                    }else if(arg =="-output") { 
                         output_file = argv[i+1] ; 
                         output= true ; 
                          i ++ ; 

                    } else if(arg=="-mask" ) { 
                        mask_file = argv[i+1] ; 
                        mask = true ; 
                        i ++ ; 

                    }else if(arg== "-p") { 
                          parallel = true ; 
                  

                    }else if (arg =="-n"){ 
                         std::string arg = argv[i+1];
                         std::stringstream intValue(arg); 
                         int number = 0 ; 
                         intValue >> number ; 
                         neighbours = number ; 
                         i ++ ; 

                    }else  { 
                    printf("INVALID ARGUMENT PROVIDED !!\n");
               }
           }
           if(!mask|| !input|| !output) { 
                 printf("NONE SUFFICIENT ARGUMENT PROVIDED !! \n");
           }else{
               /* Initilizing the files and important data to process */
               FileHandler fileHandler;
               fileHandler.input_file = input_file; 
               fileHandler.mask_file = mask_file; 
               
               // Read the input file and transform it into a 2D array 
                if ( fileHandler.readImage() == 0 )  {  return 0 ; }
                // It allows  generating  the rectangles vector for later use 
                if (   fileHandler.readMask() == 0 )  {return 0 ;}
               /** Initializing the blurrer class **/
               Blurrer blurrer(fileHandler,10);  
               blurrer.setImage();
               // The blurring can either be executed parallely or sequentially 
               if (parallel == false ) {  
                    blurrer.blurr();  // squentially
               } else { 
                   blurrer.parallelBlurr();  // parallely 
               }
               // writing the obtained result to file 
               //if file not found the the program will be aborted 
               if( blurrer.writeToFile(output_file)== 0) { return 0;} 

               }
           
          
     }


};
