#include "bvhnode.h"

void BVHNode::setBox(trimesh::box &bbox_){
    this->bbox = bbox;
}

void BVHNode::makeLeaf(unsigned int index_, unsigned int n_objs_){
    leaf = true;
}

void BVHNode::makeNode(unsigned int left_index_, BVHNode *left_node, BVHNode *right_node, unsigned int n_objs){
    index = left_index_;
    this->left_node = left_node;
    this->right_node = right_node;
}

void BVHNode::get_all_bbmin(std::vector<trimesh::point*> *bbmin){
    bbmin->push_back(this->bbox.min);
    if (!leaf){
        left_node->get_all_bbmin(bbmin);
        right_node->get_all_bbmin(bbmin);
    }
}

void BVHNode::get_all_bbmax(std::vector<trimesh::point*> *bbmax){
    bbmax->push_back(this->bbox.max);
    if (!leaf){
        left_node->get_all_bbmin(bbmax);
        right_node->get_all_bbmin(bbmax);
    }
}
