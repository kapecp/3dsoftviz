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

    /**
        *  \fn inline public constant  isSelected
        *  \brief Returns selected flag
        *  \return bool true, if the Edge is selected
        */
    bool isSelected() const
    {
        return selected;
    }

    /**
        *  \fn inline public  setSelected(bool val)
        *  \brief Sets the selected flag of the Edge
        *  \param       val   true, if the Edge is selected
        */
    void setSelected( bool val )
    {
        selected = val;
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
        *  \fn inline public constant  getLength
        *  \brief Returns length of the Edge
        *  \return float length of the Edge
        */

    double getLength() const
    {
        return length;
    }

    /**
        *  \fn inline public  getScale()
        *  \brief Gets edge scale
        *  \return float size of the scale
        */
    float getScale()
    {
        return scale;
    }

    /**
        *  \fn inline public  setScale(float val)
        *  \brief Sets new scale
        *  \param   val   new scale for the Edge
        */
    void setScale( float val )
    {
        scale = val;
    }

protected:

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

    /**
        *  osg::Vec4 edgeColor
        *  \brief Color of the Edge
        */
    osg::Vec4 edgeColor;

    /**
        *  bool selected
        *  \brief True, if edge is picked
        */
    bool selected;

    /**
        *  osg::ref_ptr camera
        *  \brief Current camera used in viewer. It is used for billboarding purposes.
        */
    osg::ref_ptr<osg::Camera> camera;

    /**
        *  float length
        *  \brief Length of the Edge
        */
    double length;

    /**
        *  float scale
        *  \brief scale of edge
        */
    float scale;
};
}
#endif // OSGEDGE_H
