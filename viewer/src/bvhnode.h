#ifndef BVHNODE_H
#define BVHNODE_H

#include "Box.h"

class BVHNode {
    
    private:
        box bbox;
        bool leaf;
        unsigned int n_objs;
        unsigned int index;
        // if leaf == false: index to left child node,
        // else if leaf == true: index to first Intersectable in Objsvector
        BVHNode left_node; // If this node is a parent : left child node.
        BVHNode right_node; // If this node is a parent : right child node.

    public:
        void setAABB(box &bbox_);
        // Set the current Node as a Leaf
        void makeLeaf(unsigned int index_, unsigned int n_objs_);
        // Set current Node as a parent Node
        void makeNode(unsigned int left_index_, BVHNode left_node, BVHNode right_node, unsigned int n_objs);// n_objs in makeNode is for debug purposes only, and may be omitted later on
        bool isLeaf() {
            return leaf;
        }
        unsigned int getIndex() {
            return index;
        }
        unsigned int getNObjs() { return n_objs; }
        box &getAABB() {
            return bbox;
        };
};

#endif
