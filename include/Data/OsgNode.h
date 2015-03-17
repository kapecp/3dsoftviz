#ifndef OSGNODE_H
#define OSGNODE_H

#include <osg/Geode>
#include <Data/Type.h>

namespace Data {


class OsgNode : public osg::Geode
{
public:
	/**
	    *  \fn private static  createLabel(const float & scale, QString name)
	    *  \brief Creates node label from name
	    *  \param      scale     label scale
	    *  \param       name     label text
	    *  \return osg::ref_ptr node label
	    */
	static osg::ref_ptr<osg::Drawable> createLabel( const float& scale, QString name );


	/**
	    *  \fn private static  createNode(const float & scale, osg::StateSet* bbState)
	    *  \brief Creates node drawable
	    *  \param      scale    node scale
	    *  \param  bbState    node stateset
	    *  \return osg::ref_ptr node drawable
	    */
	static osg::ref_ptr<osg::Drawable> createNode( const float& scale, osg::StateSet* bbState );

	/**
	    *  \fn private static  createSquare
	    *  \brief Creates square around node
	    *  \param  scale   square scale
	    *  \param  bbState     square stateset
	    *  \return osg::ref_ptr square drawable
	    */
	static osg::ref_ptr<osg::Drawable> createSquare( const float& scale, osg::StateSet* bbState );

	/**
	    *  \fn private static  createStateSet(Data::Type * type = 0)
	    *  \brief Creates node stateset
	    *  \param   type     node type
	    *  \return osg::ref_ptr node stateset
	    */
	static osg::ref_ptr<osg::StateSet> createStateSet( Data::Type* type = 0 );

	/**
	    *  osg::ref_ptr focusedSquare
	    *  \brief Focused square drawable
	    */
	osg::ref_ptr<osg::Drawable> focusedSquare;


	/**
	    *  osg::ref_ptr label
	    *  \brief Label drawable
	    */
	osg::ref_ptr<osg::Drawable> label;


	/**
	    *  osg::ref_ptr square
	    *  \brief Square drawable
	    */
	osg::ref_ptr<osg::Drawable> square;



	/**
	    *  osg::Vec4 color
	    *  \brief Color of the Node
	    */
	osg::Vec4 color;

	/**
	    *  \fn inline public constant  getColor
	    *  \brief Returns color of the Node
	    *  \return osg::Vec4 color of the Node
	    */
	osg::Vec4 getColor() const
	{
		return color;
	}

	/**
	    *  osg::Vec3f velocity
	    *  \brief Size of node force in previous iteration
	    */
	osg::Vec3f velocity;


	/**
	    *  \fn inline public constant  getVelocity
	    *  \brief Sets node force for next iteration.
	    *  \return osg::Vec3f Node force
	    */
	osg::Vec3f getVelocity() const
	{
		return velocity;
	}

	/**
	    *  osg::Sphere nested ball
	    *  \brief
	    */
	osg::Sphere* parentBall;

	osg::Geode* ball;

	osg::ref_ptr<osg::AutoTransform> outBall;

	osg::Geode* getBall()
	{
		return ball;
	}
};
}
#endif // OSGNODE_H




