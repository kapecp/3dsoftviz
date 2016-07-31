/**
*  NodeGroup.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_NODEWRAPPER_DEF
#define VIEWER_NODEWRAPPER_DEF 1

#include "Data/Node.h"

#include <osg/ref_ptr>
#include <osg/AutoTransform>
#include <osg/Group>

#include <QMap>
#include <QSet>


namespace Util {
class ApplicationConfig;
}


namespace Vwr {
/**
	*  \class NodeGroup
	*  \brief
	*  \author Michal Paprcka
	*  \date 29. 4. 2010
	*/
class NodeGroup
{
public:

	/**
		*  \fn public constructor  NodeGroup(QMap<qlonglong, osg::ref_ptr<Data::Node> > *nodes)
		*  \brief Creates node group
		*  \param  nodes    nodes to wrap
		*/
	NodeGroup( QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes );

	/**
		*  \fn public destructor  ~NodeGroup
		*  \brief destructor
		*/
	~NodeGroup( void );


	/**
		*  \fn public  updateNodeCoordinates(float interpolationSpeed)
		*  \brief Updates nodes current position with given interpolation speed
		*  \param       interpolationSpeed
		*/
	void updateNodeCoordinates( float interpolationSpeed );

	/**
		*  \fn public  synchronizeNodes
		*  \brief Synchronizes all drawn nodes with given node map
		*/
	void synchronizeNodes();

	/**
		*  \fn public  freezeNodePositions
		*  \brief Freezes nodes
		*/
	void freezeNodePositions();


	/**
		*  \fn inline public  getNodeTransforms
		*  \brief returns all nodes transformations
		*  \return QMap<qlonglong,osg::ref_ptr<osg::AutoTransform> > * node transforms
		*/
	QMap<qlonglong, osg::ref_ptr<osg::Transform> >* getNodeTransforms()
	{
		return nodeTransforms;
	}


	/**
		*  \fn inline public  getGroup
		*  \brief Returns wrapped nodes group
		*  \return osg::ref_ptr nodes group
		*/
	osg::ref_ptr<osg::Group> getGroup()
	{
		return group;
	}

private:

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * nodes
		*  \brief wrapped nodes
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes;

	/**
		*  QMap<qlonglong,osg::ref_ptr<osg::AutoTransform> > * nodeTransforms
		*  \brief node transforms
		*/
	QMap<qlonglong, osg::ref_ptr<osg::Transform> >* nodeTransforms;

	/**
		*  osg::ref_ptr group
		*  \brief nodes group
		*/
	osg::ref_ptr<osg::Group> group;


	/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
	Util::ApplicationConfig* appConf;


	/**
		*  \fn private  initNodes
		*  \brief inits nodes
		*/
	void initNodes();

	/**
		*  \fn private  wrapChild(osg::ref_ptr<Data::Node> node, float graphScale)
		*  \brief Wraps node as a transform
		*  \param      node    node to wrap
		*  \param      graphScale     graph scale
		*  \return osg::ref_ptr node transform
		*/
	osg::ref_ptr<osg::Transform> wrapChild( osg::ref_ptr<Data::Node> node, float graphScale );

	/**
		*  \fn private  getNodeGroup(osg::ref_ptr<Data::Node> node, osg::ref_ptr<Data::Edge> parentEdge, float graphScale)
		*  \brief Recursively traverses all node and its children and creates a group from them
		*  \param       node    node to group
		*  \param       parentEdge    Edge with which was this node connected to its parent
		*  \param       graphScale     graph scale
		*  \return osg::ref_ptr nodes group
		*/
	osg::ref_ptr<osg::Group> getNodeGroup( osg::ref_ptr<Data::Node> node, osg::ref_ptr<Data::Edge> parentEdge, float graphScale );
};
}

#endif
