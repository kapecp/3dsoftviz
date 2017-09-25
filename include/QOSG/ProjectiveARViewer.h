/**
*  ProjectiveARViewer.h
*  Projekt 3DVisual
*/
#ifndef QOSG_PROJECTIVE_AR_VIEWER_DEF
#define QOSG_PROJECTIVE_AR_VIEWER_DEF 1

#include "QOSG/AdapterWidget.h"
#include <osgViewer/ViewerBase>
#include <osgViewer/ViewerEventHandlers>
#include <qtimer.h>
#include <QObject>

/*!
* \brief
* Widget viewing OSG scene for projective augmented reality.
*
* \author
* Viktor Kostan
* \version
* 1.0
* \date
* 21.4.2016
*/


namespace Vwr {
class PickHandler;
class CoreGraph;
}

namespace Util {
class ApplicationConfig;
}


namespace Vwr {
class CameraManipulator;
class CoreGraph;
}


namespace QOSG {
class ProjectiveARWindow;
/**
	*  \class ProjectiveARViewer
	*  \brief
	*  \author Viktor Kostan
	*  \date 21. 4. 2016
	*/
class ProjectiveARViewer : public AdapterWidget, public osgViewer::Viewer
{
	Q_OBJECT
#define SCENE_MAX_SIZE (1000)

public:

	/*!
		*
		* \param parent
		* Rodic widgetu.
		*
		* \param name
		* Meno widgetu.
		*
		* \param shareWidget
		* Zdielanie widgetu.
		*
		* \param f
		* Znacky pre vytvaranie okna.
		*
		* \param cg
		* CoreGraf, ktory sa bude vykreslovat.
		*
		*
		* Vytvori widget, ktory dokaze zobrazit OSG grafy.
		*
		*/

	ProjectiveARViewer( const QGLFormat& format, QWidget* parent = 0, const char* name = 0, const QGLWidget* shareWidget = 0, WindowFlags f = 0, QOSG::ProjectiveARWindow* window = 0, osgViewer::Viewer* viewerPerspective = 0, Vwr::CoreGraph* coreGraph = 0 );

	/**
	 * @author Viktor Kostan
	 * @brief createProjectorState - Creates a StateSet that does projective texturing of passed texture.
	 * @param texture - texture used for projective texturing
	 * @param projectorPos - projector position
	 * @param projectorDirection - projector direction
	 * @param projectorFOV - projector field of view
	 * @return Pointer to the StateSet.
	 */
	osg::StateSet* createProjectorState( osg::Texture2D* texture, osg::Vec3d projectorPos, osg::Vec3d projectorDirection, double projectorFOV );
	/**
	 * @author Viktor Kostan
	 * @brief createBase - Creates a geometry with two perpendicular quads and two polygons.
	 * @return Pointer to created geometry.
	 */
#if 0
	//#ifdef PCL_FOUND
	osg::Node* createBase();
#else
	osg::Geode* createBase();
#endif

	/**
		  *  \fn inline public  reloadConfig
		  *  \brief Reloads configuration
		  */
	void reloadConfig();

	/**
		*  void updateScene()
		*  \brief update projector's and viewer's position, direction and field of view
		*/
	void updateScene();


	// getters
	osg::Vec3 getViewerPos()
	{
		return viewerPos;
	}
	osg::Vec3 getViewerDir()
	{
		return viewerDir;
	}
	double getViewerFOV()
	{
		return viewerFOV;
	}
	osg::Vec3 getProjectorPos()
	{
		return projectorPos;
	}
	osg::Vec3 getProjectorDir()
	{
		return projectorDir;
	}
	double getProjectorFOV()
	{
		return projectorFOV;
	}
	osg::Vec3 getGraphPos()
	{
		return graphPos;
	}
	double getGraphRadius()
	{
		return graphRadius;
	}
	bool getUseGraph()
	{
		return useGraph;
	}
	// setters
	void setViewerPos( double x, double y, double z )
	{
		viewerPos.set( x, y, z );
	}
	void setViewerDir( double x, double y, double z )
	{
		viewerDir.set( x, y, z );
	}
	void setViewerFOV( double fov )
	{
		viewerFOV = fov;
	}
	void setProjectorPos( double x, double y, double z )
	{
		projectorPos.set( x, y, z );
	}
	void setProjectorDir( double x, double y, double z )
	{
		projectorDir.set( x, y, z );
	}
	void setProjectorFOV( double fov )
	{
		projectorFOV = fov;
	}
	void setGraphPos( double x, double y, double z )
	{
		graphPos.set( x, y, z );
	}
	void setGraphRadius( double radius )
	{
		graphRadius = radius;
	}
	void setUseGraph( bool use )
	{
		useGraph = use;
	}

public slots:
	/**
	  * @author Autor: Viktor Košťan
	  * @brief setViewerPosByFaceDetection Sets viewer's position based on [x,y] coords of eye and distance from camera
	  */
	void setViewerPosByFaceDetection( double x, double y, double distance );

protected:


	/**
		*  QTimer _timer
		*  \brief
		*/
	QTimer _timer;

	/**
		*  Vwr::CoreGraph * coreGraph
		*  \brief core graph
		*/
	Vwr::CoreGraph* coreGraph;

	/**
		*  osgViewer::Viewer * viewerPerspective
		*  \brief source viewer
		*/
	osgViewer::Viewer* viewerPerspective;

	/**
		*  \fn inline protected virtual  paintGL
		*  \brief Paints new frame
		*/

	virtual void paintGL();



private:

	osg::Vec3d up = osg::Vec3d( 0.0, 1.0, 0.0 );
	QOSG::ProjectiveARWindow* window;

	osg::Camera* renderCamera;

	/**
		*  osg::Geode* base
		*  \brief camera that renders model to texture
		*/
	osg::Node* base;


	/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
	Util::ApplicationConfig* appConf;

	osg::Vec3d projectorPos;
	osg::Vec3d projectorDir;
	double projectorFOV;

	osg::Vec3d viewerPos;
	osg::Vec3d viewerDir;
	double viewerFOV;

	osg::Vec3d graphPos;
	double graphRadius;

	// option to use graphPos and graphRadius, to update render camera
	// else use viewerPerspective's camera
	bool useGraph;

	void updateRenderCamera();
	void updateViewer();
	void updateProjector();
	/**
		*  osg::Group* createProjectorScene()
		*  \brief function that creates projector scene, based on viewerPos, viewerDir, viewerFOV and viewerPerspective
		*/
	osg::Group* createProjectorScene();
	osg::Matrixd createFrustumForSphere( double radius, double distance );
};
}

#endif
