#include "trimesh_bvh.h"

using namespace std;

const int THRESHOLD = 10;

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

    rootNode.setAABB(worldBox);

    build_recursive(0, objects.size(), rootNode, 0);
}

void build_recursive(int left_index, int right_index, BVHNode *node, int depth){
    if (right_index - left_index < THRESHOLD){
        // Initiate current node as a leaf
        node.makeLeaf(left_index, right_index - left_index);
    } else {
        // Split intersectables to left and right
        int split_index = (int)((left_index + right_index)/2);

        // Creation of the two boxes
        box box_left;
        box right_box;

        // Creation of the two nodes
        BVHNode left_node;
        BVHNode right_node;

        node.makeNode(left_index, left_node, right_node); 

        for (int i = left_index; i < split_index; i++){ // [left_index, split_index[
            box_left +=
        }
        for (int i = split_index; i < right_index; i++){// [split_index, right_index[

        }

        build_recursive(left_index, split_index, left_node, depth+1);
        build_recursive(split_index, right_index, right_node, depth+1);

    }
}
