#ifndef OSGEDGE_H
#define OSGEDGE_H

#include <osg/Geode>
#include <osgText/FadeText>
#include "Data/Node.h"
#include "Data/DbEdge.h"

namespace Data {



class OsgEdge : public DbEdge, public osg::Switch
{
public:

	OsgEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, float scaling, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, osg::ref_ptr<osg::Camera> camera );

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
	    *  \fn public  updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode)
	    *  \brief Updates coordinates of the Edge
	    *  \param    srcPos    new coordinates of the starting Node
	    *  \param   dstNode    new coordinates of the ending Node
	    */
	void updateCoordinates( osg::Vec3 srcPos, osg::Vec3 dstNode );

	/**
	    * \fn inline public constant getSrcNode
	    * \brief  Returns the starting Node of the Edge
	    * \return osg::ref_ptr<Data::Node> starting Node of the Edge
	    */
	osg::ref_ptr<Data::Node> getSrcNode() const
	{
		return srcNode;
	}

	/**
	    * \fn inline public constant setSrcNode
	    * \brief  Sets new starting Node of the Edge
	    *
	    * OBSOLETE - DO NOT USE IT
	    *
	    * \param val new starting Node of the Edge
	    */
	void setSrcNode( osg::ref_ptr<Data::Node> val )
	{
		srcNode = val;
	}

	/**
	    * \fn inline public constant getDstNode
	    * \brief  Returns ending Node of the Edge
	    * \return osg::ref_ptr<Data::Node> ending Node of the Edge
	    */
	osg::ref_ptr<Data::Node> getDstNode() const
	{
		return dstNode;
	}

	/**
	    * \fn inline public setDstNode(osg::ref_ptr<Data::Node> val)
	    * \brief Sets new ending Node of the Edge
	    *
	    * OBSOLETE - DO NOT USE IT
	    *
	    * \param val new ending Node of the Edge
	    */
	void setDstNode( osg::ref_ptr<Data::Node> val )
	{
		dstNode = val;
	}

	/**
	    *  \fn inline public constant  getCooridnates
	    *  \brief Returns coordinates of the Edge
	    *  \return osg::ref_ptr coordinates of the Edge
	    */
	osg::ref_ptr<osg::Vec3Array> getCooridnates() const
	{
		return coordinates;
	}

	/**
	    *  \fn inline public constant  getCamera
	    *  \brief returns current camera looking at edge
	    *  \return osg::ref_ptr<osg::Camera>
	    */
	osg::ref_ptr<osg::Camera> getCamera() const
	{
		return camera;
	}

	/**
	    *  \fn inline public  setCamera(osg::ref_ptr<osg::Camera> val)
	    *  \brief sets current camera used in viewer.
	    *  \param       val
	    */
	void setCamera( osg::ref_ptr<osg::Camera> val )
	{
		camera = val;
	}

	/**
	    *  \fn inline public constant  getEdgeColor
	    *  \brief Returns the color of the Edge
	    *  \return osg::Vec4 color of the Edge
	    */
	osg::Vec4 getEdgeColor() const
	{
		if ( selected ) {
			return osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f );    // color of selected node
		}
		else {
			return edgeColor;
		}
	}

	/**
	    *  \fn inline public  setEdgeColor(osg::Vec4 val)
	    *  \brief Sets the color of the Edge
	    *  \param     val   new color
	    */
	void setEdgeColor( osg::Vec4 val )
	{
		edgeColor = val;
	}
protected:

	/**
	    *  osg::ref_ptr label
	    *  \brief Label of the Edge
	    */
	osg::ref_ptr<osgText::FadeText> label;

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
	    *  osg::Vec3 center
	    *  \brief Center of the Edge
	    */
	osg::ref_ptr<osg::Vec3Array> center;

	/**
	    *  double angle
	    *  \brief Angle of the rotation of the cylinder
	    */
	double angle;

	/**
	    *  osg::Vec3 rotation
	    *  \brief Rotation of the cylinder
	    */
	osg::ref_ptr<osg::Vec3Array> rotation;

	/**
	    *  osg::ref_ptr camera
	    *  \brief Current camera used in viewer. It is used for billboarding purposes.
	    */
	osg::ref_ptr<osg::Camera> camera;

	/**
	    *  osg::Vec4 edgeColor
	    *  \brief Color of the Edge
	    */
	osg::Vec4 edgeColor;
};
}
#endif // OSGEDGE_H
