#include "joint.h"

using namespace std;



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
