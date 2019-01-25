#include "joint.h"
#include <QtGui/QMatrix4x4>

using namespace std;

bool compare_str(string s1, string s2){
	return (s1.compare(s2) == 0);
}

int getIntFromStr(string s){
	if (compare_str(s, "X")){
		return 0;
	} else if (compare_str(s, "Y")){
		return 1;
	} else {
		return 2;
	}
}

void Joint::fillInformation(string name, ifstream &file, bool isEnd,
	vector<string> &animValues){
	this->_name = name;

	string buf;
	file >> buf;
#if DEBUG
	if (!compar_str(file, "{")){
		cerr << "Incorrect parsing" << endl;
		exit(1);
	}
#endif

	file >> buf;
#if DEBUG
	if (!compar_str(file, "OFFSET")){
		cerr << "Incorrect parsing" << endl;
		exit(1);
	}
#endif
	file >> this->_offX; file >> this->_offY; file >> this->_offZ;

	if (isEnd){
		return;
	}

	file >> buf;
#if DEBUG
	if (!compar_str(file, "CHANNELS")){
		cerr << "Incorrect parsing" << endl;
		exit(1);
	}
#endif

	int numberChannels = 0;
	file >> numberChannels;
	this->_dofs.resize(animValues.size());
	for (int i = 0; i < numberChannels; i++){
		string currentChannel;
		file >> currentChannel;
		string end = currentChannel.substr(1);
		if (compare_str(end, "rotation")){
			this->orderRotation.push_back(getIntFromStr(currentChannel.substr(0, 1)));
			for (int i = 0; i < animValues.size(); i++){
				float currentValue;
				stringstream s(animValues[i]);
				s >> currentValue;
				if (compare_str(currentChannel.substr(0, 1), "X")){
					this->_dofs[i]._values[3] = currentValue;
				}
				if (compare_str(currentChannel.substr(0, 1), "Y")){
					this->_dofs[i]._values[4] = currentValue;
				}
				if (compare_str(currentChannel.substr(0, 1), "Z")){
					this->_dofs[i]._values[5] = currentValue;
				}
				animValues[i] = s.str();
			}
		} else {
			this->orderTranslation.push_back(getIntFromStr(currentChannel.substr(0, 1)));
			for (int i = 0; i < animValues.size(); i++){
				float currentValue;
				stringstream s(animValues[i]);
				s >> currentValue;
				if (compare_str(currentChannel.substr(0, 1), "X")){
					this->_dofs[i]._values[0] = currentValue;
				}
				if (compare_str(currentChannel.substr(0, 1), "Y")){
					this->_dofs[i]._values[1] = currentValue;
				}
				if (compare_str(currentChannel.substr(0, 1), "Z")){
					this->_dofs[i]._values[2] = currentValue;
				}
				animValues[i] = s.str();
			}
		}
	}

}

Joint* Joint::createNewJoint(ifstream &file, string filename, bool isEnd,
	vector<string> &animValues){
	Joint* j = new Joint();
	j->fillInformation(filename, file, isEnd, animValues);
	return j;
}

Joint* Joint::createFromFile(std::string fileName) {
	Joint* root = NULL;
	cout << "Loading from " << fileName << endl;

	ifstream motionfile(fileName.data());
	vector<string> animValues;
	if(motionfile.good()) {
		// We first recover the values at the end of the file
		string buf;
		motionfile >> buf;
		while (!compare_str(buf, "MOTION")){
			motionfile >> buf;
		}
		motionfile >> buf;
		assert (compare_str(buf, "Frames:"));
		int numberFrames = 0;
		motionfile >> numberFrames;
		motionfile >> buf;
		assert (compare_str(buf, "Frame"));
		motionfile >> buf;
		assert (compare_str(buf, "Time:"));
		float frameTime = 0;
		motionfile >> frameTime;
		while(!motionfile.eof()){
			string line;
			float currentf;
			while (getline (motionfile,line)){
				animValues.push_back(line);
			}
		}
	}

	ifstream inputfile(fileName.data());
	Joint* parent;
	if(inputfile.good()) {
		// We first recover the values at the end of the file
		string buf;
		inputfile >> buf;
		while (!compare_str(buf, "ROOT")){
			inputfile >> buf;
		}
		string name;
		inputfile >> name;
		root = createNewJoint(inputfile, name, false, animValues);
		while(buf.compare("MOTION") != 0) {
			if (buf.compare("JOINT") == 0){
				string name;
				inputfile >> name;
				Joint* j = createNewJoint(inputfile, name, false, animValues);
				j->parent = parent;
				parent = j;
			} else if (compare_str(buf, "End")){
				string name;
				inputfile >> name;
				Joint* j = createNewJoint(inputfile, name, true, animValues);
				j->parent = parent;
				parent = j;
			} else if (compare_str(buf, "}")){
				parent = parent->parent;
			}
			inputfile >> buf;
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


std::ostream& operator<<(std::ostream& os, const Joint& joint){
	int global_tab_count = 0;
	joint.printRecursivly(os, &global_tab_count);
	return os;


}

void multTab(std::ostream& os, int *global_tab_count){
	for (int i = 0; i < *global_tab_count; i++)
		os << "   ";
}

int Joint::nbChannels() const{
	int nbRotation = this->nbRotation();
	int nbTranslation = this->nbTranslation();
	if ((nbRotation > 0 && nbTranslation == 0) || (nbTranslation > 0 && nbRotation == 0)){
		return 3;
	} else {
		return 6;
	}

}


void Joint::printRecursivly(std::ostream& os, int *global_tab_count) const{
	multTab(os, global_tab_count);
	if (_children.size() != 0)
		os << "JOINT " ;
	else
		os << "END "  ;
	os << this->_name << "\n";
	multTab(os, global_tab_count);
	os << "{\n";

	*global_tab_count = *global_tab_count + 1;
	multTab(os, global_tab_count);
	os << "OFFSET " << this->_offZ <<
	" " << this->_offY << " " << this->_offX << "\n";
	if (this->nbRotation() + this->nbTranslation() != 0){
		multTab(os, global_tab_count);
		os << "CHANNELS " << this->nbChannels() << " ";
		if (this->nbTranslation() != 0)
			os << "Zposition"<< " " << "Yposition" << " " << "Xposition" << " ";
		if (this->nbRotation() != 0)
			os << "Zrotation" << " " << "Yrotation" << " " << "Xrotation" << " ";
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
