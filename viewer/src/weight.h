#ifndef WEIGHT_H
#define WEIGHT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "joint.h"
#include "TriMesh.h"

using namespace std;

class Weight{

private:
	vector<float> weightJoints;

public:
    Weight(ifstream &filestream, int sizeVector,const vector<int>& indicesJoints);
	static void createFromFile(string &filename, vector<Weight>& VerticesWeights);
    void create(ifstream &filestream, int sizeVector,const vector<int>& indicesJoints);
	float getWeight(int i){return weightJoints[i];}


};


#endif
