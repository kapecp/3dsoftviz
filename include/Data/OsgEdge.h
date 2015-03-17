#ifndef OSGEDGE_H
#define OSGEDGE_H

#include <osg/Geode>
#include <osgText/FadeText>
#include "Data/Node.h"

namespace Data {


class OsgEdge : public osg::Geode
{
public:


	/**
	    *  \fn public  createLabel(QString name)
	    *  \brief Creates new label
	    *  \param      name     new label
	    *  \return osg::ref_ptr added label
	    */
	osg::ref_ptr<osg::Drawable> createLabel( QString name );

	osg::ref_ptr<osg::StateSet> createStateSet( Data::Type* type );

	osg::ref_ptr<osg::Drawable> createEdge( osg::StateSet* bbState );

	/**
	    *  osg::ref_ptr label
	    *  \brief Label of the Edge
	    */
	osg::ref_ptr<osgText::FadeText> label;

	/**
	    *  Data::Type * type
	    *  \brief Type of the Edge
	    */
	Data::Type* type;

	/**
	    *  osg::ref_ptr<Data::Node> srcNode
	    *  \brief Starting Node of the Edge
	    */
	osg::ref_ptr<Data::Node> srcNode;

	/**
	    *  osg::ref_ptr<Data::Node> dstNode
	    *  \brief Ending Node of the Edge
	    */
	osg::ref_ptr<Data::Node> dstNode;

	/**
	    *  osg::ref_ptr coordinates
	    *  \brief Coordinates of the Edge
	    */
	osg::ref_ptr<osg::Vec3Array> coordinates;

	/**
	    *  osg::ref_ptr<osg::Vec2Array> edgeTexCoords
	    *  \brief Texture coordinates array.
	    */
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords;

	/**
	    *  bool oriented
	    *  \brief True, if the Edge is oriented
	    */
	bool oriented;
};
}
#endif // OSGEDGE_H
