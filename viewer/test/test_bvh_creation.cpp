#include "../src/joint.h"

int main(){

   Joint::createFromFile("viewer/models/walk.bvh");
   for (int i = 0; i < Joint::list_names.size(); i++){
     cout << "name " << Joint::findIndexOfJoint(Joint::list_names[i]) << " " << Joint::list_names[i] << endl;
   }

    return 0;
}
