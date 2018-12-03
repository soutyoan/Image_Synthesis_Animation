#include "bvhnode.h"

void makeLeaf(unsigned int index_, unsigned int n_objs_){
    isLeaf = true;
}

void makeNode(unsigned int left_index_, BVHNode left_node, BVHNode right_node, unsigned int n_objs){
    index = left_index_;
    this.left_node = left_node;
    this.right_node = right_node; 
}
