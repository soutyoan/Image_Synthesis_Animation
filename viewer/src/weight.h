#ifndef WEIGHT_H
#define WEIGHT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "joint.h"
#include "TriMesh.h"
#include <iostream>
#include <fstream>

using namespace std;

class Weight{

private:
	vector<float> weightJoints;

public:
	Weight();
	Weight(vector<float> &weightJoints);
    Weight(ifstream &filestream, int sizeVector,const vector<int>& indicesJoints);
	static void createFromFile(string filename, vector<Weight>& VerticesWeights);
    void create(ifstream &filestream, int sizeVector, const vector<int>& indicesJoints);
	float getWeight(int i) const{return weightJoints[i];}
	int size() const{return weightJoints.size(); }

	static void writeWeightsToFile(const vector<Weight> &verticesWeights);
	static void createRigidWeights(const vector<trimesh::point> &vertices, const vector<trimesh::point>& JointPosition,
		vector<Weight> &verticesWeights);
	static int findClosestArticulation(const trimesh::point &currentPoint, const vector<trimesh::point> &articulationPositions);

};


#endif
