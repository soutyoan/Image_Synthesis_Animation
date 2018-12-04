#include "trimesh_bvh.h"

using namespace std;

const int THRESHOLD = 128;

void TriMesh_bvh::build(){
    vector<trimesh::point*> objectsCopy; // Copy the vector
    vector<BVHNode*> nodes; // New node vector

    // Create the root node
    BVHNode *rootNode = new BVHNode();

    // Creating the world box.
    trimesh::box worldBox;

    // Iterate over the objects in intersectable
    for (uint i = 0; i < vertices.size(); i++){
        worldBox += vertices[i]; // We enlarge the box to have the point in it.
    }

    rootNode->setBox(worldBox);

    build_recursive(0, (int)vertices.size(), rootNode, 0);

    Root = rootNode; // Save the root Node

}

/*** 3 sort functions ***/
bool sortX(SortPoints A, SortPoints B){ return (A.p[0] < B.p[0]); }
bool sortY(SortPoints A, SortPoints B){ return (A.p[1] < B.p[1]); }
bool sortZ(SortPoints A, SortPoints B){ return (A.p[2] < B.p[2]); }

void TriMesh_bvh::sortInDirection(int left_index, int right_index, int coordinate){

    std:cerr << "Building model \n";

    vector<SortPoints> s;
    for (int i = left_index; i < right_index; i++){
        struct SortPoints sp;
        sp.numero = i;
        sp.p = vertices[i];
        sp.n = normals[i];
        sp.c = colors[i];
        s.push_back(sp);
    }

    std::cerr << "pre sort" << vertices[0][0] << " " << vertices[0][1] << " " << vertices[0][2] << "\n";
    std::cerr << "pre sort" << vertices[1][0] << " " << vertices[1][1] << " " << vertices[1][2] << "\n";
    std::cerr << "pre sort" << vertices[2][0] << " " << vertices[2][1] << " " << vertices[2][2] << "\n";

    switch(coordinate){
        case 0:
            sort (s.begin(), s.end(), sortX);
            break;
        case 1:
            sort (s.begin(), s.end(), sortY);
            break;
        case 2:
            sort (s.begin(), s.end(), sortZ);
            break;
    }

    for (int i = 0; i < right_index - left_index; i++){
        vertices[i+left_index] = s[i].p;
        normals[i+left_index] = s[i].n;
        colors[i+left_index] = s[i].c;
    }

    std::cerr << "post sort" << vertices[0][0] << " " << vertices[0][1] << " " << vertices[0][2] << "\n";
    std::cerr << "post sort" << vertices[1][0] << " " << vertices[1][1] << " " << vertices[1][2] << "\n";
    std::cerr << "post sort" << vertices[2][0] << " " << vertices[2][1] << " " << vertices[2][2] << "\n";


    for (int i = 0; i < faces.size(); i++){
        for (int j = left_index; j < right_index; j++){
            if (faces[i][0] == s[j].numero){
                // std::cerr << "found face " << faces[i+left_index][0] << " " << faces[i+left_index][1] << " " << faces[i+left_index][2] << "\n";
                faces[i][0] = j;
                // std::cerr << s[j].numero << " " << j << "\n";
                // std::cerr << "new face " << faces[i+left_index][0] << " " << faces[i+left_index][1] << " " << faces[i+left_index][2] << "\n";
            }
            if (faces[i][1] == s[j].numero){
                faces[i][1] = j;
            }
            if (faces[i][2] == s[j].numero){
                faces[i][2] = j;
            }
        }
    }

    std::cerr << left_index << " " << right_index << "\n";
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
            box_left += vertices[i];
        }

        for (int i = split_index; i < right_index; i++){// [split_index, right_index[
            box_right += vertices[i];
        }

        // Set the boxes
        left_node->setBox(box_left);
        right_node->setBox(box_right);

        return;

        // Build recursive
        build_recursive(left_index, split_index, left_node, depth+1);
        build_recursive(split_index, right_index, right_node, depth+1);

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
