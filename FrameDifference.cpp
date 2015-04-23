/* 
 * File:   FrameDifference.cpp
 * Author: atul
 * 
 * Created on 25 March 2015, 17:08
 */

#include "FrameDifference.h"

FrameDifference::FrameDifference() {
}

FrameDifference::FrameDifference(const FrameDifference& orig) {
}

FrameDifference::~FrameDifference() {
}


// sum of absolute difference 
double FrameDifference::sumOfAbsoluteDifference(Mat &preGrayFrame, Mat &curGrayFrame)
{
      Mat differenceFrame = abs(preGrayFrame-curGrayFrame); 
      return double(sum(differenceFrame)[0])/ double((255*countNonZero(differenceFrame)));    
}


//get histogram difference based on given method 1 = correlation, 2 = Chi Square; 3 = intersection and 
// 4 = Bhattacharya Distance
double FrameDifference::histogramDifference(Mat &preGrayFrame, Mat &curGrayFrame, int compare_method)
{
    Mat preHistImage, curHistImage; 
    //get histograms 
    computeNormalizedImageHistogram(preGrayFrame, preHistImage); 
    computeNormalizedImageHistogram(curGrayFrame, curHistImage); 
    
    //compare histograms
    return compareHist( preHistImage, curHistImage, compare_method );
}


//compute normalized histogram
void FrameDifference::computeNormalizedImageHistogram(Mat &grayFrame, Mat &histImage)
{
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = {0, 255} ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  /// Compute histogram
  calcHist(&grayFrame, 1, 0, Mat(), histImage, 1, &histSize, &histRange, true, false );
  
  //normalize histogram 
  normalize(histImage, histImage, 0, 255, NORM_MINMAX, -1, Mat() );
}


//get the edge change ratio
double FrameDifference::edgeChangeRatio(Mat &preGrayFrame, Mat &curGrayFrame)
{
    Mat preEdgeImage, curEdgeImage, preEdgeDilatedImage, curEdgeDilatedImage; 
    Mat preEdgeAndCurEdgeDialatedImage, curEdgeAndPreEdgeDialatedImage; 
    
    //1. get the edge map 
    /// Reduce noise with a kernel 3x3
    blur( preGrayFrame, preEdgeImage, Size(3,3) );
    /// Canny detector
    Canny( preEdgeImage, preEdgeImage, 50, 150, 3);
    //number of edge pixels 
    double preEdgePixels = double(countNonZero(preEdgeImage)); 
    
    //for second image
    /// Reduce noise with a kernel 3x3
    blur( curGrayFrame, curEdgeImage, Size(3,3) );
    /// Canny detector
    Canny( curEdgeImage, curEdgeImage, 50, 150, 3);
    //number of edge pixels 
    double curEdgePixels = double(countNonZero(curEdgeImage)); 
    
    
    //2. dilate and invert edge images
    int dilation_size = 5; 
    Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
    //previous image
    dilate( preEdgeImage, preEdgeDilatedImage, element );
    preEdgeDilatedImage = ~preEdgeDilatedImage; 
    //current image
    dilate( curEdgeImage, curEdgeDilatedImage, element );
    curEdgeDilatedImage = ~curEdgeDilatedImage;
    
    //3. bitwise and operations
    bitwise_and(preEdgeImage, curEdgeDilatedImage, preEdgeAndCurEdgeDialatedImage, Mat()); 
    bitwise_and(curEdgeImage, preEdgeDilatedImage, curEdgeAndPreEdgeDialatedImage, Mat());
    
    
    //4. count number of entering and existing edge pixels 
    double preExistingEdgePixels = double(countNonZero(preEdgeAndCurEdgeDialatedImage));
    double curExistingEdgePixels = double(countNonZero(curEdgeAndPreEdgeDialatedImage));
    
    //get the maximum ratio      
    return max(preExistingEdgePixels / preEdgePixels , curExistingEdgePixels/ curEdgePixels);  
}

