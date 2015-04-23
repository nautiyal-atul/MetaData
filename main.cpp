//============================================================================
// Name        : main.cpp
// Author      : Atul
// Version     :
// Copyright   : CNGL
// Description : main.cpp : Defines the entry point for the console application.
//                 7 Input parameters.
//                 1. video file full path
//                 2. face cascade file full path
//                 3. threshold for shot selection. Value of threshold must be in between [0-1]
//                 4. boolean (0, 1) flag for face processing
//                 5. boolean (0, 1) flag for OCR processing
//                 6. boolean (0, 1) flag for GIF creation. Each shot will be saved as a GIF.
//                 7. interval between frames for GIF creation
//                 8. 
//                 OUTPUT
//                 1. meta data xml file will be created in /videoName/metaDataXML/videoMetaData.xml path
//                 2. GIF images will be created of flag is on in  /videoName/shotGIF/
//                
//Example command 
//                ./metadata /home/atul/CNGL/videos/tedTalks/BenGoldacre_2011G-480p.mp4 /home/atul/CNGL/Projects/haarcascades/haarcascade_frontalface_alt.xml 0.08 1 0  1 5

//Dependencies and libraries 
//                 1. tesseract library
//                 2. leptonica library
//                 3. boost library
//                 4. opencv2.3 library
//                 5. system commands "imagemagick" and "zlib" are used. Therefore, system must have imagemagick and zlib installed. 
//                 6. C++11 compiler
//============================================================================================================================================

#include <fstream>
#include<iostream>
#include <stdio.h>
#include <vector>
#include <exception>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/contrib/contrib.hpp"

#include "extractMetaData.h"


using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    string videoFilePath, faceCascadeFilePath, dictionaryRootPath;
    bool isProcessOCR, isSaveGIF;
    double threshold;
    int frameJump;
    int noOfImagesInGIF; 
    int dictionayFlag; 

    if (argc < 10) {
        cout << "!!please enter paths for Video, face cascade files, double threshold, three boolean (0 or 1) values indicating process face, ocr and gif and one value for frame jump for gif!!" << endl;
        return -1;
    }
    try
    {
        //read the input parameters
        videoFilePath = argv[1];
        faceCascadeFilePath = argv[2];
        dictionaryRootPath = argv[3];
        dictionayFlag = stoi(argv[4]);
        threshold = stod(argv[5]);
        isProcessOCR = stoi(argv[6]);
        isSaveGIF = stoi(argv[7]);
        frameJump = stoi(argv[8]);
        noOfImagesInGIF = stoi(argv[9]); 
    }
    catch(int e)
    {
        cout<< "Invalid input argument" << endl; 
    }
            

    //videoFilePath = "/home/atul/CNGL/videos/tedTalks/BenGoldacre_2011G-480p.mp4";
    //faceCascadeFilePath = "/home/atul/CNGL/Projects/haarcascades/haarcascade_frontalface_alt.xml";


    // extractMetaData *metaData = new extractMetaData(videoFilePath, faceCascadeFilePath, 0.08, 1, 1, 1, 5); 

    extractMetaData *metaData = new extractMetaData(videoFilePath, faceCascadeFilePath, dictionaryRootPath, dictionayFlag, threshold, isProcessOCR, isSaveGIF, frameJump, noOfImagesInGIF);
    //detect shots 
    metaData->extract();
    
    
    



    return 0;
}