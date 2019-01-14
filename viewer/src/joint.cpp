#include "joint.h"
#include <QtGui/QMatrix4x4>

using namespace std;

Joint* Joint::createFromFile(std::string fileName) {
	Joint* root = NULL;
	cout << "Loading from " << fileName << endl;

	ifstream inputfile(fileName.data());
	if(inputfile.good()) {
		// We first recover the values at the end of the file
		while(!inputfile.eof()) {
			string buf;
			inputfile >> buf;
			// TODO : construire la structure de donn�es root � partir du fichier
			cout << buf;
		}
		inputfile.close();
	} else {
		std::cerr << "Failed to load the file " << fileName.data() << std::endl;
		fflush(stdout);
	}

	cout << "file loaded" << endl;

	return root;
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


friend std::ostream& operator<<(std::ostream& os, const Joint& joint){
	int global_tab_count = 0;
	joint.printRecursivly(&global_tab_count);


}

void multTab(int *global_tab_count){
	for (int i = 0; i < *global_tab_count; i++)
		std::cout << "   ";
}


void Joint::printRecursivly(int *global_tab_count){
	multTab(global_tab_count);
	std::cout << "JOINT" << this->name << "\n";
	multTab(global_tab_count);
	std::cout << "{\n";

	*global_tab_count ++;
	multTab(global_tab_count);
	std::cout << "OFFSET " << this->_offZ <<
	" " << this->_offY << " " << this->_offX << "\n";
	if (this->nbRotation() + this->nbTranslation() != 0){
		multTab(global_tab_count);
		std::cout << "CHANNELS " << this->nbRotation() + this->nbTranslation() << " ";
		if (this->nbTranslation() != 0)
			std::cout << this->_curTz << " " << this->_curTy << " " << this->_curTx << " ";
		if (this->nbRotation() != 0)
			std::cout << this->_curRz << " " << this->_curRy << " " << this->_curRx << " ";
		std::std::cout << "\n";
	}
	for (std::vector<Joint *>::iterator it = this->_children.begin() ; it != this->_children.end(); ++it){
		if (*it != NULL){
			*it.printRecursivly(global_tab_count);
		}
	}

	*global_tab_count --;
	multTab(global_tab_count);
	std::cout << '}\n';

}

int Joint::nbRotation(){
	int result = 0;
	if (this->__curRx != 0)
		result ++;
	if (this->__curRy != 0)
		result ++;
	if (this->__curRz != 0)
		result ++;
	return result;

}

int Joint::nbTranslation(){
	int result = 0;
	if (this->__curTx != 0)
		result ++;
	if (this->__curTy != 0)
		result ++;
	if (this->__curTz != 0)
		result ++;
	return result;

}
