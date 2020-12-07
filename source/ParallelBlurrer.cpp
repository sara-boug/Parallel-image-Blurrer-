


public: void  readImage(char* input_file) {
	    FILE* file = fopen(input_file, "rb");
        if(!file ) { 
            printf("no file found");
            return ; 
        }else {
            fread(image, 1, WIDTH*HEIGHT, file);
            fclose(file); 
           
        }
};


private: std::vector<int>  blurr_(Rectangle* sub_rect , int rectangles_per_process, char* input_file , char * output_file) { 
  std::cout<<input_file ; 
    readImage(input_file)  ; 
    std::cout<< sub_rect[0].start_i << "\n" ; 
    std::vector<int> avg  ; 
    for( int n =0 ; n < rectangles_per_process ; n++) { 
        Rectangle rect = sub_rect[n]; 
        for(int i = rect.start_i ; i< rect.end_i ; i++) { 
             for(int j= rect.start_j ; j<rect.end_j ; j++){ 
                  // std::vector<int> avg_one = oneColumnAvg(i , j) ; 
                  
                  avg.push_back(oneColumnAvg(i , j)); // concating the array 
             }

        }

    }
    writeToFile(output_file); 
    return avg; 

}

/*
  The process of parallelizing the blurring consists of : 
  - initially setting up the imagge array in the master process 
  - using MPI_SCATTER to assign a rectangle to each process  which are the slaves 
  - each process will blurr its part in parallel with the others 
  - gathering the whole through MPI_GATHER
*/
public:void parallelBlurr(char* input_file , char * output_file) { 
 
 // setup the environment
 MPI_Init( NULL , NULL);
 int world_rank  ; 
 MPI_Comm_rank( MPI_COMM_WORLD, &world_rank);
 int world_size ;
 MPI_Comm_size( MPI_COMM_WORLD ,  &world_size);
 // environmnet set up 



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
    blurr_(sub_rect, rect_per_process , input_file , output_file) ; 
  /*
    std::vector<int> vec = 
    sub_mask_result =   vec.data(); // getting the array the the corresponds to
    int array_size = 1000;

    /*MPI_Datatype rtype; 
    MPI_Type_contiguous( array_size , MPI_INT, &rtype ); 
    MPI_Type_commit( &rtype ); 

    int* all_mask_result = NULL ; 
    if(world_rank ==0  ) { 

        all_mask_result = (int*) malloc( sizeof(int) * world_size* array_size );

    }


    MPI_Gather( sub_mask_result,
                array_size ,  // send count 
                MPI_INT  ,
                all_mask_result, 
                array_size, 
                MPI_INT,
                0,
               MPI_COMM_WORLD);
                 std::cout<<"HELLOOO"<< "\n"; 

    if (world_rank ==0)
    {    
      /*for (int n =0 ; n <world_size* array_size  ; n++ ) {
           
          std::cout<< n <<"\n"; 
      } 
    }*/
    
    // cleaning up 
    free(sub_rect);
    MPI_Barrier( MPI_COMM_WORLD);
    MPI_Finalize();

}

