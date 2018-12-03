#include "trimesh_bvh.h"

using namespace std;

const int THRESHOLD = 100;

void TriMesh_bvh::build(const vector<trimesh::point*>  &objects){
    vector<trimesh::point*> objectsCopy; // Copy the vector
    vector<BVHNode*> nodes; // New node vector

    // Create the root node
    BVHNode *rootNode;

    // Creating the world box.
    trimesh::box worldBox;

    // Iterate over the objects in intersectable
    for (uint i = 0; i < objects.size(); i++){
        worldBox += objects[i]; // We enlarge the box to have the point in it.
        objectsCopy.push_back(objects[i]); // We copy the point before pushing it
    }

    rootNode->setBox(worldBox);

    build_recursive(0, objects.size(), rootNode, 0, objectsCopy);

    Root = rootNode; // Save the root Node

    for (uint i = 0; i < objectsCopy.size(); i++){
        vertices[i] = *objectsCopy[i]; // We save the vertices
    }

}

/*** 3 sort functions ***/
bool sortX(trimesh::point* A, trimesh::point* B){ return (A[0] < B[0]); }
bool sortY(trimesh::point* A, trimesh::point* B){ return (A[1] < B[1]); }
bool sortZ(trimesh::point* A, trimesh::point* B){ return (A[2] < B[2]); }

void TriMesh_bvh::sortInDirection(vector<trimesh::point*>  &objects, int left_index, int right_index, int coordinate){
    switch(coordinate){
        case 0:
            sort (objects.begin() + left_index, objects.begin()+right_index, sortX);
            break;
        case 1:
            sort (objects.begin() + left_index, objects.begin()+right_index, sortY);
            break;
        case 2:
            sort (objects.begin() + left_index, objects.begin()+right_index, sortZ);
            break;
    }
}

void TriMesh_bvh::build_recursive(int left_index, int right_index, BVHNode *node, int depth, vector<trimesh::point*>  &objects){
    if (right_index - left_index < THRESHOLD){ // Maybe put a maximal depth
        // Initiate current node as a leaf
        node->makeLeaf(left_index, right_index - left_index);
    } else {
        // Ways to split :
        // Depth % 3 == 0 : we split in x
        // Depth %3 == 1 : We split in y
        // Depth %3 == 2 : We split in z

        // We need to sort on the right way
        sortInDirection(objects, left_index, right_index, depth%3);

        // Split intersectables to left and right
        int split_index = (int)((left_index + right_index)/2);

        // Creation of the two boxes
        trimesh::box box_left;
        trimesh::box box_right;

        // Creation of the two nodes
        BVHNode* left_node;
        BVHNode* right_node;

        // Set child nodes to parent nodes
        node->makeNode(left_index, left_node, right_node, 3);

        for (int i = left_index; i < split_index; i++){ // [left_index, split_index[
            box_left += objects[i];
        }
        for (int i = split_index; i < right_index; i++){// [split_index, right_index[
            box_right += objects[i];
        }

        // Set the boxes
        left_node->setBox(box_left);
        right_node->setBox(box_right);

        // Build recursive
        build_recursive(left_index, split_index, left_node, depth+1, objects);
        build_recursive(split_index, right_index, right_node, depth+1, objects);

    }
}

/** Get the bbmin boxes **/
std::vector<trimesh::point*> TriMesh_bvh::get_all_bbmin(){
    std::vector<trimesh::point*> *bbmin = new std::vector<trimesh::point*>();
    Root->get_all_bbmin(bbmin);
    return *bbmin;
}

/** Get the bbmin boxes **/
std::vector<trimesh::point*> TriMesh_bvh::get_all_bbmax(){
    std::vector<trimesh::point*> *bbmax = new std::vector<trimesh::point*>();
    Root->get_all_bbmax(bbmax);
    return *bbmax;
}

/** Get the indices boxes **/
std::vector<trimesh::point*> TriMesh_bvh::get_all_indices(){
    std::vector<trimesh::point*> *indices = new std::vector<trimesh::point*>();
    Root->get_all_indices(indices);
    return *indices;
}
