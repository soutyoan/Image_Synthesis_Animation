#include "joint.h"

using namespace std;

int Joint::GLOBAL_INDEX = 0;
float Joint::FRAME_RATE = 0.0;
std::vector<string> Joint::list_names;

Joint::Joint(){
	local_index = GLOBAL_INDEX;
	GLOBAL_INDEX++;
	// std::cout << "Index current " << local_index << endl;
}

Joint* Joint::createFromFile(std::string fileName) {
	Joint::GLOBAL_INDEX = 0;
	Joint* root = new Joint();
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
		exit (EXIT_FAILURE);
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
		exit (EXIT_FAILURE);
	}
	Joint* parent;
	Joint* current = root;
	double _offx; double _offy; double _offz;
	string name;

	// Creating root Joint
	Joint::parser_offset(file, buf, name, _offx, _offy, _offz);
	*current = *Joint::create(name, _offx, _offy, _offz, NULL);
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
		}
		else if (buf=="}") {
			current = current->_parent;
		}
		else if (buf.compare(kMotion)==0) {
			break;
		}
		else {
			cerr << "Could not parse .BVH file: unknown word: "<<buf<<endl;
			exit (EXIT_FAILURE);
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
		exit (EXIT_FAILURE);
	}
	try {
		file >> _offx >> _offy >> _offz;
	}
	catch(...) {
		cerr << "Could not read offset values of current Joint"<<endl;
		exit (EXIT_FAILURE);
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
		exit (EXIT_FAILURE);
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
		exit (EXIT_FAILURE);
	}
	file>>buf; // parsing 'Frames:'
	file>>buf;
	int nb_frames = stoi(buf);
	file>>buf; // parsing 'Frame' 'Time:' '<value>'
	file>>buf;
	double _rate;
	file>>_rate;
	Joint::FRAME_RATE = _rate;
	file>>buf;
	for (int frame=0; frame<nb_frames; frame++) {
		// cout<<"frame "<<frame<<endl;
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
		exit (EXIT_FAILURE);
	}
	int nb_channels = current->_dofs.size();
	// cout<<"nb "<<nb_channels<<endl;
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

std::ostream& operator<<(std::ostream& os, const Joint& joint){
	int global_tab_count = 0;
	joint.printRecursivly(os, &global_tab_count);
	return os;


}

void multTab(std::ostream& os, int *global_tab_count){
	for (int i = 0; i < *global_tab_count; i++)
		os << "   ";
}


void Joint::printRecursivly(std::ostream& os, int *global_tab_count) const{
	multTab(os, global_tab_count);
	os << "JOINT" << this->_name << "\n";
	multTab(os, global_tab_count);
	os << "{\n";

	*global_tab_count = *global_tab_count + 1;
	multTab(os, global_tab_count);
	os << "OFFSET " << this->_offZ <<
	" " << this->_offY << " " << this->_offX << "\n";
	if (this->nbRotation() + this->nbTranslation() != 0){
		multTab(os, global_tab_count);
		os << "CHANNELS " << this->nbRotation() + this->nbTranslation() << " ";
		if (this->nbTranslation() != 0)
			os << this->_curTz << " " << this->_curTy << " " << this->_curTx << " ";
		if (this->nbRotation() != 0)
			os << this->_curRz << " " << this->_curRy << " " << this->_curRx << " ";
		os << "\n";
	}
	for (int i = 0; i < int(this->_children.size()); i++)
	{
		Joint* it = this->_children[i];
		// if (*it != NULL){
			it->printRecursivly(os, global_tab_count);
		// }
	}

	*global_tab_count = *global_tab_count - 1;
	multTab(os, global_tab_count);
	os << "}\n";

}

int Joint::nbRotation() const{
	int result = 0;
	if (this->_curRx != 0)
		result ++;
	if (this->_curRy != 0)
		result ++;
	if (this->_curRz != 0)
		result ++;
	return result;

}

int Joint::nbTranslation() const{
	int result = 0;
	if (this->_curTx != 0)
		result ++;
	if (this->_curTy != 0)
		result ++;
	if (this->_curTz != 0)
		result ++;
	return result;
}


int Joint::findIndexOfJoint(string name){
	auto it = std::find(Joint::list_names.begin(), Joint::list_names.end(), name);
	if (it == Joint::list_names.end()) {
	  return -1;
	}
  	auto index = std::distance(Joint::list_names.begin(), it);
	return (int)index;
}

void Joint::getTransformationMatrices(std::vector<QMatrix4x4>& bindedMatrices, std::vector<QMatrix4x4>& transformMatrices){
    QMatrix4x4 positionOffset;
    QMatrix4x4 rotation;
    getChildTransformationMatrices(bindedMatrices, transformMatrices, positionOffset, rotation);
}

void Joint::getChildTransformationMatrices(std::vector<QMatrix4x4>& bindedMatrices,std::vector<QMatrix4x4>& transformMatrices,
                                    QMatrix4x4 parentPosition, QMatrix4x4 parentTransformation){
    QMatrix4x4 childPosition;
    QMatrix4x4 childTransformation;
    childPosition = parentPosition;
    childTransformation = parentTransformation;
    childPosition.translate(_offX, _offY, _offZ);
    childTransformation = parentTransformation;
//    if (_parent != NULL){
//        childTransformation.rotate(this->_parent->_curRz, 0, 0, 1);
//        childTransformation.rotate(this->_parent->_curRy, 0, 1, 0);
//        childTransformation.rotate(this->_parent->_curRx, 1, 0, 0);
//    }
    childTransformation.translate(_offX, _offY, _offZ);
    childTransformation.translate(_curTx, _curTy, _curTz);

    childTransformation.rotate(_curRz, 0, 0, 1);
    childTransformation.rotate(_curRy, 0, 1, 0);
    childTransformation.rotate(_curRx, 1, 0, 0);

    bindedMatrices.push_back(childPosition);
    transformMatrices.push_back(childTransformation);
    for (int i = 0; i < _children.size(); i++){
        _children[i]->getChildTransformationMatrices(bindedMatrices, transformMatrices, childPosition, childTransformation);
    }
}


vector<trimesh::point> Joint::exportPositions(){
	if (_parent != NULL){
		std::cerr << "You can only call this method on root" << endl;
		return vector<trimesh::point>();
	}
	vector<trimesh::point> positions;
	QMatrix4x4 matrix;
	matrix.translate(_offX, _offY, _offZ); // global offset
//	matrix.translate(_curTx, _curTy, _curTz); // frame translation
//	matrix.rotate(_curRx, 1, 0, 0);
//	matrix.rotate(_curRy, 0, 1, 0);
//	matrix.rotate(_curRz, 0, 0, 1); // frame rotation
	QVector3D positionRoot;
	positionRoot = matrix * QVector3D(0, 0, 0);
	float x = float(positionRoot.x());
	float y = float(positionRoot.y());
	float z = float(positionRoot.z());
	trimesh::point currentPosition(x, y, z, 1);
	positions.push_back(currentPosition);
	exportChildPositions(matrix, positionRoot, positions);
	return positions;
}

void Joint::exportChildPositions(QMatrix4x4& matriceTransformation, QVector3D& positionRoot, vector<trimesh::point> &positions){
	for (int i = 0; i < this->_children.size(); i++){

		QMatrix4x4 matrix;
		matrix = matriceTransformation;
		matrix.translate(_offX, _offY, _offZ); // global offset
		QVector3D positionChild;
		positionChild = matriceTransformation * QVector3D(0, 0, 0);
		float x = float(positionChild.x());
		float y = float(positionChild.y());
		float z = float(positionChild.z());
		trimesh::point currentPosition(x, y, z, 1);
		positions.push_back(currentPosition);
		_children[i]->exportChildPositions(matriceTransformation, positionRoot, positions);
	}
	matriceTransformation.rotate(-_curRz, 0, 0, 1);
	matriceTransformation.rotate(-_curRy, 0, 1, 0);
	matriceTransformation.rotate(-_curRx, 1, 0, 0);
	matriceTransformation.translate(-_curTx, -_curTy, -_curTz);
	matriceTransformation.translate(-_offX, -_offY, -_offZ);
}

void Joint::exportPositions(QMatrix4x4& transform, vector<trimesh::point>& positions)
{
	transform.translate(_offX, _offY, _offZ);
	transform.translate(_curTx, _curTy, _curTz);
	transform.rotate(_curRz, 0, 0, 1);
	transform.rotate(_curRy, 0, 1, 0);
	transform.rotate(_curRx, 1, 0, 0);
	QVector3D pos = transform * QVector3D(0, 0, 0);
	float x = float(pos.x());
	float y = float(pos.y());
	float z = float(pos.z());
	trimesh::point vertex(x, y, z, 1.0);
	positions.push_back(vertex);
	for (int i=0; i<this->_children.size(); i++) {
		this->_children[i]->exportPositions(transform, positions);
	}
	transform.rotate(-_curRx, 1, 0, 0);
	transform.rotate(-_curRy, 0, 1, 0);
	transform.rotate(-_curRz, 0, 0, 1);
	transform.translate(-_curTx, -_curTy, -_curTz);
	transform.translate(-_offX, -_offY, -_offZ);
}

vector<trimesh::point> Joint::exportMiddleArticulations(){
    if (_parent != NULL){
        std::cerr << "You can only call this method on root" << endl;
        return vector<trimesh::point>();
    }
    vector<trimesh::point> positions;
    QMatrix4x4 matrix;
    QVector3D positionRoot;
    exportChildMiddleArticulations(matrix, positionRoot, positions);
    return positions;
}

void Joint::exportChildMiddleArticulations(QMatrix4x4& matriceTransformation, QVector3D& positionRoot, vector<trimesh::point> &positions){
    QMatrix4x4 matrix;
    matrix = matriceTransformation;
    matrix.translate(_offX, _offY, _offZ); // global offset
    QVector3D positionChild;
    positionChild = matrix * QVector3D(0, 0, 0);
    float x = float(positionChild.x());
    float y = float(positionChild.y());
    float z = float(positionChild.z());
    trimesh::point currentPosition(x, y, z, 1);

    if (this->_children.size() > 1){
        positions.push_back(currentPosition);
    } else if (this->_children.size() == 1){
        Joint *child = _children[0];
        QMatrix4x4 matrixChild;
        matrixChild = matrix;
        matrixChild.translate(child->_offX, child->_offY, child->_offZ); // global offset
        QVector3D positionChild;
        positionChild = matrixChild * QVector3D(0, 0, 0);
        float childX = float(positionChild.x());
        float childY = float(positionChild.y());
        float childZ = float(positionChild.z());
        trimesh::point currentPosition((x + childX)/2, (y + childY)/2, (z + childZ)/2, 1);
        positions.push_back(currentPosition);
    } else {
        positions.push_back(trimesh::point(100000, 100000, 100000, 1));
    }

    for (int i = 0; i < this->_children.size(); i++){
        _children[i]->exportChildMiddleArticulations(matrix, positionRoot, positions);
    }
}

Joint::~Joint() {
	for (int i = 0; i < _children.size(); i++){
		delete _children[i];
	}
	_dofs.clear();
	_children.clear();
}
