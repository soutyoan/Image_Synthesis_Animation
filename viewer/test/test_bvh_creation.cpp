#include "../src/joint.h"
#include "../src/weight.h"
#include <string>

int main(){

   Joint::createFromFile("viewer/models/walk1.bvh");
   for (int i = 0; i < Joint::list_names.size(); i++){
     cout << "name " << Joint::findIndexOfJoint(Joint::list_names[i]) << " " << Joint::list_names[i] << endl;
   }

   vector<Weight> VerticesWeights;
   string filename = "viewer/models/weights.txt";
   Weight::createFromFile(filename, VerticesWeights);

    return 0;
}
