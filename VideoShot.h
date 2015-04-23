/* 
 * File:   VideoShot.h
 * Author: atul
 *
 * Created on 27 January 2015, 12:37
 */

#ifndef VIDEOSHOT_H
#define	VIDEOSHOT_H

#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include "FrameDifference.h"
#include "Utility.h"

using namespace std; 
using namespace cv; 

struct Shot
{
    int shotIndex, startFrame, endFrame; 
    string startTime, endTime; 
    double averageMotion; 
    string classification; 
};

class VideoShot 
{
public:
    VideoShot();
    VideoShot(string, string, double, int); 
    VideoShot(const VideoShot& orig);
    virtual ~VideoShot();
    void computeFrameDifference(); 
    void shotDetection();
    double averageMotion(vector<double> &, int , int); 
    vector<Shot>& getVideoShotsVector(); 
    void writeXMLNode(ofstream&, Shot &); 
private:
    static const string classificationValue[];  
    Shot *shot; 
    vector<Shot> videoShotsVector; 
    string rootDirPath, videoFilePath;
    int frameJump; 
    double shotChangeThreshold, fps;
    FrameDifference *frameDifference; 
    vector<double> frameDifferenceScore, edgeChangeRatioValue;
    
    Utility *util; 
};

#endif	/* VIDEOSHOT_H */

