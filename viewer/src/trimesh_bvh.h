#ifndef TRIMESH_H
#define TRIMESH_H

#include "bvhnode.h"
#include "Box.h"

class TriMesh_bvh : public TriMesh {

public:
    void build(const std::vector<Intersectable*>  &objects);
    void build_recursive(int left_index, int right_index, BVHNode *node, int depth);

    void sort(vector<Point*>  &objects, int left_index, int right_index, int coordinate, vector<Point*>  &objects); 

}

#endif
