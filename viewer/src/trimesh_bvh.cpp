#include "trimesh_bvh.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

using namespace std;

const int THRESHOLD = 100;

void Trimesh_bvh::build(const vector<Point*>  &objects){
    vector<Intersectable*> objectsCopy; // Copy the vector
    vector<BVHNode*> nodes; // New node vector

    // Create the root node
    BVHNode rootNode;

    // Creating the world box.
    box worldBox;

    // Iterate over the objects in intersectable
    for (int i = 0; i < objects.size(); i++){
        worldBox += objects[i]; // We enlarge the box to have the point in it.
        objectsCopy.push_back(Point(objects[i])); // We copy the point before pushing it
    }

    rootNode.setBox(worldBox);

    build_recursive(0, objects.size(), rootNode, 0, objectsCopy);
}

/*** 3 sort functions ***/
bool sortX(Point* A, Point* B){ return (A->x < B->x); }
bool sortY(Point* A, Point* B){ return (A->y < B->y); }
bool sortZ(Point* A, Point* B){ return (A->z < B->z); }

void sort(vector<Point*>  &objects, int left_index, int right_index, int coordinate){
    switch(coordinate){
        case 0:
            sort (objects.begin() + left_index, myvector.begin()+right_index, sortX);
            break;
        case 1:
            sort (objects.begin() + left_index, myvector.begin()+right_index, sortY);
            break;
        case 2:
            sort (objects.begin() + left_index, myvector.begin()+right_index, sortZ);
            break;
    }
}

void build_recursive(int left_index, int right_index, BVHNode *node, int depth, vector<Point*>  &objects){
    if (right_index - left_index < THRESHOLD){ // Maybe put a maximal depth
        // Initiate current node as a leaf
        node.makeLeaf(left_index, right_index - left_index);
    } else {
        // Ways to split :
        // Depth % 3 == 0 : we split in x
        // Depth %3 == 1 : We split in y
        // Depth %3 == 2 : We split in z

        // We need to sort on the right way
        sort(objects, left_index, right_index, depth%3);

        // Split intersectables to left and right
        int split_index = (int)((left_index + right_index)/2);

        // Creation of the two boxes
        box box_left;
        box right_box;

        // Creation of the two nodes
        BVHNode* left_node;
        BVHNode* right_node;

        // Set child nodes to parent nodes
        node.makeNode(left_index, left_node, right_node);

        for (int i = left_index; i < split_index; i++){ // [left_index, split_index[
            box_left += objects[i];
        }
        for (int i = split_index; i < right_index; i++){// [split_index, right_index[
            box_right += objects[i];
        }

        // Set the boxes
        left_node.setBox(box_left);
        right_node.setBox(box_right);

        // Build recursive
        build_recursive(left_index, split_index, left_node, depth+1);
        build_recursive(split_index, right_index, right_node, depth+1);

    }
}
