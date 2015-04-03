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

namespace Vwr {
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
		*  \param edges	 edges to wrap
		*/
	EdgeGroup( QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges );

	/**
		*  \fn public destructor  ~EdgeGroup
		*  \brief destructor
		*/
	~EdgeGroup( void );


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
	osg::ref_ptr<osg::Group> getGroup()
	{
		return edgeGroup;
	}

private:

	/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * edges
		*  \brief Wrpped edges
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges;


	/**
		*  osg::ref_ptr edgeGroup
		*  \brief edges group
		*/
	osg::ref_ptr<osg::Group> edgeGroup;


	/**
		*  \fn private  initEdges
		*  \brief inits geometry
		*/
	void initEdges();

};
}
#endif
