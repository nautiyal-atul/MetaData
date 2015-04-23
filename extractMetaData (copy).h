/* 
 * File:   extractMetaData.h
 * Author: atul
 *
 * Created on 26 January 2015, 10:20
 * Class responsible for extracting data from the video
 */

#ifndef EXTRACTMETADATA_H
#define	EXTRACTMETADATA_H

#include "detectOCR.h"
#include "videoShot.h"
#include "faceDetect.h"

#include <algorithm> 
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Utility.h"
#include "LanguageDictionary.h"
#include "FrameDifference.h"


using namespace cv; 
using namespace std;
using namespace boost;

class extractMetaData {
public:
    extractMetaData();
    extractMetaData(string, string, string, int, double, bool, bool, int, int); 
    extractMetaData(const extractMetaData& orig);
    virtual ~extractMetaData();
    
    //member functions 
    void detectAndAnalyzeFace(Mat &);
    void detectSlides();
    void detectVideo(); 
    void readOCR(Mat &); 
    
    //function for shot detection
    void shotDetection(); 
    
    

    
    //function to process frame difference
    void classifyFrames(vector<double> &, vector<double> &, vector<videoShot*> &);    
    void saveInXML(vector<videoShot*> &, string); 
    double averageMotion(vector<double> &, int, int);
    void saveFaceXML(ofstream &, vector<Rect> &, int);
    
private:
   string videoFilePath, rootDirPath, faceCascadeName, gifDirPath, xmlDirPath, xmlName, dictionaryRootDir, srtDirPath, videoDictionaryPath;
   bool isProcessFace;
   bool isProcessOCR; 
   bool isSaveGIF;
   bool isUseDictionary; 
   bool dictionaryLoaded; 
   double shotChangeThreshold; 
   int noOfImagesInGIF;
   int frameJump; 
   Utility *util;
   LanguageDictionary *dictionary; 
   FrameDifference *frameDifference; 
};

#endif	/* EXTRACTMETADATA_H */

