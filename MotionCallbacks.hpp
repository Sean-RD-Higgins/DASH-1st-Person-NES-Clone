#ifndef H_MOTION_CALLBACKS
#define H_MOTION_CALLBACKS

#include <osg/Node>
#include <osg/PositionAttitudeTransform>

// Callback class for rotating the object
class AlterCallback : public osg::NodeCallback  {
public:

  AlterCallback() : turn(0), turnSpeed(0), xincr(0), yincr(0), vertSpeed(0.0) {};

  double xincr, yincr, vertSpeed, turn, turnSpeed, horiSpeed;

  virtual void operator() (osg::Node* n, osg::NodeVisitor* nv) 
  {

    osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(n);

	if( turnSpeed != 0.0) 
	{
		turn += turnSpeed;
	    pat->setAttitude( osg::Quat(turn, osg::Vec3(0, 0, 1) ) );
	}

	if( vertSpeed != 0 ) 
	{
		osg::Vec3d newPos( pat->getPosition() );

		// Go forward or backwards based on the buttons pressed
		newPos.x() += cos(turn + 1.57) * 0.1 * vertSpeed;
		newPos.y() += sin(turn + 1.57) * 0.1 * vertSpeed;

		// You can only go to that location if it is free in terms of mapness
		if( isRadiusFree(newPos, 0.2) )
		{
			pat->setPosition(newPos);
		}
	
	}
	if( horiSpeed != 0 ) 
	{

		osg::Vec3d newPos( pat->getPosition() );

		// Go left or right based on the buttons pressed
		newPos.x() += cos(turn) * 0.1 * horiSpeed;
		newPos.y() += sin(turn) * 0.1 * horiSpeed;

		// You can only go to that location if it is free in terms of mapness
		if( isRadiusFree(newPos, 0.2) )
		{
			pat->setPosition(newPos);
		}
	}
    traverse(n, nv);
  }

private:
  
};

#endif //H_MOTION_CALLBACKS

