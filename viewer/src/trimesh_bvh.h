#ifndef TRIMESH_BVH_H
#define TRIMESH_BVH_H

#include "TriMesh.h"
#include "bvhnode.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

using namespace std;

class TriMesh_bvh : public trimesh::TriMesh {

private:
    BVHNode *Root;
    vector<int> *_indices_faces;

public:

    void build(vector<int> *indices_faces);
    void build_recursive(int left_index, int right_index, BVHNode *node, int depth, vector<int> *indices_faces);

    void sortInDirection(int left_index, int right_index, int coordinate, vector<int> *indices_faces);

    vector<trimesh::point*> get_all_bbmin();
    vector<trimesh::point*> get_all_bbmax();
    vector<int> get_all_indices();

};

#endif
