/**
*  EdgeGroup.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_EDGEWRAPPER_DEF
#define VIEWER_EDGEWRAPPER_DEF 1

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/ShapeDrawable>

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
		*  osg::ref_ptr drawable
		*  \brief drawable edges
		*/
	QList<osg::ref_ptr<osg::ShapeDrawable> > drawableList;

	/**
		*  int INDEX_GEOMETRY
		*  \brief index of geometry node in edgeGroup
		*/
	static const int INDEX_GEOMETRY = 0;

	/**
		*  int INDEX_ORIENTED_GEOMETRY
		*  \brief index of oriented geometry node in edgeGroup
		*/
	static const int INDEX_ORIENTED_GEOMETRY = 1;

	/**
		*  int INDEX_CYLINDERS
		*  \brief index of group of cylinders in edgeGroup
		*/
	static const int INDEX_CYLINDERS = 2;

	/**
		*  \fn private  initEdges
		*  \brief inits geometry
		*/
	void initEdges();

	/**
		*  \fn private  getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first, osg::ref_ptr<osg::Vec3Array> coordinates,osg::ref_ptr<osg::Vec2Array> edgeTexCoords,osg::ref_ptr<osg::Vec4Array> colors,osg::ref_ptr<osg::Vec4Array> orientedEdgeColors,
float* length, float* radius, osg::ref_ptr<osg::Vec3Array> center, double* angle, osg::ref_ptr<osg::Vec3Array> rotation)
		*  \brief Adds coordinates to coordinates array, texture coordinates and colors for given edge
		*  \param      edge     edge
		*  \param      first     first coordinate position in array
		*  \param      coordinates   coordinates array
		*  \param      edgeTexCoords     texture coordinates array
		*  \param      colors    color array
		*  \param      orientedEdgeColors   oriented edge color array
		*  \param	  length   length of edge cylinder
		*  \param	  radius   radius of edge cylinder
		*  \param	  center   center between source and destination node
		*  \param	  angle   angle for rotation of cylinder
		*  \param	  rotation   roation of cylinder
		*/
	void getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first,
									 osg::ref_ptr<osg::Vec3Array> coordinates,
									 osg::ref_ptr<osg::Vec2Array> edgeTexCoords,
									 osg::ref_ptr<osg::Vec4Array> colors,
									 osg::ref_ptr<osg::Vec4Array> orientedEdgeColors,
									 float* length,
									 float* radius,
									 osg::ref_ptr<osg::Vec3Array> center,
									 double* angle,
									 osg::ref_ptr<osg::Vec3Array> rotation);

	/**
		*  \fn private  createEdgeStateSets
		*  \brief creates edge statesets
		*/
	void createEdgeStateSets();

	/**
		*  \fn private setCylinder(osg::ref_ptr<osg::ShapeDrawable> drawable)
		*  \brief set properties of cylinder as representation of edge
		*  \param	  drawable   drawable from variable drawablelist
		*/
	void setCylinder(osg::ref_ptr<osg::ShapeDrawable> drawable);
};
}
#endif
