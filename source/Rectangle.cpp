#pragma once
/*
 This class describes the  coordinats that are available in the mask file  
 the mask file contains several lines, each line describes a rectangle such that : 
 <start_i> <start_j> <stop_i> <stop_j>
 If the file does not respect of the above format, Exception would be raised

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