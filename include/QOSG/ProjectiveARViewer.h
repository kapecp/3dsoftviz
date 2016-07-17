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

	ProjectiveARViewer( QWidget* parent = 0, const char* name = 0, const QGLWidget* shareWidget = 0, WindowFlags f = 0, QOSG::ProjectiveARWindow* window = 0, osgViewer::Viewer* viewerPerspective = 0, Vwr::CoreGraph* coreGraph = 0 );

	/**
	 * @author Viktor Kostan
	 * @brief createProjectorState - Creates a StateSet that does projective texturing of passed texture.
	 * @param texture - texture used for projective texturing
	 * @param projectorPos - projector position
	 * @param projectorDirection - projector direction
	 * @param projectorFOV - projector field of view
	 * @return Pointer to the StateSet.
	 */
	osg::StateSet* createProjectorState( osg::Texture2D* texture, osg::Vec3 projectorPos, osg::Vec3 projectorDirection, float projectorFOV );
	/**
	 * @author Viktor Kostan
	 * @brief createBase - Creates a geometry with two perpendicular quads and two polygons.
	 * @return Pointer to created geometry.
	 */
	osg::Geode* createBase();

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
	float getViewerFOV()
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
	float getProjectorFOV()
	{
		return projectorFOV;
	}
	osg::Vec3 getGraphPos()
	{
		return graphPos;
	}
	float getGraphRadius()
	{
		return graphRadius;
	}
	bool getUseGraph()
	{
		return useGraph;
	}
	// setters
	void setViewerPos( float x, float y, float z )
	{
		viewerPos.set( x, y, z );
	}
	void setViewerDir( float x, float y, float z )
	{
		viewerDir.set( x, y, z );
	}
	void setViewerFOV( float fov )
	{
		viewerFOV = fov;
	}
	void setProjectorPos( float x, float y, float z )
	{
		projectorPos.set( x, y, z );
	}
	void setProjectorDir( float x, float y, float z )
	{
		projectorDir.set( x, y, z );
	}
	void setProjectorFOV( float fov )
	{
		projectorFOV = fov;
	}
	void setGraphPos( float x, float y, float z )
	{
		graphPos.set( x, y, z );
	}
	void setGraphRadius( float radius )
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
	void setViewerPosByFaceDetection( float x, float y, float distance );

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

	osg::Vec3 up = osg::Vec3( 0.0f, 1.0f, 0.0f );
	QOSG::ProjectiveARWindow* window;

	osg::Camera* renderCamera;

	/**
		*  osg::Geode* base
		*  \brief camera that renders model to texture
		*/
	osg::Geode* base;


	/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
	Util::ApplicationConfig* appConf;

	osg::Vec3 projectorPos;
	osg::Vec3 projectorDir;
	float projectorFOV;

	osg::Vec3 viewerPos;
	osg::Vec3 viewerDir;
	float viewerFOV;

	osg::Vec3 graphPos;
	float graphRadius;

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
	osg::Matrixd createFrustumForSphere( float radius, float distance );
};
}

#endif
