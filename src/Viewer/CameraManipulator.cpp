#include "Viewer/CameraManipulator.h"
#include <osg/Quat>
#include <osg/Notify>
#include <osg/BoundsChecking>
#include <osgViewer/Viewer>

#include "Network/Server.h"
#include "Network/Client.h"

namespace Vwr {

CameraManipulator::CameraManipulator()
{
	appConf = Util::ApplicationConfig::get();

    _modelScale = 1.0f;
    _minimumZoomScale = 0.0f;
    _allowThrow = true;
    _thrown = false;

    _distance = 1.0f;
    _trackballSize = appConf->getValue("Viewer.CameraManipulator.Sensitivity").toFloat();
    _zoomDelta = 0.4f;

	speedDecelerationFactor = 0.4f;
	maxSpeed = appConf->getValue("Viewer.CameraManipulator.MaxSpeed").toFloat();
	speedEpsilon = 0.02f;

        stop();
}

CameraManipulator::~CameraManipulator()
{
}

void CameraManipulator::setNode(osg::Node* node)
{
    _node = node;
    if (_node.get())
    {
        const osg::BoundingSphere& boundingSphere=_node->getBound();
        _modelScale = boundingSphere._radius;
    }
    if (getAutoComputeHomePosition()) computeHomePosition();
}


const osg::Node* CameraManipulator::getNode() const
{
    return _node.get();
}


osg::Node* CameraManipulator::getNode()
{
    return _node.get();
}


void CameraManipulator::home(double /*currentTime*/)
{
    if (getAutoComputeHomePosition()) computeHomePosition();
    computePosition(_homeEye, _homeCenter, _homeUp);
    _thrown = false;
}

void CameraManipulator::home(const osgGA::GUIEventAdapter& ea ,osgGA::GUIActionAdapter& us)
{
    home(ea.getTime());
    us.requestRedraw();
    us.requestContinuousUpdate(false);
}


void CameraManipulator::init(const osgGA::GUIEventAdapter& ,osgGA::GUIActionAdapter& )
{
    flushMouseEventStack();
}


void CameraManipulator::getUsage(osg::ApplicationUsage& usage) const
{
    usage.addKeyboardMouseBinding("Trackball: Space","Reset the viewing position to home");
    usage.addKeyboardMouseBinding("Trackball: +","When in stereo, increase the fusion distance");
    usage.addKeyboardMouseBinding("Trackball: -","When in stereo, reduce the fusion distance");
}

bool CameraManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::FRAME):
        {
			return handleFrame(ea, us);
        }
        default:
            break;
    }

    if (ea.getHandled()) return false;

    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
			return handlePush(ea, us);
        }
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            return handleRelease(ea, us);
        }
        case(osgGA::GUIEventAdapter::DRAG):
        case(osgGA::GUIEventAdapter::SCROLL):
        {
			return handleScroll(ea, us);
        }
        case(osgGA::GUIEventAdapter::MOVE):
        {
            return false;
        }
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			return handleKeyDown(ea, us);
		}
		case(osgGA::GUIEventAdapter::KEYUP):
		{
			return handleKeyUp( ea, us );
		}
        case(osgGA::GUIEventAdapter::FRAME):
		{
            if (_thrown)
            {
                if (calcMovement()) us.requestRedraw();
            }

            return false;
		}
        default:
            return false;
    }
}

bool CameraManipulator::handleScroll(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	addMouseEvent(ea);
	if (calcMovement()) us.requestRedraw();
	us.requestContinuousUpdate(false);
	_thrown = false;
	return true;
}

bool CameraManipulator::handleRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (ea.getButtonMask()==0)
	{

		double timeSinceLastRecordEvent = _ga_t0.valid() ? (ea.getTime() - _ga_t0->getTime()) : DBL_MAX;
		if (timeSinceLastRecordEvent>0.02) flushMouseEventStack();

		if (isMouseMoving())
		{
			if (calcMovement())
			{
				us.requestRedraw();
				us.requestContinuousUpdate(true);
				_thrown = _allowThrow;
			}
		}
		else
		{
			flushMouseEventStack();
			addMouseEvent(ea);
			if (calcMovement()) us.requestRedraw();
			us.requestContinuousUpdate(false);
			_thrown = false;
		}

	}
	else
	{
		flushMouseEventStack();
		addMouseEvent(ea);
		if (calcMovement()) us.requestRedraw();
		us.requestContinuousUpdate(false);
		_thrown = false;
        }

        notifyServer();
        notifyClients();

	return true;
}

bool CameraManipulator::handlePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		if (_distance != 0)
		{
			lastDistance = _distance;

			osg::Vec3d eye, cameraCenter, up;

			osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &us );
			viewer->getCamera()->getViewMatrixAsLookAt(eye, cameraCenter, up);

			_center = eye;
			_distance = 0;
		}
		else
                        _distance = lastDistance;

                notifyServer();
                notifyClients();

		return true;
	}
	else
	{
		flushMouseEventStack();
		addMouseEvent(ea);
		if (calcMovement()) us.requestRedraw();
		us.requestContinuousUpdate(false);
		_thrown = false;
		return true;
	}
}

bool CameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	double current_frame_time = ea.getTime();

	_delta_frame_time = current_frame_time - _last_frame_time;
	_last_frame_time = current_frame_time;

	if (_thrown && _allowThrow)
	{
		if (calcMovement()) us.requestRedraw();
	}

        frame(ea,us);
	return false;
}

bool CameraManipulator::isMouseMoving()
{
    if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

    static const float velocity = 0.1f;

    float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
    float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
    float len = sqrtf(dx*dx+dy*dy);
    float dt = _ga_t0->getTime()-_ga_t1->getTime();

    return (len>dt*velocity);
}


void CameraManipulator::flushMouseEventStack()
{
    _ga_t1 = NULL;
    _ga_t0 = NULL;
}


void CameraManipulator::addMouseEvent(const osgGA::GUIEventAdapter& ea)
{
    _ga_t1 = _ga_t0;
    _ga_t0 = &ea;
}

void CameraManipulator::setByMatrix(const osg::Matrixd& matrix)
{
    _center = osg::Vec3(0.0f,0.0f,-_distance)*matrix;
    _rotation = matrix.getRotate();

}

osg::Matrixd CameraManipulator::getMatrix() const
{
    return osg::Matrixd::translate(0.0,0.0,_distance)*osg::Matrixd::rotate(_rotation)*osg::Matrixd::translate(_center);
}

osg::Matrixd CameraManipulator::getInverseMatrix() const
{
    return osg::Matrixd::translate(-_center)*osg::Matrixd::rotate(_rotation.inverse())*osg::Matrixd::translate(0.0,0.0,-_distance);
}

void CameraManipulator::computePosition(const osg::Vec3& eye,const osg::Vec3& center,const osg::Vec3& up)
{

    osg::Vec3 lv(center-eye);

    osg::Vec3 f(lv);
    f.normalize();
    osg::Vec3 s(f^up);
    s.normalize();
    osg::Vec3 u(s^f);
    u.normalize();
    
    osg::Matrix rotation_matrix(s[0],     u[0],     -f[0],     0.0f,
                                s[1],     u[1],     -f[1],     0.0f,
                                s[2],     u[2],     -f[2],     0.0f,
                                0.0f,     0.0f,     0.0f,      1.0f);
                   
    _center = center;
    _distance = lv.length();
    _rotation = rotation_matrix.getRotate().inverse();

    notifyServer();
    notifyClients();

}


bool CameraManipulator::calcMovement()
{
    // mouse scroll is only a single event
    if (_ga_t0.get()==NULL) return false;

    float dx=0.0f;
    float dy=0.0f;
    unsigned int buttonMask=osgGA::GUIEventAdapter::NONE;

    if (_ga_t0->getEventType()==osgGA::GUIEventAdapter::SCROLL)
    {
        switch (_ga_t0->getScrollingMotion()) {
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
            dx = _ga_t0->getScrollingDeltaX() / ((_ga_t0->getXmax()-_ga_t0->getXmin()) * 0.5f);
            dy = _ga_t0->getScrollingDeltaY() / ((_ga_t0->getYmax()-_ga_t0->getYmin()) * 0.5f);

            dx *= _zoomDelta;
            dy *= _zoomDelta;
            break;
        default:
            break;
        }
        buttonMask=osgGA::GUIEventAdapter::SCROLL;
    } 
    else 
    {

        if (_ga_t1.get()==NULL) return false;
        dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
        dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
        float distance = sqrtf(dx*dx + dy*dy);
        
        // return if movement is too fast, indicating an error in event values or change in screen.
        if (distance>0.5)
        {
            return false;
        }
        
        // return if there is no movement.
        if (distance==0.0f)
        {
            return false;
        }

        buttonMask = _ga_t1->getButtonMask();
    }


    double throwScale =  (_thrown && _ga_t0.valid() && _ga_t1.valid()) ?
            _delta_frame_time / (_ga_t0->getTime() - _ga_t1->getTime()) :
            1.0;

    if (buttonMask==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
    {

        // rotate camera.

        osg::Vec3 axis;
        float angle;

        float px0 = _ga_t0->getXnormalized();
        float py0 = _ga_t0->getYnormalized();
        
        float px1 = _ga_t1->getXnormalized();
        float py1 = _ga_t1->getYnormalized();
        

        trackball(axis,angle,px1,py1,px0,py0);

        osg::Quat new_rotate;

        new_rotate.makeRotate(angle * throwScale,axis);

        _rotation = _rotation*new_rotate;


        notifyServer();
        notifyClients();

        return true;

    }
    else if (buttonMask==(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON|osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON))
    {

        // pan model.

        float scale = -0.3f * _distance * throwScale;

        osg::Matrix rotation_matrix;
        rotation_matrix.makeRotate(_rotation);

        osg::Vec3 dv(dx*scale,dy*scale,0.0f);

        _center += dv*rotation_matrix;

        notifyServer();
        notifyClients();

    }
    else if (buttonMask==osgGA::GUIEventAdapter::SCROLL)
    {

        // zoom model.

		float fd = _distance;
		float scale = 1.0f+ dy * throwScale;
		if (fd*scale>_modelScale*_minimumZoomScale)
		{
			if (_distance * scale < 10000)
				_distance *= scale;
		}
       
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
void CameraManipulator::setTrackballSize(float size)
{
    _trackballSize = size;
     osg::clampBetweenRange(_trackballSize,0.1f,1.0f,"CameraManipulator::setTrackballSize(float)");
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
void CameraManipulator::trackball(osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y)
{
    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */

    osg::Matrix rotation_matrix(_rotation);


    osg::Vec3 uv = osg::Vec3(0.0f,1.0f,0.0f)*rotation_matrix;
    osg::Vec3 sv = osg::Vec3(1.0f,0.0f,0.0f)*rotation_matrix;
    osg::Vec3 lv = osg::Vec3(0.0f,0.0f,-1.0f)*rotation_matrix;

    osg::Vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(_trackballSize, p1x, p1y);
    osg::Vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(_trackballSize, p2x, p2y);

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
    float t = (p2 - p1).length() / (2.0 * _trackballSize);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    angle = osg::inRadians(asin(t));

}


/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
float CameraManipulator::tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;

    d = sqrt(x*x + y*y);
                                 /* Inside sphere */
    if (d < r * 0.70710678118654752440)
    {
        z = sqrt(r*r - d*d);
    }                            /* On hyperbola */
    else
    {
        t = r / 1.41421356237309504880;
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
bool CameraManipulator::handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter & us)
{
	switch( ea.getKey() )
	{
	case osgGA::GUIEventAdapter::KEY_Space:
	{
		flushMouseEventStack();
		_thrown = false;
		_distance = 1.0f;
		home(ea,us);
		us.requestRedraw();
		us.requestContinuousUpdate(false);

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
bool CameraManipulator::handleKeyDown( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter & )
{
	int i = ea.getKey();
	switch( ea.getKey() )
	{
	case osgGA::GUIEventAdapter::KEY_Up:
		{
			forwardSpeed = 2 * maxSpeed;
			decelerateForwardRate = false;
			break;
		}

	case osgGA::GUIEventAdapter::KEY_Down:
		{
			forwardSpeed = -2 * maxSpeed;
			decelerateForwardRate = false;
			break;
		}

	case osgGA::GUIEventAdapter::KEY_Right:
		{
			sideSpeed = 2 * maxSpeed;
			decelerateSideRate = false;
			break;
		}

	case osgGA::GUIEventAdapter::KEY_Left:
		{
			sideSpeed = -2 * maxSpeed;;
			decelerateSideRate = false;
			break;
		}
	case osgGA::GUIEventAdapter::KEY_Page_Up:
		{
			verticalSpeed = 2 * maxSpeed;
			decelerateVerticalRate = false;
			break;
		}
	case osgGA::GUIEventAdapter::KEY_Page_Down:
		{
			verticalSpeed = -2 * maxSpeed;
			decelerateVerticalRate = false;
			break;
		}
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
void CameraManipulator::frame( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &aa );

	double t1 = ea.getTime();

	if( t0 == 0.0 )
	{
		t0 = ea.getTime();
		dt = 0.0;
	}
	else
	{
		dt = t1 - t0;
		t0 = t1;
	}

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);

	osg::Vec3d directionVec = center - eye;
	osg::Vec3d sideVec = getSideVector(osg::Matrixd(_rotation)); /* direction * osg::Matrix::rotate(-M_PI * 0.5, upVector);*/
	
        _center += ((directionVec * forwardSpeed) + (sideVec * sideSpeed) + (up * verticalSpeed)) * dt;

	if( decelerateSideRate )
	{
		sideSpeed *= speedDecelerationFactor;
	}
	if( decelerateForwardRate )
	{
		forwardSpeed *= speedDecelerationFactor;
	} 
	if (decelerateVerticalRate)
	{
		verticalSpeed *= speedDecelerationFactor;
	}
	if (deceleratePitchRate)
	{
		pitchSpeed *= speedDecelerationFactor;
        }
}

/*!
 * 
 * Zastavi pohyb kamery.
 */
void CameraManipulator::stop()
{
	forwardSpeed = 0.0;
	sideSpeed = 0.0;
	verticalSpeed = 0.0;
	pitchSpeed = 0.0;
}

void CameraManipulator::notifyClients() {
    Network::Server * server = Network::Server::getInstance();
    if (server->isListening()) {
        server->sendMyView(_center,_rotation, _distance);
    }
}

void CameraManipulator::notifyServer() {
    Network::Client * client = Network::Client::getInstance();
    if (client->isConnected()) {
        client->sendMyView(_center,_rotation, _distance);
    }
}

} // namespace
