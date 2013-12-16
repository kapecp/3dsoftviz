/**
*  CoreGraph.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_CORE_GRAPH_DEF
#define VIEWER_CORE_GRAPH_DEF 1

#include <math.h>

#include <osg/ref_ptr>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/Depth>
#include <osg/TextureCubeMap>
#include <osg/AutoTransform>

#include <QMap>
#include <QLinkedList>
#include <QSharedPointer>

#include "Viewer/PerlinNoiseTextureGenerator.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/DataHelper.h"
#include "Viewer/SkyTransform.h"
#include "Viewer/TextureWrapper.h"
#include "Viewer/EdgeGroup.h"
#include "Viewer/NodeGroup.h"
#include "Viewer/RestrictionVisualizationsGroup.h"
#include "Viewer/RestrictionManipulatorsGroup.h"

#include "Util/ApplicationConfig.h"
#include "Data/Edge.h"
#include "Data/Node.h"
#include "Data/Graph.h"

namespace Vwr
{
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
class CoreGraph
{
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
	CoreGraph(Data::Graph * graph = 0, osg::ref_ptr<osg::Camera> camera = 0);
	/*!
		 *
		 *
		 * Destruktor.
		 *
		 */
	~CoreGraph(void);


	/**
		*  \fn public  reload(Data::Graph * graph = 0)
		*  \brief
		*  \param   graph
		*/
	void reload(Data::Graph * graph = 0);

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
	QLinkedList<osg::ref_ptr<osg::Node> > * getCustomNodeList() { return &customNodeList; }


	/*!
		 * \brief
		 * Metoda, ktora vracia odkaz na koren grafu.
		 *
		 * \returns
		 * vrati odkaz na scenu
		 *
		 */
	osg::ref_ptr<osg::Group> const getScene()  { return root; }


	/**
		*  \fn inline public  setCamera(osg::ref_ptr<osg::Camera> camera)
		*  \brief Sets current viewing camera to all edges
		*  \param     camera     current camera
		*/
	void setCamera(osg::ref_ptr<osg::Camera> camera)
	{
		this->camera = camera;

		QMapIterator<qlonglong, osg::ref_ptr<Data::Edge> > i(*in_edges);

		while (i.hasNext())
		{
			i.next();
			i.value()->setCamera(camera);
		}
	}

	osg::ref_ptr<osg::Camera> getCamera() { return camera; }


	/**
		*  \fn public  setEdgeLabelsVisible(bool visible)
		*  \brief If true, edge labels will be visible
		*  \param      visible     edge label visibility
		*/
	void setEdgeLabelsVisible(bool visible);

	/**
		*  \fn public  setNodeLabelsVisible(bool visible)
		*  \brief If true, node labels will be visible
		*  \param       visible     node label visibility
		*/
	void setNodeLabelsVisible(bool visible);


	/**
		*  \fn inline public constant  getNodesFreezed
		*  \brief True, if nodes are freezed
		*  \return bool nodes freeze state
		*/
	bool getNodesFreezed() const { return nodesFreezed; }

	/**
		*  \fn inline public  setNodesFreezed(bool val)
		*  \brief Sets nodes freeze state
		*  \param      val     nodes freeze state
		*/
	void setNodesFreezed(bool val)
	{
		this->nodesFreezed = val;
		nodesGroup->freezeNodePositions();
		qmetaNodesGroup->freezeNodePositions();
	}

	Vwr::NodeGroup * getNodesGroup() { return nodesGroup; }
	Vwr::NodeGroup * getMetaNodesGroup() { return qmetaNodesGroup; }
	Vwr::EdgeGroup * getEdgesGroup() { return edgesGroup; }
	Vwr::EdgeGroup * getMetaEdgesGroup() { return qmetaEdgesGroup; }

private:

	/**
		*  Vwr::EdgeGroup * edgesGroup
		*  \brief edge group
		*/
	Vwr::EdgeGroup * edgesGroup;

	/**
		*  Vwr::EdgeGroup * qmetaEdgesGroup
		*  \brief metaedge group
		*/
	Vwr::EdgeGroup * qmetaEdgesGroup;


	/**
		*  Vwr::NodeGroup * nodesGroup
		*  \brief node group
		*/
	Vwr::NodeGroup * nodesGroup;

	/**
		*  Vwr::NodeGroup * qmetaNodesGroup
		*  \brief metanode group
		*/
	Vwr::NodeGroup * qmetaNodesGroup;

	/**
		 * \brief Contains part of OSG visualizing current restrictions.
		 */
	QSharedPointer<Vwr::RestrictionVisualizationsGroup> restrictionVisualizationsGroup;

	/**
				 * \brief Encapsulates part of the OSG including restriction manipulations.
				 */
	QSharedPointer<Vwr::RestrictionManipulatorsGroup> restrictionManipulatorsGroup;

	/**
		*  Data::Graph * graph
		*  \brief current graph
		*/
	Data::Graph * graph;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * in_nodes
		*  \brief graph nodes map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Node> > *in_nodes;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * in_edges
		*  \brief graph edges map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> > *in_edges;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * qmetaNodes
		*  \brief graph metanodes map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Node> > *qmetaNodes;

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * qmetaEdges
		*  \brief graph metaedges map
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> > *qmetaEdges;

	/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
	Util::ApplicationConfig* appConf;

	/**
		*  \fn private  initEdgeLabels
		*  \brief inits edge labels
		*  \return osg::ref_ptr
		*/
	osg::ref_ptr<osg::Group> initEdgeLabels();

	/**
		*  \fn private  initCustomNodes
		*  \brief inits custom nodes
		*  \return osg::ref_ptr
		*/
	osg::ref_ptr<osg::Group> initCustomNodes();

	/**
		*  \fn private  createSkyBox
		*  \brief creates sky
		*  \return osg::ref_ptr skybox node
		*/
	osg::ref_ptr<osg::Node> createSkyBox();

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

	/**
		*  bool nodesFreezed
		*  \brief true, if nodes are freezed
		*/
	bool nodesFreezed;

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
	int backgroundPosition;

	/**
		*  int nodesPosition
		*  \brief nodes group position
		*/
	int nodesPosition;

	/**
		*  int edgesPosition
		*  \brief edges group position
		*/
	int edgesPosition;

	/**
		*  int qmetaNodesPosition
		*  \brief metanodes group position
		*/
	int qmetaNodesPosition;

	/**
		*  int qmetaEdgesPosition
		*  \brief metaedges groups position
		*/
	int qmetaEdgesPosition;

	/**
		*  int labelsPosition
		*  \brief labels group position
		*/
	int labelsPosition;

	/**
		 * \brief Index of restrictionVisualisationsGroup in the root group.
		 */
	int restrictionVisualizationsPosition;

	/**
				 * \brief Index of restrictionManipulatorsGroup in the root group.
				 */
	int restrictionManipulatorsPosition;

	/**
		*  int customNodesPosition
		*  \brief custom nodes group position
		*/
	int customNodesPosition;

	int prevTime;
};
}

#endif
