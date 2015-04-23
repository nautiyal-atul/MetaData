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
    this->videoFilePath = videoFilePath; 
    this->faceCascadeName = faceCascadeName; 
    this->dictionaryRootDir = dictionaryRootDir; 
    this->shotChangeThreshold = shotChangeThreshold; 
    
    this->isProcessFace = faceCascadeName.size()>1?true:false; 
    this->isProcessOCR = isProcessOCR; 
    this->isUseDictionary = dictionaryRootDir.size()>1?true:false;; 
    this->isSaveGIF = isSaveGIF; 
    this->noOfImagesInGIF = noOfImagesInGIF; 
    this->frameJump = frameJump; 
    rootDirPath = boost::regex_replace(videoFilePath, boost::regex(".mp4"), "");
    gifDirPath= rootDirPath + "/shotGIF/"; 
    xmlDirPath = rootDirPath + "/metaDataXML/";
    srtDirPath = rootDirPath + "/srt/";
    videoDictionaryPath = rootDirPath + "/Dictionary/";
    xmlName = "videoMetaData.xml";   
    
    
    util = new Utility();
    frameDifference = new FrameDifference(); 
    
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
void extractMetaData::shotDetection()
{
    //get the video 
    VideoCapture cap(videoFilePath);
    
    //create source directory
    boost::filesystem::path dir((rootDirPath + "/metaDataXML").c_str());
	if (boost::filesystem::create_directory(dir))
	    std::cout << "Directory Created Successfully" << "\n";
    
    int count = 0;  
    int frameCount = 0;
    //if video file is opened
    if (cap.isOpened()) 
    {// if succeeded
        
        cout<<"Video loaded successfully. Processing frames..." << endl;
//        vector<double> sumOfAbsoluteDifferenceValue, histogramDifferenceValue; 
        vector<double> edgeChangeRatioValue; 

        vector<double> frameDifferenceValue;
        
        Mat curFrame, preFrame, curGrayFrame, preGrayFrame; 
        //read the first frame 
        cap.read(preFrame);
        //convert into gray 
        cvtColor(preFrame, preGrayFrame, CV_BGR2GRAY);
        
                
        //process frames
        while (cap.read(curFrame) &&  frameCount < 2000)
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
                frameDifferenceValue.push_back(frameDifference->histogramDifference(preGrayFrame, curGrayFrame, 3)); 
                //move the current frame to previous frame 
                preGrayFrame = curGrayFrame.clone(); 
            }
            frameCount++; 
        }
        
        vector<videoShot*> videoShotVector;
        cout<<"Classifying frames" << endl; 
        //classify frames 
        classifyFrames(frameDifferenceValue, edgeChangeRatioValue, videoShotVector); 
        cout<< "Creating XML" << endl; 
        //save in XML 
        saveInXML(videoShotVector, xmlDirPath + xmlName); 
        cap.release(); 
    }
}

//classify frames 
void extractMetaData::classifyFrames(vector<double> &frameDifferenceScore, vector<double> &edgeChangeRatioValue, vector<videoShot*> &videoShotVector)
{
    unsigned int ii = 0; 
    int startFrame, endFrame; 
    startFrame = 0; 
    endFrame = 1; 
    int numberOfShots = 0; 
    double averageMotionValue; 
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
           //get average motion 
            averageMotionValue = averageMotion(edgeChangeRatioValue, startFrame, endFrame); 

            //save shot      
            videoShotVector.push_back(new videoShot(numberOfShots, startFrame, endFrame, averageMotionValue));

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
    videoShotVector.push_back(new videoShot(numberOfShots, startFrame, (ii-1)*frameJump, averageMotion(edgeChangeRatioValue, startFrame, (ii-1)*frameJump)));
}

//save data in XML files 
void extractMetaData::saveInXML(vector<videoShot*> &videoShotVector, string xmlFilePath)
{
    //sort the tree
	ofstream videoXML;
	char time_buf[21];
	time_t now;
	time(&now);
	//get current time
	strftime(time_buf, 21, "%Y-%m-%dT%H:%S:%MZ", gmtime(&now));

	videoXML.open(xmlFilePath);
	//videoXML << "< ? xml version = \"1.0\" ? >" << endl;
	//videoXML << "<Video timestamp = "<< " xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\" xsi : noNamespaceSchemaLocation = \"../schema/ConferenceAnnotation.xsd\"> " << endl;

        videoXML<<"<Video>"<<endl;
        int startFrame, endFrame, frameCount;
        VideoCapture cap(videoFilePath);
        Mat frame, grayFrame; 
        
        faceDetect *fd; 
        detectOCR *ocr; 
        vector<string> slideStrings; 
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
        
        
        frameCount = 0; 
	//write data in xml file
	for(int ii = 0; ii<videoShotVector.size(); ii++)
        {
          //write first node in XML
          videoXML <<"\t<Shot>"<< endl;
          videoShotVector[ii]->writeXMLNode(videoXML); 
            
          startFrame =  videoShotVector[ii]->getStartFrame(); 
          endFrame = videoShotVector[ii]->getEndFrame();
          int fileJumpGIF = (endFrame/frameJump-startFrame/frameJump)>noOfImagesInGIF ? (endFrame/frameJump-startFrame/frameJump)/noOfImagesInGIF:(endFrame/frameJump-startFrame/frameJump) ; 
          
          
          //clear vector
          slideStrings.clear(); 
          //move the cap pointer to the start of the shot
          while(frameCount< startFrame)
          {
              cap.read(frame);
              frameCount++; 
          }

          //still shot
          if(videoShotVector[ii]->getAverageMotion() ==0)
          {  
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
                       boost::regex spaces("[ ]{2,}");
                       ocrText = regex_replace(ocrText, spaces, " ");
                       boost::trim(ocrText); 
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
          else // shot with moving frames
          {
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
          
          //if save gif flag is true
          if(isSaveGIF)
          {
              cout<< "Creating " <<  ii+1 << ".gif" << endl; 
              //create gif    
              string jpeg2gifCommand = "convert " + (gifDirPath + "temp/") + "*.jpg " + gifDirPath + std::to_string(ii+1) + ".gif";
              std::system(jpeg2gifCommand.c_str());  
              //remove all jpegs 
              std::system(("find "+ (gifDirPath + "temp/" ) +" -iname '*.jpg' -delete").c_str()); 
          }
          videoXML <<"\t</Shot>"<< endl;        
        }
        
        videoXML << "</Video>";
        cout<<"XML file saved is saved "<< xmlFilePath << endl;
	videoXML.close();
        cap.release(); 
}