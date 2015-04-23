/* 
 * File:   FrameDifference.h
 * Author: atul
 *
 * Created on 25 March 2015, 17:08
 */

#ifndef FRAMEDIFFERENCE_H
#define	FRAMEDIFFERENCE_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv; 

class FrameDifference 
{
public:
    FrameDifference();
    FrameDifference(const FrameDifference& orig);
    virtual ~FrameDifference();
    
    //functions to measure frame differences 
    double sumOfAbsoluteDifference(Mat &, Mat &); 
    double histogramDifference(Mat &, Mat &, int); 
    double edgeChangeRatio(Mat &, Mat &); 
    void computeNormalizedImageHistogram(Mat &, Mat &); 
    
private:

};

#endif	/* FRAMEDIFFERENCE_H */

