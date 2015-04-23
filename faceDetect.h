/* 
 * File:   faceDetect.h
 * Author: atul
 *
 * Created on 28 January 2015, 11:07
 */

#ifndef FACEDETECT_H
#define	FACEDETECT_H

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv; 

class faceDetect 
{
public:
    faceDetect();
    faceDetect(string); 
    faceDetect(const faceDetect& orig);
    virtual ~faceDetect();
    
    void detect(Mat&);
    int loadCascade();
    void draw();
    void saveFaceXML(ofstream&); 
    int getFacesSize(); 
    vector<Rect> getFaces(); 
    
private:
   string faceCascadeName;
   CascadeClassifier faceCascade;
   vector<Rect> faces; 
   Mat frame;
};

#endif	/* FACEDETECT_H */

