#ifndef H_PLAYER
#define H_PLAYER

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

class Player 
{
	private:
	// End private

	public:

		// Nodes, Callbacks and textures oh my. (unenthusiastic >.<)
		osg::ref_ptr<osg::Group> linkGroup;
		osg::ref_ptr<osg::Node> linkMainNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> alterNode;
		osg::ref_ptr<AlterCallback> alterCB;

		osg::ref_ptr<osg::Group> guiGroup;
		osg::ref_ptr<osg::PositionAttitudeTransform> guiHintAttrNode;
		osg::ref_ptr<osg::Node> guiHintNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> guiHPAttrNode;
		osg::ref_ptr<osg::Node> guiHPNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> guiSPAttrNode;
		osg::ref_ptr<osg::Node> guiSPNode;

		osg::ref_ptr<osg::PositionAttitudeTransform> swordGroupNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> swordRotateXNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> swordRotateYNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> swordAttrNode;
		osg::ref_ptr<osg::Node> swordNode;

		osg::ref_ptr<osg::PositionAttitudeTransform> handAttrNode;
		osg::ref_ptr<osg::Node> handNode;
		
		osg::Texture2D * texWood;
		osg::Texture2D * texHand;
		osg::Texture2D * texHP;
		osg::Texture2D * texSP;
		osg::Texture2D * texHint;

		// Sword rotations... remember them
		float swordRotX;
		float swordRotY;

		// Battle Engine info
		int HP;
		int maxHP;
		int SP;
		int maxSP;
		int attack;
		int maxAttack;
		float turnSpeed;
		float runSpeed;
		int invincTime;
		bool isAttacking;
		float radius;
		float attackRadius;

		// Map location
		int mapX;
		int mapY;

		// Constructor
		Player() 
		{

			// Map location init
			mapX = 6;
			mapY = -6;

			// Init sword location data
			swordRotX = -0.98;
			swordRotY = -0.98;

			// Init all textures
			texHand = new osg::Texture2D;
			texHand->setImage( osgDB::readImageFile( "Texture/Hand.jpg" ) );
			texWood = new osg::Texture2D;
			texWood->setImage( osgDB::readImageFile( "Texture/hide1.jpg" ) );
			texHP = new osg::Texture2D;
			texHP->setImage( osgDB::readImageFile( "Texture/HP.png" ) );
			texSP = new osg::Texture2D;
			texSP->setImage( osgDB::readImageFile( "Texture/SP.png" ) );
			texHint = new osg::Texture2D;
			texHint->setImage( osgDB::readImageFile( "Texture/Hint.png" ) );

			// linkGroup
			linkGroup = new osg::Group();

			// linkMainNode
			linkMainNode = new osg::Node();

			// alterNode - alters the general position/movement and rotation of the player
			alterNode = new osg::PositionAttitudeTransform();
			alterNode->setPosition( osg::Vec3(6.0, - 6.0, 0.5) );
			alterCB = new AlterCallback();
			alterNode->setUpdateCallback( alterCB.get() );

			// guiGroup
			guiGroup = new osg::Group();
			guiHPAttrNode = new osg::PositionAttitudeTransform;
			guiHPAttrNode->setPosition( osg::Vec3(0, 0.100, 0.025) );
			guiHPAttrNode->setScale( osg::Vec3(0.04, 0.005, 0.001) );
			guiHPNode = osgDB::readNodeFile("Model/cube.obj");
			guiHPNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texHP);
			guiSPAttrNode = new osg::PositionAttitudeTransform;
			guiSPAttrNode->setPosition( osg::Vec3(0, 0.105, 0.025) );
			guiSPAttrNode->setScale( osg::Vec3(0.04, 0.005, 0.001) );
			guiSPNode = osgDB::readNodeFile("Model/cube.obj");
			guiSPNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texSP);
			guiHintAttrNode = new osg::PositionAttitudeTransform;
			guiHintAttrNode->setPosition( osg::Vec3(0, 0.110, 0.025) );
			guiHintAttrNode->setScale( osg::Vec3(0.04, 0.005, 0.001) );
			guiHintNode = osgDB::readNodeFile("Model/cube.obj");
			guiHintNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texHint);

			// handNode
			handNode = osgDB::readNodeFile("Model/cube.obj");
			handNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texHand);

			// handAttrNode - The hand that holds the sword's attributes
			handAttrNode = new osg::PositionAttitudeTransform();
			handAttrNode->setScale( osg::Vec3(0.05, 0.05, 0.2) );
			handAttrNode->setAttitude( osg::Quat( 3.14 , osg::Vec3(0, 1, 0) ) );

			// swordGroupNode - The Group node that will be the general attitude for all the hand and sword operations
			swordGroupNode = new osg::PositionAttitudeTransform();
			swordGroupNode->setPosition( osg::Vec3(-0.075, 0.1, -0.1) );

			// swordRotatYNode - Fan-rotation
			swordRotateYNode = new osg::PositionAttitudeTransform();
			swordRotateYNode->setAttitude( osg::Quat( 0 , osg::Vec3(0, 1, 0) ) );

			// swordRotateXNode - Rolling Pin-rotation
			swordRotateXNode = new osg::PositionAttitudeTransform();
			swordRotateXNode->setAttitude( osg::Quat( 0 , osg::Vec3(1, 0, 0) ) );

			// swordAttrNode - The Sword's attributes
			swordAttrNode = new osg::PositionAttitudeTransform();
			swordAttrNode->setScale( osg::Vec3(0.00075, 0.00075, 0.00075) );
			swordAttrNode->setPosition( osg::Vec3(0, 0, -0.01) );
			
			// swordNode - Applies texture
			swordNode = osgDB::readNodeFile("Model/rusword.obj");
			swordNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texWood);

			// The player's SCENE GRAPH
			/* [linkGroup]->[alterNode]+->[linkMainNode]
                                       |
				 		               +->[swordGroupNode]->[swordRotateXNode]->[swordRotateYNode]+->[handAttrNode]->[handNode]
                                       |                                                          |
			         	 		  	   | 	                                                      +->[swordAttrNode]->[swordNode] 
									   |
									   +->[guiGroup]->[guiHPAttrNode]->[guiHPNode]*/
			// line 1
			linkGroup->addChild( alterNode.get() );
			alterNode->addChild( linkMainNode );

			// line 2
			alterNode->addChild( swordGroupNode.get() );
			swordGroupNode->addChild( swordRotateXNode.get() );
			swordRotateXNode->addChild( swordRotateYNode.get() );
			swordRotateYNode->addChild( handAttrNode.get() );
			handAttrNode->addChild( handNode.get() );

			// line 3
			swordRotateYNode->addChild( swordAttrNode.get() );
			swordAttrNode->addChild( swordNode.get() );

			// line 4
			alterNode->addChild( guiGroup.get() );
			guiGroup->addChild( guiHPAttrNode.get() );
			guiHPAttrNode->addChild( guiHPNode );
			guiGroup->addChild( guiSPAttrNode.get() );
			guiSPAttrNode->addChild( guiSPNode );
			guiGroup->addChild( guiHintAttrNode.get() );
			guiHintAttrNode->addChild( guiHintNode );

			//init battle data
			initBattleData();

		}

		// Destructor created for garbage collection purposes.
		~Player() 
		{
			delete &linkGroup;
			delete &linkMainNode;
			delete &handNode;
			delete &swordNode;
			delete &alterNode;
			delete &alterCB;
			delete &swordGroupNode;
			delete &swordRotateXNode;
			delete &swordRotateYNode;
			delete &swordAttrNode;
			delete &handAttrNode;
			delete &texWood;
			delete &texHand;
		}

		// Generic function to make a bike ride in a really interesting fashion... keyboard stuff
		void turnLeft()
		{ 
			alterCB->turnSpeed = turnSpeed;
		}
		void turnRight()    
		{ 
			alterCB->turnSpeed = -turnSpeed; 
		}
		void noTurn()       
		{ 
			alterCB->turnSpeed = 0.0; 
		}
		void moveForward()  
		{
			alterCB->vertSpeed = runSpeed;
		}
		void moveBackward() 
		{ 
			alterCB->vertSpeed = -runSpeed; 
		}
		void noVertMove()   
		{ 
			alterCB->vertSpeed = 0; 
		}
		void moveLeft()     
		{ 
			alterCB->horiSpeed = -runSpeed; 
		}
		void moveRight()    
		{ 
			alterCB->horiSpeed = runSpeed;
		}
		void noHoriMove()   
		{ 
			alterCB->horiSpeed = 0; 
		}

		// Init battle engine data
		void initBattleData() 
		{

			// Init battle engine data
			maxHP = 30;
			HP = maxHP;
			maxSP = 30;
			SP = maxSP;
			attack = 4;
			maxAttack = 4;
			turnSpeed = 0.03;
			runSpeed = 0.4;
			invincTime = 0;
			isAttacking = false;
			radius = 0.2;
			attackRadius = 1.2;

			alterNode->setPosition( osg::Vec3(6.0, - 6.0, 0.5) );

		}

		// a common cycle
		void cycle () 
		{

			// Who cares about your cycle if you're dead
			if( HP < 0 ) 
			{
				initBattleData();
				guiHPAttrNode->setScale( osg::Vec3( 0.04 * (HP / (float)maxHP), 0.005, 0.001) );
			}

			// Check for damage, dependant on invincibility
			if( invincTime <= 0) 
			{

				// Are we hit?
				int damage = playerTakeDamageMaybe();
				
				// Can you swim, Nelson?
				if( damage > 0 ) 
				{

					// For about 10 cycles per damage point, yes.
					invincTime = damage * 15;
					
					// let's eat the damage
					HP -= damage;

					// Resize your HP bar accordingly
					guiHPAttrNode->setScale( osg::Vec3( 0.04 * (HP / (float)maxHP), 0.005, 0.001) );

					PlaySound( TEXT("Audio\\link_dmg.wav"), NULL, SND_ASYNC);

					cout << "You have taken " << damage << " damage!  Your health remains at " << HP << "\n";
				}

			}
			else 
			{
				invincTime--;
			}

		}

		// Change the direction the sword is facing based on the position of the mouse
		void swordSlash( osg::Vec2d mouseDelta ) 
		{

			// Assume not attacking
			isAttacking = false;

			// define our caps in advance
			float minX = -1.7;
			float maxX = -1.1;
			float minY = -0.2;
			float maxY =  0.4;

			// Our saved values for sword's rotation
			swordRotX -= mouseDelta.y() / 1000;
			swordRotY += mouseDelta.x() / 1000;

			// Let's make caps so people dont stick the sword in their nose or something
			swordRotX = (swordRotX < minX)?minX:swordRotX;
			swordRotX = (swordRotX > maxX)?maxX:swordRotX;
			swordRotY = (swordRotY < minY)?minY:swordRotY;
			swordRotY = (swordRotY > maxY)?maxY:swordRotY;

			// Our mouse's y is imitating the sword's x rotation 
			swordRotateXNode->setAttitude( osg::Quat( swordRotX , osg::Vec3d(1, 0, 0) ) );

			// Our mouse's x is imitating the sword's y rotation 
			swordRotateYNode->setAttitude( osg::Quat( swordRotY , osg::Vec3d(0, 1, 0) ) );

			// Our current overall position relative to the centerpoint is imitating the sword's z rotation 
			float percentX = ( maxX - swordRotX ) / (maxX - minX);
			float percentY = ( maxY - swordRotY ) / (maxY - minY);

			// make a change if not in center
			if ( ! (percentY > 0.25 && percentY < 0.75 && percentX > 0.25 && percentX < 0.75) ) 
			{

				float swordRotZ = 0;
				
				// horizontal aim 
				if ( percentX > 0.25 && percentX < 0.75 ) 
				{
					swordRotZ = 0;
				}
				// vertical aim 
				else if ( percentY > 0.25 && percentY < 0.75 ) 
				{
					swordRotZ = 1.57;
				}
				//  \  aim
				else if ( percentY > 0.75 && percentX < 0.25 || percentX > 0.75 && percentY < 0.25 ) 
				{
					swordRotZ = 0.98;
				}
				//  /  aim
				else if ( percentY < 0.25 && percentX < 0.25 || percentX > 0.75 && percentY > 0.75 ) 
				{
					swordRotZ = 2.55;
				}

				swordAttrNode->setAttitude( osg::Quat( swordRotZ , osg::Vec3(0, 0, 1) ) ); // Thread-rotation

				// Any SP gain will help
				if(SP < maxSP) 
				{
					SP++;
					guiSPAttrNode->setScale( osg::Vec3( 0.04 * (SP + maxSP) / (maxSP * 2.0f), 0.005, 0.001) );
				}
			}
			// If the player's sword is in the center, drain stamina and do damage
			else 
			{

				// if SP is below -maxSP, no slashing can be done
				if( SP < -maxSP ) 
				{
					isAttacking = false;
					swordRotX = minX;
					swordRotY = minY;
					SP = -maxSP * 2; // penalty
					cout << "You are far too tired to swing your weapon\n";
				}
				// if SP is below 0, but still within values, can slash but for half damage and double SP drain
				else if( SP < 0 ) 
				{
					SP -= 5;
					isAttacking = true;
					attack = maxAttack / 2;
					guiSPAttrNode->setScale( osg::Vec3( 0.04 * (SP + maxSP) / (maxSP * 2.0f), 0.005, 0.001) );
					cout << "You swing your weapon forward with " << SP << " stamina remaining\n";
				}
				// if SP is within high values, drain slow and still attack with normal damage
				else 
				{
					SP -= 10;
					isAttacking = true;
					guiSPAttrNode->setScale( osg::Vec3( 0.04 * (SP + maxSP) / (maxSP * 2.0f), 0.005, 0.001) );
					cout << "You swing your weapon vigorously with " << SP << " stamina remaining\n";
					attack = maxAttack;
				}

			}

		}

	// End public
};

#endif //H_PLAYER
