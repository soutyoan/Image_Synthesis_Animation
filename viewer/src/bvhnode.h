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
    public:
        void setAABB(box &bbox_)
        void makeLeaf(unsigned int index_, unsigned int n_objs_)
        void makeNode(unsigned int left_index_, unsigned int n_objs)// n_objsin makeNodeis for debug purposes only, and may be omitted later on
        boolisLeaf() { return leaf; }
        unsigned intgetIndex() {
            return index;
        }
        unsigned int getNObjs() { return n_objs; }
        box &getAABB() {
            return bbox;
        };
};

#endif
