#include "bvhnode.h"

void BVHNode::setBox(trimesh::box &bbox_){
    this->bbox = bbox;
}

void BVHNode::makeLeaf(int index_, int n_objs_){
    leaf = true;
}

void BVHNode::makeNode(int left_index_, BVHNode *left_node, BVHNode *right_node, int n_objs){
    index = left_index_;
    this->left_node = left_node;
    this->right_node = right_node;
}

void BVHNode::get_all_bbmin(std::vector<trimesh::point*> *bbmin){
    bbmin->push_back(&this->bbox.min);
    if (!leaf){
        left_node->get_all_bbmin(bbmin);
        right_node->get_all_bbmin(bbmin);
    }
}

void BVHNode::get_all_bbmax(std::vector<trimesh::point*> *bbmax){
    bbmax->push_back(&this->bbox.max);
    if (!leaf){
        left_node->get_all_bbmax(bbmax);
        right_node->get_all_bbmax(bbmax);
    }
}

void BVHNode::get_all_indices(std::vector<trimesh::point*> *indices){
    trimesh::point p(index, index+n_objs, 0, 0);
    indices->push_back(&p);
    if (!leaf){
        left_node->get_all_indices(indices);
        right_node->get_all_indices(indices);
    }
}

BVHNode::~BVHNode(){
    delete left_node;
    delete right_node;
}
