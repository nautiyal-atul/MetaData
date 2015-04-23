/* 
 * File:   LanguageDictionary.h
 * Author: atul
 *
 * Created on 23 March 2015, 12:23
 * Dictionary functionality to improve OCR
 */

#ifndef LANGUAGEDICTIONARY_H
#define	LANGUAGEDICTIONARY_H

#include "Utility.h"
#include "TextBinaryTree.h"
#include <boost/algorithm/string.hpp>

using namespace std; 

class LanguageDictionary {
public:
    LanguageDictionary();
    LanguageDictionary(int, string, string, string);
    LanguageDictionary(const LanguageDictionary& orig);
    virtual ~LanguageDictionary();
    //load dictionary
    bool loadDictionaries(); 
    void loadDictionaries(string, string, string);
    
    pair<string, float> matchWord(string); 
    string matchSentence(string);
    
    
private:
    string dictionaryRootPath, videoDictionaryPath, srtDirPath; 
    vector<TextBinaryTree *> dictionaries;
    Utility *util;
    int dictionayFlag; 

};

#endif	/* LANGUAGEDICTIONARY_H */

