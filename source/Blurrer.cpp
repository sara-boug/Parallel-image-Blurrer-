#include "Dimension.cpp"
#include "Rectangle.cpp"
#include "FileHandler.cpp"
#include "mpi.h"
using namespace Dimension; 


// this  object contains a column with coordinates i , j  accompanied with its content wich is the average
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
   int  n =0 ; 
   for (int i=0 ; i< WIDTH ; i++ ) { 
       for(int j=0 ; j<HEIGHT ; j++){ 
         image[i][j] = fileHandler.image[j][i] ;    
          n++ ;  
       }
   }
} 
   /* This methods averages  a single column according :
       - to a given area size n
       - to a given a given coordinates (i , j)
     and modify the 2D array provided  */
   private: int oneColumnAvg(int i , int j ) { 
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
     //image[i][j] =  average ; 
     std::vector<int> avg  ; 
     avg.push_back(i);
     avg.push_back(j);
     avg.push_back(average);
     return average; 
    // std::cout << (int ) sum / number << "  " ; 

   }

      private: void  oneColumnAvg2(int i , int j ) { 
            MPI_Init( NULL , NULL);
    int world_rank  ; 
    MPI_Comm_rank( MPI_COMM_WORLD, &world_rank);
    int world_size ;
    MPI_Comm_size( MPI_COMM_WORLD ,  &world_size);
    // environmnet set up 
    if ( world_rank == 0) {
        printf("hello world");
     } 
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
     image[i][j] =  average ; 


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
               oneColumnAvg2(i , j); 
            }

        }

    }

}

private: std::vector<int>  blurr_(Rectangle* sub_rect , int rectangles_per_process) { 
    std::cout<< sub_rect[0].start_i << "\n" ; 
    std::vector<int> avg  ; 
    for( int n =0 ; n < rectangles_per_process ; n++) { 
        Rectangle rect = sub_rect[n]; 
        for(int i = rect.start_i ; i< rect.end_i ; i++) { 
             for(int j= rect.start_j ; j<rect.end_j ; j++){ 
                 // std::vector<int> avg_one = oneColumnAvg(i , j) ; 
                  //avg.insert(std::end(avg), std::begin( avg_one ), std::end( avg_one ));
                 avg.push_back(oneColumnAvg(i , j)); // concating the array 
             }

        }

    }
    return avg; 

}


public:void parallelBlurr2() { 
    setImage() ; 
    std::vector<Rectangle> rectangles = fileHandler.rectangles ; 
    int value = fileHandler.rectangles.size();
    int * send ; 
    int *recv; 
        // setup the environment
    MPI_Init( NULL , NULL);
    int world_rank  ; 
    MPI_Comm_rank( MPI_COMM_WORLD, &world_rank);
    int world_size ;
    MPI_Comm_size( MPI_COMM_WORLD ,  &world_size);
    // environmnet set up 
    if ( world_rank == 0) {
                MPI_Send(image,  HEIGHT*WIDTH,  MPI_INT , 1, 0, MPI_COMM_WORLD);

    } else {
            recv = (int*) malloc( sizeof(int) * HEIGHT*WIDTH);

//            MPI_Recv( , HEIGHT*WIDTH, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);        
    
    }
    // cleaning up 
    MPI_Barrier( MPI_COMM_WORLD);
    MPI_Finalize();



};

/*
  The process of parallelizing the blurring consists of : 
  - initially setting up the imagge array in the master process 
  - using MPI_SCATTER to assign a rectangle to each process  which are the slaves 
  - each process will blurr its part in parallel with the others 
  - gathering the whole through MPI_GATHER
*/
public:void parallelBlurr() { 
 

 // setup the environment
 MPI_Init( NULL , NULL);
 int world_rank  ; 
 MPI_Comm_rank( MPI_COMM_WORLD, &world_rank);
 int world_size ;
 MPI_Comm_size( MPI_COMM_WORLD ,  &world_size);
 // environmnet set up 


   int array_size ;
  // initializing the rectangles 
   Rectangle* rectangles ; 
   int* sub_mask_result ;
   int rect_per_process = 1 ;  // defines the number of rectangles that would be assigned to each process  
  
  
    // setting up custom datatype for each rectangle (considering that rectangle is made of 4 integer points )
    MPI_Datatype MPI_Rectangle  ; 
    MPI_Type_contiguous( 4 , MPI_INT, &MPI_Rectangle);
    MPI_Type_commit( &MPI_Rectangle);
   
    if(world_rank == 0 ) { // Master 
        rectangles  = fileHandler.rectangles.data();  // setting up the rectangle array 
        
    }  
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
    
    std::vector<int> vec = blurr_(sub_rect, rect_per_process) ; 
    sub_mask_result =   vec.data(); // getting the array the the corresponds to
    array_size =vec.size();
          std::cout<<array_size <<"\n"; 

    MPI_Datatype rtype; 
    MPI_Type_contiguous( array_size , MPI_INT, &rtype ); 
    MPI_Type_commit( &rtype ); 

    int* all_mask_result = NULL ; 
    if(world_rank ==0  ) { 

        all_mask_result = (int*) malloc( sizeof(int) * world_size* array_size*10);

    }


    MPI_Gather( sub_mask_result,
                array_size,  // send count 
                MPI_INT  ,
                all_mask_result, 
                1,  // nummber per process 
                rtype,
                0,
               MPI_COMM_WORLD);
                 std::cout<<"HELLOOO"<< "\n"; 

    if (world_rank ==0)
    {    
        int n =0 ; 
       while(n<world_size* array_size  ) {
           std::cout << sub_mask_result[n] <<"\n"; 
           //image[sub_mask_result[n]][sub_mask_result[n+1]] = sub_mask_result[n+2];
           n=n +1; 
      } 
    }
    
    // cleaning up 
    free(sub_rect);
    MPI_Barrier( MPI_COMM_WORLD);
    MPI_Finalize();

}



};

