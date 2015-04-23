/* 
 * File:   detectOCR.cpp
 * Author: atul
 * 
 * Created on 29 January 2015, 15:51
 */

#include "detectOCR.h"

detectOCR::detectOCR() 
{
    tesseractApi.Init("", "eng", tesseract::OEM_DEFAULT);
    tesseractApi.SetVariable("tessedit_char_whitelist", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    tesseractApi.SetPageSegMode(static_cast<tesseract::PageSegMode> (3));
}

detectOCR::detectOCR(const detectOCR& orig) {
}

detectOCR::~detectOCR() 
{
}

//method to detect OCR text 

string detectOCR::detectOCRusingTesseract(Mat &image) 
{
    Mat mserMask = createMSERMask(image); 
   // imshow("mser", mserMask); 
   // waitKey(30); 
    //set tesseract
    tesseractApi.SetImage((uchar*) mserMask.data, mserMask.cols, mserMask.rows, 1, mserMask.cols);
    //get the text
    string ocrText = string(tesseractApi.GetUTF8Text());
    
    boost::regex spaces("[ ]{2,}");
    ocrText = regex_replace(ocrText, spaces, " ");
//    trim(ocrText); 
    //return detected text
    return ocrText;
}

//save OCR in XML

void detectOCR::saveOcrXML(ofstream& ocrXML, string ocrText) 
{
    if(ocrText.length()>0)
    {
        //save face data in XML 
        ocrXML << "\t\t<OCR>" << parseTextForXML(ocrText) << "</OCR>" << endl;
    }
}


//parse text for xml. remove special characters

string detectOCR::parseTextForXML(string text) {
    text = boost::regex_replace(text, boost::regex("&"), "&amp");
    text = boost::regex_replace(text, boost::regex("<"), "&lt");
    text = boost::regex_replace(text, boost::regex(">"), "&gt");
    text = boost::regex_replace(text, boost::regex("\""), "&quot");
    text = boost::regex_replace(text, boost::regex("'"), "&pos");

    //trim all extra spaces

    return text;
}


//method to detect MSER regions and returns a binary mask holding MSER regions
Mat detectOCR::createMSERMask(Mat& grey) 
{
    /* Find MSER components */
    vector<vector < Point>> contours;

    int delta = 8;
    int minArea = 3;
    int maxArea = 2000;
    double maxVariation = 0.25;
    double minDiversity = .05; //.1
    int maxEvolution = 200; //100
    double areaThreshold = 1.01;
    double minMargin = 0.003; //0.03
    int edgeBlurSize = 0;
 
    MSER mser(delta, minArea, maxArea, maxVariation, minDiversity, maxEvolution,
            areaThreshold, minMargin, edgeBlurSize);

    mser(grey, contours, Mat());
    /* Create a binary mask out of the MSER */
    Mat mser_mask(grey.size(), CV_8UC1, Scalar(0));

    for (unsigned int i = 0; i < contours.size(); i++) {
        for (Point& point : contours[i])
            mser_mask.at<uchar>(point) = 255;
    }

    return mser_mask;
}

