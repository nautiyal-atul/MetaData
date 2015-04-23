/*
 * TextBinaryTree.h
 *
 *  Created on: Sep 16, 2014
 *      Author: atul
 */

#ifndef TEXTBINARYTREE_H_
#define TEXTBINARYTREE_H_

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "Utility.h"


using namespace std;

class TextBinaryTree
{
public:

	//tree node
	struct treeNode
	{
		treeNode *left, *right;
		string text;
	};

	treeNode *root;

public:
	TextBinaryTree();
	~TextBinaryTree();
	//stop word tree

	//load binary tree
	void loadBinaryTreeFromFile(string);
	void loadBinaryTreeFromString(string);
	void loadBinaryTree(vector<string> &);

	//seach in binary tree
	int searchBinaryTree(string);

	//search the best string in the tree
	pair<string, float> getBestMatch(string);
	void inOrderLenghtBasedMatch(treeNode *, string, string*, float*);
	void inOrderLeveinsteinDistanceBasedMatch(treeNode *curNode, string word, string *likelyWord, float *highscore);

	//serialize and deserialize binary tree
	void serializeBinaryTreeINFile(string);
	void serializeBinaryTree(treeNode *, ofstream &);
	void deSerializeBinaryTreeFromFile(string);
	void deSerializeBinaryTree(treeNode *, treeNode *, vector<string> &, int);
	void deSerializeBinaryTree(treeNode *&, vector<string> &, int *);

	//save tree text in a file
	void saveTreeInFile(string);
	void inOrderPrint(treeNode *curNode, ofstream &);

private:
	Utility *util;

};


#endif /* TEXTBINARYTREE_H_ */
