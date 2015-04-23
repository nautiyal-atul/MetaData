/* 
 * File:   LanguageDictionary.cpp
 * Author: atul
 * 
 * Created on 23 March 2015, 12:23
 */

#include "LanguageDictionary.h"

LanguageDictionary::LanguageDictionary() {
    util = new Utility();
}

LanguageDictionary::LanguageDictionary(int dictionayFlag, string dictionaryRootPath, string videoDictionaryPath, string srtDirPath) 
{
    util = new Utility();
    this->dictionayFlag = dictionayFlag; 
    this->dictionaryRootPath = dictionaryRootPath;    
    this->videoDictionaryPath = videoDictionaryPath; 
    this->srtDirPath = srtDirPath; 
}

LanguageDictionary::LanguageDictionary(const LanguageDictionary& orig) {

}

LanguageDictionary::~LanguageDictionary() {

}


//load dictionaries from given directory 
bool LanguageDictionary::loadDictionaries() 
{
    //load dictionaries based on selected dictionary flag
    switch (dictionayFlag)
    {
        case 1:
	    //only srt dictionary
	    loadDictionaries(srtDirPath, videoDictionaryPath, ".srt"); 
	    break;
	case 2:
	    //only other dictionaries
	    loadDictionaries(dictionaryRootPath, dictionaryRootPath, ".txt");
	    break;
	case 3:
	    //load both
	    loadDictionaries(srtDirPath, videoDictionaryPath, ".srt");
	    loadDictionaries(dictionaryRootPath, dictionaryRootPath, ".txt");
	    break;
	default:
		cout << "please select a valid dictionary option" << endl;
    }
    return (dictionaries.size()>0)?true:false;
}
    
        
//load and if necessary create dictionaries 
void LanguageDictionary::loadDictionaries(string inputDir, string outputDir, string extention )
{   
    vector<string> fileNames; 
    //get dictionary names
    util->getFileNames(inputDir, extention, fileNames);    
    for(int ii = 0; ii< fileNames.size(); ii++)
    {
        //insert pointer
        dictionaries.push_back(new TextBinaryTree());

        //load serialize binary tree file if available
        if (boost::filesystem::exists(outputDir + "SerializeDictionaries/" + fileNames[ii])) 
        {
            dictionaries[dictionaries.size() - 1]->deSerializeBinaryTreeFromFile(outputDir + "SerializeDictionaries/" + fileNames[ii]);
            cout << "Dictionary Loaded Successfully" << endl; 
        } 
        else 
        { 
            //create tree from text file
            dictionaries[dictionaries.size() - 1]->loadBinaryTreeFromFile(inputDir + fileNames[ii]);
            //serialize it for future
            boost::filesystem::path dir((outputDir + "/SerializeDictionaries").c_str());
			if (boost::filesystem::create_directories(dir))
			    std::cout << "Directory Created Successfully" << "\n";
            
            dictionaries[dictionaries.size() - 1]->serializeBinaryTreeINFile(outputDir + "SerializeDictionaries/" + fileNames[ii]);
            cout << "Dictionary Created and Loaded Successfully" << endl; 
        }
    }
}
  

//get best word from directory 
pair<string, float> LanguageDictionary::matchWord(string word) 
{
    string likelyWord = "";
    float highScore = -1;
    double MATCH_SCORE_THRESH = 0.75; 
    
    unsigned int currentDictionaryNumber = 0;
    pair<string, float> dictionaryMatchResult;

    //match word in slide dictionary followed by Aspell if required
    while (currentDictionaryNumber < dictionaries.size()) 
    {
        if (dictionaries[currentDictionaryNumber]->searchBinaryTree(word) > 0) {
            highScore = 1.0;
            likelyWord = word;
            break;
        } else
            currentDictionaryNumber++;
    }

    // otherwise determine score by examining words of the same
    //length from the dictionary, building a score based on a
    //character by character comparison score = (#matching / #char)
    if (likelyWord.empty()) {
        currentDictionaryNumber = 0;
        while (currentDictionaryNumber < dictionaries.size()) 
        {
            dictionaryMatchResult = dictionaries[currentDictionaryNumber]->getBestMatch(word);
            if (dictionaryMatchResult.second > highScore && dictionaryMatchResult.second > MATCH_SCORE_THRESH) 
            {
                likelyWord = dictionaryMatchResult.first;
                highScore = dictionaryMatchResult.second;
            }

            //go to the second dictionary
            currentDictionaryNumber++;
        }
    }

    return pair<string, float>(likelyWord, highScore);
}



string LanguageDictionary::matchSentence(string sentence) 
{
        sentence = util->removeWhiteSpace(sentence);
	//split the sentence in words
	std::vector<std::string> words;
	boost::split(words, sentence, boost::is_any_of("  \t"));

	vector<string> matchWords(words.size());
	vector<float> matchScores(words.size());

	pair<string, float> singleWordScore;

        double MATCH_SCORE_THRESH = 0.75; 
//	int currentDictionaryNumber;

	//for each word
	for (unsigned int ii = 0; ii < words.size(); ii++)
	{
		singleWordScore = matchWord(words[ii]);
		matchWords[ii] = singleWordScore.first;
		matchScores[ii] = singleWordScore.second;
	}

	for (unsigned int ii = 0; ii < words.size()-1; ii++)
	{
		//two continuous words are garbage
		if (matchScores[ii] < MATCH_SCORE_THRESH && matchScores[ii+1] < MATCH_SCORE_THRESH)
		{
			//check if joint word valid or not
			singleWordScore = matchWord(words[ii] + words[ii + 1]);
			if (singleWordScore.second >= MATCH_SCORE_THRESH)
			{
				matchWords[ii] = singleWordScore.first;
				matchScores[ii] = singleWordScore.second;
				matchWords[ii+1] = "";
				matchScores[ii + 1] = singleWordScore.second;

			}
		}
	}

        string text = ""; 
        for (unsigned int ii = 0; ii < matchWords.size(); ii++)
	{
            text = text + matchWords[ii] + " "; 
        }
        boost::algorithm::trim(text);
	return text;
}