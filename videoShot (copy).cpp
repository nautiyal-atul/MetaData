/* 
 * File:   videoShot.cpp
 * Author: atul
 * 
 * Created on 27 January 2015, 12:37
 */

#include "videoShot.h"

 
videoShot::videoShot() 
{
    shot = new Shot(); 
} 

const string videoShot::faceValue[] = {"Single",  "Single Close up", "Multiple", "Nill"}; 
const string videoShot::classificationValue[] = {"Still", "Video"}; 




//constructor with parameters 
videoShot::videoShot(int shotIndex, int startFrame, int endFrame, double averageMotion) 
{
    this->shotIndex = shotIndex; 
    this->startFrame = startFrame; 
    this->endFrame = endFrame; 
    this->averageMotion = averageMotion; 
}

videoShot::videoShot(const videoShot& orig) 
{
    
}

videoShot::~videoShot() 
{

}

//write values in XML
void videoShot::writeXMLNode(ofstream& videoXML)
{		
    videoXML << "\t\t<UID>" <<shotIndex<<"</UID>" << endl;
    videoXML << "\t\t<StartFrameNumber>" << startFrame<< "</StartFrameNumber>" << endl;
    videoXML << "\t\t<EndFrameNumber>" <<endFrame<< "</EndFrameNumber>" << endl;
    videoXML << "\t\t<Classification>" << classificationValue[averageMotion>0?1:0]<< "</Classification>" << endl;
    videoXML << "\t\t<AverageMotion>" <<averageMotion<< "</AverageMotion>" << endl;
}

    int videoShot::getShotIndex()
    {
        return shotIndex; 
    }
    
    int videoShot::getStartFrame()
    {
        return startFrame; 
    }
    
    int videoShot::getEndFrame()
    {
        return endFrame; 
    }
    
    double videoShot::getAverageMotion()
    {
        return averageMotion; 
  }
    
  