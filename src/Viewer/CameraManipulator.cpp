#include "Viewer/CameraManipulator.h"
#include <osg/Quat>
#include <osg/Notify>
#include <osg/BoundsChecking>
#include <osgViewer/Viewer>
#include "Manager/Manager.h"

#include "Network/Server.h"
#include "Network/Client.h"

//using namespace osg;
//using namespace osgGA;
//using namespace Vwr;

double Vwr::CameraManipulator::EYE_MOVEMENT_SPEED;
double Vwr::CameraManipulator::TARGET_MOVEMENT_SPEED;
double Vwr::CameraManipulator::SCREEN_MARGIN;

namespace Vwr {

Vwr::CameraManipulator::CameraManipulator(Vwr::CoreGraph * coreGraph)
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

	movingAutomatically = false;

	EYE_MOVEMENT_SPEED = 0.005;
	TARGET_MOVEMENT_SPEED = 0.005;
	SCREEN_MARGIN = 200;

	this->coreGraph = coreGraph;
	stop();
}

Vwr::CameraManipulator::~CameraManipulator()
{
}

void Vwr::CameraManipulator::setNode(osg::Node* node)
{
	_node = node;
	if (_node.get())
	{
		const osg::BoundingSphere& boundingSphere=_node->getBound();
		_modelScale = boundingSphere._radius;
	}
	if (getAutoComputeHomePosition()) computeHomePosition();
}


const osg::Node* Vwr::CameraManipulator::getNode() const
{
	return _node.get();
}


osg::Node* Vwr::CameraManipulator::getNode()
{
	return _node.get();
}


void Vwr::CameraManipulator::home(double /*currentTime*/)
{
	if (getAutoComputeHomePosition()) computeHomePosition();
	computePosition(_homeEye, _homeCenter, _homeUp);
	_thrown = false;
}

void Vwr::CameraManipulator::home(const GUIEventAdapter& ea ,GUIActionAdapter& us)
{
	home(ea.getTime());
	us.requestRedraw();
	us.requestContinuousUpdate(false);
}


void Vwr::CameraManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
{
	flushMouseEventStack();
}


void Vwr::CameraManipulator::getUsage(osg::ApplicationUsage& usage) const
{
	usage.addKeyboardMouseBinding("Trackball: Space","Reset the viewing position to home");
	usage.addKeyboardMouseBinding("Trackball: +","When in stereo, increase the fusion distance");
	usage.addKeyboardMouseBinding("Trackball: -","When in stereo, reduce the fusion distance");
}

bool Vwr::CameraManipulator::handle(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	switch(ea.getEventType())
	{
	case(GUIEventAdapter::FRAME):
	{
		return handleFrame(ea, us);
	}
	default:
		break;
	}

	if (ea.getHandled()) return false;

	switch(ea.getEventType())
	{
	case(GUIEventAdapter::PUSH):
	{
		return handlePush(ea, us);
	}
	case(GUIEventAdapter::RELEASE):
	{
		return handleRelease(ea, us);
	}
	case(GUIEventAdapter::DRAG):
	case(GUIEventAdapter::SCROLL):
	{
		return handleScroll(ea, us);
	}
	case(GUIEventAdapter::MOVE):
	{
		return false;
	}
	case(GUIEventAdapter::KEYDOWN):
	{
		return handleKeyDown(ea, us);
	}
	case(GUIEventAdapter::KEYUP):
	{
		return handleKeyUp( ea, us );
	}
	case(GUIEventAdapter::FRAME):
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

bool Vwr::CameraManipulator::handleScroll(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	addMouseEvent(ea);
	if (calcMovement()) us.requestRedraw();
	us.requestContinuousUpdate(false);
	_thrown = false;
	return true;
}

bool Vwr::CameraManipulator::handleRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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

bool Vwr::CameraManipulator::handlePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (ea.getButtonMask() == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
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
		else {
			_distance = lastDistance;
		}

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

bool Vwr::CameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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

bool Vwr::CameraManipulator::isMouseMoving()
{
	if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

	static const float velocity = 0.1f;

	float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
	float len = sqrtf(dx*dx+dy*dy);
	float dt = _ga_t0->getTime()-_ga_t1->getTime();

	return (len>dt*velocity);
}


void Vwr::CameraManipulator::flushMouseEventStack()
{
	_ga_t1 = NULL;
	_ga_t0 = NULL;
}


void Vwr::CameraManipulator::addMouseEvent(const GUIEventAdapter& ea)
{
	_ga_t1 = _ga_t0;
	_ga_t0 = &ea;
}

void Vwr::CameraManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	_center = osg::Vec3(0.0f,0.0f,-_distance)*matrix;
	_rotation = matrix.getRotate();
}

osg::Matrixd Vwr::CameraManipulator::getMatrix() const
{
	return osg::Matrixd::translate(0.0,0.0,_distance)*osg::Matrixd::rotate(_rotation*_rotationHead)*osg::Matrixd::translate(_center);
}

osg::Matrixd Vwr::CameraManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate(-_center)*osg::Matrixd::rotate((_rotation*_rotationHead).inverse())*osg::Matrixd::translate(0.0,0.0,-_distance);
}

void Vwr::CameraManipulator::computePosition(const osg::Vec3& eye,const osg::Vec3& center,const osg::Vec3& up)
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


bool Vwr::CameraManipulator::calcMovement()
{
	// mouse scroll is only a single event
	if (_ga_t0.get()==NULL) return false;

	float dx=0.0f;
	float dy=0.0f;
	unsigned int buttonMask=osgGA::GUIEventAdapter::NONE;

	if (_ga_t0->getEventType()==GUIEventAdapter::SCROLL)
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
		buttonMask=GUIEventAdapter::SCROLL;
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

	if (buttonMask==GUIEventAdapter::RIGHT_MOUSE_BUTTON)
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
	else if (buttonMask==(GUIEventAdapter::LEFT_MOUSE_BUTTON|GUIEventAdapter::RIGHT_MOUSE_BUTTON))
	{

		// pan model.

		float scale = -0.3f * _distance * throwScale;

		osg::Matrix rotation_matrix;
		rotation_matrix.makeRotate(_rotation);

		osg::Vec3 dv(dx*scale,dy*scale,0.0f);

		_center += dv*rotation_matrix;

		notifyServer();
		notifyClients();

		return true;
	}
	else if (buttonMask==GUIEventAdapter::SCROLL)
	{
		// zoom model.

		float fd = _distance;
		float scale = 1.0f+ dy * throwScale;
		if (fd*scale>_modelScale*_minimumZoomScale)
		{
			if (_distance * scale < 10000)
				_distance *= scale;
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
void Vwr::CameraManipulator::setTrackballSize(float size)
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
void Vwr::CameraManipulator::trackball(osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y)
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
float Vwr::CameraManipulator::tb_project_to_sphere(float r, float x, float y)
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
bool Vwr::CameraManipulator::handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter & us)
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
bool Vwr::CameraManipulator::handleKeyDown( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter & )
{
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
void Vwr::CameraManipulator::frame( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &aa );

	// ak sme v rezime automatickeho pohybu
	if(movingAutomatically)
	{
		// inicializujeme premenne
		if (!automaticMovementInitialized)
			initAutomaticMovement(viewer);

		// dokym pohlad a pozicia neopisu celu trajektoriu
		if (t1 <= 1 || t2 <= 1)
		{
			// ziskanie novych pozicii na krivke
			if (t1 <= 1)
			{
				cameraPosition = CameraMath::getPointOnNextBezierCurve(t1, cameraPositions, w1);
				t1 += EYE_MOVEMENT_SPEED;
			}

			if (t2 <= 1)
			{
				targetPoint = CameraMath::getPointOnNextBezierCurve(t2, targetPositions, w2 );
				t2 += TARGET_MOVEMENT_SPEED;
			}


			// aktualne pozicie kamery
			osg::Vec3d eye;
			osg::Vec3d center;
			osg::Vec3d up;

			viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);

			// prepocitanie koordinatov kamerery s novymi hodnotami
			computePosition(cameraPosition, targetPoint, up);

			// zobrazenie trajektorie kamery
			if (appConf->getValue("Viewer.Display.CameraPath").toInt() == 1)
			{
				QLinkedList<osg::ref_ptr<osg::Node> > * list = coreGraph->getCustomNodeList();
				osg::ref_ptr<osg::Group> group = new osg::Group;
				osg::ref_ptr<osg::Geode> g1 = new osg::Geode;

				g1->addDrawable(CameraMath::createAxis(eye, lastPosition));
				g1->addDrawable(CameraMath::createAxis(targetPoint, lastTargetPoint, osg::Vec4d(1,0,0,1)));

				group->addChild(g1);
				lastPosition = eye;
				lastTargetPoint = targetPoint;
				list->push_back(group);
			}

			// vypis metriky do konzoly
			if (appConf->getValue("Graph.Metrics.ViewMetrics").toInt() == 1)
			{
				computeViewMetrics(viewer, selectedCluster);
			}

		}

		// ukoncenie pohybu
		else
		{
			movingAutomatically = false;

			if (appConf->getValue("Viewer.Display.CameraPositions").toInt() == 1)
			{
				osg::Vec3d eye;
				osg::Vec3d center;
				osg::Vec3d up;

				viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);

				Data::Graph * g = Manager::GraphManager::getInstance()->getActiveGraph();
				g->addNode("EndNode", g->getNodeMetaType(), eye);
			}

			//		osg::Vec3d eye;
			//		osg::Vec3d center;
			//		osg::Vec3d up;

			//		viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);
			//			cout << eye.x() << " " << eye.y() << " " << eye.z() << "\n";
		}
	}
	// standardny frame
	else
	{
		computeStandardFrame(ea, aa);
	}
}

void Vwr::CameraManipulator::computeStandardFrame(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

void Vwr::CameraManipulator::setNewPosition(osg::Vec3d cameraTargetPoint, osg::Vec3d cameraInterestPoint, std::list<osg::ref_ptr<Data::Node> > selectedCluster, std::list<osg::ref_ptr<Data::Edge> > selectedEdges)
{
	movingAutomatically = true;
	this->cameraTargetPoint = cameraTargetPoint;
	this->cameraInterestPoint = cameraInterestPoint;

	//cout << "Camera targetPoint: " << cameraTargetPoint.x() << " " << cameraTargetPoint.y() << " " << cameraTargetPoint.z() << "\n";
	//cout << "Camera interest targetPoint: " << cameraInterestPoint.x() << " " << cameraInterestPoint.y() << " " << cameraInterestPoint.z() << "\n";

	std::list<osg::ref_ptr<Data::Edge> >::iterator i;

	// prida hrany medzi body zaujmu
	for (i = selectedEdges.begin(); i != selectedEdges.end(); ++i)
	{
		selectedCluster.push_back((*i)->getSrcNode());
		selectedCluster.push_back((*i)->getDstNode());
	}

	selectedCluster.unique();

	this->selectedCluster = selectedCluster;

	automaticMovementInitialized = false;
}

void Vwr::CameraManipulator::initAutomaticMovement(osgViewer::Viewer* viewer)
{
	t1 = t2 = 0;

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);

	this->lastPosition = originalEyePosition = eye;
	this->lastTargetPoint = center;

	float scale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat();

	weightPoint = (eye + cameraInterestPoint + cameraTargetPoint)/3;
	targetDistance = alterCameraTargetPoint(viewer);

	cameraPositions = new QVector<osg::Vec3d>();
	cameraPositions->push_back(originalEyePosition);
	cameraPositions->push_back(cameraInterestPoint);
	cameraPositions->push_back(cameraTargetPoint);

	// vahy kontrolnych bodov pre trajektoriu kamery
	w1[0] = 1;
	w1[1] = -0.1;
	w1[2] = 1;

	targetPositions = new QVector<osg::Vec3d>();

	targetPositions->push_back(center);
	targetPositions->push_back(cameraInterestPoint);
	targetPositions->push_back(weightPoint);

	// vahy kontrolnych bodov pre trajektriu pohladu
	w2[0] = 1;
	w2[1] = 0.5f;
	w2[2] = 1;

	// uprava vah aby boli viditelne vsetky body zaujmu
	alterWeights(viewer, selectedCluster);

	// zobrazenie kontornych bodov
	if (appConf->getValue("Viewer.Display.CameraPositions").toInt() == 1)
	{
		Data::Graph * g = Manager::GraphManager::getInstance()->getActiveGraph();

		g->addNode("centerNode", g->getNodeMetaType(), center)->setColor(osg::Vec4(0, 0.5, 1, 0.5));
		g->addNode("startNode", g->getNodeMetaType(), originalEyePosition / scale);
		g->addNode("interestPoint", g->getNodeMetaType(), cameraInterestPoint / scale)->setColor(osg::Vec4(0, 1, 1, 1));
		g->addNode("weigthPoint", g->getNodeMetaType(), weightPoint)->setColor(osg::Vec4(0, 0.5, 1, 1));
	}

	automaticMovementInitialized = true;
}

float Vwr::CameraManipulator::alterCameraTargetPoint(osgViewer::Viewer* viewer)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera(*(viewer->getCamera()), osg::CopyOp::DEEP_COPY_ALL);
	//osg::ref_ptr<osg::Camera> camera = viewer->getCamera();

	int width = camera->getViewport()->width();
	int height = camera->getViewport()->height();

	osg::Vec3d basicVec = cameraTargetPoint - weightPoint;
	float scale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat();

	// minimum distance from center
	float dist = basicVec.length() + 50 * scale;

	osg::Vec3d eyePosition;

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);

	// move camera target point backwards until all interest points can be seen
	while(true)
	{
		eyePosition = CameraMath::getPointOnVector(weightPoint, cameraTargetPoint, dist);
		camera->setViewMatrixAsLookAt(eyePosition, weightPoint, up);

		osg::Vec3d eyeScr = CameraMath::projectOnScreen(camera, originalEyePosition);
		osg::Vec3d cameraInterestPointScr = CameraMath::projectOnScreen(camera, cameraInterestPoint);

		//cout << "eyeScr: " << eyeScr.x() << " " << eyeScr.y() << "\n";
		//cout << "cameraInterestPointScr: " << cameraInterestPointScr.x() << " " << cameraInterestPointScr.y() << "\n";

		bool onScreen1 = CameraMath::isInRect(eyeScr, width, height, SCREEN_MARGIN);
		bool onScreen2 = CameraMath::isInRect(cameraInterestPointScr, width, height, SCREEN_MARGIN);

		if (!(onScreen1 && onScreen2))
		{
			dist += 10;
		}
		else
			break;
	}
	cameraTargetPoint = eyePosition;

	/*cout << "Altered target: " << eyePosition.x() << " " << eyePosition.y() << " " << eyePosition.z() << "\n";*/

	return dist;
}

void Vwr::CameraManipulator::alterWeights(osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera(*(viewer->getCamera()), osg::CopyOp::DEEP_COPY_ALL);

	int width = camera->getViewport()->width();
	int height = camera->getViewport()->height();

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	// get initial camera position
	camera->getViewMatrixAsLookAt(eye, center, up);

	// alter weights until whole cluster is seen
	while(true)
	{
		// get position and orientation in t = 0.5
		osg::Vec3d eyePosition = CameraMath::getPointOnNextBezierCurve(0.5f, cameraPositions, w1);
		osg::Vec3d targetPosition = CameraMath::getPointOnNextBezierCurve(0.5f / (EYE_MOVEMENT_SPEED / TARGET_MOVEMENT_SPEED), targetPositions, w2);

		camera->setViewMatrixAsLookAt(eyePosition, targetPosition, up);

		// get cluster nodes in extreme positions in t = 0.5
		QVector<osg::ref_ptr<Data::Node> > * extremes = CameraMath::getViewExtremes(camera, selectedCluster);

		bool onScreen = true;

		// check for visibility of extremes
		for (int x = 0; x < 4; x++)
		{
			onScreen &= CameraMath::isInRect(CameraMath::projectOnScreen(camera, extremes->at(x)->getCurrentPosition()), width, height, SCREEN_MARGIN);
		}

		if (!onScreen)
		{
			w1[1] -= 0.0005f;
		}
		else
			break;
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

void Vwr::CameraManipulator::computeViewMetrics(osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster)
{
	int cnt = 0;

	std::list<osg::ref_ptr<Data::Node> >::iterator i;

	for (i = selectedCluster.begin(); i != selectedCluster.end(); ++i)
	{
		if (CameraMath::isInRect(CameraMath::projectOnScreen(viewer->getCamera(), (*i)->getCurrentPosition()), viewer->getCamera()->getViewport()->width(), viewer->getCamera()->getViewport()->height(), 0))
		{
			cnt++;
		}
	}

	cout << "Currently visible: " << cnt << " nodes\n";
}

/*
 * Compute both horizontal & vertical rotation according head translation.
 * Result rotation is in _rotationHead quaternion.
 * ! distance is not implemented yet
 */
void Vwr::CameraManipulator::setRotationHead(float x, float y, float distance)
{

	osg::Vec3	axis;
	float	angle;
	double	throwScale;

	x /= 100;
	y /= 100;

	if ( (-1.0 <= x && x <= 1.0) && (-1.0 <= y && y <= 1.0)){

		throwScale =  (_thrown && _ga_t0.valid() && _ga_t1.valid()) ?
					_delta_frame_time / (_ga_t0->getTime() - _ga_t1->getTime()) :
					1.0;

		// both rotation must be separated
		// horizontal rotation
		trackball(axis,angle, x, 0.0, 0.0, 0.0);
		osg::Quat Xnew_rotate(angle * throwScale,axis);

		// vertical rotation
		trackball(axis,angle,0.0, y, 0.0, 0.0);
		osg::Quat Ynew_rotate(angle * throwScale,axis);

		// both rotation
		_rotationHead = Xnew_rotate*Ynew_rotate;


		// will we correct projection according face position
		bool projectionConrrection = false;
		projectionConrrection = this->appConf->getValue("FaceDecetion.EnableProjectionCorrection").toInt();

		if( projectionConrrection ){
			updateProjectionAccordingFace( x, y, -distance );
		}


	}
	else{
		qDebug() << "Warning: setRotationHead(): wrong parameters";
	}
}

void Vwr::CameraManipulator::updateProjectionAccordingFace(const float x, const float y, const float distance)
{
	double left, right, bottom, top, zNear, zFar;
	double fovy, ratio, width, height;

	// get current projection setting
	this->coreGraph->getCamera()->getProjectionMatrixAsPerspective(fovy, ratio, zNear, zFar);

	// compute new frustrum
	width = height = (zNear * distance)/2;

	top		= height * (1 - y*distance);
	bottom	= -(height + height) + top;

	right	= width * (1- x*distance);
	left	= -(width + width) + right;

	// repair projection ratio for screen resizing
	left	*= ratio;
	right	*= ratio;

	this->coreGraph->getCamera()->setProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);
}

} // namespace
