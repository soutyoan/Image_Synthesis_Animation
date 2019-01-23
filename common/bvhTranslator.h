// Projet MAYA Ensimag 2018-2019
// Plug-In C++ implementation

#ifndef BVH_TRANSLATOR
#define BVH_TRANSLATOR

#include <maya/MStatus.h>
#include <maya/MFnIkJoint.h>
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MString.h>
#include <maya/MVector.h>
#include <maya/MStringArray.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>
#include <maya/MIOStream.h>
#include <maya/MFStream.h>
#include <maya/MFileIO.h>
#include <maya/MFnTransform.h>
#include <maya/MNamespace.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <map>
#include "../viewer/src/joint.h"

using namespace std;
//using namespace MGlobal;

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

class BvhTranslator : public MPxFileTranslator {
public:

	/// Default constructor
	BvhTranslator() = default;

	/// Destructor
	~BvhTranslator() {};

	/// Tells MAYA that translator can read .bvh files. (import, open)
	bool haveReadMethod() const { return true; }

	/// Tells MAYA that translator cannot write .bvh files (export, save)
	bool haveWriteMethod() const { return false; }

	///If this method returns true, and the lep file is referenced in a scene, the write method will be
	///called when a write operation is performed on the parent file.  This use is for users who wish
	///to implement a custom file referencing system.
	///For this example, we will return false as we will use Maya's file referencing system.
	bool haveReferenceMethod() const { return false; }

	///If this method returns true, it means we support namespaces.
	bool haveNamespaceSupport()    const { return true; }

	///This method is used by Maya to create instances of the translator.
	static void* creator();

	///This returns the default extension ".lep" in this case.
	MString defaultExtension() const;

	//If this method returns true it means that the translator can handle opening files
	//as well as importing them.
	//If the method returns false then only imports are handled. The difference between
	//an open and an import is that the scene is cleared(e.g. 'file -new') prior to an
	//open, which may affect the behaviour of the translator.
	bool canBeOpened() const { return true; }

	//Maya will call this method to determine if our translator
	//is capable of handling this file.
	MFileKind identifyFile(const MFileObject& fileName,
		const char* buffer,
		short size) const;

	//This function is called by maya when import or open is called.
	MStatus reader(const MFileObject& file,
		const MString& optionsString,
		MPxFileTranslator::FileAccessMode mode);

	MStatus doIt(const MArgList&);

private:
	//The magic string to verify it's a LEP file
	//simply "<LEP>"
	static MString const magic;

	///List of joints from the Joint class from the SIA project.
	static Joint* root;

	MStatus parser_hierarchy(ifstream& file);

	MStatus parser_joint(ifstream& file, Joint* parent, Joint* current);

	MStatus parser_motion(ifstream& file);
};


#endif
