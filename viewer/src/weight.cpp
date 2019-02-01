#include "weight.h"

void Weight::createFromFile(string &filename, vector<Weight> &VerticesWeights){
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
    while(!file.eof()){
        Weight w(file, numberJoints, indicesJoints, indexVertex);
        VerticesWeights.push_back(w);
        indexVertex ++;
    }
}

Weight::Weight(ifstream &filestream, int sizeVector, vector<int> indicesJoints, int expectedElement){
    weightJoints.resize(sizeVector);
    create(filestream, sizeVector, indicesJoints, expectedElement);
}

void Weight::create(ifstream &filestream, int sizeVector, vector<int> indicesJoints, int expectedElement){
    int expected;
    filestream >> expected;

    if (expected != expectedElement) {
        std::cerr << "Unexpected element : " << expected << " should be " << expectedElement << endl;
    }

    float weightJ;
    for (int i = 0; i < sizeVector; i++){
        filestream >> weightJ;
        indicesJoints[indicesJoints[i]] = weightJ;
    }
}
