#include "../src/joint.h"

void createExampleJoint1(Joint &joint){
    joint = *Joint::create("ROOOT", 2, 1, 0, NULL);
    joint._curTx = 2;
    joint._curRx = 1;
    Joint::create("WESH", 5, 2, 1, &joint);

}

int main(){

    Joint j;
    // j.createFromFile("models/walk.bvh");
    createExampleJoint1(j);

    std::cout << j << "\n";

    return 0;
}
