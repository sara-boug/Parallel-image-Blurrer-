#include "Dimension.cpp"
#include "Rectangle.cpp"
#include "FileHandler.cpp"
#include "mpi.h"
using namespace Dimension; 

class Blurrer { 
  
public: 
    unsigned char image[HEIGHT][WIDTH];
    FileHandler fileHandler ; 
    int neighbourhood ; 
    int width ; //  number of the image pixels horizontally 
    int height; //  number of the image pixels vertically 
    Blurrer( FileHandler fileHandler_,int neighbourhood_ ) {     
       fileHandler = fileHandler_; 
       neighbourhood = neighbourhood_;
    
   };
/* This method is used to copy  the file handler  image to this file to  allow modifying it ! */ 
public: void setImage() { 
   for (int i=0 ; i< WIDTH ; i++ ) { 
       for(int j=0 ; j<HEIGHT ; j++){ 
         image[j][i] = fileHandler.image[j][i] ;     
       }
   }
} 
   /* This methods averages  a single column according :
       - to a given area size n
       - to a given a given coordinates (i , j)
     and modify the 2D array provided  */
   private: void oneColumnAvg(int i , int j ) { 
      int start_j = horizontalBorder( j-neighbourhood ) ;
      int start_i = verticalBorder(i-neighbourhood) ; 
      int end_j =   horizontalBorder(j + neighbourhood); 
      int end_i =   verticalBorder(i + neighbourhood); 
      // average = sum/ number
      int sum =0 ; // Will be used to keep track of the value in order to average them in the end 
      int number =0;  // keep track of the number of pixels
      for(int i1=start_i; i1<end_i; i1 ++ ){
          for(int j1=start_j; j1<end_j ; j1++) { 
              sum = sum + image[i1][j1]; 
              number = number + 1 ; 

          }
      }
      
     image[i][j] =  (int) sum / number ; 
    // std::cout << (int ) sum / number << "  " ; 

   }
   /*  Handling the overflow that may occur horizontally : 
       if the value is negative : 0 is returned ; 
       if the value  is supprior than the width : width-1 is returned 
       which is considered as the suitable way to handle horizontal overflow*/
   private: int horizontalBorder(int i ) { 
       if( i < 0 ) {   return 0 ;  }
       else if (i >= WIDTH) { return WIDTH-1 ; }
       else { return i; } //  nothing is done elsewise   
   }
  /*  Handling the overflow that may occur vertically : 
       if the value is negative : 0 is returned ; 
       if the value  is supprior than the height : height-1 is returned 
       which is considered as the suitable way to handle vertical overflow*/
   private: int verticalBorder(int j) { 
       if( j < 0 ) {   return 0 ;  }
       else if (j>= HEIGHT) { return HEIGHT-1 ; }
       else { return j ;} //  nothing is done elsewise   

   }

   public: void writeToFile(char* path) { 
    std::ofstream output_file(path) ; 
    output_file.write((char*) &image , sizeof(image)); 
    output_file.close(); 
 };


 public: void blurr() { 
    setImage() ; // setting up the 2D array 
    std::vector<Rectangle> rectangles = fileHandler.rectangles ; 
    std::cout << fileHandler.rectangles.size() << "\n";   
    for( int n =0 ; n < sizeof(rectangles) ; n++) { 
        Rectangle rect = rectangles[n]; 
        for(int i = rect.start_i ; i< rect.end_i ; i++) { 
            for(int j= rect.start_j ; j<rect.end_j ; j++){ 
               oneColumnAvg(i , j);
            }

        }

    }

}

private: Rectangle* vectorToArray()  {
    Rectangle* rectangles_arr = NULL ;  
    std::vector<Rectangle> rectangles = fileHandler.rectangles ;
    for( int n =0 ; n < sizeof(rectangles) ; n++) { 
         std::cout<< n  ; 
         rectangles_arr[n] = rectangles[n];    
       }
    return rectangles_arr;
}

private: void  blurr_(Rectangle* sub_rect , int rectangles_per_process) { 
    for( int n =0 ; n < rectangles_per_process ; n++) { 
        Rectangle rect = sub_rect[n]; 
        for(int i = rect.start_i ; i< rect.end_i ; i++) { 
            for(int j= rect.start_j ; j<rect.end_j ; j++){ 
               oneColumnAvg(i , j);
            }

        }

    }

}

/*
  The process of parallelizing the blurring consists of : 
  - initially setting up the imagge array in the master process 
  - using MPI_SCATTER to assign a rectangle to each process  which are the slaves 
  - each process will blurr its part in parallel with the others 
  - gathering the whole through MPI_GATHER
*/
public:void parallelBlurr() { 
 setImage() ; // setting up the image 

 // setup the environment
 MPI_Init( NULL , NULL);
 int world_rank  ; 
 MPI_Comm_rank( MPI_COMM_WORLD, &world_rank);
 int world_size ;
 MPI_Comm_size( MPI_COMM_WORLD ,  &world_size);
 // environmnet set up 



  // initializing the rectangles 
   Rectangle* rectangles ; 
   int rect_per_process = 1 ;  // defines the number of rectangles that would be assigned to each process  
  
  
    // setting up custom datatype for each rectangle (considering that rectangle is made of 4 integer points )
    MPI_Datatype MPI_Rectangle  ; 
    MPI_Type_contiguous( 4 , MPI_INT, &MPI_Rectangle);
    MPI_Type_commit( &MPI_Rectangle);
   
    if(world_rank == 0 ) { // Master 
        rectangles  = fileHandler.rectangles.data(); 
    }

    printf("I am in another rank "); 
    // Allocating memory buffers for the rectangles 
    Rectangle *sub_rect =(Rectangle *) malloc( sizeof(Rectangle) * rect_per_process);
    MPI_Scatter( rectangles,  // root process: Master
                 rect_per_process, // elements per process 
                 MPI_Rectangle ,    // data type in the root process
                 sub_rect ,      // the allocated memory buffer for each process 
                 rect_per_process , 
                MPI_Rectangle ,
                0 ,      // root 
                MPI_COMM_WORLD);
    blurr_(sub_rect, rect_per_process); 
    free(sub_rect);
    MPI_Barrier( MPI_COMM_WORLD);
    MPI_Finalize();

}



};

