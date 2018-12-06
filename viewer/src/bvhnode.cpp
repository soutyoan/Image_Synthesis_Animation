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

void BVHNode::get_bvh_infos(std::vector<struct bvh> *infos){
    trimesh::point p(index, n_objs, 0, 0);
    struct bvh s = bvh();
    s.bbmin = this->bbox.min;
    s.bbmax = this->bbox.max;
    s.other_info = p;
    infos->push_back(s);
    if (!leaf){
        left_node->get_bvh_infos(infos);
        right_node->get_bvh_infos(infos);
    }
}

BVHNode::~BVHNode(){
    delete left_node;
    delete right_node;
}
