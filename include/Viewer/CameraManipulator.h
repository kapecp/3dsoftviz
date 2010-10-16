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

#include <osgGA/CameraManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/Quat>

#include "Util/ApplicationConfig.h"

namespace Vwr{

	/**
	*  \class CameraManipulator
	*  \brief Camera manipulator class. Handles all camera movements.
	*  \author Michal Paprcka
	*  \date 29. 4. 2010
	*
	*	This class is basically extended TrackballManipulator class with new functionality.
	*/
	class CameraManipulator : public osgGA::CameraManipulator
{
    public:
		CameraManipulator();

        virtual const char* className() const { return "Trackball"; }

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        virtual void setByMatrix(const osg::Matrixd& matrix);

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }

        /** get the position of the manipulator as 4x4 Matrix.*/
        virtual osg::Matrixd getMatrix() const;

        /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
        virtual osg::Matrixd getInverseMatrix() const;

        /** Get the FusionDistanceMode. Used by SceneView for setting up stereo convergence.*/
        virtual osgUtil::SceneView::FusionDistanceMode getFusionDistanceMode() const { return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE; }

        /** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
        virtual float getFusionDistanceValue() const { return _distance; }

        /** Attach a node to the manipulator. 
            Automatically detaches previously attached node.
            setNode(NULL) detaches previously nodes.
            Is ignored by manipulators which do not require a reference model.*/
        virtual void setNode(osg::Node*);

        /** Return node if attached.*/
        virtual const osg::Node* getNode() const;

        /** Return node if attached.*/
        virtual osg::Node* getNode();

        /** Move the camera to the default position. 
            May be ignored by manipulators if home functionality is not appropriate.*/
        virtual void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
        virtual void home(double time = 0);
        
        /** Start/restart the manipulator.*/
        virtual void init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

        /** handle events, return true if handled, false otherwise.*/
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

        /** Get the keyboard and mouse usage of this manipulator.*/
        virtual void getUsage(osg::ApplicationUsage& usage) const;


        /** set the minimum distance (as ratio) the eye point can be zoomed in towards the
            center before the center is pushed forward.*/        
        void setMinimumZoomScale(double minimumZoomScale) { _minimumZoomScale=minimumZoomScale; }

        /** get the minimum distance (as ratio) the eye point can be zoomed in */
        double getMinimumZoomScale() const { return _minimumZoomScale; }

        /** set the mouse scroll wheel zoom delta.
          * Range -1.0 to +1.0,  -ve value inverts wheel direction and zero switches off scroll wheel. */
        void setScroolWheelZoomDelta(double zoomDelta) { _zoomDelta = zoomDelta; }

        /** get the mouse scroll wheel zoom delta. */
        double getScroolWheelZoomDelta() const { return _zoomDelta; }

        /** Set the center of the trackball. */
        void setCenter(const osg::Vec3d& center) { _center = center; }

        /** Get the center of the trackball. */
        const osg::Vec3d& getCenter() const { return _center; }

        /** Set the rotation of the trackball. */
        void setRotation(const osg::Quat& rotation) { _rotation = rotation; }

        /** Get the rotation of the trackball. */
        const osg::Quat& getRotation() const { return _rotation; }

        /** Set the distance of the trackball. */
        void setDistance(double distance) { _distance = distance; }

        /** Get the distance of the trackball. */
        double getDistance() const { return _distance; }

        /** Set the size of the trackball. */
        void setTrackballSize(float size);

        /** Get the size of the trackball. */
        float getTrackballSize() const { return _trackballSize; }

        /** Set the 'allow throw' flag. Releasing the mouse button while moving the camera results in a throw. */
        void setAllowThrow(bool allowThrow) { _allowThrow = allowThrow; }

        /** Returns true if the camera can be thrown, false otherwise. This defaults to true. */
        bool getAllowThrow() const { return _allowThrow; }


		/**
		*  \fn inline public  setMaxSpeed(float speed)
		*  \brief Sets maximum camera speed
		*  \param     speed    maximum speed 
		*/
		void setMaxSpeed(float speed) { this->maxSpeed = speed; }

		/**
		*  \fn inline public  getMaxSpeed
		*  \brief Returns maximum camera speed
		*  \return float maximum speed
		*/
		float getMaxSpeed() { return maxSpeed; }

    protected:

        virtual ~CameraManipulator();

        /** Reset the internal GUIEvent stack.*/
        void flushMouseEventStack();
        /** Add the current mouse GUIEvent to internal stack.*/
        void addMouseEvent(const osgGA::GUIEventAdapter& ea);

        void computePosition(const osg::Vec3& eye,const osg::Vec3& lv,const osg::Vec3& up);

        /** For the give mouse movement calculate the movement of the camera.
            Return true is camera has moved and a redraw is required.*/
        bool calcMovement();
        
        void trackball(osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y);
        float tb_project_to_sphere(float r, float x, float y);


        /** Check the speed at which the mouse is moving.
            If speed is below a threshold then return false, otherwise return true.*/
        bool isMouseMoving();

        // Internal event stack comprising last two mouse events.

		/**
		*  osg::ref_ptr _ga_t1
		*  \brief Last event
		*/
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;

		/**
		*  osg::ref_ptr _ga_t0
		*  \brief Current event
		*/
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;


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
		*  osg::Quat _rotation
		*  \brief camera rotation
		*/
		osg::Quat    _rotation;

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
		void frame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter &);

		private:


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
		bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		*  \fn private  handlePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles push event
		*  \param   ea    event adapter
		*  \param   us    action adapter
		*  \return bool true, if handled
		*/
		bool handlePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		*  \fn private  handleRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles release event
		*  \param  ea     event adapter
		*  \param  us     action adapter
		*  \return bool true, if handled
		*/
		bool handleRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		*  \fn private  handleScroll(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles scroll event
		*  \param  ea    event adapter
		*  \param  us   action adapter
		*  \return bool true, if handled
		*/
		bool handleScroll(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		*  \fn private  handleKeyDown( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles keydown event
		*  \param  ea    event adapter
		*  \param  us    action adapter
		*  \return bool true, if handled
		*/
		bool handleKeyDown( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter& us);

		/**
		*  \fn private  handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
		*  \brief Handles keyup event
		*  \param   ea   event adapter
		*  \param   us   action adapter
		*  \return bool true, if handled
		*/
		bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	};

}

#endif
