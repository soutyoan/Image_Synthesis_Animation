#ifndef TRIMESH_BVH_H
#define TRIMESH_BVH_H

#include "TriMesh.h"
#include "bvhnode.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

struct SortPoints{
    int numero; 
    trimesh::point p;
    trimesh::point n;
    trimesh::Color c;
    trimesh::TriMesh::Face f;
};

class TriMesh_bvh : public trimesh::TriMesh {

private:
    BVHNode *Root;

public:

    void build();
    void build_recursive(int left_index, int right_index, BVHNode *node, int depth);

    void sortInDirection(int left_index, int right_index, int coordinate);

    std::vector<trimesh::point*> get_all_bbmin();
    std::vector<trimesh::point*> get_all_bbmax();
    std::vector<trimesh::point*> get_all_indices();

};

#endif
