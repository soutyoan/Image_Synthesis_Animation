#include "weight.h"

Weight::Weight(vector<float> &_weightJoints){
    weightJoints = _weightJoints;
}

void Weight::createFromFile(string filename, vector<Weight> &VerticesWeights){
    ifstream file(filename.data());
    string buf;
    file >> buf;

    if (buf != "id"){std::cerr << "WARNING : unexpected " << buf << " expected id" << endl;}
    int numberJoints = Joint::list_names.size();
    vector<int> indicesJoints(numberJoints);

    for(int i = 0; i < numberJoints; i++){
        file >> buf;
        indicesJoints[i] = Joint::findIndexOfJoint(buf);
    }

    int indexVertex = 0;
    int expected = 0;
    while(!file.eof()){
        file >> expected;

        if (file.eof()){
            return;
        }

        if (expected != indexVertex) {
            if ((indexVertex == 0) || (expected == 0)){
                std::cerr << "Your bvh file does not seem to have the expected number "
                "of joints by the weights file." << endl;
                exit(EXIT_FAILURE);
            }
            std::cerr << "Unexpected element : " << expected << " should be " << indexVertex << endl;
            exit(EXIT_FAILURE);
        }

        Weight w(file, numberJoints, indicesJoints);
        VerticesWeights.push_back(w);
        indexVertex ++;
    }
}

Weight::Weight(ifstream &filestream, int sizeVector,const vector<int>& indicesJoints){
    weightJoints.resize(sizeVector);
    create(filestream, sizeVector, indicesJoints);
}

void Weight::create(ifstream &filestream, int sizeVector,const vector<int>& indicesJoints){
    float weightJ;
    for (int i = 0; i < sizeVector; i++){
        filestream >> weightJ;
        weightJoints[indicesJoints[i]] = weightJ;
    }
}

void Weight::writeWeightsToFile(const vector<Weight> &verticesWeights){

    ofstream weightsFile;
    weightsFile.open ("viewer/models/new_weights.txt");
    cerr << "TEST" << endl;

    weightsFile << "id ";

    for (int i = 0; i < Joint::list_names.size(); i++){
        weightsFile << Joint::list_names[i] << " ";
    }

    weightsFile << endl;

    for (int i = 0; i < verticesWeights.size(); i++){
        weightsFile << i << " ";
        for (int j = 0; j < verticesWeights[i].size(); j++){
            weightsFile << verticesWeights[i].getWeight(j) << " ";
        }
        weightsFile << endl;
    }

    weightsFile.close();
}

void Weight::createRigidWeights(const vector<trimesh::point> &vertices, const vector<trimesh::point>& JointPosition,
    vector<Weight> &verticesWeights){

    int numberArticulations = JointPosition.size();

    for (int  i = 0; i < vertices.size(); i++){
        int index = findClosestArticulation(vertices[i], JointPosition);
        vector<float> weightsVector(numberArticulations);
        for (int j = 0; j < numberArticulations; j++){
            weightsVector[j] = (j==index) ? 1 : 0;
        }
        Weight w(weightsVector);
        verticesWeights.push_back(w);
    }
}

int Weight::findClosestArticulation(const trimesh::point &currentPoint, const vector<trimesh::point> &articulationPositions){
    int closestIndex = 0;
    float closest = trimesh::dist(articulationPositions[0], currentPoint);
    for (int i = 1; i < articulationPositions.size(); i++){
        float newDistance = trimesh::dist(articulationPositions[i], currentPoint);
        if (newDistance < closest){
            closest = newDistance;
            closestIndex = i;
        }
    }
    return closestIndex;
}
