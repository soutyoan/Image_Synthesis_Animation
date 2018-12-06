#include "trimesh_bvh.h"

using namespace std;

const int THRESHOLD = 128;

void TriMesh_bvh::build(BVHNode *Root){
    vector<trimesh::point*> objectsCopy; // Copy the vector
    vector<BVHNode*> nodes; // New node vector

    for (int i = 0; i < faces.size(); i++){
        // std::cerr << i << "\n";
        _indices_faces.push_back(i);
    }

    // Creating the world box.
    trimesh::box worldBox;

    // Iterate over the objects in intersectable
    for (uint i = 0; i < vertices.size(); i++){
        worldBox += vertices[i]; // We enlarge the box to have the point in it.
    }

    Root->setBox(worldBox);

    build_recursive(0, (int)_indices_faces.size(), Root, 0);

    this->_indices_faces = _indices_faces;

}

void TriMesh_bvh::sortInDirection(int left_index, int right_index, int coordinate){

    // sort
    for (int i = left_index; i < right_index; i++){
        for (int j = left_index; j < i; j++){
            float xA = (vertices[faces[_indices_faces.at(i)][0]][coordinate] + vertices[faces[_indices_faces.at(i)][1]][coordinate]
                + vertices[faces[_indices_faces.at(i)][2]][coordinate]) / 3;
            float xB = (vertices[faces[_indices_faces.at(j)][0]][coordinate] + vertices[faces[_indices_faces.at(j)][1]][coordinate]
                + vertices[faces[_indices_faces.at(j)][2]][coordinate]) / 3;
            if (xB < xA){
                int tmp = _indices_faces.at(i);
                _indices_faces.at(i) = _indices_faces.at(j);
                _indices_faces.at(j) = tmp;
            }
        }
    }
}

void TriMesh_bvh::build_recursive(int left_index, int right_index, BVHNode *node, int depth){
    if (right_index - left_index < THRESHOLD){ // Maybe put a maximal depth
        // Initiate current node as a leaf
        node->makeLeaf(left_index, right_index - left_index);
    } else {
        // Ways to split :
        // Depth % 3 == 0 : we split in x
        // Depth %3 == 1 : We split in y
        // Depth %3 == 2 : We split in z

        // We need to sort on the right way
        sortInDirection(left_index, right_index, depth%3);

        // Split intersectables to left and right
        int split_index = (int)((left_index + right_index)/2);

        // Creation of the two boxes
        trimesh::box box_left;
        trimesh::box box_right;

        // Creation of the two nodes
        BVHNode* left_node = new BVHNode();
        BVHNode* right_node = new BVHNode();

        // Set child nodes to parent nodes
        node->makeNode(left_index, left_node, right_node, 3);

        for (int i = left_index; i < split_index; i++){ // [left_index, split_index[
            int a = _indices_faces.at(0);
            box_left += vertices[faces[_indices_faces.at(i)][0]];
            box_left += vertices[faces[_indices_faces.at(i)][1]];
            box_left += vertices[faces[_indices_faces.at(i)][2]];
        }

        for (int i = split_index; i < right_index; i++){// [split_index, right_index[
            box_right += vertices[faces[_indices_faces.at(i)][0]];
            box_right += vertices[faces[_indices_faces.at(i)][1]];
            box_right += vertices[faces[_indices_faces.at(i)][2]];
        }

        // Set the boxes
        left_node->setBox(box_left);
        right_node->setBox(box_right);

        // Build recursive
        build_recursive(left_index, split_index, left_node, depth+1);
        build_recursive(split_index, right_index, right_node, depth+1);

    }
}

std::vector<struct bvh> TriMesh_bvh::get_bvh_info(BVHNode *Root){
    std::vector<struct bvh> *bvh_infos = new std::vector<struct bvh>();
    Root->get_bvh_infos(bvh_infos);
    return *bvh_infos;
}

TriMesh_bvh::~TriMesh_bvh(){
    // delete Root;
}
