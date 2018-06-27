#ifndef H_FOE
#define H_FOE

#include <iostream>
#include <Windows.h>
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

class Foe {
	private:
	// End private

	public:

		// Nodes, Callbacks and textures oh my. (unenthusiastic >.<)
		osg::ref_ptr<osg::Group> foeGroup;
		osg::ref_ptr<osg::PositionAttitudeTransform> foeAttrNode;
		osg::ref_ptr<osg::Node> foeNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> alterNode;
		osg::ref_ptr<AlterCallback> alterCB;

		// Battle Engine stats
		int foeId;
		int HP;
		int attack;
		int radius;
		int invincTime;
		bool seenPlayer;

		// Constructor
		Foe() {

			// Init all gfx data
			foeGroup = new osg::Group();
			foeNode = osgDB::readNodeFile("Model/redead.obj");
			
			// foeAttrNode
			foeAttrNode = new osg::PositionAttitudeTransform();
			foeAttrNode->setScale( osg::Vec3d(0.004, 0.004, 0.004) );
			foeAttrNode->setAttitude( osg::Quat( 3.14 , osg::Vec3(0, 0, 1) ) );

			// The node that alters the general facing and direction of the player
			alterNode = new osg::PositionAttitudeTransform;
			alterNode->setPosition( osg::Vec3( 0, 0, 0.0) );
			alterCB = new AlterCallback;
			alterNode->setUpdateCallback( alterCB.get() );
			
			// The foe graph tree
			foeGroup->addChild( alterNode.get() );
			alterNode->addChild( foeAttrNode.get() );
			foeAttrNode->addChild( foeNode );
			
			// init all battle data
			initBattleData();

		} // Foe()
		
		// Destructor created for garbage collection purposes.
		~Foe() {
			delete &foeGroup;
			delete &foeNode;
			delete &foeAttrNode;
			delete &alterNode;
			delete &alterCB;
		} // ~Foe()

		// init all battle data
		void initBattleData() {
			
			// init all battle data
			HP = 5 + rand() % 10;
			attack = 1 + rand() % 9;
			radius = 0.2;
			invincTime = 0;
			seenPlayer = false;

			alterNode->setPosition( osg::Vec3( 0, 0, 0.0) );

		}

		// A common cycle
		void cycle() {

			// Who cares about your cycle if you're dead
			if( HP < 0 ) {

				// Link's guide will tell him what he got
				osg::Texture2D * texHint = new osg::Texture2D;

				// Some HP gain maybe?
				switch( rand() & 6 ) {
					case 0:
						texHint->setImage( osgDB::readImageFile( "Texture/HPconsole.png" ) );
						plyr->HP += 5;
						if( plyr->HP > plyr->maxHP ) {
							plyr->HP = plyr->maxHP;
						}
						// Resize Link's HP bar accordingly
						plyr->guiHPAttrNode->setScale( osg::Vec3( 0.04 * (plyr->HP / (float)plyr->maxHP), 0.005, 0.001) );

						cout << "You feel rejuvinated to " << plyr->HP << "\n";
					break;
					case 1:
						texHint->setImage( osgDB::readImageFile( "Texture/maxSPconsole.png" ) );
						plyr->maxSP += 5;
						plyr->guiSPAttrNode->setScale( osg::Vec3( 0.04 * (plyr->SP + plyr->maxSP) / (plyr->maxSP * 2.0f), 0.005, 0.001) );
						cout << "You feel like your endurance increased to " << plyr->maxSP << "\n";
					break;
					case 2:
						texHint->setImage( osgDB::readImageFile( "Texture/speedconsole.png" ) );
						plyr->runSpeed += 0.05;
						cout << "You feel like you could run like the wind to " << plyr->runSpeed << "\n";
					break;
					case 3:
						texHint->setImage( osgDB::readImageFile( "Texture/maxHPconsole.png" ) );
						plyr->maxHP += 5;
						plyr->HP += 5;

						// Resize Link's HP bar accordingly
						plyr->guiHPAttrNode->setScale( osg::Vec3( 0.04 * (plyr->HP / (float)plyr->maxHP), 0.005, 0.001) );

						cout << "You feel that you could take more hits to " << plyr->maxHP << "\n";
					break;
					case 4:
						texHint->setImage( osgDB::readImageFile( "Texture/attackconsole.png" ) );
						plyr->maxAttack += 1;
						cout << "You feel stronger to " << plyr->maxAttack << "\n";
					break;
					default:
						texHint->setImage( osgDB::readImageFile( "Texture/SPconsole.png" ) );
						plyr->SP = plyr->maxSP;
						plyr->guiSPAttrNode->setScale( osg::Vec3( 0.04 * (plyr->SP + plyr->maxSP) / (plyr->maxSP * 2.0f), 0.005, 0.001) );
						cout << "You feel your energy returning." << "\n";
					break;
				}

				// set the new texture
				//plyr->guiHintNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texHint);

 				initBattleData();
			}

			// We care about where the player is
			osg::Vec3d plyrPos = plyr->alterNode->getPosition();
			
			// Let the enemy die if it's too far away from the player
			if( getDistance( alterNode->getPosition() , plyrPos ) > 20 ) {
				initBattleData();
			}

			// we'll be using this alot.
			int random = 0;

			// Respawn elsewhere if we were forced on a place we cant be
			if( isPlaceFree( alterNode->getPosition() ) == false ) {
				float foeInitOffsetX = plyrPos.x() - 10 + ( rand() % 21 ) ;
				float foeInitOffsetY = plyrPos.y() - 10 + ( rand() % 21 ) ;

				alterNode->setPosition( osg::Vec3d(  foeInitOffsetX , foeInitOffsetY , 0.0 ) );
			}

			// have we seen player?
			if( seenPlayer ) {
				
				alterCB->turn = atan2( ( alterNode->getPosition().y() - plyrPos.y() ) , ( alterNode->getPosition().x() - plyrPos.x() ) ) + 1.57;
			    alterNode->setAttitude( osg::Quat(alterCB->turn, osg::Vec3(0, 0, 1) ) );

				// Can't walk towards the player if hurt
				if( invincTime <= 0 ) {

					// Walk towards the player
					alterCB->turnSpeed = 0;
					alterCB->vertSpeed = 0.05;
				}
				else {
					alterCB->vertSpeed = 0.0;
				}

				// 1/4000 chance of moaning
				random = rand() % 4000;
				if( random < 1 ) {
					PlaySound( TEXT("Audio\\redead_moan.wav"), NULL, SND_ASYNC);
				}

			}

			// wander around aimlessly
			else {

				// 2/100 chance of turning, 1/100 per direction
				random = rand() % 100;
				if( random < 2 ) {
					alterCB->turnSpeed = ( random < 1 )?0.05:-0.05;
				}

				// 1/50 chance to stop turning
				random = rand() % 50;
				if( random < 1 ) {
					alterCB->turnSpeed = 0;
				}

				// 9/10 chance of moving forward
				random = rand() % 10;
				if( random < 9 ) {
					alterCB->vertSpeed = 0.05;
				}
				else {
					alterCB->vertSpeed = 0;
				}

				// check if the player is in our view
				seenPlayer = (abs(alterCB->turn - atan2( ( alterNode->getPosition().y() - plyrPos.y() ) , ( alterNode->getPosition().x() - plyrPos.x() ) ) + 1.57) < 0.98);
			}

			// Check for damage, dependant on invincibility
			if( invincTime <= 0 ) {

				// Are we hit?
				int damage = foeTakeDamageMaybe( foeId );
				
				// Can you swim, Nelson?
				if( damage > 0 ) {

					// For about 10 cycles per damage point, yes.
					invincTime = damage * 7;
					
					// let's eat the damage
					HP -= damage;

					// if the foes is hit, assume it sees the player
					seenPlayer = true;

					cout << "Your foe has taken " << damage << " damage!  It's health remains at " << HP << "\n";

					// play damage sound
					PlaySound( TEXT("Audio\\redead_hurt.wav") , NULL, SND_ASYNC);

				}

			}
			else {
				invincTime--;
				foeAttrNode->setAttitude( osg::Quat( invincTime / 14.0f , osg::Vec3(1, 0, 0) ) );
			}

		} // cycle()

	// End public
};

#endif //H_FOE
