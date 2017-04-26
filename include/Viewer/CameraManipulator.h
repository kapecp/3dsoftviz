/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#ifndef Viewer_CameraManipulator
#define Viewer_CameraManipulator 1

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/Quat>
#include <osgViewer/Viewer>
#include <QObject>

#include "Util/ApplicationConfig.h"
#include "Data/Node.h"
#include "Math/CameraMath.h"
#include "Viewer/CoreGraph.h"

#include <list>

namespace Util {
class ApplicationConfig;
}

namespace Vwr {

using namespace osgGA;

class CoreGraph;

/**
	*  \class CameraManipulator
	*  \brief Camera manipulator class. Handles all camera movements.
	*  \author Michal Paprcka
	*  \date 29. 4. 2010
	*
	*	This class is basically extended TrackballManipulator class with new functionality.
	*/
class CameraManipulator : public QObject, public KeySwitchMatrixManipulator
{
	Q_OBJECT

public:
	CameraManipulator( Vwr::CoreGraph* coreGraph );

	/**
	 * @brief Enum used to define what direction of movement we are using for camera movement
	 */
	enum Movement {
		RIGHT, LEFT, UP, DOWN, FORWARD, BACKWARD
	};

	/**
	 * @brief Enables camera movement to a specific direction
	 * @param movement - direction where we want to move camera(enum Movement)
	 */
	void enableCameraMovement( Vwr::CameraManipulator::Movement movement );

	/**
	 * @brief Disables all camera movements
	 */
	void disableCameraMovement();

	virtual const char* className() const
	{
		return "Trackball";
	}

	/** set the position of the matrix manipulator using a 4x4 Matrix.*/
	virtual void setByMatrix( const osg::Matrixd& matrix );

	/** set the position of the matrix manipulator using a 4x4 Matrix.*/
	virtual void setByInverseMatrix( const osg::Matrixd& matrix )
	{
		setByMatrix( osg::Matrixd::inverse( matrix ) );
	}

	/** get the position of the manipulator as 4x4 Matrix.*/
	virtual osg::Matrixd getMatrix() const;

	/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
	virtual osg::Matrixd getInverseMatrix() const;

	/** Get the FusionDistanceMode. Used by SceneView for setting up stereo convergence.*/
	virtual osgUtil::SceneView::FusionDistanceMode getFusionDistanceMode() const
	{
		return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE;
	}

	/** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
	virtual float getFusionDistanceValue() const
	{
		return ( float )_distance;
	}

	virtual void setVertigoMode( bool value );

	/** Attach a node to the manipulator.
			Automatically detaches previously attached node.
			setNode(NULL) detaches previously nodes.
			Is ignored by manipulators which do not require a reference model.*/
	virtual void setNode( osg::Node* );

	/** Return node if attached.*/
	virtual const osg::Node* getNode() const;

	/** Return node if attached.*/
	virtual osg::Node* getNode();

	/** Move the camera to the default position.
			May be ignored by manipulators if home functionality is not appropriate.*/
	virtual void home( const GUIEventAdapter& ea,GUIActionAdapter& us );
	virtual void home( double time = 0 );

	/** Start/restart the manipulator.*/
	virtual void init( const GUIEventAdapter& ea,GUIActionAdapter& us );

	/** handle events, return true if handled, false otherwise.*/
	virtual bool handle( const GUIEventAdapter& ea,GUIActionAdapter& us );

	/** Get the keyboard and mouse usage of this manipulator.*/
	virtual void getUsage( osg::ApplicationUsage& usage ) const;


	/** set the minimum distance (as ratio) the eye point can be zoomed in towards the
			center before the center is pushed forward.*/
	void setMinimumZoomScale( double minimumZoomScale )
	{
		_minimumZoomScale=minimumZoomScale;
	}

	/** get the minimum distance (as ratio) the eye point can be zoomed in */
	double getMinimumZoomScale() const
	{
		return _minimumZoomScale;
	}

	/** set the mouse scroll wheel zoom delta.
		  * Range -1.0 to +1.0,  -ve value inverts wheel direction and zero switches off scroll wheel. */

	void setScroolWheelZoomDelta( float zoomDelta )
	{
		_zoomDelta = zoomDelta;
	}

	/** get the mouse scroll wheel zoom delta. */
	double getScroolWheelZoomDelta() const
	{
		return _zoomDelta;
	}

	/** Set the center of the trackball. */
	void setCenter( const osg::Vec3d& center )
	{
		_center = center;
	}

	/** Enable/disable camera functions. */
	void setCameraActive( bool active )
	{
		_cameraActive = active;
	}

	/** Get the center of the trackball. */
	const osg::Vec3d& getCenter() const
	{
		return _center;
	}

	/** Set the rotation of the trackball. */
	void setRotation( const osg::Quat& rotation )
	{
		_rotation = rotation;
	}

	/** Get the rotation of the trackball. */
	const osg::Quat& getRotation() const
	{
		return _rotation;
	}

	/** Set the distance of the trackball. */

	void setDistance( double distance )
	{
		_distance = distance;
	}

	/** Get the distance of the trackball. */
	double getDistance() const
	{
		return _distance;
	}

	/** Set the size of the trackball. */
	void setTrackballSize( float size );

	/** Get the size of the trackball. */
	float getTrackballSize() const
	{
		return _trackballSize;
	}

	/** Set the 'allow throw' flag. Releasing the mouse button while moving the camera results in a throw. */
	void setAllowThrow( bool allowThrow )
	{
		_allowThrow = allowThrow;
	}

	/** Returns true if the camera can be thrown, false otherwise. This defaults to true. */
	bool getAllowThrow() const
	{
		return _allowThrow;
	}

	/** Resets the projection matrix to default values */
	void resetProjectionMatrixToDefault();

	/**
		*  \fn inline public  setMaxSpeed(float speed)
		*  \brief Sets maximum camera speed
		*  \param     speed    maximum speed
		*/
	void setMaxSpeed( double speed )
	{
		this->maxSpeed = speed;
	}

	/**
		*  \fn inline public  getMaxSpeed
		*  \brief Returns maximum camera speed
		*  \return float maximum speed
		*/
	double getMaxSpeed()
	{
		return maxSpeed;
	}

	/**
		 * @author Viktor Vinczler
		 * @brief setCenterSmoothly Initializes smooth center view
		 * @param newCenter Destination point for smooth center view
		 */
	void setCenterSmoothly( osg::Vec3 newCenter );

	void setNewPosition( osg::Vec3d cameraTargetPoint, osg::Vec3d cameraInterestPoint, std::list<osg::ref_ptr<Data::Node> > selectedCluster, std::list<osg::ref_ptr<Data::Edge> > selectedEdges );

	osg::Vec3d getCameraPosition();

	void rotateCamera( float py0, float px0, double throwScale, float py1, float px1 );
	bool getDecelerateForwardRate() const;

signals:

	/**
		 * @author Autor: David Durcak
		 * @brief sendMouseRotation Send rotation quaternion caused by mouse
		 * @param quat Quaternion that disribe rotation of mouse
		 */
	void sendMouseRotation( const osg::Quat quat );

	/**
		 * @author Autor: David Durcak
		 * @brief sendFaceDetRotation Send rotation quaternion caused by faceDetection
		 * @param quat Quaternion that disribe rotation of face
		 */
	void sendFaceDetRotation( const osg::Quat quat );

	void sendTranslatePosition( osg::Vec3d pos );



public slots:
	/**
		 * @author Autor: David Durcak
		 * @brief setRotationHeadFaceDet Update _rotHeadFaceDet quaternion to rotate camera according head and call setRotationHead()
		 * @param x % distance from middle on horizontal axis
		 * @param y % distance from middle on vertical axis
		 * @param distance from camera
		 */
	void setRotationHeadFaceDet( float x, float y, float distance );

	/**
		 * @author Autor: David Durcak
		 * @brief setRotationHeadKinect Update _rotHeadKinect quaternion to rotate camera according Kinect and call setRotationHead()
		 * @param x % distance from middle on horizontal axis
		 * @param y % distance from middle on vertical axis
		 * @param distance from camera
		 */
	void setRotationHeadKinect( float x, float y, float distance );

	/**
		 * @author Autor: David Durcak
		 * @brief updateArucoGrafPosition Update _centerArucoTrans vector to change graph position according by position of marker
		 * @param pos Vector that disribe posiotion of marker
		 */
	void updateArucoGraphPosition( osg::Vec3d pos );

	/**
		 * @author Autor: David Durcak
		 * @brief setCameraCanRot Set _cameraCanRot member.
		 * @param cameraCanRot
		 */
	void setCameraCanRot( bool cameraCanRot );

    void rotateForLeapBackgroundSync();

protected:

	virtual ~CameraManipulator();

	/** Reset the internal GUIEvent stack.*/
	void flushMouseEventStack();
	/** Add the current mouse GUIEvent to internal stack.*/
	void addMouseEvent( const GUIEventAdapter& ea );

	void computePosition( const osg::Vec3& eye,const osg::Vec3& lv,const osg::Vec3& up );

	/** For the give mouse movement calculate the movement of the camera.
			Return true is camera has moved and a redraw is required.*/
	bool calcMovement();

	void trackball( osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y );
	float tb_project_to_sphere( float r, float x, float y );


	/** Check the speed at which the mouse is moving.
			If speed is below a threshold then return false, otherwise return true.*/
	bool isMouseMoving();

	// Internal event stack comprising last two mouse events.

	/**
		*  osg::ref_ptr _ga_t1
		*  \brief Last event
		*/
	osg::ref_ptr<const GUIEventAdapter> _ga_t1;

	/**
		*  osg::ref_ptr _ga_t0
		*  \brief Current event
		*/
	osg::ref_ptr<const GUIEventAdapter> _ga_t0;


	/**
		*  osg::observer_ptr<osg::Node> _node
		*  \brief Node observer
		*/
	osg::observer_ptr<osg::Node>       _node;


	/**
		*  double _modelScale
		*  \brief model scale
		*/
	double _modelScale;

	/**
		*  double _minimumZoomScale
		*  \brief minimum zoom scale
		*/
	double _minimumZoomScale;


	/**
		*  bool _allowThrow
		*  \brief true, if throwing is allowed
		*/
	bool _allowThrow;

	/**
		*  bool _thrown
		*  \brief true, if camera was thrown
		*/
	bool _thrown;

	/**
		*  bool _vertigo
		*  \brief true, if camera is in vertigo mode
		*/
	bool _vertigo;

	/**
	 * double PI
	 * \brief value of mathematical constant PI
	 */

	double PI;

	double fovy, ratio, zNear, zFar, _width;

	/** The approximate amount of time it is currently taking to draw a frame.
		  * This is used to compute the delta in translation/rotation during a thrown display update.
		  * It allows us to match an delta in position/rotation independent of the rendering frame rate.
		  */
	double _delta_frame_time;

	/** The time the last frame started.
		  * Used when _rate_sensitive is true so that we can match display update rate to rotation/translation rate.
		  */
	double _last_frame_time;


	/**
		*  osg::Vec3d _center
		*  \brief camera center
		*/
	osg::Vec3d   _center;

	/**
	 * bool _cameraActive
	 * @brief enable/disable camera functions
	 */

	bool _cameraActive;

	/**
		*  osg::Vec3d _centerArucoTrans
		*  \brief transformation of marker, updated by updateArucoGrafPosition()
		*/
	osg::Vec3d   _centerArucoTrans;

	/**
		*  osg::Quat _rotation
		*  \brief camera rotation
		*/
	osg::Quat    _rotation;

	/**

		*  osg::Quat _rotationHead
		*  \brief another camera rotation according user head
		*/
	osg::Quat    _rotationHead;

	/**
		*  osg::Quat _rotHeadFaceDet
		*  \brief last head rotation from FaceDet
		*/
	osg::Quat	 _rotHeadFaceDet;

	/**
		*  osg::Quat _rotHeadKinect
		*  \brief last head rotation from Kinect
		*/
	osg::Quat	 _rotHeadKinect;

	/**
		*  osg::Quat _rotationAux
		*  \brief Continous rotation when Kinect/FaceDet coords are out of margins
		*/
	osg::Quat    _rotationAux;


	/**
		*  double _distance
		*  \brief current distance from rotation center
		*/
	double       _distance;


	/**
		*  float _trackballSize
		*  \brief size of trackball
		*/
	float        _trackballSize;

	/**
		*  float _zoomDelta
		*  \brief zoom change
		*/
	float        _zoomDelta;


	/**
		*  double speedEpsilon
		*  \brief speed change
		*/
	double    speedEpsilon;

	/**
		*  double maxSpeed
		*  \brief maximum speed
		*/
	double    maxSpeed;

	/**
		*  double forwardSpeed
		*  \brief forward speed
		*/
	double    forwardSpeed;

	/**
		*  double sideSpeed
		*  \brief side speed
		*/
	double    sideSpeed;

	/**
		*  double verticalSpeed
		*  \brief vertical speed
		*/
	double    verticalSpeed;

	/**
		*  double pitchSpeed
		*  \brief pitch speed
		*/
	double	  pitchSpeed;

	/**
		*  double speedDecelerationFactor
		*  \brief speed deceleration factor
		*/
	double    speedDecelerationFactor;


	/**
		*  bool decelerateSideRate
		*  \brief side deceleration rate
		*/
	bool      decelerateSideRate;

	/**
		*  bool decelerateForwardRate
		*  \brief forward deceleration rate
		*/
	bool      decelerateForwardRate;

	/**
		*  bool decelerateVerticalRate
		*  \brief vertical deceleration rate
		*/
	bool      decelerateVerticalRate;

	/**
		*  bool deceleratePitchRate
		*  \brief pitch deceleration rate
		*/
	bool	  deceleratePitchRate;


	/**
		*  double t0
		*  \brief time t0
		*/
	double    t0;

	/**
		*  double dt
		*  \brief time change
		*/
	double    dt;


	/**
		*  double lastDistance
		*  \brief lst distance form center
		*/
	double lastDistance;

	/**
		*  osg::Vec3d lastCenter
		*  \brief last center position
		*/
	osg::Vec3d lastCenter;


	/**
		*  \fn protected  stop
		*  \brief stops camera movement
		*/
	void stop();

	/**
		*  \fn protected  frame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter &)
		*  \brief Computes new frame calculations
		*  \param       ea  event adapter
		*  \param    osgGA::GUIActionAdapter &    action adapter
		*/
	void frame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& );


	/**
		 * @author Autor: David Durcak
		 * @brief setRotationHead Set _rotationHead quaternion to rotate camera according head or kinect
		 * @param x % distance from middle on horizontal axis
		 * @param y % distance from middle on vertical axis
		 * @param distance from camera
		 * @param caller who call this method 0-FaceDet  1-Kinect
		 */
	void setRotationHead( float x, float y, float distance, int caller );

private:
	/**
	 * A pointer to viewer widget from which we can set the field of view
	 * from setProjectionMatrixAsPerspective
	 */
	QOSG::ViewerQT* mViewerWidget;

	/**
			*  Util::ApplicationConfig * appConf
			*  \brief application configuration
			*/
	Util::ApplicationConfig* appConf;


	/**
		*  \fn private  handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles frame event
		*  \param   ea     event adapter
		*  \param   us     action adapter
		*  \return bool true, if handled
		*/
	bool handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	/**
		*  \fn private  handlePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles push event
		*  \param   ea    event adapter
		*  \param   us    action adapter
		*  \return bool true, if handled
		*/
	bool handlePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	/**
		*  \fn private  handleRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles release event
		*  \param  ea     event adapter
		*  \param  us     action adapter
		*  \return bool true, if handled
		*/
	bool handleRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	/**
		*  \fn private  handleScroll(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles scroll event
		*  \param  ea    event adapter
		*  \param  us   action adapter
		*  \return bool true, if handled
		*/
	bool handleScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	/**
		*  \fn private  handleKeyDown( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles keydown event
		*  \param  ea    event adapter
		*  \param  us    action adapter
		*  \return bool true, if handled
		*/
	bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	/**
		*  \fn private  handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles keyup event
		*  \param   ea   event adapter
		*  \param   us   action adapter
		*  \return bool true, if handled
		*/
	bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	bool handleKeyDownGraph( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

	void notifyClients();
	void notifyServer();

	/**
		*  \fn private  computeStandardFrame(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
		*  \brief standard frame computation
		*  \param  ea     event adapter
		*  \param  us     action adapter
		*/
	void computeStandardFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

	/**
		*  bool movingCenter
		*  \brief Flag for camera movement in smooth center view.
		*  If true and view is not centered - camera is moving.
		*  If camera moved through all points(view is centered), this flag is removed.
		*/
	bool movingCenter;

	/**
		*  int pointID
		*  \brief Point order for camera movement in smooth center view.
		*  For get information if camera moved through all points.
		*/
	int pointID;

	/**
		*  osg::Vec3 originalCenter
		*  \brief Initial point - actual center view.
		*  Initial point of camera movement in smooth center view.
		*/
	osg::Vec3 originalCenter;

	/**
		*  osg::Vec3 newCenter
		*  \brief Destination point - center of nodes/edges selection.
		*  Destination point of camera movement in smooth center view.
		*/
	osg::Vec3 newCenter;

	/**
		*  bool movingAutomatically
		*  \brief automatic movement flag
		*/
	bool movingAutomatically;

	/**
		*  bool automaticMovementInitialized
		*  \brief automatic movement initialized flag
		*/
	bool automaticMovementInitialized;

	/**
		*  osg::Vec3d cameraTargetPoint
		*  \brief camera target point
		*/
	osg::Vec3d cameraTargetPoint;

	/**
		*  osg::Vec3d cameraInterestPoint
		*  \brief camera interest point
		*/
	osg::Vec3d cameraInterestPoint;

	/**
		*  osg::Vec3d weightPoint
		*  \brief weigth point
		*/
	osg::Vec3d weightPoint;

	/**
		*  double t1, t2
		*  \brief time
		*/
	double t1, t2;

	/**
		*  double w1[3]
		*  \brief camera position control points weigths
		*/
	double w1[3];

	/**
		*  double w2[3]
		*  \brief camera target point control points weigths
		*/
	double w2[3];

	/**
		*  double targetDistance
		*  \brief target distance of camera from center
		*/
	double targetDistance;

	/**
		*  osg::Vec3d originalEyePosition
		*  \brief eye position at the beginning of movement
		*/
	osg::Vec3d originalEyePosition;

	/**
		*  osg::Vec3d cameraPosition
		*  \brief current camera position
		*/
	osg::Vec3d cameraPosition;

	/**
		*  osg::Vec3d targetPoint
		*  \brief current camera target point
		*/
	osg::Vec3d targetPoint;

	/**
		*  QVector<osg::Vec3d> * cameraPositions
		*  \brief control points for camera position curve
		*/
	QVector<osg::Vec3d>* cameraPositions;

	/**
		*  QVector<osg::Vec3d> * targetPositions
		*  \brief control points for camera target curve
		*/
	QVector<osg::Vec3d>* targetPositions;

	/**
		*  std::list<osg::ref_ptr<Data::Node> > selectedCluster
		*  \brief selected nodes
		*/
	std::list<osg::ref_ptr<Data::Node> > selectedCluster;

	/**
		*  static double EYE_MOVEMENT_SPEED
		*  \brief eye speed
		*/
	static double EYE_MOVEMENT_SPEED;

	/**
		*  static double TARGET_MOVEMENT_SPEED
		*  \brief target speed
		*/
	static double TARGET_MOVEMENT_SPEED;

	/**
		*  static double SCREEN_MARGIN
		*  \brief minimum distance of selected cluster from screen in t=0.5
		*/

	static double SCREEN_MARGIN;

	/**
		*  \fn private  alterWeights(osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster)
		*  \brief alter control point weigths
		*  \param  viewer     viewer
		*  \param  selectedCluster     selected nodes
		*/
	void alterWeights( osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster );

	/**
		*  \fn private  alterCameraTargetPoint(osgViewer::Viewer* viewer)
		*  \brief moves camera target point to see all important nodes
		*  \param  viewer     viewer
		*  \return float distance from center
		*/
	float alterCameraTargetPoint( osgViewer::Viewer* viewer );

	/**
		*  \fn private  initAutomaticMovement(osgViewer::Viewer* viewer)
		*  \brief inits automatic movement
		*  \param  viewer     viewer
		*/
	void initAutomaticMovement( osgViewer::Viewer* viewer );

	/**
		*  \fn private  computeViewMetrics(osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster)
		*  \brief compute viewe metrics
		*  \param  viewer     viewer
		*  \param  selectedCluster     selected nodes
		*/
	void computeViewMetrics( osgViewer::Viewer* viewer, std::list<osg::ref_ptr<Data::Node> > selectedCluster );

	/**
		*  osg::Vec3 lastPosition
		*  \brief last camera position
		*/
	osg::Vec3 lastPosition;

	/**
		*  osg::Vec3 lastTargetPoint
		*  \brief last target point position
		*/
	osg::Vec3 lastTargetPoint;

	Vwr::CoreGraph* coreGraph;

	/**
		 * @author Autor: David Durcak
		 * @brief Update (correction) camera projection according head coordinates
		 * @param x coordinate of head on horizontal axis
		 * @param y coordinate of head on vertical axis
		 * @param distance of head from camera
		 */
	void updateProjectionAccordingFace( const double x, const double y, const double distance );


	/**
		*  \brief cameraCanRot if true camera is rotating normally, else rotation quaternion by mouse is emited away to CoreGraph to rotate graph directly
		*/
	bool _cameraCanRot;

	bool ctrlPressed;
	bool shiftPressed;

};



}

#endif

