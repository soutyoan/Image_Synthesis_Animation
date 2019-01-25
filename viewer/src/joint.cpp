#include "joint.h"
#include <QtGui/QMatrix4x4>

using namespace std;

Joint* Joint::createFromFile(std::string fileName) {
	Joint* root = NULL;
	cout << "Loading from " << fileName << endl;

	ifstream inputfile(fileName.data());
	if(inputfile.good()) {
		while(!inputfile.eof()) {
			string buf;
			Joint::parser_hierarchy(inputfile, buf);
			Joint::parser_joints(inputfile, buf, root);
			Joint::parser_motion(inputfile, buf, root);
		}
		inputfile.close();
	} else {
		std::cerr << "Failed to load the file " << fileName.data() << std::endl;
		fflush(stdout);
	}
	cout << "file loaded" << endl;
	return root;
}

/**
 * Parser for HIERARCHY format in BVH file
 * @param file input .bvh file
 * @param buf  current token
 */
void Joint::parser_hierarchy(ifstream& file, string& buf) {
	file >> buf;
	if (buf!=kHierarchy) {
		cerr << "Could not load .BVH file : missing HIERARCHY keyword"<<endl;
	}
}

/**
 * Parser for Joints part of a .bvh file
 * @param file input .bvh file
 * @param buf  current token
 * @param root Joint pointing to first Joint member
 */
void Joint::parser_joints(ifstream& file, string& buf, Joint* root) {
	file >> buf;
	if (buf!=kRoot) {
		cerr << "Could not load .BVH file : missing ROOT keyword"<<endl;
	}
	Joint* parent;
	Joint* current = root;
	double _offx; double _offy; double _offz;
	string name;

	// Creating root Joint
	Joint::parser_offset(file, buf, name, _offx, _offy, _offz);
	*current = *Joint::create(name, _offx, _offy, _offz, parent);

	Joint::parser_channels(file, buf, current);

	// Children joints parsing
	while (buf!=kMotion) {
		file >> buf;
		if (buf==kJoint) {
			parent = current;
			Joint::parser_offset(file, buf, name, _offx, _offy, _offz);
			current = Joint::create(name, _offx, _offy, _offz, parent);
			Joint::parser_channels(file, buf, current);
		}
		else if (buf==kEnd) {
			parent = current;
			Joint::parser_offset(file, buf, name, _offx, _offy, _offz);
			current = Joint::create(name, _offx, _offy, _offz, parent);
			Joint::parser_channels(file, buf, current);
		}
		else if (buf=="}") {
			current = current->_parent;
		}
		else {
			cerr << "Could not parse .BVH file: unknown word: "<<buf<<endl;
		}
	}
}

/**
 * Parsing a Joint OFFSET by updating name, _offx, _offy, _offz values
 * @param file  input .bvh file
 * @param buf   current token
 * @param name  name of the current joint
 * @param _offx OFFX value
 * @param _offy OFFY value
 * @param _offz OFFZ value
 */
void Joint::parser_offset(ifstream& file, string& buf, string& name, double& _offx, double& _offy, double& _offz) {
	file >> name;
	file >> buf; // parsing '{'
	file >> buf;
	if (buf!=kOffset) {
		cerr << "Could not parse .BVH file: missing OFFSET word" << endl;
	}
	try {
		file >> _offx >> _offy >> _offz;
	}
	catch(...) {
		cerr << "Could not read offset values of current Joint"<<endl;
	}
}

/**
 * Parsing a channel of a current joint
 * @param file    input .bvh file
 * @param buf     current token
 * @param current current Joint
 */
void Joint::parser_channels(ifstream& file, string& buf, Joint* current) {
	file >> buf;
	if (buf!=kChannels) {
		cerr << "Could not parse .BVH file: missing CHANNELS word "<<endl;
	}
	file >> buf;
	int _nb = stoi(buf);
	for (int i=0; i<_nb; i++) {
		file >> buf;
		AnimCurve _currAnim;
		_currAnim.name = buf;
		current->_dofs.push_back(_currAnim);
	}
}

/**
 * Parsing motion part with all data at each frame
 * @param file input .bvh file
 * @param buf  current token
 * @param root Joint of root
 */
void Joint::parser_motion(ifstream& file, string& buf, Joint* root) {
	if (buf!=kMotion) {
		cerr << "Could not parse .BVH file: missing MOTION word"<<endl;
	}
	file >> buf; // parsing 'Frames:'
	file >> buf;
	int nb_frames = stoi(buf);
	file >> buf; // parsing 'Frame' 'Time:' '<value>'
	file >> buf;
	file >> buf;
	file >> buf;
	for (int frame=0; frame<nb_frames; frame++) {
		Joint::parse_frame(file, buf, root);
	}
}

/**
 * Parsing a frame line for all joints (recursive way)
 * @param file    input .bvh file
 * @param buf     current token
 * @param current current Joint
 */
void Joint::parse_frame(ifstream& file, string& buf, Joint* current) {
	if (current==NULL) {
		cerr << "Error while parsing frame : current Joint cannot be NULL" << endl;
	}
	int nb_channels = current->_dofs.size();
	for (int i=0; i<nb_channels; i++) {
		current->_dofs[i]._values.push_back(stod(buf));
		file >> buf;
	}
	vector<Joint*>::iterator children;
	for (children=current->_children.begin(); children!=current->_children.end(); children++) {
		Joint::parse_frame(file, buf, *children);
	}
}

void Joint::animate(int iframe)
{
	// Update dofs :
	_curTx = 0; _curTy = 0; _curTz = 0;
	_curRx = 0; _curRy = 0; _curRz = 0;
	for (unsigned int idof = 0 ; idof < _dofs.size() ; idof++) {
		if(!_dofs[idof].name.compare("Xposition")) _curTx = _dofs[idof]._values[iframe];
		if(!_dofs[idof].name.compare("Yposition")) _curTy = _dofs[idof]._values[iframe];
		if(!_dofs[idof].name.compare("Zposition")) _curTz = _dofs[idof]._values[iframe];
		if(!_dofs[idof].name.compare("Zrotation")) _curRz = _dofs[idof]._values[iframe];
		if(!_dofs[idof].name.compare("Yrotation")) _curRy = _dofs[idof]._values[iframe];
		if(!_dofs[idof].name.compare("Xrotation")) _curRx = _dofs[idof]._values[iframe];
	}
	// Animate children :
	for (unsigned int ichild = 0 ; ichild < _children.size() ; ichild++) {
		_children[ichild]->animate(iframe);
	}
}


void Joint::nbDofs() {
	if (_dofs.empty()) return;

	double tol = 1e-4;

	int nbDofsR = -1;

	// TODO :
	cout << _name << " : " << nbDofsR << " degree(s) of freedom in rotation\n";

	// Propagate to children :
	for (unsigned int ichild = 0 ; ichild < _children.size() ; ichild++) {
		_children[ichild]->nbDofs();
	}

}
