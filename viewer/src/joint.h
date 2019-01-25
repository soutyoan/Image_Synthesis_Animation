#ifndef _JOINT_H_
#define _JOINT_H_

#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

namespace {
	const std::string kChannels = "CHANNELS";
	const std::string kEnd = "End";
	const std::string kEndSite = "End Site";
	const std::string kFrame = "Frame";
	const std::string kFrames = "Frames:";
	const std::string kHierarchy = "HIERARCHY";
	const std::string kJoint = "JOINT";
	const std::string kMotion = "MOTION";
	const std::string kOffset = "OFFSET";
	const std::string kRoot = "ROOT";
	const std::string kXpos = "Xposition";
	const std::string kYpos = "Yposition";
	const std::string kZpos = "Zposition";
	const std::string kXrot = "Xrotation";
	const std::string kYrot = "Yrotation";
	const std::string kZrot = "Zrotation";

}

class AnimCurve {
public :
	AnimCurve() {};
	~AnimCurve() {
		_values.clear();
	}
public :
	std::string name;					// name of dof
	std::vector<double> _values;		// different keyframes = animation curve
};


enum RotateOrder {roXYZ=0, roYZX, roZXY, roXZY, roYXZ, roZYX};

class Joint {
public:
	std::string _name;					// name of joint
	double _offX;						// initial offset in X
	double _offY;						// initial offset in Y
	double _offZ;						// initial offset in Z
	std::vector<AnimCurve> _dofs;		// keyframes : _animCurves[i][f] = i-th dof at frame f;
	double _curTx;						// current value of translation on X
	double _curTy;						// current value of translation on Y
	double _curTz;						// current value of translation on Z
	double _curRx;						// current value of rotation about X (deg)
	double _curRy;						// current value of rotation about Y (deg)
	double _curRz;						// current value of rotation about Z (deg)
	int _rorder;						// order of euler angles to reconstruct rotation
	std::vector<Joint*> _children;	// children of the current joint
	Joint* _parent; 						// parent of current joint


public:
	// Constructor :
	Joint() {};
	// Destructor :
	~Joint() {
		_dofs.clear();
		_children.clear();
	}

	// Create from data :
	static Joint* create(std::string name, double offX, double offY, double offZ, Joint* parent) {
		Joint* child = new Joint();
		child->_name = name;
		child->_offX = offX;
		child->_offY = offY;
		child->_offZ = offZ;
		child->_curTx = 0;
		child->_curTy = 0;
		child->_curTz = 0;
		child->_curRx = 0;
		child->_curRy = 0;
		child->_curRz = 0;
		if(parent != NULL) {
			parent->_children.push_back(child);
			child->_parent = parent;
		}
		return child;
	}

	// Load from file (.bvh) :
	static Joint* createFromFile(std::string fileName);

	void animate(int iframe=0);

	// Analysis of degrees of freedom :
	void nbDofs();

private:
	static void parser_hierarchy(ifstream& file, string& buf);

	static void parser_joints(ifstream& file, string& buf, Joint* root);

	static void parser_offset(ifstream& file, string& buf, string& name, double& _offx, double& _offy, double& _offz);

	static void parser_channels(ifstream& file, string& buf, Joint* current);

	static void parser_motion(ifstream& file, string& buf, Joint* root);

	static void parse_frame(ifstream& file, string& buf, Joint* current);
};


#endif
