#include "Dimension.cpp"
#include "Rectangle.cpp"
#include "FileHandler.cpp"
#include "mpi.h"
using namespace Dimension; 

/**
 *  This sub class contains a column with coordinates i , j  accompanied 
 * with its content which is the average 
 * */
class Average { 
   public:   int i ; 
   public:   int j ; 
   public:   int avg ; 
   public: Average(int i_, int j_ , int avg_){
           i = i_ ; 
           j = j_; 
           avg = avg_ ; 
       }
};
/**
 *  This class performs the blurring, offerring two options either sequnetial blurring or parallel blurring 
 * 
 * */

class Blurrer { 
  
public: 
    unsigned char image[HEIGHT][WIDTH];
    FileHandler fileHandler ; 
    int neighbourhood ; 
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
    *   - to a given area size n
    *  - to a given a given coordinates (i , j)
    * and modify the 2D array provided  
    * */
   private: Average oneColumnAvg(int i , int j  , bool display) { 
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
     int average = (int) sum / number ; 
     if (display ) {  image[i][j] =  average ;  } 
     Average avg(i , j ,average );

     return avg ; 
   }

   /**  Handling the overflow that may occur horizontally : 
    *   if the value is negative : 0 is returned ; 
    *   if the value  is supprior than the width : width-1 is returned 
    *   which is considered as the suitable way to handle horizontal overflow
    **/
   private: int horizontalBorder(int i ) { 
       if( i < 0 ) {   return 0 ;  }
       else if (i >= WIDTH) { return WIDTH-1 ; }
       else { return i; } //  nothing is done elsewise   
   }
   /**  Handling the overflow that may occur vertically : 
    *   if the value is negative : 0 is returned ; 
    *   if the value  is supprior than the height : height-1 is returned 
    *   which is considered as the suitable way to handle vertical overflow
    **/
   private: int verticalBorder(int j) { 
       if( j < 0 ) {   return 0 ;  }
       else if (j>= HEIGHT) { return HEIGHT-1 ; }
       else { return j ;} //  nothing is done elsewise   

   }
    /**
     *  This method  is used to  write the image  to a given  file path
     * */ 
   public: int  writeToFile(char* path) { 
    std::ofstream output_file(path) ; 
    if( ! output_file) { 
        printf("output file not find "); 
        return 0 ; 
    } else {
        output_file.write((char*) &image , sizeof(image)); 
        output_file.close(); 
       return 1 ; 
    }
 };

/** 
 * This method represents a simple sequantial blurring 
 */
 public: void blurr() { 
    std::vector<Rectangle> rectangles = fileHandler.rectangles ; 
     
    for( int n =0 ; n < sizeof(rectangles) ; n++) { 
        Rectangle rect = rectangles[n]; 
        for(int i = rect.start_i ; i< rect.end_i ; i++) { 

            for(int j= rect.start_j ; j<rect.end_j ; j++){ 
               oneColumnAvg(i , j , true ); 
            }

        }

    }

}

/** 
 * This method recieves a number of rectangles and does the blurring accordingly, 
 * used only for implementing the parallel blurring 
 * returning Average object countaing i ,i coordinate and the avg obtained  at the spcific coorrdinates and neighbourhood 
*/
private: std::vector<Average> blurr_(Rectangle* sub_rect , int rectangles_per_process) { 
  
    std::vector<Average> results   ; 

    for( int n =0 ; n < rectangles_per_process ; n++) { 
        Rectangle rect = sub_rect[0]; 
        for(int i = rect.start_i ; i< rect.end_i ; i++) { 
             for(int j= rect.start_j ; j<rect.end_j ; j++){ 
                 results.push_back( oneColumnAvg(i , j , false ) ); // concating the array 
             }
        }
    }
   return results ; 
}

// Im plementing the parallel blurring 
public:void parallelBlurr() { 

 //  Environment set up 
 MPI_Init( NULL , NULL);
 int world_rank  ; 
 MPI_Comm_rank( MPI_COMM_WORLD, &world_rank);
 int world_size ;
 MPI_Comm_size( MPI_COMM_WORLD ,  &world_size);
 

  // Initializing the rectangles 
   Rectangle* rectangles ; 
   int rect_per_process  ;  // defines the number of rectangles that would be assigned to each process  
   std::vector<Average> modified_part; 
   Average * modified_part_array ;
   int size ;
   
    // setting up custom datatype for each rectangle (considering that rectangle is made of 4 integer points )
    MPI_Datatype MPI_Rectangle  ; 
    MPI_Type_contiguous( 4, MPI_INT, &MPI_Rectangle);
    MPI_Type_commit( &MPI_Rectangle);
   
   // Master is about to distrubute tasks equally among the  available processes  
    if(world_rank == 0 ) {  // Master 
        rectangles  = fileHandler.rectangles.data();  // setting up the rectangle array 
        // in order to get an evne number of rectangles per process 
        // the availabe rectangles are devided by the number of processes 
         rect_per_process = (int) (fileHandler.rectangles.size() / world_size ); 
    }  
    // Allocating memory buffers for the rectangles 
    Rectangle *sub_rect =(Rectangle *) malloc( sizeof(Rectangle) * rect_per_process);
    // distributing the rectangles to processes evenly 
    MPI_Scatter( rectangles, // rectangle array to be distributed
                 rect_per_process, // elements per process 
                 MPI_Rectangle ,    // data type in the root process
                 sub_rect ,      // the allocated memory buffer for each process 
                 rect_per_process , 
                MPI_Rectangle ,
                0 ,      // root 
                MPI_COMM_WORLD);
    
    modified_part = blurr_(sub_rect, rect_per_process ) ; 
   
    Average* all_modified_parts = NULL ; 
    if(world_rank ==0  ) { 
            // The vector should be transformed to an array, 
            //since the returned value from the blurr_ method is a vector which can not be accepted 

            modified_part_array =  modified_part.data();  
            size = modified_part.size(); 

            // allocating memory for the gather procedure 
            all_modified_parts = (Average*) malloc( sizeof(Average) * size *world_size) ; 

    }
    
    // Creating  new Mpi data type that would corresponds to the Average object 
    MPI_Datatype MPI_Average ; 
    MPI_Type_contiguous( 3, MPI_INT, &MPI_Average);
    MPI_Type_commit( &MPI_Average);
    
    
    MPI_Gather( modified_part_array, // the array generated previously 
                size,  // send count 
                MPI_Average , // the custom data type created  
                all_modified_parts, // the allocated memory 
                size,  // size of the element that is returned per process  
               MPI_Average,
                0,   // the process to gather in data 
                MPI_COMM_WORLD);

    if (world_rank ==0)
    {   // Modifying the image 
            for(int n =0 ; n <size*world_size ; n++ ) { 
                Average a = all_modified_parts[n];
                image[a.i][a.j] = a.avg; 
            }
    }
    // cleaning up 
    MPI_Finalize();
}


};

