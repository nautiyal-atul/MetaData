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
#include "VideoShot.h"
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
#include "VideoShot.h"


using namespace cv; 
using namespace std;
using namespace boost;

class extractMetaData {
public:
    extractMetaData();
    extractMetaData(string, string, string, int, double, bool, bool, int, int); 
    extractMetaData(const extractMetaData& orig);
    virtual ~extractMetaData();
    
    //function to set class variables 
    void setClassVariables(); 
    //function for shot detection
    void extract();    
    //function to process frame difference
    void saveInXML();     
    //process shots
    void processShot(ofstream &);
    //process frames of the still shot
    void processStillShot(VideoCapture &, ofstream &, int, int, int &);
    //process frames of the moving shot
    void processMovingShot(VideoCapture &, ofstream &, int, int, int &);
    //save shot images as GIF
    void saveGIF(int, string); 
    //compute number of frames in a GIF image
    int computeFileJumpForGIF(int, int);  
    //load dictionaries 
    void loadDictionaries(); 
    
    
private:
   string videoFilePath, rootDirPath, faceCascadeName, gifDirPath, xmlName, dictionaryRootDir, srtDirPath, videoDictionaryPath;
   bool isProcessFace;
   bool isProcessOCR; 
   bool isSaveGIF;
   bool isUseDictionary; 
   bool dictionaryLoaded; 
   double shotChangeThreshold; 
   int noOfImagesInGIF;
   int frameJump; 
   int dictionaryFlag; 
   double fps; 
   
   Utility *util;
   LanguageDictionary *dictionary; 
   faceDetect *fd; 
   detectOCR *ocr; 
   VideoShot *videoShot;
        
};

#endif	/* EXTRACTMETADATA_H */

