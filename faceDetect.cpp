/* 
 * File:   faceDetect.cpp
 * Author: atul
 * 
 * Created on 28 January 2015, 11:07
 */

#include "faceDetect.h"

faceDetect::faceDetect() 
{
}


faceDetect::faceDetect(string faceCascadeName)
{
  this->faceCascadeName = faceCascadeName;   
}

//load cascade
int faceDetect::loadCascade()
{
  if (!faceCascade.load(faceCascadeName)) 
  {
        cout<<"--(!)Error loading in cascade file" << endl;;
  } 
  else
  {
      cout<< "Face cascade loaded" << endl; 
  }
}


faceDetect::faceDetect(const faceDetect& orig) 
{
}

faceDetect::~faceDetect() 
{
}


//detect face in the video frame
void faceDetect::detect(Mat &inputFrame) 
{  
    //clear face vector
    faces.clear(); 
    //convert frame to gray
    if(inputFrame.depth()>2)
    {
       cvtColor(inputFrame, this->frame, CV_BGR2GRAY);
    }
    else
    {
         this->frame = inputFrame.clone();
    } 
  
    Mat frameResize; 
   /*
    //use resize frame for face detection 
    if(frame.cols > 720 && this->frame.rows >640)
    {
        frameResize = Mat::zeros(720, 640, this->frame.type()); 
        resize(frame, frameResize, frameResize.size(), (double)frameResize.cols/frame.cols, (double)frameResize.rows/frame.rows, INTER_LINEAR);
        equalizeHist(frameResize, frameResize); 
    }
    else
    {
       frameResize = this->frame.clone(); 
    }  
    */
    frameResize = this->frame.clone(); 
    faceCascade.detectMultiScale(frameResize, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(21, 21));
    
    //rescale face coordinates 
    for(int ii = 0; ii<faces.size(); ii++)
    {
       faces[ii].x =  (double)faces[ii].x * (double)frame.cols/frameResize.cols; 
       faces[ii].y =  (double)faces[ii].y * (double)frame.rows/frameResize.rows; 
       faces[ii].width =  (double)faces[ii].width * (double)frame.cols/frameResize.cols; 
       faces[ii].height =  (double)faces[ii].height * (double)frame.rows/frameResize.rows;    
    }
}

//draw faces
void faceDetect::draw()
{
    
    Mat multiChannelFrame; 
    vector<Mat> channels;
    channels.push_back(frame);
    channels.push_back(frame);
    channels.push_back(frame);
    //three channel gray scale image
    merge(channels, multiChannelFrame);
    
    namedWindow("Face", CV_WINDOW_AUTOSIZE);
    RNG rng(12345);
    
    //for each face
    for(int ii = 0; ii<this->faces.size(); ii++)
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) ); 
        rectangle(multiChannelFrame, this->faces[ii], color, 2, 8, 0);
    }    
    imshow("Face", multiChannelFrame);
    waitKey(30);  
    destroyWindow("Face");
}

//save faces coordinates in XML
void faceDetect::saveFaceXML(ofstream& faceXML)
{
    if(faces.size()>0)
    {
        //save face data in XML 
        for (int ii = 0; ii<faces.size(); ii++) 
        {
            faceXML<<"\t\t\t<Face>"<<faces[ii].x<<","<<faces[ii].y<<","<<faces[ii].width<<","<<faces[ii].height<<"</Face>"<<endl;
        }
    }
}

vector<Rect>faceDetect::getFaces()
{
    return faces; 
}

int faceDetect::getFacesSize()
{
    return faces.empty()?-1:faces.size();  
}

