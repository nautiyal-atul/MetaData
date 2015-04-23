/*
 * Utility.h
 *
 *  Created on: Sep 16, 2014
 *      Author: atul
 */

//utility routines

#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <fstream>
#include <stdio.h>

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

namespace fs = ::boost::filesystem;
using namespace std;
using namespace cv;
using namespace boost;

class Utility
{
public:
	Utility();
	~Utility();

	//compare string ignore case and white space
	int stringCompareIgnoreCaseWhiteSpace(string, string);

	//remove white spaces
	string removeWhiteSpace(string);

	//Leveinstein Distance
	//template<class T> unsigned int levenshtein_distance(T &,T &);
	//get the levenshtein distance between two strings
	template<class T> unsigned int levenshtein_distance(T &s1, T & s2)
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


	//get the most frequent string
	string frequentString(vector<string>);


	//mode of 1d vector
	int mode(vector<int> &, int);

	//sort vector in acending order
	template<typename T> vector<T>  sortVector(vector<T> &);

	//remove value from 1 d vector
	template<typename T>vector<T>  removeElement(vector<T> &, T);

	// file paths from a dictionary
	void getFileNames(string, string, vector<string>&);

	//check whether file/directory exists or not
	bool isExists(string );
        
        //change seconds to formated time hh:mm:ss
        string cvtFrameNumber2Time(int, int); 
        
        //string difference
        string stringDifference(string, string); 
        

};

#endif /* UTILITY_H_ */
