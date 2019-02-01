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
    int expected = 0;
    while(!file.eof()){
        file >> expected;

        if (file.eof()){
            return;
        }

        if (expected != indexVertex) {
            std::cerr << "Unexpected element : " << expected << " should be " << indexVertex << endl;
        }

        Weight w(file, numberJoints, indicesJoints);
        VerticesWeights.push_back(w);
        indexVertex ++;
    }
}

Weight::Weight(ifstream &filestream, int sizeVector, vector<int> indicesJoints){
    weightJoints.resize(sizeVector);
    create(filestream, sizeVector, indicesJoints);
}

void Weight::create(ifstream &filestream, int sizeVector, vector<int> indicesJoints){
    float weightJ;
    for (int i = 0; i < sizeVector; i++){
        filestream >> weightJ;
        weightJoints[indicesJoints[i]] = weightJ;
    }
}
