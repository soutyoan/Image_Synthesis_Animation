#ifndef WEIGHT_H
#define WEIGHT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "joint.h"


using namespace std;

class Weight{

private:
	vector<float> weightJoints;

public:
	Weight(ifstream &filestream, int sizeVector, vector<int> indicesJoints, int expectedElement);
	static void createFromFile(string &filename, vector<Weight>& VerticesWeights);
	void create(ifstream &filestream, int sizeVector, vector<int> indicesJoints, int expectedElement);

};


#endif
