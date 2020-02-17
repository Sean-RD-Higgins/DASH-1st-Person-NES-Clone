// author: Sean Higgins
#pragma region Includes

#include <ctime>
#include <iostream>
#include <windows.h>
#include "mmsystem.h"
#include <osg/Group>
#include <osg/LightSource>
#include <osg/Node>
#include <osg/Switch>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/Shader>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgParticle/FireEffect>
#include <osgParticle/SmokeEffect>

osg::ref_ptr<osg::Group> root;

#include "Globals.hpp"

#pragma endregion

using namespace std;

#pragma region Movement Callbacks


// Generic movement functionality...
void turnLeft() { plyr->turnLeft(); }
void turnRight() { plyr->turnRight(); }
void noTurn() { plyr->noTurn(); }
void moveForward() { plyr->moveForward(); }
void moveBackward() { plyr->moveBackward(); }
void noVertMove() { plyr->noVertMove(); }
void moveLeft() { plyr->moveLeft(); }
void moveRight() { plyr->moveRight(); }
void noHoriMove() { plyr->noHoriMove(); }


#pragma endregion

// ummm... MAIN!
int main() {

	// The one node to rule them all.  The one node to bind them!  The one node to find them.
	root = new osg::Group();

	// init the textures and their applicable nodes
	for(int i = 0; i < 6 ; i++ ) {
		texture[i] = new osg::Texture2D;
		texture[i]->setImage( osgDB::readImageFile( texName[i] ) );
		mapNode[i] = osgDB::readNodeFile("Model/cube.obj");
		osg::ref_ptr<osg::StateSet> stateNode = mapNode[i]->getOrCreateStateSet();
		stateNode->setTextureAttributeAndModes(0, texture[i] );
	}

	// Init the land group
	landNode = new osg::Group();
	setLandNode();

	// create background
	osg::Texture2D * texture;
	texture = new osg::Texture2D;
	texture->setImage( osgDB::readImageFile( "Texture/bg.jpg" ) );
	osg::Node * cubeNode = osgDB::readNodeFile("Model/cube.obj");
	osg::ref_ptr<osg::PositionAttitudeTransform> posNode = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::StateSet> stateNode = cubeNode->getOrCreateStateSet();
	posNode->setPosition(osg::Vec3(50, - 50, 0));
	posNode->setAttitude( osg::Quat( 0.98 ,  osg::Vec3(0, 0, 1) ) );
	posNode->setScale(osg::Vec3(200, 200, 30));
	stateNode->setTextureAttributeAndModes(0, texture );
	posNode->addChild( cubeNode );
	root->addChild(posNode);

	// Add Zelda
	osg::Node * zeldaNode = osgDB::readNodeFile("Model/Zelda/Zelda.obj");
	osg::ref_ptr<osg::PositionAttitudeTransform> zeldaAttrNode = new osg::PositionAttitudeTransform;
	zeldaAttrNode->setPosition(osg::Vec3(85, - 6, 0));
	zeldaAttrNode->setScale(osg::Vec3(0.004, 0.004, 0.004));
	zeldaAttrNode->addChild( zeldaNode );
	root->addChild( zeldaAttrNode );

	// create a "distant" light source
	osg::Vec3 lightDirection(-1, -1, -1);
	osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
	ls->getLight()->setPosition(osg::Vec4(lightDirection, 0));
	ls->getLight()->setAmbient(osg::Vec4(0.3, 0.1, 0.1, 1.0));

	// Like looking through a window
	osgViewer::Viewer viewer;

	//TODO hide cursor

	// these handlers provide useful interactive functions
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	gui = new osgGA::GUIEventAdapter();

	// for keyboard input
	KeyboardEventHandler* keyHandler = new KeyboardEventHandler(); 

	keyHandler->addFunction('q', KeyboardEventHandler::KEY_DOWN, turnLeft);
	keyHandler->addFunction('q', KeyboardEventHandler::KEY_UP, noTurn);
	keyHandler->addFunction('e', KeyboardEventHandler::KEY_DOWN, turnRight);
	keyHandler->addFunction('e', KeyboardEventHandler::KEY_UP, noTurn);
	keyHandler->addFunction('w', KeyboardEventHandler::KEY_DOWN, moveForward);
	keyHandler->addFunction('w', KeyboardEventHandler::KEY_UP, noVertMove);
	keyHandler->addFunction('s', KeyboardEventHandler::KEY_DOWN, moveBackward);
	keyHandler->addFunction('s', KeyboardEventHandler::KEY_UP, noVertMove);
	keyHandler->addFunction('a', KeyboardEventHandler::KEY_DOWN, moveLeft);
	keyHandler->addFunction('a', KeyboardEventHandler::KEY_UP, noHoriMove);
	keyHandler->addFunction('d', KeyboardEventHandler::KEY_DOWN, moveRight);
	keyHandler->addFunction('d', KeyboardEventHandler::KEY_UP, noHoriMove);

	viewer.addEventHandler(keyHandler); 

	// set up manipulator to force camera to track the centerpoint
	osg::ref_ptr<osgGA::NodeTrackerManipulator> manip = new osgGA::NodeTrackerManipulator;
	manip->setTrackNode( plyr->linkMainNode.get() );
	manip->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
	manip->setHomePosition(osg::Vec3(5, -5, 0.5), osg::Vec3(5, -4.99, 0.5), osg::Vec3(0, 0, 0.5), false);
	viewer.setCameraManipulator( manip.get() );

	// attach nodes to the root of the graph
	root->addChild( plyr->linkGroup.get() );
	for( int i = 0; i < MAX_FOES; i++ ) {
		foe[i] = new Foe();
		foe[i]->foeId = i;
		root->addChild( foe[i]->foeGroup.get() );
	}
	root->addChild( ls.get() );
	root->addChild( landNode );
 
	// prepare the scene for viewing
	viewer.setSceneData( root.get() );
	viewer.realize();	
	
	// Make the console be cooler... without debug stuff
	cout << "\n\n\n\n\n\n\n\n\n You awaken in a field, barely aware of your surroundings as you look around with Q and E or mouse movement. \n  Your movements with WASD are slggish at first, but eventually you find your body easy to use. \n  The sword in your hand is easy to swing with simple movement while holding the right mouse button. (no presses needed)\n";

	// start the main event loop
	while( ! viewer.done() ) {

		// mah mouse changed positions??  TELL MAH SWORD~  Fix mah mouse!!!
		POINT pos;
		GetCursorPos(&pos);
		osg::Vec2d mouseNow = osg::Vec2d(pos.x, pos.y);

		// PLAYER!  DO YOUR CYCLE THING
		plyr->cycle();

		if (GetAsyncKeyState(0x02)) 
		{
			plyr->swordSlash(mouseNow - osg::Vec2d(500, 500));
		}
		else 
		{
			plyr->alterCB->turn -= (mouseNow.x() - 500.0f) / 500.0f;
			plyr->alterNode->setAttitude(osg::Quat(plyr->alterCB->turn, osg::Vec3(0, 0, 1)));
		}

		// Center cursor... sorta
		SetCursorPos(500, 500);
		//TODO put cursor in CENTER of window intead
		//TODO hide the cursor

		// Where our player is currently
		double x = plyr->alterNode->getPosition().x();
		double y = plyr->alterNode->getPosition().y();

		// Let the enemy do its thing
		for (int i = 0; i < MAX_FOES; i++) {
			foe[i]->cycle();
		}

		viewer.frame();
	}
	
}
