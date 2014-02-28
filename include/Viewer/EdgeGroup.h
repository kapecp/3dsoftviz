/**
*  EdgeGroup.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_EDGEWRAPPER_DEF
#define VIEWER_EDGEWRAPPER_DEF 1

#include <osg/ref_ptr>
#include <osg/Group>

#include <QMap>

#include "Data/Edge.h"

namespace Vwr
{
/**
	*  \class EdgeGroup
	*  \brief Edge group wrapper
	*  \author Michal Paprcka
	*  \date 29. 4. 2010
	*/
class EdgeGroup
{
public:

	/**
		*  \fn public constructor  EdgeGroup(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges, float scale)
		*  \brief Creates edge group
		*  \param edges     edges to wrap
		*  \param scale     edges scale
		*/
	EdgeGroup(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges, float scale);

	/**
		*  \fn public destructor  ~EdgeGroup
		*  \brief destructor
		*/
	~EdgeGroup(void);


	/**
		*  \fn public  updateEdgeCoords
		*  \brief Updates edges coordinates
		*/
	void updateEdgeCoords();

	/**
		*  \fn public  synchronizeEdges
		*  \brief synchronizes drawn edges with wrapped edge map
		*/
	void synchronizeEdges();


	/**
		*  \fn inline public  getGroup
		*  \brief returns edges group
		*  \return osg::ref_ptr edges group
		*/
	osg::ref_ptr<osg::Group> getGroup() {return edgeGroup;}

private:

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * edges
		*  \brief Wrpped edges
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges;


	/**
		*  float scale
		*  \brief edge scale
		*/
	float scale;


	/**
		*  osg::ref_ptr edgeGroup
		*  \brief edges group
		*/
	osg::ref_ptr<osg::Group> edgeGroup;

	/**
		*  osg::ref_ptr edgeStateSet
		*  \brief edges stateset
		*/
	osg::ref_ptr<osg::StateSet> edgeStateSet;

	/**
		*  osg::ref_ptr orientedEdgeStateSet
		*  \brief oriented edges stateset
		*/
	osg::ref_ptr<osg::StateSet> orientedEdgeStateSet;


	/**
		*  osg::ref_ptr geometry
		*  \brief edges geometry
		*/
	osg::ref_ptr<osg::Geometry> geometry;

	/**
		*  osg::ref_ptr orientedGeometry
		*  \brief oriented edges geometry
		*/
	osg::ref_ptr<osg::Geometry> orientedGeometry;


	/**
		*  \fn private  initEdges
		*  \brief inits geometry
		*/
	void initEdges();

	/**
		*  \fn private  getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first, osg::ref_ptr<osg::Vec3Array> coordinates,osg::ref_ptr<osg::Vec2Array> edgeTexCoords,osg::ref_ptr<osg::Vec4Array> colors,osg::ref_ptr<osg::Vec4Array> orientedEdgeColors)
		*  \brief Adds coordinates to coordinates array, texture coordinates and colors for given edge
		*  \param      edge     edge
		*  \param      first     first coordinate position in array
		*  \param      coordinates   coordinates array
		*  \param      edgeTexCoords     texture coordinates array
		*  \param      colors    color array
		*  \param      orientedEdgeColors   oriented edge color array
		*/
	void getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first,
									 osg::ref_ptr<osg::Vec3Array> coordinates,
									 osg::ref_ptr<osg::Vec2Array> edgeTexCoords,
									 osg::ref_ptr<osg::Vec4Array> colors,
									 osg::ref_ptr<osg::Vec4Array> orientedEdgeColors);

	/**
		*  \fn private  createEdgeStateSets
		*  \brief creates edge statesets
		*/
	void createEdgeStateSets();
};
}
#endif
