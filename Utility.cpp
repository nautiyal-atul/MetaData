/*
 * Utility.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: atul
 */

#include "Utility.h"


Utility::Utility()
{
}


Utility::~Utility()
{
}


//compare string ignore case and white space
int Utility::stringCompareIgnoreCaseWhiteSpace(string str1, string str2)
{
	//remove white spaces
	boost::algorithm::trim(str1);
	boost::algorithm::trim(str2);

        //convert to lower 
        boost::to_lower(str1);
        boost::to_lower(str2); 
        
        //compare an return
	return str1.compare(str2);
}

//remove leading and trailing white space and doble white spaces inbetween text
string Utility::removeWhiteSpace(string sentence)
{
  boost::regex spaces("[ ]{2,}");
  return regex_replace(sentence, spaces, " ");
  
/*  
    //trim leading and trailing white spaces
	boost::algorithm::trim(sentence);
	sentence = boost::regex_replace(sentence,boost:: regex("\\s+"), " ");
//	sentence = regex_replace(sentence,regex("\\s+"), " ");
	return sentence;
  */      
}



//get the levenshtein distance between two strings
/*
template<class T> unsigned int Utility::levenshtein_distance(T &s1, T & s2)
{
	const size_t len1 = s1.size(), len2 = s2.size();
	vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++) {
		col[0] = i + 1;
		for (unsigned int j = 0; j < len2; j++)
			col[j + 1] = min(min(prevCol[1 + j] + 1, col[j] + 1),
			prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
		col.swap(prevCol);
	}
	return prevCol[len2];
}
*/

//get most frequent string characters
string Utility::frequentString(vector<string> slideStrings)
{

	//divide the slide text in lines
	vector< vector<string>> slideTextLines;
	vector<string> lines;
	for (unsigned int ii = 0; ii < slideStrings.size(); ii++)
	{
		lines.clear();
		boost::split(lines, slideStrings[ii], boost::is_any_of("\n"));
		slideTextLines.push_back(lines);
	}


	int iterator = 0;
	int modeValue;
	vector<int> asciiVector;
	string result = "";
	char alpha;
	int numberOfEmpltyLines = 0;
	int noCharaterCount = 0;

	//for all lines of the first slide
	for (unsigned int jj = 0; jj < slideTextLines[0].size(); jj++)
	{
		//for each alphabet
		while (true)
		{
			noCharaterCount = 0;
			numberOfEmpltyLines = 0;
			asciiVector.clear();
			//for each slide
			for (unsigned int ii = 0; ii < slideTextLines.size(); ii++)
			{
				try
				{
					//count number of empty lines
					if (slideTextLines[ii].size() <  (jj + 1))
					{
						numberOfEmpltyLines++;
						noCharaterCount++;
					}
					else
					{
						if (slideTextLines[ii].size() >= (jj + 1) && slideTextLines[ii][jj].size() == 0)
						{
							numberOfEmpltyLines++;
						}
						alpha = slideTextLines[ii][jj].at(iterator);
						asciiVector.push_back(int(alpha));

					}

				}
				catch (const std::out_of_range& oor)
				{
					noCharaterCount++;
				}

			}


			if (noCharaterCount >= 0.6*slideStrings.size() || numberOfEmpltyLines >= 0.7*slideTextLines[0].size())
			{
				break;
			}

			noCharaterCount = 0;
			numberOfEmpltyLines = 0;
			//get frequency
			modeValue = mode(asciiVector, -1);
			result = result + char(modeValue);
			iterator++;
		}

		result = result + '\n';
		iterator = 0;;
	}

	return result;
}


//mode value from 1d vector
int  Utility::mode(vector<int> &numberVector, int eraseValue)
{
	if (numberVector.size() <= 0)return 0;

	//remove element from vector
	if (eraseValue >= 0)
	{
		numberVector = removeElement(numberVector, eraseValue);
	}

	//sort vector
	numberVector = sortVector(numberVector);

	int maxCount = 0, maxPosition = 0;
	int count = 1;

	for (unsigned int ii = 0; ii<numberVector.size() - 1; ii++)
	{
		if (numberVector[ii] == numberVector[ii + 1])
		{
			count++;
		}
		else
			count = 1;

		if (count>maxCount)
		{
			maxCount = count;
			maxPosition = ii;
		}
	}


	return numberVector[maxPosition];
}


//sort vector in ascending order
template<typename T> vector<T>  Utility::sortVector(vector<T> &numberVector)
{

	unsigned int maxIndex, index;
	index = maxIndex = 0;
	vector<T> outVector;

	for (unsigned int i = 0; i < numberVector.size(); i++)
	{
		index = 0;
		//insert in vector in ascending order
		while (outVector.size() > 0 && index < outVector.size() && numberVector[i] < outVector[index])
		{
			index++;
		}
		outVector.insert(outVector.begin() + index, numberVector[i]);
	}

	return outVector;

}

//remove element from vector
template<typename T>vector<T>  Utility::removeElement(vector<T> &numberVector, T eraseValue)
{
	numberVector.erase(std::remove(numberVector.begin(), numberVector.end(), eraseValue), numberVector.end());
	return numberVector;
}




// return the filenames of all files that have the specified extension
// in the specified directory
void Utility::getFileNames(string root, string ext, vector<string> &filePath)
{
	if (!fs::exists(root)) return;

	if (fs::is_directory(root))
	{
		//fs::recursive_directory_iterator it(root);
		//fs::recursive_directory_iterator endit;
		fs::directory_iterator it(root);
		fs::directory_iterator endit;
		while (it != endit)
		{
			if (fs::is_regular_file(*it) && it->path().extension() == ext)
			{
				filePath.push_back(it->path().filename().string());
			}
			++it;
		}
	}
}

bool Utility::isExists(string root)
{

	if (!fs::exists(root))
		return true;
	else
		return false;
}

string Utility::cvtFrameNumber2Time(int frameNumber, int fps)
{
    int hours, minutes, seconds, milliseconds; 
    seconds = frameNumber/fps;    
    milliseconds = (1000/fps) * (frameNumber%fps);    
    hours = seconds / 3600;
    minutes = (seconds / 60) %60;
    //update seconds as well
    seconds = seconds%60; 
    char timeString[25];
    sprintf(timeString, "%02d:%02d:%02d:%03d", hours, minutes, seconds, milliseconds);
    
    return timeString; 
}

//get the string difference
string Utility::stringDifference(string str1, string str2)
{
    string differenceString = ""; 
    vector<string> words1, words2; 
    //break string into words
    boost::split(words1, str1, boost::is_any_of("  ,\n, \t"));
    boost::split(words2, str2, boost::is_any_of("  ,\n, \t"));
    bool match; 
    //compare strings 
    for(int ii = 0; ii < words2.size(); ii++)
    {
        match = false; 
        for(int jj = 0; jj<words1.size(); jj++)
        {
            if(stringCompareIgnoreCaseWhiteSpace(words2[ii], words1[jj]) ==0)
            {
                match = true;
                break;
            }
        }
      
        //if not match 
        if(!match)
        {
            differenceString = differenceString + words2[ii] + " "; 
        }
    }
    
    boost::trim(differenceString);  
    
    return differenceString;        
}