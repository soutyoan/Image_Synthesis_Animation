#include "bvhnode.h"

void BVHNode::setBox(trimesh::box &bbox){
    this->bbox = bbox;
    // std::cerr << "test bvh" << this->bbox.min[0] << "\n";
}

void BVHNode::makeLeaf(int index_, int right_index, int _index_face){
    leaf = true;
    index = index_;
    right_index = right_index;
    index_face = _index_face;
}

void BVHNode::makeNode(int left_index_, BVHNode *left_node, BVHNode *right_node, int n_objs){
    index = left_index_;
    this->left_node = left_node;
    this->right_node = right_node;
}

void BVHNode::get_bvh_infos(std::vector<struct bvh> *infos){
    trimesh::point p(index, right_index, index_face, leaf ? 1 : 0);
    struct bvh s = bvh();
    s.bbmin = this->bbox.min;
    s.bbmax = this->bbox.max;
    s.other_info = p;
    // if (leaf) {
    //     std::cerr << index_face << "\n";
    // }
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
