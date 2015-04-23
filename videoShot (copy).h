/* 
 * File:   videoShot.h
 * Author: atul
 *
 * Created on 27 January 2015, 12:37
 */

#ifndef VIDEOSHOT_H
#define	VIDEOSHOT_H

#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"

using namespace std; 
using namespace cv; 

struct Shot
{
    int shotIndex; 
    int startFrame;  
    int endFrame; 
    double averageMotion; 
    string classification; 
};

class videoShot 
{
public:
    videoShot();
    videoShot(int , int , int , double ) ;
    videoShot(const videoShot& orig);
    virtual ~videoShot();
    
    void writeXMLNode(ofstream &); 
    int getShotIndex();
    int getStartFrame();
    int getEndFrame();
    double getAverageMotion();
    

private:
    int shotIndex; 
    int startFrame; 
    int endFrame; 
    double averageMotion; 
    string faces; 
    string classification; 
    static const string faceValue[];  
    static const string classificationValue[]; 
    
    Shot *shot; 

    
    
    
};

#endif	/* VIDEOSHOT_H */

