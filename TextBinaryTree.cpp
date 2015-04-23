/*
 * TextBinaryTree.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: atul
 */

#include "TextBinaryTree.h"


TextBinaryTree::TextBinaryTree()
{
	util = new Utility();
	root = NULL;
}


TextBinaryTree::~TextBinaryTree()
{
}


//save tree content in a file
void TextBinaryTree::saveTreeInFile(string filePath)
{
	ofstream pptDictionaryFile;
	pptDictionaryFile.open(filePath);
	inOrderPrint(root, pptDictionaryFile);
	pptDictionaryFile.close();
}

void TextBinaryTree::inOrderPrint(treeNode *curNode, ofstream &outFile)
{
if (curNode != NULL)
	{
		if (curNode->left)
		{
			inOrderPrint(curNode->left, outFile);
		}
		//printData
		outFile << curNode->text << endl;
		if (curNode->right)
		{
			inOrderPrint(curNode->right, outFile);
		}
	}
}



//load binary tree from file
void TextBinaryTree::loadBinaryTreeFromFile(string filePath)
{
	//read file
	ifstream wordFile(filePath);
	string wholeFileText;

	//read size
	wordFile.seekg(0, std::ios::end);
	//allocate memory in advance
	wholeFileText.reserve(wordFile.tellg());
	wordFile.seekg(0, std::ios::beg);

	//read file into a string
	wholeFileText.assign((std::istreambuf_iterator<char>(wordFile)),
		std::istreambuf_iterator<char>());

        //remove anything but alphabets
        boost::regex reg_wordsOnly("[^a-zA-Z' ]");
	wholeFileText = regex_replace(wholeFileText, reg_wordsOnly, " ");
        boost::regex spaces("[ ]{2,}");
        wholeFileText = regex_replace(wholeFileText, spaces, " ");

        boost::replace_all(wholeFileText, "  ", " ");
        boost::trim(wholeFileText); 
	//call function to load tree from string
	loadBinaryTreeFromString(wholeFileText);
	wordFile.close();

}

//load binary tree from string
void TextBinaryTree::loadBinaryTreeFromString(string page)
{
	//load file
	string word;
 	std::vector<std::string> pageWords;
	boost::split(pageWords, page, boost::is_any_of("  ,\n,\t"));
	loadBinaryTree(pageWords);

}

void TextBinaryTree::loadBinaryTree(vector<string> &pageWords)
{
	bool nodeExits = false;
	for (unsigned int ii = 0; ii < pageWords.size(); ii++)
	{
		nodeExits = false;
		//new node
		treeNode *newNode = new treeNode();
		//push data into new node
		newNode->text = pageWords[ii];
		newNode->left = NULL;
		newNode->right = NULL;

		//insert new node in the tree
		if (root == NULL)
		{
			//set root to the first node
			root = newNode;
		}
		else
		{
			treeNode *curNode, *parentNode;
			parentNode = curNode = root;

			while (curNode)
			{
				parentNode = curNode;
				//	if (text== curNode->text)
				if (util->stringCompareIgnoreCaseWhiteSpace(pageWords[ii], curNode->text) == 0)
				{
					nodeExits = true;
					break;
				}
				//else if (text > curNode->text)
				else if (util->stringCompareIgnoreCaseWhiteSpace(pageWords[ii], curNode->text) > 0)
				{
					curNode = curNode->right;
				}
				else
				{
					curNode = curNode->left;
				}
			}
			//add node to the parent node
			if (!nodeExits && util->stringCompareIgnoreCaseWhiteSpace(pageWords[ii], parentNode->text) > 0)
			{
				parentNode->right = newNode;
			}
			else if (!nodeExits && util->stringCompareIgnoreCaseWhiteSpace(pageWords[ii], parentNode->text) < 0)
			{
				parentNode->left = newNode;
			}
		}
	}
}


//search in binary tree
int TextBinaryTree::searchBinaryTree(string word)
{
	bool matchFound = false;
	//insert new node in the tree
	if (root == NULL)
	{
		//set root to the first node
		return 0;
	}
	else
	{
		treeNode *curNode, *parentNode;
		parentNode = curNode = root;

		while (curNode)
		{
                        parentNode = curNode;
			//	if (text== curNode->text)
			if (util->stringCompareIgnoreCaseWhiteSpace(word, curNode->text) == 0)
			{
				matchFound = true;
				break;
			}
			else if (util->stringCompareIgnoreCaseWhiteSpace(word, curNode->text) > 0)
			{
				curNode = curNode->right;
			}
			else
			{
				curNode = curNode->left;
			}
		}
	}

	return matchFound == true ? 1 : 0;
}


//get best score in case of no match
pair<string, float> TextBinaryTree::getBestMatch(string word)
{
	treeNode *curNode;
	curNode = root;
	string *likelyWord = new string();
	float *highscore = new float();

	//initialize highscore value to -1
	*highscore = -1;

	inOrderLeveinsteinDistanceBasedMatch(curNode, word, likelyWord, highscore);

	pair<string, float> matchResult;
	matchResult.second = *highscore;
	if (*highscore == 0)
	{
		matchResult.first = "";
	}
	else
	{
		matchResult.first = *likelyWord;
	}

	return matchResult;

}

//inorder traverse
void TextBinaryTree::inOrderLenghtBasedMatch(treeNode *curNode, string word, string *likelyWord, float *highscore)
{


	if (curNode != NULL)
	{
		if (curNode->left)
		{
			inOrderLenghtBasedMatch(curNode->left, word, likelyWord, highscore);
		}

		float score = 0;
		if (curNode->text.size() == word.size())
		{
			//character by character match
			for (unsigned int jj = 0; jj < word.size(); jj++)
			{
				if (word[jj] == curNode->text[jj])
				{
					score++;
				}
			}
			if (score / float(word.size()) > *highscore)
			{
				*highscore = score / float(word.size());
				*likelyWord = curNode->text;
			}
		}

		if (curNode->right)
		{
			inOrderLenghtBasedMatch(curNode->right, word, likelyWord, highscore);
		}
	}
}

//inorder traverse
void TextBinaryTree::inOrderLeveinsteinDistanceBasedMatch(treeNode *curNode, string word, string *likelyWord, float *highscore)
{

	if (curNode != NULL)
	{
		if (curNode->left)
		{
			inOrderLeveinsteinDistanceBasedMatch(curNode->left, word, likelyWord, highscore);
		}

		int lengthDifference = abs(int(curNode->text.size() - word.size()));
		if (lengthDifference <= 3)
		{

			//	float score = 1.0 - float(util->levenshtein_distance(curNode->text, word)) / max(word.length(), curNode->text.length());
			float score = 1.0 - float(util->levenshtein_distance(curNode->text, word)) / (word.length() + curNode->text.length());
			if (score > *highscore)
			{
				*highscore = score;
				*likelyWord = curNode->text;
			}
		}

		if (curNode->right)
		{
			inOrderLeveinsteinDistanceBasedMatch(curNode->right, word, likelyWord, highscore);
		}
	}
}


//serialize binary tree
void TextBinaryTree::serializeBinaryTreeINFile(string filePath)
{
	ofstream serializeFile;
	serializeFile.open(filePath);
	serializeBinaryTree(root, serializeFile);
	serializeFile.close();
}


//serialize binary tree
void TextBinaryTree::serializeBinaryTree(treeNode *curNode, ofstream &out)
{
	if (!curNode) {
		out << "##NULL## ";
	}
	else {
		out << curNode->text << " ";
		serializeBinaryTree(curNode->left, out);
		serializeBinaryTree(curNode->right, out);
	}
}



//deserialize binary tree from file
void TextBinaryTree::deSerializeBinaryTreeFromFile(string filePath)
{
	//read file
	ifstream wordFile(filePath);
	string wholeFileText;

	//read size
	wordFile.seekg(0, std::ios::end);
	//allocate memory in advance
	wholeFileText.reserve(wordFile.tellg());
	wordFile.seekg(0, std::ios::beg);

	//read file into a string
	wholeFileText.assign((std::istreambuf_iterator<char>(wordFile)),
		std::istreambuf_iterator<char>());

	//convert string to vector
	string word;
	std::vector<std::string> pageWords;
	boost::split(pageWords, wholeFileText, boost::is_any_of("  ,\n,\t"));

//	treeNode *node;
	//node = root;
	int *index;
	int startValue = 0;
	index = &startValue;

	//deserialize binary tree from string
	deSerializeBinaryTree(root, pageWords, index);
}

//deserialize binary tree from string
void TextBinaryTree::deSerializeBinaryTree(treeNode *&curNode, vector<string> &treeWords, int *index)
{
	int token;
	bool isNumber;
	if (*index == treeWords.size() - 1)
	{
		return;
	}

	//get current word
	string word = treeWords[*index];
	*index = *index + 1;
	if (word == "##NULL##")
	{
		return;
	}

	treeNode *newNode = new treeNode();
	//push data into new node
	newNode->text = word;
	newNode->left = NULL;
	newNode->right = NULL;

	//insert new node in the tree
	if (root == NULL)
	{
		//set root to the first node
		root = newNode;
		curNode = root;
	}
	else
	{
		curNode = newNode;
	}

	deSerializeBinaryTree(curNode->left, treeWords, index);
	deSerializeBinaryTree(curNode->right, treeWords, index);
}
