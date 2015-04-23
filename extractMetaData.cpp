/* 
 * File:   extractMetaData.cpp
 * Author: atul
 * 
 * Created on 26 January 2015, 10:20
 */

#include "extractMetaData.h"

extractMetaData::extractMetaData() 
{
    
}

extractMetaData::extractMetaData(const extractMetaData& orig) {
}

extractMetaData::~extractMetaData() {
}


extractMetaData::extractMetaData(string videoFilePath, string faceCascadeName, string dictionaryRootDir, int dictionaryFlag, double shotChangeThreshold, bool isProcessOCR, bool isSaveGIF, int frameJump, int noOfImagesInGIF) 
{
    //input variables
    this->videoFilePath = videoFilePath; 
    this->faceCascadeName = faceCascadeName; 
    this->dictionaryRootDir = dictionaryRootDir; 
    this->dictionaryFlag = dictionaryFlag; 
    this->shotChangeThreshold = shotChangeThreshold; 
    this->isProcessOCR = isProcessOCR; 
    this->isSaveGIF = isSaveGIF; 
    this->frameJump = frameJump; 
    this->noOfImagesInGIF = noOfImagesInGIF; 
}

//set other class variables 
void extractMetaData::setClassVariables()
{
    isProcessFace = faceCascadeName.size()>1?true:false;    
    isUseDictionary = dictionaryRootDir.size()>1?true:false;
    rootDirPath = boost::regex_replace(videoFilePath, boost::regex(".mp4"), "");
    gifDirPath= rootDirPath + "/shotGIF/"; 
    srtDirPath = rootDirPath + "/srt/";
    videoDictionaryPath = rootDirPath + "/Dictionary/";
    xmlName = "videoMetaData.xml";
}

void extractMetaData::loadDictionaries()
{
    //load dictionaries if flags are true
    if(isProcessOCR && isUseDictionary )
    {
        dictionary = new LanguageDictionary(dictionaryFlag, dictionaryRootDir, videoDictionaryPath, srtDirPath); 
        //load dictionary 
        dictionaryLoaded  = dictionary->loadDictionaries();
        if(!dictionaryLoaded)
        {
            cout<< "Dictionaries are not loaded. Processing without it"   << endl; 
        }
    }
}

//shot change detection 
void extractMetaData::extract()
{
   util = new Utility();
   //set class variables 
   setClassVariables(); 
   //load dictionaries
   loadDictionaries();
   //video shot
   videoShot = new VideoShot(rootDirPath, videoFilePath, shotChangeThreshold, frameJump); 
   //get frame difference
   videoShot->computeFrameDifference(); 
   //detect shots using frame difference
   videoShot->shotDetection(); 
   //save and process video shots 
   cout<< "Creating XML" << endl; 
   saveInXML(); 
}

//save data in XML files 
void extractMetaData::saveInXML()
{
        //sort the tree
	ofstream videoXML;
	char time_buf[21];
	time_t now;
	time(&now);
	//get current time
	strftime(time_buf, 21, "%Y-%m-%dT%H:%S:%MZ", gmtime(&now));
	videoXML.open(rootDirPath + "/" + xmlName);    
        videoXML << "<Video xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" timestamp=\""<<time_buf<<"\" xsi:noNamespaceSchemaLocation=\"../schema/ConferenceAnnotation.xsd\">"<<endl; 
        //process frames belonging to the shot for OCR and Face detection
        processShot(videoXML); 
        videoXML << "</Video>";
        cout<<"XML file saved is saved "<< rootDirPath + "/" + xmlName << endl;
	videoXML.close();
        
}


void extractMetaData::processShot(ofstream &videoXML)
{
    if(isProcessFace)
    {
        fd = new faceDetect(faceCascadeName); 
        fd->loadCascade(); 
    }
    if(isProcessOCR)
    {
        ocr = new detectOCR(); 
    }
    if(isSaveGIF)
    {          
        //create output directory
        boost::filesystem::path dir((gifDirPath + "temp/").c_str());
        if (boost::filesystem::create_directories(dir))
            std::cout << "Directory Created Successfully" << "\n";
        //remove all previous gifs
        std::system(("find "+ gifDirPath + " -iname '*.gif' -delete").c_str()); 
    }
    
    VideoCapture cap(videoFilePath);
    //get fps
    fps = cap.get(CV_CAP_PROP_FPS); 
    
    Mat frame;            
    int frameCount = 0;
    
    vector<Shot>videoShotsVector = videoShot->getVideoShotsVector(); 
    //process frames belonging to shots
    for(int ii = 0; ii<videoShotsVector.size(); ii++)
    {
        //write first node in XML
        videoXML <<"\t<Shot>"<< endl;
        videoShot->writeXMLNode(videoXML, videoShotsVector[ii]); 
            
        //move the video pointer to the start of the shot
        while(frameCount< videoShotsVector[ii].startFrame)
        {
            cap.read(frame);
            frameCount++; 
        }

        //still shot
        if(videoShotsVector[ii].averageMotion ==0)
        {  
            processStillShot(cap, videoXML, videoShotsVector[ii].startFrame, videoShotsVector[ii].endFrame, frameCount);
        }
        else // shot with moving frames
        {
            processMovingShot(cap, videoXML, videoShotsVector[ii].startFrame, videoShotsVector[ii].endFrame, frameCount);
        }
          
        //if save gif flag is true
        if(isSaveGIF)
        {
            saveGIF(ii+1, gifDirPath); 
        }
        videoXML <<"\t</Shot>"<< endl;        
    }
    cap.release(); 
}


void extractMetaData::processStillShot(VideoCapture &cap, ofstream &videoXML, int startFrame, int endFrame, int &frameCount)
{
    vector<string> slideStrings; 
    Mat frame, grayFrame; 
    
    while(frameCount <= endFrame)
    {
        cap.read(frame); 
        //process every n = frameJump frame
        if(frameCount%frameJump == 0)
        {
            cvtColor(frame, grayFrame, CV_BGR2GRAY); 
            //process still frames for OCR
            if(isProcessOCR)
            {
                //string text =  ocr->detectOCRusingTesseract(frame); 
                string ocrText= ocr->detectOCRusingTesseract(grayFrame); 
                if(ocrText.length()>0)
                {
                    slideStrings.push_back(ocrText);
                }
            }
            // if save gif true
            if(isSaveGIF && frameCount == startFrame)
            {          
                imwrite(gifDirPath + "temp/" + std::to_string(frameCount) + ".jpg", frame); 
            }
        }                                
        frameCount++; 
    }   
    
    //process frames in group of 'n' frames
    int ocrFrameGroup = 10; 
    vector<string> slideStringGroup; 
    if(isProcessOCR && slideStrings.size()>0)
    {
        string preText, curText, string_difference; 
        preText = "";
        for(int ii = 0; ii< (slideStrings.size()/ocrFrameGroup + std::min(1, (int)slideStrings.size()%ocrFrameGroup)); ii++)
        {
            vector<string>::const_iterator first = slideStrings.begin() + ocrFrameGroup*ii;
            vector<string>::const_iterator last =  slideStrings.begin() + std::min( (ocrFrameGroup*(ii+1)-1), int(slideStrings.size()) );
            vector<string> slideStringGroup(first, last);
            
            string ocrText = util->frequentString(slideStringGroup);
            boost::trim(ocrText);
            //fix the string using dictionary      
            if(dictionaryLoaded)
            {
              curText = dictionary->matchSentence(ocrText); 
            }        
            
            string_difference = util->stringDifference(preText, curText); 
            
            if(string_difference.length()>0)
            {
                 ocr->saveOcrXML(videoXML, string_difference); 
            }               
            preText = curText; 
            curText = ""; 
        }
    }
}


/*

void extractMetaData::processStillShot(VideoCapture &cap, ofstream &videoXML, int startFrame, int endFrame, int &frameCount)
{
    vector<string> slideStrings; 
    Mat frame, grayFrame; 
    
    while(frameCount <= endFrame)
    {
        cap.read(frame); 
        //process every n = frameJump frame
        if(frameCount%frameJump == 0)
        {
            cvtColor(frame, grayFrame, CV_BGR2GRAY); 
            //process still frames for OCR
            if(isProcessOCR)
            {
                //string text =  ocr->detectOCRusingTesseract(frame); 
                string ocrText= ocr->detectOCRusingTesseract(grayFrame); 
                if(ocrText.length()>0)
                {
                    slideStrings.push_back(ocrText);
                }
            }
            // if save gif true
            if(isSaveGIF && frameCount == startFrame)
            {          
                imwrite(gifDirPath + "temp/" + std::to_string(frameCount) + ".jpg", frame); 
            }
        }                                
        frameCount++; 
    }   
    
    //save OCR
    if(isProcessOCR && slideStrings.size()>0)
    {
        string ocrText = util->frequentString(slideStrings);
        string frequentText=""; 
        boost::trim(ocrText);
        //fix the string using dictionary      
        if(dictionaryLoaded)
        {
            frequentText = dictionary->matchSentence(ocrText); 
        }
        if(frequentText.length()>0)
        {
            ocr->saveOcrXML(videoXML, frequentText); 
        }
    }
}
*/

void extractMetaData::processMovingShot(VideoCapture &cap, ofstream &videoXML, int startFrame, int endFrame, int &frameCount)
{
    Mat frame, grayFrame; 
    int fileJumpGIF = computeFileJumpForGIF(startFrame, endFrame); 
    
    while(frameCount <= endFrame)
    {
        cap.read(frame);
        if(frameCount%frameJump == 0)
        {
            cvtColor(frame, grayFrame, CV_BGR2GRAY); 
            //detect faces if flag is ture
            if(isProcessFace)
            {
                fd->detect(grayFrame); 
                if(fd->getFacesSize()>0)
                {
                    videoXML <<"\t\t<Frame>"<<to_string(frameCount + startFrame)<<"</Frame>"<<endl; 
                    videoXML <<"\t\t\t<TimeStamp>"<<util->cvtFrameNumber2Time(frameCount + startFrame, fps)<<"</TimeStamp>"<<endl; 
                    fd->saveFaceXML(videoXML); 
                }
            }   

            if(isSaveGIF && (frameCount == startFrame || frameCount%fileJumpGIF ==0) )
            {
               imwrite(gifDirPath + "temp/" + std::to_string(frameCount-startFrame+1) + ".jpg", frame); 
            }
        }
        frameCount++; 
    }
}


int extractMetaData::computeFileJumpForGIF(int startFrame, int endFrame)
{
    return (endFrame/frameJump-startFrame/frameJump)>noOfImagesInGIF ? (endFrame/frameJump-startFrame/frameJump)/noOfImagesInGIF:(endFrame/frameJump-startFrame/frameJump) ; 
}


//save GIF image
void extractMetaData::saveGIF(int gifImageNumber, string outputDir)
{           
    cout<< "Creating " <<  gifImageNumber << ".gif" << endl; 
     //create gif    
     string jpeg2gifCommand = "convert " + (outputDir + "temp/") + "*.jpg " + outputDir + std::to_string(gifImageNumber) + ".gif";
     std::system(jpeg2gifCommand.c_str());  
     //remove all jpegs 
     std::system(("find "+ (outputDir + "temp/" ) +" -iname '*.jpg' -delete").c_str()); 
}