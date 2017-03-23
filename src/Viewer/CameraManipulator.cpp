#include "Viewer/CameraManipulator.h"

#include "Viewer/CoreGraph.h"
#include "QOSG/ViewerQT.h"
#include "QOSG/CoreWindow.h"

#include "Manager/Manager.h"

#include "Network/Server.h"
#include "Network/Client.h"

#include "Math/CameraMath.h"

#include "Util/ApplicationConfig.h"

#include "osgViewer/Viewer"

#include <osg/Notify>
#include <osg/BoundsChecking>

#include <iostream>
#include <cmath>
#include <list>

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(push)
#pragma warning(disable:4244)

double Vwr::CameraManipulator::EYE_MOVEMENT_SPEED;
double Vwr::CameraManipulator::TARGET_MOVEMENT_SPEED;
double Vwr::CameraManipulator::SCREEN_MARGIN;

namespace Vwr {

Vwr::CameraManipulator::CameraManipulator( Vwr::CoreGraph* coreGraph )
{
	appConf = Util::ApplicationConfig::get();

	_modelScale = 1.0;
	_minimumZoomScale = 0.0;
	_allowThrow = true;
	_thrown = false;

	_vertigo = false;
	PI = 3.141592653589793;

	_distance = 1.0;
	_trackballSize = appConf->getValue( "Viewer.CameraManipulator.Sensitivity" ).toFloat();
	_zoomDelta = 0.4f;

	speedDecelerationFactor = 0.4;
	maxSpeed = appConf->getValue( "Viewer.CameraManipulator.MaxSpeed" ).toDouble();
	speedEpsilon = 0.02;

	movingAutomatically = false;

	EYE_MOVEMENT_SPEED = 0.005;
	TARGET_MOVEMENT_SPEED = 0.005;
	SCREEN_MARGIN = 200;

	ctrlPressed = false;
	shiftPressed = false;

	this->coreGraph = coreGraph;
	stop();

	// dont rotate camera if video background
	if ( appConf->getValue( "Viewer.SkyBox.Noise" ).toInt() == 2 ) {
		_cameraCanRot = false;
	}
	else {
		_cameraCanRot = true;
	}

	_cameraActive = true;

}

Vwr::CameraManipulator::~CameraManipulator()
{
}

void Vwr::CameraManipulator::setVertigoMode( bool value )
{
	_vertigo = value;
}

void Vwr::CameraManipulator::setNode( osg::Node* node )
{
	_node = node;
	if ( _node.get() ) {
		const osg::BoundingSphere& boundingSphere=_node->getBound();
		_modelScale = boundingSphere._radius;
	}
	if ( getAutoComputeHomePosition() ) {
		computeHomePosition();
	}
}


const osg::Node* Vwr::CameraManipulator::getNode() const
{
	return _node.get();
}


osg::Node* Vwr::CameraManipulator::getNode()
{
	return _node.get();
}


void Vwr::CameraManipulator::home( double /*currentTime*/ )
{
	if ( getAutoComputeHomePosition() ) {
		computeHomePosition();
	}
	computePosition( _homeEye, _homeCenter, _homeUp );
	_thrown = false;
}

void Vwr::CameraManipulator::home( const GUIEventAdapter& ea ,GUIActionAdapter& us )
{
	home( ea.getTime() );
	us.requestRedraw();
	us.requestContinuousUpdate( false );
}


void Vwr::CameraManipulator::init( const GUIEventAdapter&,GUIActionAdapter& )
{
	flushMouseEventStack();
}


void Vwr::CameraManipulator::getUsage( osg::ApplicationUsage& usage ) const
{
	usage.addKeyboardMouseBinding( "Trackball: Space","Reset the viewing position to home" );
	usage.addKeyboardMouseBinding( "Trackball: +","When in stereo, increase the fusion distance" );
	usage.addKeyboardMouseBinding( "Trackball: -","When in stereo, reduce the fusion distance" );
}

void Vwr::CameraManipulator::rotateForLeapBackgroundSync( )
{
    _rotation = osg::Quat(0.0,-1.0,0.0,0.0);
    _center = osg::Vec3d(0.0,0.0,-5.0);
}

bool Vwr::CameraManipulator::handle( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
	switch ( ea.getEventType() ) {
		case ( GUIEventAdapter::FRAME ): {
			return handleFrame( ea, us );
		}
		default:
			break;
	}

	if ( ea.getHandled() ) {
		return false;
	}

	if ( _cameraActive ) {

		switch ( ea.getEventType() ) {
			case ( GUIEventAdapter::PUSH ): {
				return handlePush( ea, us );
			}
			case ( GUIEventAdapter::RELEASE ): {
				return handleRelease( ea, us );
			}
			case ( GUIEventAdapter::DRAG ):
			case ( GUIEventAdapter::SCROLL ): {
				return handleScroll( ea, us );
			}
			case ( GUIEventAdapter::MOVE ): {
				return false;
			}
			case ( GUIEventAdapter::KEYDOWN ): {
				return handleKeyDown( ea, us );
			}
			case ( GUIEventAdapter::KEYUP ): {
				return handleKeyUp( ea, us );
			}
			case ( GUIEventAdapter::FRAME ): {
				if ( _thrown ) {
					if ( calcMovement() ) {
						us.requestRedraw();
					}
				}

				return false;
			}
			default:
				return false;
		}
	}
	else {
		switch ( ea.getEventType() ) {
			case ( GUIEventAdapter::KEYDOWN ): {
				return handleKeyDownGraph( ea, us );
			}
			default:
				return false;

		}
	}
}

bool Vwr::CameraManipulator::handleKeyDownGraph( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
	osg::Vec3d pos;
	int speed = 2;

	switch ( ea.getKey() ) {
		case osgGA::GUIEventAdapter::KEY_Control_R:
		case osgGA::GUIEventAdapter::KEY_Control_L: {
			ctrlPressed = true;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Shift_R:
		case osgGA::GUIEventAdapter::KEY_Shift_L: {
			shiftPressed = true;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Up: {
			pos.y() = speed;
			break;
		}

		case osgGA::GUIEventAdapter::KEY_Down: {
			pos.y() = -speed;
			break;
		}

		case osgGA::GUIEventAdapter::KEY_Right: {
			pos.x() = speed;
			break;
		}

		case osgGA::GUIEventAdapter::KEY_Left: {
			pos.x() = -speed;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Page_Up: {
			pos.z() = speed;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Page_Down: {
			pos.z() = -speed;
			break;
		}

		default:
			break;
	}

	emit sendTranslatePosition( pos );

	return true;
}

bool Vwr::CameraManipulator::handleScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	addMouseEvent( ea );
	if ( calcMovement() ) {
		us.requestRedraw();
	}
	us.requestContinuousUpdate( false );
	_thrown = false;
	return true;
}

bool Vwr::CameraManipulator::handleRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	if ( ea.getButtonMask()==0 ) {

		double timeSinceLastRecordEvent = _ga_t0.valid() ? ( ea.getTime() - _ga_t0->getTime() ) : DBL_MAX;
		if ( timeSinceLastRecordEvent>0.02 ) {
			flushMouseEventStack();
		}

		if ( isMouseMoving() ) {
			if ( calcMovement() ) {
				us.requestRedraw();
				us.requestContinuousUpdate( true );
				_thrown = _allowThrow;
			}
		}
		else {
			flushMouseEventStack();
			addMouseEvent( ea );
			if ( calcMovement() ) {
				us.requestRedraw();
			}
			us.requestContinuousUpdate( false );
			_thrown = false;
		}

	}
	else {
		flushMouseEventStack();
		addMouseEvent( ea );
		if ( calcMovement() ) {
			us.requestRedraw();
		}
		us.requestContinuousUpdate( false );
		_thrown = false;
	}

	notifyServer();
	notifyClients();

	return true;
}

bool Vwr::CameraManipulator::handlePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	if ( ea.getButtonMask() == GUIEventAdapter::MIDDLE_MOUSE_BUTTON ) {
		bool notNullDistance = !qFuzzyCompare( _distance,0.0 );
		if ( notNullDistance ) {
			lastDistance = _distance;

			osg::Vec3d eye, cameraCenter, up;

			osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &us );
			viewer->getCamera()->getViewMatrixAsLookAt( eye, cameraCenter, up );

			_center = eye;
			_distance = 0.0;
		}
		else {
			_distance = lastDistance ;
		}

		notifyServer();
		notifyClients();

		return true;
	}
	else {
		flushMouseEventStack();
		addMouseEvent( ea );
		if ( calcMovement() ) {
			us.requestRedraw();
		}
		us.requestContinuousUpdate( false );
		_thrown = false;
		return true;
	}
}

bool Vwr::CameraManipulator::handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	double current_frame_time = ea.getTime();

	_delta_frame_time = current_frame_time - _last_frame_time;
	_last_frame_time = current_frame_time;

	if ( _thrown && _allowThrow ) {
		if ( calcMovement() ) {
			us.requestRedraw();
		}
	}

	frame( ea,us );

	return false;
}

bool Vwr::CameraManipulator::isMouseMoving()
{
	if ( _ga_t0.get()==NULL || _ga_t1.get()==NULL ) {
		return false;
	}

	static const float velocity = 0.1f;

	float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
	float len = sqrtf( dx*dx+dy*dy );

	float dt = static_cast<float>( _ga_t0->getTime()-_ga_t1->getTime() );

	return ( len>dt*velocity );
}
bool CameraManipulator::getDecelerateForwardRate() const
{
	return decelerateForwardRate;
}



void Vwr::CameraManipulator::flushMouseEventStack()
{
	_ga_t1 = NULL;
	_ga_t0 = NULL;
}


void Vwr::CameraManipulator::addMouseEvent( const GUIEventAdapter& ea )
{
	_ga_t1 = _ga_t0;
	_ga_t0 = &ea;
}

void Vwr::CameraManipulator::setByMatrix( const osg::Matrixd& matrix )
{
	_center = osg::Vec3( 0.0f,0.0f,-static_cast<float>( _distance ) )*matrix;
	_rotation = matrix.getRotate();
}

osg::Matrixd Vwr::CameraManipulator::getMatrix() const
{
	return osg::Matrixd::translate( _centerArucoTrans )*osg::Matrixd::translate( 0.0,0.0,_distance )*osg::Matrixd::rotate( _rotation*_rotationHead )*osg::Matrixd::translate( _center );

}

osg::Matrixd Vwr::CameraManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate( -_centerArucoTrans )*osg::Matrixd::translate( -_center )*osg::Matrixd::rotate( ( _rotation*_rotationHead ).inverse() )*osg::Matrixd::translate( 0.0,0.0,-_distance );

}

void Vwr::CameraManipulator::computePosition( const osg::Vec3& eye,const osg::Vec3& center,const osg::Vec3& up )
{

	osg::Vec3d lv( center-eye );

	osg::Vec3d f( lv );
	f.normalize();
	osg::Vec3d s( f^up );
	s.normalize();
	osg::Vec3d u( s^f );
	u.normalize();

	osg::Matrix rotation_matrix( s[0],     u[0],     -f[0],     0.0,
								 s[1],     u[1],     -f[1],     0.0,
								 s[2],     u[2],     -f[2],     0.0,
								 0.0,     0.0,     0.0,      1.0 );

	_center = center;
	_distance = lv.length();
	_rotation = rotation_matrix.getRotate().inverse();

	notifyServer();
	notifyClients();

}


void CameraManipulator::rotateCamera( float py0, float px0, double throwScale, float py1, float px1 )
{
	osg::Vec3 axis;
	float angle;

	trackball( axis,angle,px1,py1,px0,py0 );

	osg::Quat new_rotate;

	new_rotate.makeRotate( angle * throwScale,axis );

	if ( _cameraCanRot ) {
		_rotation = _rotation*new_rotate;
	}
	else {
		emit sendMouseRotation( new_rotate.inverse() );
	}
}

bool Vwr::CameraManipulator::calcMovement()
{
	// mouse scroll is only a single event
	if ( _ga_t0.get()==NULL ) {
		return false;
	}

	// ignoruj event ak je stlaceny ctrl
	if ( ctrlPressed || shiftPressed ) {
		return false;
	}

	float dx=0.0f;
	float dy=0.0f;
	unsigned int buttonMask=osgGA::GUIEventAdapter::NONE;

	if ( _ga_t0->getEventType()==GUIEventAdapter::SCROLL ) {
		switch ( _ga_t0->getScrollingMotion() ) {
			case osgGA::GUIEventAdapter::SCROLL_UP:
				dy = -_zoomDelta;
				break;
			case osgGA::GUIEventAdapter::SCROLL_DOWN:
				dy = _zoomDelta;
				break;
			case osgGA::GUIEventAdapter::SCROLL_LEFT:
			case osgGA::GUIEventAdapter::SCROLL_RIGHT:
				// pass
				break;
			case osgGA::GUIEventAdapter::SCROLL_2D:
				// normalize scrolling delta
				dx = _ga_t0->getScrollingDeltaX() / ( ( _ga_t0->getXmax()-_ga_t0->getXmin() ) * 0.5f );
				dy = _ga_t0->getScrollingDeltaY() / ( ( _ga_t0->getYmax()-_ga_t0->getYmin() ) * 0.5f );

				dx *= _zoomDelta;
				dy *= _zoomDelta;
				break;
			default:
				break;
		}//}
		buttonMask=GUIEventAdapter::SCROLL;
	}
	else {

		if ( _ga_t1.get()==NULL ) {
			return false;
		}
		dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
		dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
		float distance = sqrtf( dx*dx + dy*dy );

		// return if movement is too fast, indicating an error in event values or change in screen.
		if ( distance>0.5 ) {
			return false;
		}

		// return if there is no movement.
		//if (distance==0.0f)
		if ( qFuzzyCompare( distance, 0.0f ) ) {
			return false;
		}

		buttonMask = _ga_t1->getButtonMask();
	}


	double throwScale = ( _thrown && _ga_t0.valid() && _ga_t1.valid() ) ?
						_delta_frame_time / ( _ga_t0->getTime() - _ga_t1->getTime() ) :
						1.0;

	if ( buttonMask==GUIEventAdapter::RIGHT_MOUSE_BUTTON ) {

		// rotate camera.

		float px0 = _ga_t0->getXnormalized();
		float py0 = _ga_t0->getYnormalized();

		float px1 = _ga_t1->getXnormalized();
		float py1 = _ga_t1->getYnormalized();

		rotateCamera( py0, px0, throwScale, py1, px1 );


		notifyServer();
		notifyClients();

		return true;

	}
	else if ( buttonMask==( GUIEventAdapter::LEFT_MOUSE_BUTTON|GUIEventAdapter::RIGHT_MOUSE_BUTTON ) ) {

		// pan model.

		float scale = -0.3f * static_cast<float>( _distance ) * static_cast<float>( throwScale );

		osg::Matrix rotation_matrix;
		rotation_matrix.makeRotate( _rotation );

		osg::Vec3 dv( dx*scale,dy*scale,0.0f );

		_center += dv*rotation_matrix;

		notifyServer();
		notifyClients();

		return true;
	}
	else if ( buttonMask==GUIEventAdapter::SCROLL ) {

		// zoom model.

		float fd = static_cast<float>( _distance );
		float scale = 1.0f+ dy * static_cast<float>( throwScale );
		if ( fd*scale>_modelScale*_minimumZoomScale ) {
			if ( _distance * scale < 10000 ) {
				_distance *= scale;

				// Duransky start - zmena uhlu zobrazenia spolu so zmenou vzdialenosti pri zapnutom vertigo mode
				if ( _vertigo ) {

					// ziskanie sirky sceny
					_width = this->coreGraph->getCamera()->getViewport()->width();

					// ziskanie aktualneho FOV - field of view
					this->coreGraph->getCamera()->getProjectionMatrixAsPerspective( fovy, ratio, zNear, zFar );

					// nove FOV vypocitane podla vzorca
					double newFovInDegree = atan( _width/_distance ) * ( 180 / PI );

					// nastavenie novej projekcnej matice s novym FOV
					coreGraph->getCamera()->setProjectionMatrixAsPerspective( newFovInDegree, ratio, zNear, zFar );

					// Duransky end - zmena uhlu zobrazenia spolu so zmenou vzdialenosti pri zapnutom vertigo mode

				}
			}
		}

		notifyServer();
		notifyClients();

		return true;
	}

	return false;
}


/*
 * This size should really be based on the distance from the center of
 * rotation to the point on the object underneath the mouse.  That
 * point would then track the mouse as closely as possible.  This is a
 * simple example, though, so that is left as an Exercise for the
 * Programmer.
 */
void Vwr::CameraManipulator::setTrackballSize( float size )
{
	_trackballSize = size;
	osg::clampBetweenRange( _trackballSize,0.1f,1.0f,"CameraManipulator::setTrackballSize(float)" );
}

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void Vwr::CameraManipulator::trackball( osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y )
{
	/*
	 * First, figure out z-coordinates for projection of P1 and P2 to
	 * deformed sphere
	 */

	osg::Matrix rotation_matrix( _rotation * _rotationAux );


	osg::Vec3 uv = osg::Vec3( 0.0f,1.0f,0.0f )*rotation_matrix;
	osg::Vec3 sv = osg::Vec3( 1.0f,0.0f,0.0f )*rotation_matrix;
	osg::Vec3 lv = osg::Vec3( 0.0f,0.0f,-1.0f )*rotation_matrix;

	osg::Vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere( _trackballSize, p1x, p1y );
	osg::Vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere( _trackballSize, p2x, p2y );

	/*
	 *  Now, we want the cross product of P1 and P2
	 */

	// Robert,
	//
	// This was the quick 'n' dirty  fix to get the trackball doing the right
	// thing after fixing the Quat rotations to be right-handed.  You may want
	// to do something more elegant.
	//   axis = p1^p2;
	axis = p2^p1;
	axis.normalize();

	/*
	 *  Figure out how much to rotate around that axis.
	 */

	float t = ( float )( p2 - p1 ).length() / ( 2.0f * ( float ) _trackballSize );

	/*
	 * Avoid problems with out-of-control values...
	 */
	if ( t > 1.0 ) {
		t = 1.0;
	}
	if ( t < -1.0 ) {
		t = -1.0;
	}

	angle =( float ) osg::inRadians( asin( t ) );

}


/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
float Vwr::CameraManipulator::tb_project_to_sphere( float r, float x, float y )
{
	float d, t, z;


	d =( float ) sqrt( x*x + y*y );
	/* Inside sphere */
	if ( d < r * 0.70710678118654752440 ) {
		z = ( float )sqrt( r*r - d*d );
	}                            /* On hyperbola */
	else {
		t =( float ) r / 1.41421356237309504880f;
		z = t*t / d;
	}
	return z;
}

/*!
 *
 * \param ea
 * Adapter udalosti.
 *
 * Metoda spomali pohyb na zaklade zdroja prijatej udalosti.
 *
 */
bool Vwr::CameraManipulator::handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	switch ( ea.getKey() ) {
		case osgGA::GUIEventAdapter::KEY_Control_R:
		case osgGA::GUIEventAdapter::KEY_Control_L: {
			ctrlPressed = false;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Shift_R:
		case osgGA::GUIEventAdapter::KEY_Shift_L: {
			shiftPressed = false;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Space: {
			flushMouseEventStack();
			_thrown = false;
			_distance = 1.0;
			home( ea,us );
			us.requestRedraw();
			us.requestContinuousUpdate( false );

			stop();
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Up:
		case osgGA::GUIEventAdapter::KEY_Down:
			decelerateForwardRate = true;
			break;

		case osgGA::GUIEventAdapter::KEY_Right:
		case osgGA::GUIEventAdapter::KEY_Left:
			decelerateSideRate = true;
			break;

		case osgGA::GUIEventAdapter::KEY_Page_Up:
		case osgGA::GUIEventAdapter::KEY_Page_Down:
			decelerateVerticalRate = true;
			break;
		case osgGA::GUIEventAdapter::KEY_C:
			// Print camera position
			qDebug() << "_center.set(" << _center.x() << "," << _center.y() << "," << _center.z() << ");\n_rotation.set(" << _rotation.x() << "," << _rotation.y() << "," << _rotation.z() << "," << _rotation.w() << ");";
			break;
		case osgGA::GUIEventAdapter::KEY_V: {
			// Set camera position (use for debug & setting specific camera position)
			_center.set( 15.9042 , -277.226 , -372.165 );
			_rotation.set( 0.467275 , -0.0320081 , 0.0985734 , 0.878017 );
			break;
		}
		default:
			break;
	}

	notifyServer();
	notifyClients();

	return true;
}

/*!
 *
 * \param ea
 * Adapter udalosti.
 *
 * Metoda zrychli pohyb na zaklade zdroja prijatej udalosti.
 *
 */
bool Vwr::CameraManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& )
{
	switch ( ea.getKey() ) {
		case osgGA::GUIEventAdapter::KEY_Control_R:
		case osgGA::GUIEventAdapter::KEY_Control_L: {
			ctrlPressed = true;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Shift_R:
		case osgGA::GUIEventAdapter::KEY_Shift_L: {
			shiftPressed = true;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Up: {
			forwardSpeed = 2 * maxSpeed;
			decelerateForwardRate = false;
			break;
		}

		case osgGA::GUIEventAdapter::KEY_Down: {
			forwardSpeed = -2 * maxSpeed;
			decelerateForwardRate = false;
			break;
		}

		case osgGA::GUIEventAdapter::KEY_Right: {
			sideSpeed = 2 * maxSpeed;
			decelerateSideRate = false;
			break;
		}

		case osgGA::GUIEventAdapter::KEY_Left: {
			sideSpeed = -2 * maxSpeed;;
			decelerateSideRate = false;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Page_Up: {
			verticalSpeed = 2 * maxSpeed;
			decelerateVerticalRate = false;
			break;
		}
		case osgGA::GUIEventAdapter::KEY_Page_Down: {
			verticalSpeed = -2 * maxSpeed;
			decelerateVerticalRate = false;
			break;
		}

		default:
			break;
	}

	notifyServer();
	notifyClients();

	return true;
}

/*!
 * \param ea
 * Adapter udalosti.
 *
 *
 * Funkcia vypocita poziciu kamery a zmensi rychlost pohybu.
 *
 */
void Vwr::CameraManipulator::frame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &aa );

	//Smooth center view speed > 0, because it is factor of points count
	//between initial and destination points in smooth center view
	//Increase in the value of v results in a decrease in the points count for camera movement in smooth center view
	//and camera movement speed is higher
	int v = 5; //speed 5 for not too fast and not too slow camera movement
	osg::Vec3 d = this->newCenter - this->originalCenter;
	int points = static_cast<int>( d.length() )/v;

	//if camera moved through all points, stop camera movement
	if ( pointID > points ) {
		movingCenter = false;
	}

	//if movingCenter flag is set and view is not centered
	if ( movingCenter && points > 0 ) {
		float x = this->originalCenter.x() + d.x() * pointID/points;
		float y = this->originalCenter.y() + d.y() * pointID/points;
		float z = this->originalCenter.z() + d.z() * pointID/points;
		pointID++;

		_center = osg::Vec3( x, y, z );
		notifyServer();
		notifyClients();
	}

	// ak sme v rezime automatickeho pohybu
	if ( movingAutomatically ) {
		// inicializujeme premenne
		if ( !automaticMovementInitialized ) {
			initAutomaticMovement( viewer );
		}

		// dokym pohlad a pozicia neopisu celu trajektoriu
		if ( t1 <= 1 || t2 <= 1 ) {
			// ziskanie novych pozicii na krivke
			if ( t1 <= 1 ) {
				cameraPosition = MathModule::CameraMath::getPointOnNextBezierCurve( t1, cameraPositions, w1 );
				t1 += EYE_MOVEMENT_SPEED;
			}

			if ( t2 <= 1 ) {
				targetPoint = MathModule::CameraMath::getPointOnNextBezierCurve( t2, targetPositions, w2 );
				t2 += TARGET_MOVEMENT_SPEED;
			}


			// aktualne pozicie kamery
			osg::Vec3d eye;
			osg::Vec3d center;
			osg::Vec3d up;

			viewer->getCamera()->getViewMatrixAsLookAt( eye, center, up );

			// prepocitanie koordinatov kamerery s novymi hodnotami
			computePosition( cameraPosition, targetPoint, up );

			// zobrazenie trajektorie kamery
			if ( appConf->getValue( "Viewer.Display.CameraPath" ).toInt() == 1 ) {
				QLinkedList<osg::ref_ptr<osg::Node> >* list = coreGraph->getCustomNodeList();
				osg::ref_ptr<osg::Group> group = new osg::Group;
				osg::ref_ptr<osg::Geode> g1 = new osg::Geode;

				g1->addDrawable( MathModule::CameraMath::createAxis( eye, lastPosition ) );
				g1->addDrawable( MathModule::CameraMath::createAxis( targetPoint, lastTargetPoint, osg::Vec4d( 1,0,0,1 ) ) );

				group->addChild( g1 );
				lastPosition = eye;
				lastTargetPoint = targetPoint;
				list->push_back( group );
			}

			// vypis metriky do konzoly
			if ( appConf->getValue( "Graph.Metrics.ViewMetrics" ).toInt() == 1 ) {
				computeViewMetrics( viewer, selectedCluster );
			}

		}

		// ukoncenie pohybu
		else {
			movingAutomatically = false;

			if ( appConf->getValue( "Viewer.Display.CameraPositions" ).toInt() == 1 ) {
				osg::Vec3d eye;
				osg::Vec3d center;
				osg::Vec3d up;

				viewer->getCamera()->getViewMatrixAsLookAt( eye, center, up );

				Data::Graph* g = Manager::GraphManager::getInstance()->getActiveGraph();
				g->addNode( "EndNode", g->getNodeMetaType(), eye );
			}

			//		osg::Vec3d eye;
			//		osg::Vec3d center;
			//		osg::Vec3d up;

			//		viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);
			//			cout << eye.x() << " " << eye.y() << " " << eye.z() << "\n";
		}
	}
	// standardny frame
	else {
		computeStandardFrame( ea, aa );
	}
}

void Vwr::CameraManipulator::computeStandardFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &aa );
	double t1 = ea.getTime();
	bool nullT0=qFuzzyCompare( t0,0.0 );
	if ( nullT0 ) {
		t0 = ea.getTime();
		dt = 0.0;
	}
	else {
		dt = t1 - t0;
		t0 = t1;
	}

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	viewer->getCamera()->getViewMatrixAsLookAt( eye, center, up );

	osg::Vec3d directionVec = center - eye;
	osg::Vec3d sideVec = getSideVector( osg::Matrixd( _rotation ) ); /* direction * osg::Matrix::rotate(-M_PI * 0.5, upVector);*/

	_center += ( ( directionVec * forwardSpeed ) + ( sideVec * sideSpeed ) + ( up * verticalSpeed ) ) * dt;

	if ( decelerateSideRate ) {
		sideSpeed *= speedDecelerationFactor;
	}
	if ( decelerateForwardRate ) {
		forwardSpeed *= speedDecelerationFactor;
	}
	if ( decelerateVerticalRate ) {
		verticalSpeed *= speedDecelerationFactor;
	}
	if ( deceleratePitchRate ) {
		pitchSpeed *= speedDecelerationFactor;
	}
}

void Vwr::CameraManipulator::setCenterSmoothly( osg::Vec3 newCenter )
{
	this->originalCenter = getCenter();
	this->newCenter = newCenter;
	pointID = 1;

	movingCenter = true;
}

void Vwr::CameraManipulator::setNewPosition( osg::Vec3d cameraTargetPoint, osg::Vec3d cameraInterestPoint, std::list<osg::ref_ptr<Data::Node> > selectedCluster, std::list<osg::ref_ptr<Data::Edge> > selectedEdges )
{
	movingAutomatically = true;
	this->cameraTargetPoint = cameraTargetPoint;
	this->cameraInterestPoint = cameraInterestPoint;

	//cout << "Camera targetPoint: " << cameraTargetPoint.x() << " " << cameraTargetPoint.y() << " " << cameraTargetPoint.z() << "\n";
	//cout << "Camera interest targetPoint: " << cameraInterestPoint.x() << " " << cameraInterestPoint.y() << " " << cameraInterestPoint.z() << "\n";

	std::list<osg::ref_ptr<Data::Edge> >::iterator i;

	// prida hrany medzi body zaujmu
	for ( i = selectedEdges.begin(); i != selectedEdges.end(); ++i ) {
		selectedCluster.push_back( ( *i )->getSrcNode() );
		selectedCluster.push_back( ( *i )->getDstNode() );
	}

	selectedCluster.unique();

	this->selectedCluster = selectedCluster;

	automaticMovementInitialized = false;
}

void Vwr::CameraManipulator::initAutomaticMovement( osgViewer::Viewer* viewer )
{
	t1 = t2 = 0;

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	viewer->getCamera()->getViewMatrixAsLookAt( eye, center, up );

	this->lastPosition = originalEyePosition = eye;
	this->lastTargetPoint = center;

	float scale = appConf->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	weightPoint = ( eye + cameraInterestPoint + cameraTargetPoint )/3;
	targetDistance = alterCameraTargetPoint( viewer );

	cameraPositions = new QVector<osg::Vec3d>();
	cameraPositions->push_back( originalEyePosition );
	cameraPositions->push_back( cameraInterestPoint );
	cameraPositions->push_back( cameraTargetPoint );

	// vahy kontrolnych bodov pre trajektoriu kamery
	w1[0] = 1;
	w1[1] = -0.1;
	w1[2] = 1;

	targetPositions = new QVector<osg::Vec3d>();

	targetPositions->push_back( center );
	targetPositions->push_back( cameraInterestPoint );
	targetPositions->push_back( weightPoint );

	// vahy kontrolnych bodov pre trajektriu pohladu
	w2[0] = 1;
	w2[1] = 0.5;
	w2[2] = 1;

	// uprava vah aby boli viditelne vsetky body zaujmu
	alterWeights( viewer, selectedCluster );

	// zobrazenie kontornych bodov
	if ( appConf->getValue( "Viewer.Display.CameraPositions" ).toInt() == 1 ) {
		Data::Graph* g = Manager::GraphManager::getInstance()->getActiveGraph();

		g->addNode( "centerNode", g->getNodeMetaType(), center )->setColor( osg::Vec4( 0, 0.5, 1, 0.5 ) );
		g->addNode( "startNode", g->getNodeMetaType(), originalEyePosition / scale );
		g->addNode( "interestPoint", g->getNodeMetaType(), cameraInterestPoint / scale )->setColor( osg::Vec4( 0, 1, 1, 1 ) );
		g->addNode( "weigthPoint", g->getNodeMetaType(), weightPoint )->setColor( osg::Vec4( 0, 0.5, 1, 1 ) );
	}

	automaticMovementInitialized = true;
}

float Vwr::CameraManipulator::alterCameraTargetPoint( osgViewer::Viewer* viewer )
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera( *( viewer->getCamera() ), osg::CopyOp::DEEP_COPY_ALL );
	//osg::ref_ptr<osg::Camera> camera = viewer->getCamera();

	double width =  camera->getViewport()->width();
	double height =  camera->getViewport()->height();

	osg::Vec3d basicVec = cameraTargetPoint - weightPoint;
	double scale = appConf->getValue( "Viewer.Display.NodeDistanceScale" ).toDouble();

	// minimum distance from center

	double dist =  basicVec.length() + 50 * scale;

	osg::Vec3d eyePosition;

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	viewer->getCamera()->getViewMatrixAsLookAt( eye, center, up );

	// move camera target point backwards until all interest points can be seen
	while ( true ) {
		eyePosition = MathModule::CameraMath::getPointOnVector( weightPoint, cameraTargetPoint, dist );
		camera->setViewMatrixAsLookAt( eyePosition, weightPoint, up );

		osg::Vec3d eyeScr = MathModule::CameraMath::projectOnScreen( camera, originalEyePosition );
		osg::Vec3d cameraInterestPointScr = MathModule::CameraMath::projectOnScreen( camera, cameraInterestPoint );

		//cout << "eyeScr: " << eyeScr.x() << " " << eyeScr.y() << "\n";
		//cout << "cameraInterestPointScr: " << cameraInterestPointScr.x() << " " << cameraInterestPointScr.y() << "\n";

		bool onScreen1 = MathModule::CameraMath::isInRect( eyeScr, width, height, SCREEN_MARGIN );
		bool onScreen2 = MathModule::CameraMath::isInRect( cameraInterestPointScr, width, height, SCREEN_MARGIN );

		if ( !( onScreen1 && onScreen2 ) ) {
			dist += 10;
		}
		else {
			break;
		}
	}
	cameraTargetPoint = eyePosition;

	/*cout << "Altered target: " << eyePosition.x() << " " << eyePosition.y() << " " << eyePosition.z() << "\n";*/

	return static_cast<float>( dist );
}

void Vwr::CameraManipulator::alterWeights( osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster )
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera( *( viewer->getCamera() ), osg::CopyOp::DEEP_COPY_ALL );

	float width = ( float ) camera->getViewport()->width();
	float height = ( float ) camera->getViewport()->height();

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	// get initial camera position
	camera->getViewMatrixAsLookAt( eye, center, up );

	// alter weights until whole cluster is seen
	while ( true ) {
		// get position and orientation in t = 0.5
		osg::Vec3d eyePosition = MathModule::CameraMath::getPointOnNextBezierCurve( 0.5, cameraPositions, w1 );
		osg::Vec3d targetPosition = MathModule::CameraMath::getPointOnNextBezierCurve( 0.5 / ( EYE_MOVEMENT_SPEED / TARGET_MOVEMENT_SPEED ), targetPositions, w2 );

		camera->setViewMatrixAsLookAt( eyePosition, targetPosition, up );

		// get cluster nodes in extreme positions in t = 0.5
		QVector<osg::ref_ptr<Data::Node> >* extremes = MathModule::CameraMath::getViewExtremes( camera, selectedCluster );

		bool onScreen = true;

		// check for visibility of extremes
		for ( int x = 0; x < 4; x++ ) {
			onScreen &= MathModule::CameraMath::isInRect( MathModule::CameraMath::projectOnScreen( camera, extremes->at( x )->getCurrentPosition() ), width, height, SCREEN_MARGIN );
		}

		if ( !onScreen ) {
			w1[1] -= 0.0005;
		}
		else {
			break;
		}
	}
}

/*!
 *
 * Zastavi pohyb kamery.
 */
void Vwr::CameraManipulator::stop()
{
	forwardSpeed = 0.0;
	sideSpeed = 0.0;
	verticalSpeed = 0.0;
	pitchSpeed = 0.0;
}

void Vwr::CameraManipulator::notifyClients()
{
	Network::Server* server = Network::Server::getInstance();
	if ( server->isListening() ) {
		server->sendMyView( _center,_rotation, _distance );
	}
}

void Vwr::CameraManipulator::notifyServer()
{
	Network::Client* client = Network::Client::getInstance();
	if ( client->isConnected() ) {
		client->sendMyView( _center,_rotation, static_cast<float>( _distance ) );
	}
}

void Vwr::CameraManipulator::computeViewMetrics( osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster )
{
	int cnt = 0;

	std::list<osg::ref_ptr<Data::Node> >::iterator i;

	for ( i = selectedCluster.begin(); i != selectedCluster.end(); ++i ) {
		if ( MathModule::CameraMath::isInRect( MathModule::CameraMath::projectOnScreen( viewer->getCamera(), ( *i )->getCurrentPosition() ), ( float ) viewer->getCamera()->getViewport()->width(),( float ) viewer->getCamera()->getViewport()->height(), 0.f ) ) {
			cnt++;
		}
	}

	std::cout << "Currently visible: " << cnt << " nodes\n";
}



/*
 * Compute both horizontal & vertical rotation according head translation.
 * Result rotation is in _rotationHead quaternion.
 * ! distance is not implemented yet
 */

void Vwr::CameraManipulator::setRotationHeadKinect( float x, float y, float distance )
{

	osg::Vec3	axis;
	float	angle;
	double	throwScale;

	x /= 100;
	y /= 100;

	if ( ( -1.0f <= x && x <= 1.0f ) && ( -1.0f <= y && y <= 1.0f ) ) {

		throwScale = ( _thrown && _ga_t0.valid() && _ga_t1.valid() ) ?
					 _delta_frame_time / ( _ga_t0->getTime() - _ga_t1->getTime() ) :
					 1.0;

		// both rotation must be separated
		// horizontal rotation
		trackball( axis,angle, x, 0.0, 0.0, 0.0 );
		osg::Quat Xnew_rotate( angle * throwScale,axis );

		// vertical rotation
		trackball( axis,angle,0.0, y, 0.0, 0.0 );
		osg::Quat Ynew_rotate( angle * throwScale,axis );

		_rotHeadKinect = Xnew_rotate * Ynew_rotate;

		setRotationHead( x, y, distance, 1 ); // 0 because its from Kinect
	}
	else {
		qDebug() << "Warning: setRotationHead(): wrong parameters";
	}
}

void Vwr::CameraManipulator::setRotationHeadFaceDet( float x, float y, float distance )
{

	osg::Vec3	axis;
	float	angle;
	double	throwScale;

	x /= 100;
	y /= 100;

	if ( ( -1.0f <= x && x <= 1.0f ) && ( -1.0f <= y && y <= 1.0f ) ) {

		throwScale = ( _thrown && _ga_t0.valid() && _ga_t1.valid() ) ?
					 _delta_frame_time / ( _ga_t0->getTime() - _ga_t1->getTime() ) :
					 1.0;

		// both rotation must be separated
		// horizontal rotation
		trackball( axis,angle, x, 0.0, 0.0, 0.0 );
		osg::Quat Xnew_rotate( angle * throwScale,axis );

		// vertical rotation
		trackball( axis,angle,0.0, y, 0.0, 0.0 );
		osg::Quat Ynew_rotate( angle * throwScale,axis );

		_rotHeadFaceDet = Xnew_rotate * Ynew_rotate;

		setRotationHead( x, y, distance, 0 ); // 0 because its from face detection
	}
	else {
		qDebug() << "Warning: setRotationHead(): wrong parameters";
	}
}


void Vwr::CameraManipulator::setRotationHead( float x, float y, float distance, int caller )
{

	double	throwScale;
	float aux, angleAux;
	osg::Vec3	axisAux;

	throwScale = ( _thrown && _ga_t0.valid() && _ga_t1.valid() ) ?
				 _delta_frame_time / ( _ga_t0->getTime() - _ga_t1->getTime() ) :
				 1.0;

	// rotation continously if x/y is out of margins
	float region = this->appConf->getValue( "Kinect.RegionRotationTreshold" ).toFloat();
	float step = this->appConf->getValue( "Kinect.RegionRotationStep" ).toFloat();

	if ( x < -region || x > region ) {
		aux = x < 0.0f ? -step : step;
		trackball( axisAux, angleAux, aux, 0.0, 0.0, 0.0 );
		osg::Quat rotHorAux = osg::Quat( angleAux * throwScale, axisAux );
		_rotationAux = _rotationAux * rotHorAux;
	}
	if ( y < -region || y > region ) {
		aux = y < 0.0f ? -step : step;
		trackball( axisAux, angleAux, 0.0, aux, 0.0, 0.0 );
		osg::Quat rotVerAux = osg::Quat( angleAux * throwScale, axisAux );
		_rotationAux = _rotationAux * rotVerAux;
	}



	if ( _cameraCanRot ) { // rotate camera

		// both rotation
		_rotationHead = _rotationAux * _rotHeadFaceDet * _rotHeadKinect;

		// will we correct projection according face position
		bool projectionConrrection;
		projectionConrrection = ( this->appConf->getValue( "FaceDecetion.EnableProjectionCorrection" ).toInt() ? true : false );

		if ( projectionConrrection && caller == 0 ) {
			updateProjectionAccordingFace( x, y, -distance );
		}


	}
	else {   // rotate graph
		sendFaceDetRotation( _rotationAux * _rotHeadFaceDet * _rotHeadKinect );
	}


}

void Vwr::CameraManipulator::updateProjectionAccordingFace( const double x, const double y, const double distance )
{
	double left, right, bottom, top, zNear, zFar;
	double fovy, ratio, width, height;

	// get current projection setting
	this->coreGraph->getCamera()->getProjectionMatrixAsPerspective( fovy, ratio, zNear, zFar );

	// compute new frustrum
	width = height = ( zNear * distance )/2;

	top		= height * ( 1 - y*distance );
	bottom	= -( height + height ) + top;

	right	= width * ( 1- x*distance );
	left	= -( width + width ) + right;

	// repair projection ratio for screen resizing
	left	*= ratio;
	right	*= ratio;

	this->coreGraph->getCamera()->setProjectionMatrixAsFrustum( left, right, bottom, top, zNear, zFar );
}


osg::Vec3d Vwr::CameraManipulator::getCameraPosition()
{
	osg::Vec3d center = getCenter();
	float distance = static_cast<float>( getDistance() ) ;
	osg::Quat rotation = getRotation();

	osg::Vec3 direction = rotation * osg::Vec3( 0, 0, 1 );
	direction *= distance;

	return center+direction;
}
void Vwr::CameraManipulator::updateArucoGraphPosition( osg::Vec3d pos )
{
	QString str;
	//str  = "arMat " + QString::number( pos.x(), 'f', 2);
	//str += " " + QString::number( pos.y(), 'f', 2);
	//str += " " + QString::number( pos.z(), 'f', 2);
	//qDebug() << ": " << str;

	double distArc =  pos.z()  < 0.0 ? - pos.z()	:  pos.z();		// distance of marker
	double distGra = _distance < 0.0 ? -_distance	: _distance;	// distance of graph
	if ( distGra < 1.0 ) {
		distGra = 1.0;
	}

	_centerArucoTrans[1] = distGra * ( pos.z() + 0.5 ); //constDist);			// distance
	_centerArucoTrans[0] = distGra * pos.x()*0.8;							// horizontal
	_centerArucoTrans[2] = distGra * pos.y()*0.8;			// vertical


	str  = "  " + QString::number( _centerArucoTrans[1], 'f', 2 );
	str += "  " + QString::number( _centerArucoTrans[0], 'f', 2 );
	str += "  " + QString::number( _centerArucoTrans[2], 'f', 2 );
	//qDebug() << ": " << str;
	//qDebug() << ": " << _distance ;

}

void Vwr::CameraManipulator::setCameraCanRot( bool cameraCanRot )
{
	//if SKYBOX 2 disable camera rotation
	if ( appConf->getValue( "Viewer.SkyBox.Noise" ).toInt() != 2 ) {
		_cameraCanRot = cameraCanRot;
	}
	else {
		_cameraCanRot=false;
	}
}

// Duransky start - Resetovanie projekcnej matice pri vypnuti vertigo modu
void Vwr::CameraManipulator::resetProjectionMatrixToDefault()
{
	this->coreGraph->getCamera()->setProjectionMatrixAsPerspective( 60, ratio, 0.01, appConf->getValue( "Viewer.Display.ViewDistance" ).toDouble() );
}
// Duransky end - Resetovanie projekcnej matice pri vypnuti vertigo modu

// pridame funkciu setSpeed, ktoru budeme volat na zaklade druhej ruky (pocet vystretych prstov napr)
void Vwr::CameraManipulator::enableCameraMovement( Vwr::CameraManipulator::Movement movement )
{
	switch ( movement ) {
		case Vwr::CameraManipulator::Movement::RIGHT : {
			sideSpeed = 2 * maxSpeed;;
			decelerateSideRate = false;
			break;
		}

		case Vwr::CameraManipulator::Movement::LEFT : {
			sideSpeed = -2 * maxSpeed;;
			decelerateSideRate = false;
			break;
		}

		case Vwr::CameraManipulator::Movement::UP : {
			verticalSpeed = -2 * maxSpeed;;
			decelerateVerticalRate = false;
			break;
		}

		case Vwr::CameraManipulator::Movement::DOWN : {
			verticalSpeed = 2 * maxSpeed;;
			decelerateVerticalRate = false;
			break;
		}

		case Vwr::CameraManipulator::Movement::FORWARD : {
			forwardSpeed = 2 * maxSpeed;
			decelerateForwardRate = false;
			break;
		}

		case Vwr::CameraManipulator::Movement::BACKWARD : {
			forwardSpeed = -2 * maxSpeed;
			decelerateForwardRate = false;
			break;
		}

		default:
			break;
	}
}

void Vwr::CameraManipulator::disableCameraMovement()
{
	decelerateSideRate = true;
	decelerateVerticalRate = true;
	decelerateForwardRate = true;
}

} // namespace Vwr

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
