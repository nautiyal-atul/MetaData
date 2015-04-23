/* 
 * File:   detectOCR.h
 * Author: atul
 *
 * Created on 29 January 2015, 15:51
 */

#ifndef DETECTOCR_H
#define	DETECTOCR_H
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/features2d/features2d.hpp"
#include <boost/regex.hpp>

using namespace std;
using namespace cv; 

class detectOCR 
{
public:
    detectOCR();
    detectOCR(const detectOCR& orig);
    virtual ~detectOCR();
    string detectOCRusingTesseract(Mat &);
    void saveOcrXML(ofstream&, string);
    string parseTextForXML(string); 
    Mat createMSERMask(Mat&);     
private:
    tesseract::TessBaseAPI tesseractApi;
};

#endif	/* DETECTOCR_H */

