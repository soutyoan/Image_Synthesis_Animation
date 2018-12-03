#ifndef TRIMESH_BVH_H
#define TRIMESH_BVH_H

#include "TriMesh.h"
#include "bvhnode.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

class TriMesh_bvh : public trimesh::TriMesh {

private:
    BVHNode Root;

public:

    void build(const std::vector<trimesh::point*>  &objects);
    void build_recursive(int left_index, int right_index, BVHNode *node, int depth, std::vector<trimesh::point*>  &objects);

    void sortInDirection(std::vector<trimesh::point*>  &objects, int left_index, int right_index, int coordinate);

    std::vector<trimesh::point*> get_all_bbmin();
    std::vector<trimesh::point*> get_all_bbmax();

};

#endif
