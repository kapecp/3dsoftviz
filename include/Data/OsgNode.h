#ifndef OSGNODE_H
#define OSGNODE_H

#include <osg/AutoTransform>
#include <osg/Geode>
#include <osg/Switch>
#include <Data/Type.h>

#include "Data/DbNode.h"
#include <City/Building.h>

namespace City {
class Residence;
}

namespace Data {

typedef osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,1> ColorIndexArray;

class OsgNode : public DbNode, public osg::Switch
{
public:

	OsgNode( qlonglong id, QString name, Data::Type* type, Data::Graph* graph, float scaling, osg::Vec3f position );

	~OsgNode( void );

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
	* \fn public setModule
	* \brief Sets subgraph for drawing modules.
	* \param module osg subgraph
	*/
	void setModule( osg::Node* module );

	/**
	* \fn public setResidence
	* \brief Sets subgraph for drawing residence of city.
	* \param residence osg subgraph
	*/
	void setResidence( osg::Node* residence );

	/**
	* \fn public getResidence
	* \brief Gets subgraph for drawing residence of city.
	* \return osg::ref_ptr of residence osg subgraph
	*/
	City::Residence* getResidence();

	/**
	* \fn public getBuilding
	* \brief Gets subgraph for drawing building of residence.
	* \return osg::ref_ptr of building osg subgraph
	*/
	City::Building* getBuilding();

	/**
	     * \fn public constant targetPosition
	     * \brief Gets target position of a node.
	     * \return target position
	     * Returned target position IS NOT multiplied by the graph scale.
	     */
	osg::Vec3f targetPosition() const;

	osg::Vec3f getTargetPosition() const;

	/**
	     * \fn public constant targetPositionConstRef
	     * \brief Gets target position of a node.
	     * \return target position
	     * Returned target position IS NOT multiplied by the graph scale.
	     */
	const osg::Vec3f& targetPositionConstRef() const;

	/**
	     * \fn public setTargetPosition(const osg::Vec3f &position)
	     * \brief Sets target position of a node.
	     * \param position target position
	     * Target position being set MUST NOT BE multiplied by the graph scale.
	     */
	void setTargetPosition( const osg::Vec3f& position );

	/**
	     * \fn public constant restrictedTargetPosition
	     * \brief Gets restricted target position of a node.
	     * \return restricted target position
	     * Returned restricted target position IS NOT multiplied by the graph scale.
	     */
	osg::Vec3f restrictedTargetPosition() const;

	/**
	     * \fn public constant restrictedTargetPositionConstRef
	     * \brief Gets restricted target position of a node.
	     * \return restricted target position
	     * Returned restricted target position IS NOT multiplied by the graph scale.
	     */
	const osg::Vec3f& restrictedTargetPositionConstRef() const;

	/**
	     * \fn public setRestrictedTargetPosition(const osg::Vec3f &position)
	     * \brief Sets restricted target position of a node.
	     * \param position restricted target position
	     * Restricted target position being set MUST NOT BE multiplied by the graph scale.
	     */
	void setRestrictedTargetPosition( const osg::Vec3f& position );

	/**
	    *  \fn public  getCurrentPosition(bool calculateNew = false, float interpolationSpeed = 1.0f)
	    *  \brief Returns node actual position
	    *  \param      calculateNew    If true, new position will be calculated through interpolation
	    *  \param      float   interpolation speed
	    *  \return osg::Vec3f actual position
	    *  returned currentPosition IS already multiplied by the graph scale
	    */
	osg::Vec3f getCurrentPosition( bool calculateNew = false, float interpolationSpeed = 1.0f );

	/**
	    *  \fn inline public  setCurrentPosition(osg::Vec3f val)
	    *  \brief Sets current node position
	    *  \param   val  current node position
	    *  currentPosition being set MUST BE multiplied by the graph scale
	    */
	void setCurrentPosition( osg::Vec3f val )
	{
		currentPosition.set( val );
	}

	/**
	    *  \fn inline public  setForce(osg::Vec3f v)
	    *  \brief Sets force of node
	    *  \param      v  Force vector
	    */
	void setForce( osg::Vec3f v )
	{
		force = v;
	}

	/**
	    *  \fn inline public constant  getForce
	    *  \brief Gets force of node
	    *  \return osg::Vec3f Force vector
	    */
	osg::Vec3f getForce() const
	{
		return force;
	}

	/**
	    *  \fn inline public  addForce(bool fixed)
	    *  \brief Adds force V to node force
	    *  \param       v  Force V
	    */
	void addForce( osg::Vec3f v )
	{
		force += v;
	}

	/**
	    *  \fn inline public  resetForce
	    *  \brief Sets nodes force to zero value.
	    */
	void resetForce()
	{
		force = osg::Vec3f( 0,0,0 );
	}

	/**
	    *  \fn inline public  setVelocity(osg::Vec3f v)
	    *  \brief Sets node force for next iteration
	    *  \param    v  Force in actual iteration
	    */
	void setVelocity( osg::Vec3f v )
	{
		velocity = v;
	}

	/**
	    *  \fn inline public  resetVelocity
	    *  \brief Reset node force for next iteration
	    */
	void resetVelocity()
	{
		velocity = osg::Vec3f( 0,0,0 );
	}

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
	    *  \fn inline public  setColor(osg::Vec4 color)
	    *  \brief Sets default node color
	    *  \param     color   default color
	    */
	void setColor( osg::Vec4 color )
	{
		this->color = color;

		if ( !selected ) {
			setDrawableColor( color );
		}
	}

	/**
	    *  \fn inline public constant  getColor
	    *  \brief Returns color of the Node
	    *  \return osg::Vec4 color of the Node
	    */
	osg::Vec4 getColor() const
	{
		return color;
	}

	void setScale( float val ) override;

	bool setInvisible( bool invisible );

	void setParentBall( osg::Sphere* val )
	{
		parentBall = val;
	}

	osg::Sphere* getParentBall()
	{
		return parentBall;
	}

	void setBall( osg::Geode* val )
	{
		ball = val;
	}

	/**
	    *  \fn inline public  setSelected(bool selected) and  color of selected node
	    *  \brief Sets node picked state
	    *  \param     selected     picked state
	    */
	void setSelected( bool selected )
	{
		if ( selected ) {
			setDrawableColor( osg::Vec4( 0.0f, 0.1f, 1.0f, 1.0f ) );    // color of selected node
		}
		else {
			setDrawableColor( color );
		}

		this->selected = selected;
	}

	/**
	    *  \fn inline public constant  isSelected
	    *  \brief Returns if the Node is selected
	    *  \return bool true, if the Node is selected
	    */
	bool isSelected() const
	{
		return selected;
	}

	osg::ref_ptr<osg::AutoTransform> getOutBall()
	{
		return outBall;
	}

	void setOutBall( osg::ref_ptr<osg::AutoTransform> val )
	{
		outBall = val;
	}

	osg::Geode* getBall()
	{
		return ball;
	}

	/**
	    *  \fn private  setDrawableColor(int pos, osg::Vec4 color)
	    *  \brief Sets drawble color
	    *  \param     color     drawable color
	    */
	void setDrawableColor( osg::Vec4 color );

	/**
	    *  \fn inline public  setUsingInterpolation(bool val)
	    *  \brief Sets if the Node is using interpolation or not
	    *  \param      val
	    */
	void setUsingInterpolation( bool val )
	{
		usingInterpolation = val;
	}

	/**
	    *  \fn inline public  setSelectedWith(osg::Vec4 selColor) and  color of selected node
	    *  \brief Select node with specific color
	    *  \param     selColor     selected node color
	    */
	void setSelectedWith( osg::Vec4 selColor )
	{
		setDrawableColor( selColor );  // color of selected node
		this->selected = true;
	}

	void setVisual( unsigned int index );


	/**
	    *  \fn public  reloadConfig
	    *  \brief Reloads node configuration
	    */
	void reloadConfig();

	/**
	    *  \fn public  showLabel(bool visible)
	    *  \brief If true, node name will be shown.
	    *  \param     visible     node name shown
	    */
	void showLabel( bool visible, bool labelsForResidence );

	/**
	     * \fn public constant isFocused
	     * \brief Gets whether this node is focused at the moment.
	     * \return whether this node is focused at the moment
	     */
	bool isFocused() const;

	/**
	     * \fn public setIsFocused(bool value)
	     * \brief Sets whether this node is focused at the moment.
	     * \param value whether this node is focused at the moment
	     */
	void setIsFocused( bool value );


	/**
	    *  \fn private static  createNodeSquare(const float & scale, osg::StateSet* bbState)
	    *  \brief Creates node drawable - square
	    *  \param	  scale	node scale
	    *  \param  bbState	node stateset
	    *  \return osg::ref_ptr node geode
	    */
	static osg::ref_ptr<osg::Node> createNodeSquare( const float& scale, osg::StateSet* bbState );

	/**
	    *  \fn private static  createNodeSphere(const float & scale, osg::StateSet* bbState)
	    *  \brief Creates node drawable - sphere
	    *  \param	  scale	node scale
	    *  \param  bbState	node stateset
	    *  \return osg::ref_ptr node geode
	    */
	static osg::ref_ptr<osg::Node> createNodeSphere( const float& scale, osg::StateSet* bbState );

	/**
	*  \fn private static  createNodeResidence(const float & scale, osg::StateSet* bbState)
	*  \brief Creates node drawable - residence
	*  \param	  scale	node scale
	*  \param  bbState	node stateset
	*  \return osg::ref_ptr node geode
	*/
	static osg::ref_ptr<osg::Node> createNodeResidence( const float& scale );

	/**
	*  \fn private static  createNodeModule(const float & scale, osg::StateSet* bbState)
	*  \brief Creates node drawable - module
	*  \param	  scale	node scale
	*  \param  bbState	node stateset
	*  \return osg::ref_ptr node geode
	*/
	static osg::ref_ptr<osg::Node> createNodeModule( const float& scale );
	/**
	    *  \fn private static  createStateSet(Data::Type * type = 0)
	    *  \brief Creates node stateset
	    *  \param   type     node type
	    *  \return osg::ref_ptr node stateset
	    */
	static osg::ref_ptr<osg::StateSet> createStateSet( const osg::ref_ptr<osg::Texture2D>& texture );

	/**
	    *  \fn private static  createLabel(const float & scale, QString name)
	    *  \brief Creates node label from name
	    *  \param      scale     label scale
	    *  \param       name     label text
	    *  \return osg::ref_ptr node label
	    */
	static osg::ref_ptr<osg::Node> createLabel( const float& scale, QString name );

	static const int INDEX_LABEL = 0;
	static const int INDEX_SQUARE = 1;
	static const int INDEX_SPHERE = 2;
	static const int INDEX_RESIDENCE = 3;
	static const int INDEX_MODULE = 4;

protected:
	/**
	    *  osg::ref_ptr focusedSquare
	    *  \brief Focused square drawable
	    */
	osg::ref_ptr<osg::Drawable> focusedSquare;

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
	     * osg::Vec3f mTargetPosition
	     * \brief Target position of a node.
	     */
	osg::Vec3f mTargetPosition;

	/**
	  *  osg::Vec3f force
	  *  \brief Node force
	  */
	osg::Vec3f force;

	/**
	    *  osg::Vec3f velocity
	    *  \brief Size of node force in previous iteration
	    */
	osg::Vec3 velocity;

	/**
	     * osg::Vec3f mRestrictedTargetPosition
	     * \brief Restricted target position of a node.
	     */
	osg::Vec3f mRestrictedTargetPosition;

	/**
	    *  osg::Vec3f currentPosition
	    *  \brief node current position
	    */
	osg::Vec3f currentPosition;

	osg::Vec4 colorOfNode;

	/**
	    *  bool selected
	    *  \brief node picked state
	    */
	bool selected;

	/**
	    *  bool usingInterpolation
	    *  \brief node interpolation usage
	    */
	bool usingInterpolation;

	/**
	    *  osg::Sphere nested ball
	    *  \brief
	    */
	osg::Sphere* parentBall;

	osg::Geode* ball;

	osg::ref_ptr<osg::AutoTransform> outBall;

	/**
	     * bool mIsFocused
	     * \brief Flag whether this node is focused at the moment.
	     */
	bool mIsFocused;

};
}
#endif // OSGNODE_H




