#ifndef TRIMESH_H
#define TRIMESH_H

class TriMesh_bvh : public TriMesh {

public:
    void build(const std::vector<Intersectable*>  &objects);
    void build_recursive(int left_index, int right_index, BVHNode *node, int depth);

}

#endif
