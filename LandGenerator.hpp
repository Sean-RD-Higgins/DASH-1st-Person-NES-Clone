#ifndef H_LAND_GENERATOR
#define H_LAND_GENERATOR

#include <iostream>
#include <osg/Group>
#include <osg/LightSource>
#include <osg/Node>
#include <osg/Switch>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/Shader>
#include <osg/PositionAttitudeTransform>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgParticle/FireEffect>
#include <osgParticle/SmokeEffect>

using namespace std;

// Return the latest group with it's changes to the land based on an acre position
void setLandNode( ) {

	// Go through each character in the string and put it as a block with a texture on it
	for(int col = 0; col < 100; col++) {
		for(int row = 0; row < 100; row++) {

			// Create a single
			float z = -0.5f;
			osg::ref_ptr<osg::PositionAttitudeTransform> posNode = new osg::PositionAttitudeTransform;
			char landChar = mapString[col*100 + row];
			int landInt = landChar - 48;
			landInt = landInt % 6;

			// vertical displacement based only on map object
			if( landInt == MAP_WATER ) {
				z -= 0.1;
			}
			else if( landInt == MAP_MOUNTAIN ) {
				z += 1.0;
			}
			else if( landInt == MAP_BARK ) {
				z += 1.0;
			}

			posNode->setPosition(osg::Vec3((float)row, - (float)col, z));
			landNode->addChild(posNode);
			posNode->addChild(mapNode[landInt]);
		}
	}

}

#endif
