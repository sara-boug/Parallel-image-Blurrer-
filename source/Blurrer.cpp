#include "Dimension.cpp"
#include "Rectangle.cpp"
#include "FileHandler.cpp"
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
/* This method is used to copy  the file handler  image to this file to  allow modifying it  */ 
public: void setImage() { 
   for (int i=0 ; i< WIDTH ; i++ ) { 
       for(int j=0 ; j<HEIGHT ; j++){ 
         image[j][i] = fileHandler.image[j][i] ;     
       }
   }
} 

public: void blurr() { 
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
   /* This methods averages  a single column according :
       - to a given area size n
       - to a given a given coordinates (i , j)
     and modify the 2D array provided in the constructor */
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


};

