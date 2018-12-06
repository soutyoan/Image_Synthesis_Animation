#ifndef TRIMESH_BVH_H
#define TRIMESH_BVH_H

#include "TriMesh.h"
#include "bvhnode.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

using namespace std;

class TriMesh_bvh : public trimesh::TriMesh {

public:

    void build(BVHNode* Root);
    void build_recursive(int left_index, int right_index, BVHNode *node, int depth);

    void sortInDirection(int left_index, int right_index, int coordinate);

    vector<struct bvh> get_bvh_info(BVHNode *Root);

    ~TriMesh_bvh();
};

#endif
