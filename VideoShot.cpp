/* 
 * File:   videoShot.cpp
 * Author: atul
 * 
 * Created on 27 January 2015, 12:37
 */

#include "VideoShot.h"

 
VideoShot::VideoShot() 
{
} 

const string VideoShot::classificationValue[] = {"Still", "Video"}; 


VideoShot::VideoShot(string rootDirPath, string videoFilePath, double shotChangeThreshold, int frameJump)
{
    this->rootDirPath = rootDirPath; 
    this->videoFilePath = videoFilePath;
    this->shotChangeThreshold = shotChangeThreshold;
    this->frameJump = frameJump; 
    frameDifference = new FrameDifference(); 
}

VideoShot::VideoShot(const VideoShot& orig) 
{
}

VideoShot::~VideoShot() 
{

}

//write values in XML
void VideoShot::writeXMLNode(ofstream& videoXML, Shot &shot )
{		
    videoXML << "\t\t<UID>" <<shot.shotIndex<<"</UID>" << endl;
    videoXML << "\t\t<StartFrameNumber>" << shot.startFrame<< "</StartFrameNumber>" << endl;
    videoXML << "\t\t<StartTime>" << shot.startTime<< "</StartTime>" << endl;
    videoXML << "\t\t<EndFrameNumber>" <<shot.endFrame<< "</EndFrameNumber>" << endl;
    videoXML << "\t\t<EndTime>" <<shot.endTime<< "</EndTime>" << endl;
    videoXML << "\t\t<Classification>" << shot.classification<< "</Classification>" << endl;
    videoXML << "\t\t<AverageMotion>" <<shot.averageMotion<< "</AverageMotion>" << endl;
}
    
    //shot change detection 
void VideoShot::computeFrameDifference()
{
    //get the video 
    VideoCapture cap(videoFilePath);
    //get fps
    fps = cap.get(CV_CAP_PROP_FPS);     
    int count = 0;  
    int frameCount = 0;
    //if video file is opened
    if (cap.isOpened()) 
    {// if succeeded
        
        cout<<"Video loaded successfully. Processing frames..." << endl;       
        Mat curFrame, preFrame, curGrayFrame, preGrayFrame; 
        //read the first frame 
        cap.read(preFrame);
        //convert into gray 
        cvtColor(preFrame, preGrayFrame, CV_BGR2GRAY);
        
                
        //process frames
        while (cap.read(curFrame) &&  frameCount < 1400)
        {
            //process each frame after n = frameJump
            if(frameCount%frameJump == 0)
            {
                //convert into gray 
                cvtColor(curFrame, curGrayFrame, CV_BGR2GRAY);

                //get the frame difference
                //sumOfAbsoluteDifferenceValue.push_back(sumOfAbsoluteDifference(preGrayFrame, curGrayFrame));
                edgeChangeRatioValue.push_back(frameDifference->edgeChangeRatio(preGrayFrame, curGrayFrame));  
              //  histogramDifferenceValue.push_back(frameDifference->histogramDifference(preGrayFrame, curGrayFrame, 3));

                //frame difference is histogram difference
                frameDifferenceScore.push_back(frameDifference->histogramDifference(preGrayFrame, curGrayFrame, 3)); 
                //move the current frame to previous frame 
                preGrayFrame = curGrayFrame.clone(); 
            }
            frameCount++; 
        }
        
        cout<<"Classifying frames" << endl; 
        cap.release(); 
    }
}

//classify frames 
void VideoShot::shotDetection()
{
    unsigned int ii = 0; 
    int startFrame, endFrame; 
    int numberOfShots = 0; 
    util = new Utility(); 
    //process frame difference scores 
    while(ii<frameDifferenceScore.size())
    {
        if(frameDifferenceScore[ii]>=shotChangeThreshold)
        {
            endFrame = ii * frameJump; 
            //move out of the transition period    
            while(frameDifferenceScore[++ii]>=shotChangeThreshold)
            {
            }
            //save shot      
            videoShotsVector.push_back(Shot());
            videoShotsVector.back().startFrame = startFrame; 
            videoShotsVector.back().startTime = util->cvtFrameNumber2Time(startFrame, fps); 
            videoShotsVector.back().endFrame = endFrame;
            videoShotsVector.back().endTime = util->cvtFrameNumber2Time(endFrame, fps);
            videoShotsVector.back().shotIndex = numberOfShots; 
            videoShotsVector.back().averageMotion = averageMotion(edgeChangeRatioValue, startFrame/frameJump, ii); 
            videoShotsVector.back().classification = classificationValue[(videoShotsVector.back().averageMotion)>0?1:0]; 
            
            //frame after the transition period
            startFrame = ii * frameJump; 
            //increase number of shots variable
            numberOfShots++;    
        }
        else
        {
            ii++;
        }
    }
    //save last shot
    videoShotsVector.push_back(Shot());
    videoShotsVector.back().startFrame = startFrame; 
    videoShotsVector.back().startTime = util->cvtFrameNumber2Time(startFrame, fps); 
    videoShotsVector.back().endFrame = ii*frameJump; 
    videoShotsVector.back().endTime = util->cvtFrameNumber2Time(ii*frameJump, fps);
    videoShotsVector.back().shotIndex = numberOfShots; 
    videoShotsVector.back().averageMotion = averageMotion(edgeChangeRatioValue, startFrame/frameJump, ii); 
    videoShotsVector.back().classification = classificationValue[(videoShotsVector.back().averageMotion)>0?1:0];
}

//get average motion
double VideoShot::averageMotion(vector<double> &frameDifferenceScore, int startIndex, int endIndex)
{
    if(startIndex==endIndex)
    {
        return 0; 
    }
    //copy vector
    vector<double>::const_iterator first = frameDifferenceScore.begin() + startIndex;
    vector<double>::const_iterator last = frameDifferenceScore.begin() + endIndex;
    vector<double> newVec(first, last);

    double median;
    size_t size = newVec.size();

    sort(newVec.begin(), newVec.end());

    if (size  % 2 == 0)
    {
        median = (newVec[size / 2 - 1] + newVec[size / 2]) / 2;
    }
    else 
    {
        median = newVec[size / 2];
    }
    return median; 
}


vector<Shot>& VideoShot::getVideoShotsVector()
{
    return videoShotsVector; 
}


