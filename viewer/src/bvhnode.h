#ifndef BVHNODE_H
#define BVHNODE_H

#include "TriMesh.h"

struct bvh{
    trimesh::point bbmin;
    trimesh::point bbmax;
    trimesh::point other_info;
};

class BVHNode {

    private:
        trimesh::box bbox;
        bool leaf;
        int n_objs;
        int index;
        // if leaf == false: index to left child node,
        // else if leaf == true: index to first Intersectable in Objsvector
        BVHNode *left_node; // If this node is a parent : left child node.
        BVHNode *right_node; // If this node is a parent : right child node.

    public:
        void setBox(trimesh::box &bbox_);
        // Set the current Node as a Leaf
        void makeLeaf(int index_, int n_objs_);
        // Set current Node as a parent Node
        void makeNode(int left_index_, BVHNode *left_node, BVHNode *right_node, int n_objs);// n_objs in makeNode is for debug purposes only, and may be omitted later on
        bool isLeaf() {
            return leaf;
        }
        int getIndex() {
            return index;
        }
        int getNObjs() { return n_objs; }
        trimesh::box &getAABB() {
            return bbox;
        };

        void get_bvh_infos(std::vector<struct bvh> *infos);

        ~BVHNode();
};

#endif
