/**
*  CoreGraph.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_CORE_GRAPH_DEF
#define VIEWER_CORE_GRAPH_DEF 1

#include <osg/ref_ptr>

#include "Viewer/RestrictionVisualizationsGroup.h"
#include "Viewer/RestrictionManipulatorsGroup.h"
#include "Viewer/EdgeGroup.h"
#include "Viewer/NodeGroup.h"
#include "Viewer/BrowserGroup.h"
#include "Data/Edge.h"
#include "Data/Node.h"

#include "Data/Cluster.h"

#include <osgManipulator/Translate2DDragger>

#include <QMap>
#include <QLinkedList>
#include <QSharedPointer>
#include <QObject>
#include <QTime>

#include "OsgQtBrowser/QWebViewImage.h"

#include <osgShadow/ShadowedScene>

namespace Data {
class Graph;
}

namespace Vwr {
class EdgeGroup;
//class CameraStream;
//	class NodeGroup;

osg::ref_ptr<osg::AutoTransform> getSphere( qlonglong id, osg::Vec3 position, float radius, osg::Vec4 color );
osg::ref_ptr<osg::AutoTransform> getCube( qlonglong id, osg::Vec3 position, float width, osg::Vec4 color );
osg::ref_ptr<osg::AutoTransform> getCone( qlonglong id, osg::Vec3 position, float radius, osg::Vec4 color );
osg::ref_ptr<osg::AutoTransform> getCylinder( qlonglong id, osg::Vec3 position, float radius, osg::Vec4 color );
osg::Geode* test();
osg::Vec4 getNewColor( int colorCounter );
osg::Vec3f getMidPoint( QSet<Data::Node*> nodes );
float getRadius( QSet<Data::Node*> nodes, osg::Vec3f midPoint );
}

namespace Util {
class ApplicationConfig;
}

namespace OpenCV {
#ifdef OPENCV_FOUND
class CameraStream;
#endif

}

namespace Vwr {
/*!
	 * \brief
	 * Trieda vykreslujuca aktualny graf.
	 *
	 * \author
	 * Michal Paprcka
	 * \version
	 * 3.0
	 * \date
	 * 7.12.2009
	 */
class CoreGraph : public QObject
{
	Q_OBJECT

public:
	/*!
		 *
		 * \param in_nodes
		 * Zoznam uzlov
		 *
		 * \param in_edges
		 * Zoznam hran
		 *
		 * \param in_types
		 * Zoznam typov
		 *
		 * Konstruktor triedy.
		 *
		 */
	CoreGraph( Data::Graph* graph = 0, osg::ref_ptr<osg::Camera> camera = 0 );
	/*!
		 *
		 *
		 * Destruktor.
		 *
		 */
	~CoreGraph( void );


	/**
		*  \fn public  reload(Data::Graph * graph = 0)
		*  \brief
		*  \param   graph
		*/
	void reload( Data::Graph* graph = 0 );

	/**
		*  \fn public  reloadConfig
		*  \brief
		*/
	void reloadConfig();


	/*!
		 *
		 *
		 * Aktualizuje pozicie uzlov a hran.
		 *
		 */
	void update();


	/**
		*  \fn inline public  getCustomNodeList
		*  \brief
		*  \return QLinkedList<osg::ref_ptr<osg::Node> > *
		*/
	QLinkedList<osg::ref_ptr<osg::Node> >* getCustomNodeList()
	{
		return &customNodeList;
	}


	/*!
		 * \brief
		 * Metoda, ktora vracia odkaz na koren grafu.
		 *
		 * \returns
		 * vrati odkaz na scenu
		 *
		 */
	osg::ref_ptr<osg::Group> const getScene()
	{
		return root;
	}


	/**
		*  \fn inline public  setCamera(osg::ref_ptr<osg::Camera> camera)
		*  \brief Sets current viewing camera to all edges
		*  \param     camera     current camera
		*/
	void setCamera( osg::ref_ptr<osg::Camera> camera )
	{
		this->camera = camera;

		QMapIterator<qlonglong, osg::ref_ptr<Data::Edge> > i( *in_edges );


		while ( i.hasNext() ) {
			i.next();
			i.value()->setCamera( camera );
		}
	}

	osg::ref_ptr<osg::Camera> getCamera()
	{
		return camera;
	}

	osg::ref_ptr<osg::AutoTransform> dodecahedron( qlonglong id, osg::Vec3 midpoint, float radius, osg::Vec4 color );

	/**
		*  \fn public  setEdgeLabelsVisible(bool visible)
		*  \brief If true, edge labels will be visible
		*  \param      visible     edge label visibility
		*/
	void setEdgeLabelsVisible( bool visible );

	/**
		*  \fn public  setNodeLabelsVisible(bool visible)
		*  \brief If true, node labels will be visible
		*  \param       visible     node label visibility
		*/
	void setNodeLabelsVisible( bool visible );


	/**
		*  \fn inline public constant  getNodesFreezed
		*  \brief True, if nodes are freezed
		*  \return bool nodes freeze state
		*/
	bool getNodesFreezed() const
	{
		return nodesFreezed;
	}

	/**
		*  \fn inline public  setNodesFreezed(bool val)
		*  \brief Sets nodes freeze state
		*  \param      val     nodes freeze state
		*/
	void setNodesFreezed( bool val );

	bool getInterpolationDenied() const
	{
		return interpolationDenied;
	}
	void setInterpolationDenied( bool val )
	{
		this->interpolationDenied = val;
	}

	void setClustersOpacityAutomatic( bool automatic );
	void setClustersOpacitySelected( bool selected );
	void setClustersOpacity( double opacity );
	void setClustersShapeBoundary( int value );
	void setCameraManipulator( CameraManipulator* cameraManipulator );
	void createClusterGroup( QMap<qlonglong, osg::ref_ptr<Data::Cluster> > clusters );

	Vwr::NodeGroup* getNodesGroup()
	{
		return nodesGroup;
	}
	Vwr::NodeGroup* getMetaNodesGroup()
	{
		return qmetaNodesGroup;
	}
	Vwr::EdgeGroup* getEdgesGroup()
	{
		return edgesGroup;
	}
	Vwr::EdgeGroup* getMetaEdgesGroup()
	{
		return qmetaEdgesGroup;
	}
	Vwr::BrowserGroup* getBrowsersGroup()
	{
		return browsersGroup;
	}

#ifdef OPENCV_FOUND
	OpenCV::CameraStream* getCameraStream() const;
#endif

	//jurik
	/**
		*  \fn public  set shadow technique to shadowMap
		*  \brief
		*/
	void turnOnShadows();

	/**
		*  \fn public  set shadow technique to NULL
		*  \brief
		*/
	void turnOffShadows();

	void turnOnBase();
	void turnOffBase();
	void createBase();
	float compare(float a, float b);
	//*****

public slots:

	/**
		 * @author Autor: David Durcak
		 * @brief updateGraphRotByAruco Update graphRotTransf MatrixTransform node for rotating a graph by rotation of marker
		 * @param quat Quaternion that desribe rotation of marker
		 */
	void updateGraphRotByAruco( const osg::Quat quat );

	void updateGraphPosAndRotByAruco( const osg::Quat quat, osg::Vec3d pos );

	void translateGraph( osg::Vec3d pos );

	/**
		 * @author Autor: David Durcak
		 * @brief updateGraphRotByMouse Update graphRotTransf MatrixTransform node for rotating a graph by mouse
		 * @param quat Quaternion that desribe rotation of mouse
		 */
	void updateGraphRotByMouse( const osg::Quat quat );

	/**
		 * @author Autor: David Durcak
		 * @brief updateGraphRotByFaceDet Update graphRotTransf MatrixTransform node for rotating a graph by face detection
		 * @param quat Quaternion that desribe rotation of face
		 */
	void updateGraphRotByFaceDet( const osg::Quat quat );

	/**
		 * @brief setNodeType Set representation of nodes
		 */
	void setNodeVisual( unsigned int index );

	/**
		 * @brief setEdgeType Set representation of edges
		 */
	void setEdgeVisual( int index );

private:

	/**
		 * @author Autor: David Durcak
		 * @brief computeGraphRotTransf Update graphRotTransf MatrixTransform node for rotating a graph
		 */
	void computeGraphRotTransf();

	void addTranslateToGraphRotTransf( osg::Vec3d pos );

	/**
		*  Vwr::EdgeGroup * edgesGroup
		*  \brief edge group
		*/
	Vwr::EdgeGroup* edgesGroup;

	/**
		*  Vwr::EdgeGroup * qmetaEdgesGroup
		*  \brief metaedge group
		*/
	Vwr::EdgeGroup* qmetaEdgesGroup;

	Vwr::NodeGroup* refNodeGroup;


	/**
		*  Vwr::NodeGroup * nodesGroup
		*  \brief node group
		*/
	Vwr::NodeGroup* nodesGroup;

	/**
		*  Vwr::NodeGroup * qmetaNodesGroup
		*  \brief metanode group
		*/
	Vwr::NodeGroup* qmetaNodesGroup;

	/**
		 * \brief Contains part of OSG visualizing current restrictions.
		 */
	QSharedPointer<Vwr::RestrictionVisualizationsGroup> restrictionVisualizationsGroup;

	/**
				 * \brief Encapsulates part of the OSG including restriction manipulations.
				 */
	QSharedPointer<Vwr::RestrictionManipulatorsGroup> restrictionManipulatorsGroup;

	/**
		*  Vwr::BrowserGroup * browsersGroup
		*  \brief browser group
		*/
	Vwr::BrowserGroup* browsersGroup;

	/**
		*  Data::Graph * graph
		*  \brief current graph
		*/
	Data::Graph* graph;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * in_nodes
		*  \brief graph nodes map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* in_nodes;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* in_refnodes;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * in_edges
		*  \brief graph edges map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* in_edges;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * qmetaNodes
		*  \brief graph metanodes map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* qmetaNodes;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * qmetaEdges
		*  \brief graph metaedges map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* qmetaEdges;

	/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
	Util::ApplicationConfig* appConf;

//	/**
//		*  \fn private  initEdgeLabels
//		*  \brief inits edge labels
//		*  \return osg::ref_ptr
//		*/
	osg::ref_ptr<osg::Group> initEdgeLabels();

	/**
		*  \fn private  initCustomNodes
		*  \brief inits custom nodes
		*  \return osg::ref_ptr
		*/
	osg::ref_ptr<osg::Group> initCustomNodes();


	/**
		*  \fn private  createBackground
		*  \brief creates background (skybox, skynoise, video3d, video2dOrtho or nothing )
		*  \return osg::ref_ptr node
		*/
	osg::ref_ptr<osg::Node> createBackground();

	/**
		*  \fn private  createSkyNoiseBox
		*  \brief creates sky noise globe box
		*  \return osg::ref_ptr node
		*/
	osg::ref_ptr<osg::Node> createSkyNoiseBox();

#ifdef OPENCV_FOUND
	/**
		*  \fn private  createTextureBackground
		*  \brief creates video background as 3d rectangle. It uses image, that can be dynamicly changed
		*  \return osg::ref_ptr node
		*/
	osg::ref_ptr<osg::Node> createTextureBackground();

	/**
		*  \fn private  createOrtho2dBackground
		*  \brief creates video background rectangle in ortho2d mode. It uses image, that can be dynamicly changed
		*  \return osg::ref_ptr node
		*/
	osg::ref_ptr<osg::Node> createOrtho2dBackground();
#endif
	/**
		*  osg::ref_ptr camera
		*  \brief current viewing camera
		*/
	osg::ref_ptr<osg::Camera> camera;

	/**
		*  osg::ref_ptr root
		*  \brief root node
		*/
	osg::ref_ptr<osg::Group> root;

	osg::ref_ptr<osg::Group> testGroup;

	osg::ref_ptr<osg::Group> test2();

	/**
		*  osg::ref_ptr graphGroup
		*  \brief graphGroup node
		*/
	osg::ref_ptr<osg::Group> graphGroup;

	osg::ref_ptr<osg::Group> manipulatorGroup;

	/**
		*  osg::ref_ptr graphRotTransf
		*  \brief graphRotTransf node for rotation graph according marker rotation from aruco
		*/
	osg::ref_ptr<osg::MatrixTransform> graphRotTransf;

	/**
		*  osg::Quat mRotAruco
		*  \brief mRotAruco quaternion of rotation by Aruco
		*/
	osg::Quat mRotAruco;

	/**
		*  osg::Quat mRotMouse
		*  \brief mRotMouse quaternion of rotation by mouse
		*/
	osg::Quat mRotMouse;

	/**
		*  osg::Quat mRotFaceDet
		*  \brief mRotFaceDet quaternion of rotation by face
		*/
	osg::Quat mRotFaceDet;

	/**
		*  bool nodesFreezed
		*  \brief true, if nodes are freezed
		*/
	bool nodesFreezed;

	/**
		*  bool interpolationDenied
		*  \brief true, if interpolation is denied
		*/
	bool interpolationDenied;

	/**
		*  QLinkedList<osg::ref_ptr<osg::Node> > customNodeList
		*  \brief list of custom nodes
		*/
	QLinkedList<osg::ref_ptr<osg::Node> > customNodeList;

	/**
		*  \fn private  synchronize
		*  \brief Synchronizes nodes and edges
		*/
	void synchronize();

	/**
		*  \fn private  cleanUp
		*  \brief Cleans up memory
		*/
	void cleanUp();

	/**
		*  int backgroundPosition
		*  \brief background node position
		*/
	unsigned int backgroundPosition;

	/**
		*  int nodesPosition
		*  \brief nodes group position
		*/
	unsigned int nodesPosition;

	/**
		*  int edgesPosition
		*  \brief edges group position
		*/
	unsigned int edgesPosition;

	/**
		*  int qmetaNodesPosition
		*  \brief metanodes group position
		*/
	unsigned int qmetaNodesPosition;

	/**
		*  int qmetaEdgesPosition
		*  \brief metaedges groups position
		*/
	unsigned int qmetaEdgesPosition;

	/**
		*  int labelsPosition
		*  \brief labels group position
		*/
	unsigned int labelsPosition;

	/**
		 * \brief Index of restrictionVisualisationsGroup in the root group.
		 */
	unsigned int restrictionVisualizationsPosition;

	/**
		 * \brief Index of restrictionManipulatorsGroup in the root group.
		 */
	unsigned int restrictionManipulatorsPosition;

	/**
		*  int groupsPosition
		*  \brief browsers group position
		*/
	unsigned int browsersPosition;

	/**
		*  int customNodesPosition
		*  \brief custom nodes group position
		*/
	unsigned int customNodesPosition;

	int prevTime;

#ifdef OPENCV_FOUND
	osg::ref_ptr<OpenCV::CameraStream> mCameraStream;
#endif

	bool clustersOpacityAutomatic;
	bool clustersOpacitySelected;
	double clustersOpacity;
	int clustersShapeBoundary;

	osg::ref_ptr<osg::Group> clustersGroup;

	CameraManipulator* cameraManipulator;

	void updateClustersCoords();

	double computeOpacity( osg::Vec3 clusterPosition );
	bool cameraInsideSphere( osg::Vec3d midPoint, float radius );
	bool cameraInsideCube( osg::Vec3d lowerPoint, osg::Vec3d upperPoint );

	osgManipulator::Translate2DDragger* manipulator;

	//jurik
	/**
		*  osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene
		*  \brief node for shadows definition
		*/
		osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene;

	/**
		* osg::Geode* baseGeode
		*  \brief node base
		*/
	osg::ref_ptr<osg::Geode> baseGeode;
	osg::ref_ptr<osg::PositionAttitudeTransform> baseTransform;

	//*****
};
}

#endif
