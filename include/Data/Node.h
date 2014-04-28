/*!
 * Node.h
 * Projekt 3DVisual
 */
#ifndef DATA_NODE_DEF
#define DATA_NODE_DEF 1

#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/AutoTransform>

#include <QMap>
#include <QString>


namespace Data
{
class Edge;
class Type;
class Graph;
class GraphLayout;

/**
	*  \class Node
	*  \brief Node object represents a single node in a Graph
	*  \author Aurel Paulovic, Michal Paprcka
	*  \date 29. 4. 2010
	*/
class Node : public osg::Geode
{
public:

	/**
		*  \fn public constructor  Node(qlonglong id, QString name, Data::Type* type, Data::Graph* graph, osg::Vec3f position)
		*  \brief Creates new Node object
		*  \param  id     ID of the Node
		*  \param  name     name of the Node
		*  \param  type    Type of the Node
		*  \param  graph   Graph to which the Node belongs
		*  \param  position    Node position in space
		*/
	Node(qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position);

	/**
		*  \fn public virtual destructor  ~Node
		*  \brief Destroys the Node object
		*/
	~Node(void);


	/**
		*  \fn inline public constant  getId
		*  \brief Returns ID of the Node
		*  \return qlonglong ID of the Node
		*/
	qlonglong getId() const { return id; }

	/**
		*  \fn inline public  getGraph
		*  \brief Returns Graph to which the Nodes belongs
		*  \return Data::Graph * Graph
		*/
	Data::Graph* getGraph() { return graph; }


	/**
		*  \fn inline public constant  getName
		*  \brief Returns name of the Node
		*  \return QString name of the Node
		*/
	QString getName() const { return name; }

	/**
		*  \fn inline public  setName(QString val)
		*  \brief Sets new name to the Node
		*  \param   val    new name
		*/
	void setName(QString val) { name = val; }

	/**
		*  \fn inline public  getScale()
		*  \brief Gets node scale
		*  \return float size of the scale
		*/
	float getScale() { return scale; }

	/**
		*  \fn inline public  setScale(float val)
		*  \brief Sets new scale
		*  \param   val   new scale for the Node
		*/
	void setScale(float val) { scale = val; }

	/**
		*  \fn inline public constant  getType
		*  \brief Returns Type of the Node
		*  \return Data::Type * Type of the Node
		*/
	Data::Type* getType() const { return type; }

	/**
		*  \fn inline public  setType(Data::Type* val)
		*  \brief Sets new Type of the Node
		*  \param   val  new Type
		*/
	void setType(Data::Type* val) { type = val; }

	bool isParentNode() { return hasNestedNodes; }

	void setAsParentNode() { hasNestedNodes = true; }

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
	void setIsFocused(bool value);

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
	const osg::Vec3f &targetPositionConstRef() const;

	/**
		 * \fn public setTargetPosition(const osg::Vec3f &position)
		 * \brief Sets target position of a node.
		 * \param position target position
		 * Target position being set MUST NOT BE multiplied by the graph scale.
		 */
	void setTargetPosition(const osg::Vec3f &position);

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
	const osg::Vec3f &restrictedTargetPositionConstRef() const;

	/**
		 * \fn public setRestrictedTargetPosition(const osg::Vec3f &position)
		 * \brief Sets restricted target position of a node.
		 * \param position restricted target position
		 * Restricted target position being set MUST NOT BE multiplied by the graph scale.
		 */
	void setRestrictedTargetPosition(const osg::Vec3f &position);

	/**
		*  \fn public  getCurrentPosition(bool calculateNew = false, float interpolationSpeed = 1.0f)
		*  \brief Returns node actual position
		*  \param      calculateNew    If true, new position will be calculated through interpolation
		*  \param      float   interpolation speed
		*  \return osg::Vec3f actual position
		*  returned currentPosition IS already multiplied by the graph scale
		*/
	osg::Vec3f getCurrentPosition(bool calculateNew = false, float interpolationSpeed = 1.0f);

	/**
		*  \fn inline public  setCurrentPosition(osg::Vec3f val)
		*  \brief Sets current node position
		*  \param   val  current node position
		*  currentPosition being set MUST BE multiplied by the graph scale
		*/
	void setCurrentPosition(osg::Vec3f val) { currentPosition.set(val); }

	/**
		*	\fn public removeAllEdges
		*	\brief Removes all Edges which connect to the Node
		*
		*	It unlinks all edges connected to the Node and removes them from the Graph (an Edge can't exist without both it's Nodes)
		*/
	void removeAllEdges();

	/**
		*	\fn public getParentNode
		*	\brief Return parent node, else null
		*
		*
		*/
	Data::Node* getParentNode();

	/**
		*	\fn public setParentNode
		*	\brief set parent's node
		*
		*
		*/
	void setParentNode(Node* parent);


	/**
		*  \fn inline public constant  getEdges
		*  \brief Returns all Edges connected to the Node
		*  \return QMap<qlonglong,osg::ref_ptr<Data::Edge> > * Edges connected to the Node
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> > * getEdges() const { return edges; }

	/**
		*  \fn inline public  setEdges(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *val)
		*  \brief Sets (overrides) new Edges which are connected to the Node
		*  \param   val   new Edges
		*/
	void setEdges(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *val) { edges = val; }


	/**
		*  \fn public  addEdge(osg::ref_ptr<Data::Edge> edge)
		*  \brief Adds new Edge to the Node
		*  \param    edge   new Edge
		*/
	void addEdge(osg::ref_ptr<Data::Edge> edge);
	void removeEdge(osg::ref_ptr<Data::Edge> edge);


	/**
		*  \fn inline public  setForce(osg::Vec3f v)
		*  \brief Sets force of node
		*  \param      v  Force vector
		*/
	void setForce(osg::Vec3f v) { force = v; }

	/**
		*  \fn inline public constant  getForce
		*  \brief Gets force of node
		*  \return osg::Vec3f Force vector
		*/
	osg::Vec3f getForce() const { return force; }

	/**
		*  \fn inline public  addForce(bool fixed)
		*  \brief Adds force V to node force
		*  \param       v  Force V
		*/
	void addForce(osg::Vec3f v) { force += v; }

	/**
		*  \fn inline public  resetForce
		*  \brief Sets nodes force to zero value.
		*/
	void resetForce() { force = osg::Vec3f(0,0,0); }


	/**
		*  \fn inline public  setFixed(bool fixed)
		*  \brief Sets node fixed state
		*  \param     fixed     fixed state
		*/
	void setFixed(bool fixed)
	{
		this->fixed = fixed;

		if (fixed && !this->containsDrawable(square))
			this->addDrawable(square);
		else if (!fixed)
			this->removeDrawable(square);
	}

	/**
		*  \fn inline public constant  isFixed
		*  \brief Returns if the Node is fixed
		*  \return bool true, if the Node is fixed
		*/
	bool isFixed() const { return fixed; }

	/**
		*  \fn inline public  setSelected(bool selected) and  color of selected node
		*  \brief Sets node picked state
		*  \param     selected     picked state
		*/
	void setSelected(bool selected)
	{
		if (selected)
			setDrawableColor(0, osg::Vec4(0.0f, 0.1f, 1.0f, 1.0f));    // color of selected node
		else
			setDrawableColor(0, color);

		this->selected = selected;
	}

	/**
		*  \fn inline public constant  isSelected
		*  \brief Returns if the Node is selected
		*  \return bool true, if the Node is selected
		*/
	bool isSelected() const { return selected; }


	/**
		*  \fn inline public  setIgnored(bool b)
		*  \brief Sets ignoring by layout algorithm.
		*  \param       b  True, if node is ignored.
		*/
	void setIgnored(bool b) { ignore = b; }

	/**
		*  \fn inline public constant  isIgnored
		*  \brief Return if the Node is ignored
		*  \return bool true, if the Node is ignored
		*/
	bool isIgnored() const { return ignore; }

	/**
		 * \brief \link Node::positionCanBeRestricted [setter] \endlink
		 */
	void setPositionCanBeRestricted (bool b) {positionCanBeRestricted = b; }

	/**
		 * \brief \link Node::positionCanBeRestricted [getter] \endlink
		 */
	bool getPositionCanBeRestricted () {return positionCanBeRestricted; }

	/**
		 * \brief \link Node::removableByUser [setter] \endlink
		 */
	void setRemovableByUser (bool b) {removableByUser = b; }

	/**
		 * \brief \link Node::removableByUser [getter] \endlink
		 */
	bool isRemovableByUser () {return removableByUser; }

	/**
		*  \fn inline public  setVelocity(osg::Vec3f v)
		*  \brief Sets node force for next iteration
		*  \param    v  Force in actual iteration
		*/
	void setVelocity(osg::Vec3f v) { velocity = v; }

	/**
		*  \fn inline public  resetVelocity
		*  \brief Reset node force for next iteration
		*/
	void resetVelocity() {velocity = osg::Vec3(0,0,0);}

	/**
		*  \fn inline public constant  getVelocity
		*  \brief Sets node force for next iteration.
		*  \return osg::Vec3f Node force
		*/
	osg::Vec3f getVelocity() const { return velocity; }


	/**
		*  \fn public  equals(Node* node)
		*  \brief Checks if the node and this object is the same object
		*  \param   node     Node to compare
		*  \return bool true, if this object and node are the same object
		*/
	bool equals(Node* node);


	/**
		*  \fn inline public constant  toString
		*  \brief Returns human-readable string representing the Node
		*  \return QString
		*/
	QString toString() const;



	/**
		*  \fn inline public  setColor(osg::Vec4 color)
		*  \brief Sets default node color
		*  \param     color   default color
		*/
	void setColor(osg::Vec4 color)
	{
		this->color = color;

		if (!selected)
			setDrawableColor(0, color);
	}

	void setInvisible(){
		setColor(osg::Vec4 (0,0,0,0));
	}


	/**
		*  \fn inline public constant  getColor
		*  \brief Returns color of the Node
		*  \return osg::Vec4 color of the Node
		*/
	osg::Vec4 getColor() const { return color; }


	/**
		*  \fn public  showLabel(bool visible)
		*  \brief If true, node name will be shown.
		*  \param     visible     node name shown
		*/
	void showLabel(bool visible);

	/**
		*  \fn inline public  isInDB
		*  \brief Returns if the Node is already in database or not
		*  \return bool true, if the Node is in database
		*/
	bool isInDB() { return inDB; }

	/**
		*  \fn inline public  setIsInDB
		*  \brief  Sets the inDB flag of the Node to true (meaning that the Node is in database)
		*/
	void setIsInDB() { inDB = true; };


	/**
		*  \fn inline public constant  isUsingInterpolation
		*  \brief Returns if the Node is using interpolation or not
		*  \return bool true, if the Node is using interpolation
		*/
	bool isUsingInterpolation() const { return usingInterpolation; }

	/**
		*  \fn inline public  setUsingInterpolation(bool val)
		*  \brief Sets if the Node is using interpolation or not
		*  \param      val
		*/
	void setUsingInterpolation(bool val) { usingInterpolation = val; }

	void setParentBall(osg::Sphere * val) { parentBall = val; }

	osg::Sphere * getParentBall() { return parentBall; }

	void setBall(osg::Geode * val) { ball = val; }

	osg::Geode * getBall() { return ball; }

	osg::ref_ptr<osg::AutoTransform> getOutBall() { return outBall; }

	void setOutBall(osg::ref_ptr<osg::AutoTransform> val) { outBall = val; }


	/**
		*  \fn public  reloadConfig
		*  \brief Reloads node configuration
		*/
	void reloadConfig();

	/**
		*  \fn inline public  setNestedParent(Data::Node val)
		*  \brief Sets parent of note, if null, node has no parent
		*  \param      val
		*/
	void setNestedParent(osg::ref_ptr<Data::Node> val) { nested_parent = val; }

	/**
		*  \fn inline public  getNestedParent
		*  \brief Return node parent, if no parent then return NULL
			*  \return QString name of the Node
		*/
	osg::ref_ptr<Data::Node> getNestedParent() { return nested_parent; }

	/**
		*  \fn inline public  getName
		*  \brief Return node name
			*  \return QString name of the Node
		*/
	QString getName() { return name; }

	/**
		*  \fn inline public constant  getSettings
		*  \brief Returns settings of the Node
		*  \return QMap<QString,QString> * settings of the Node
		*/
	QMap<QString, QString> * getSettings() const { return settings; }

	/**
		*  \fn inline public  setSettings(QMap<QString, QString> * val)
		*  \brief Sets (overrides) new settings of the Node
		*  \param   val    new settings
		*/
	void setSettings(QMap<QString, QString> * val) { settings = val; }

	void setLabelText(QString label) { labelText = label; }

	float getNodeDegree() { return nodeDegree; }
	void setNodeDegree(float nodeDegree) { this->nodeDegree = nodeDegree; }

	float getNodeCloseness() { return nodeCloseness; }
	void setNodeCloseness(float nodeCloseness) { this->nodeCloseness = nodeCloseness; }

	float getNodeBetweeness() { return nodeBetweeness; }
	void setNodeBetweeness(float nodeBetweeness) { this->nodeBetweeness = nodeBetweeness; }

	float getOverallWeight() { return overallWeight; }
	void setOverallWeight(float overallWeight) { this->overallWeight = overallWeight; }

	bool isNodeMatched() { return nodeMatched; }
	void setNodeMatched(bool nodeMatched) { this->nodeMatched = nodeMatched; }

private:


	/**
		*	bool inDB
		*	\brief Flag if the Type is in database
		*/
	bool inDB;

	/**
		*	qlonglong id
		*	\brief ID of the Node
		*/
	qlonglong id;

	/**
		*	float scale
		*	\brief scaling of the Node
		*/
	float scale;

	/**
		*	Node parent
		*	\brief parent node of current node in nested graphs, in top level graph is null
		*/
	osg::ref_ptr<Data::Node> nested_parent;

	/**
		*  QString name
		*  \brief Name of the Node
		*/
	QString name;

	/**
		*  Data::Type * type
		*  \brief Type of the Node
		*/
	Data::Type* type;

	/**
		*  Data::Graph * graph
		*  \brief Graph object to which the Node belongs
		*/
	Data::Graph* graph;

	/**
		 * bool mIsFocused
		 * \brief Flag whether this node is focused at the moment.
		 */
	bool mIsFocused;

	/**
		 * osg::Vec3f mTargetPosition
		 * \brief Target position of a node.
		 */
	osg::Vec3f mTargetPosition;

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

	/**
		*  osg::Sphere nested ball
		*  \brief
		*/
	osg::Sphere * parentBall;

	osg::Geode * ball;

	osg::ref_ptr<osg::AutoTransform> outBall;

	/**
		*  QMap<qlonglong, osg::ref_ptr<Data::Edge> > * edges
		*  \brief Edges connected to the Node
		*/
	QMap<qlonglong, osg::ref_ptr<Data::Edge> > * edges;

			  /**
				  *  osg::Vec3f force
				  *  \brief Node force
				  */
			  osg::Vec3f force;

	/**
		*  osg::Vec3f velocity
		*  \brief Size of node force in previous iteration
		*/
	osg::Vec3f velocity;

	/**
		*  bool fixed
		*  \brief node fixed state
		*/
	bool fixed;

	bool hasNestedNodes;

	/**
		*  bool ignore
		*  \brief node ignoring flag
		*/
	bool ignore;

	/**
		 * \brief If the node position can be restricted by layout restrictions.
		 * RestrictionsManager uses this to determine if the restriction can be attached to this node.
		 */
	bool positionCanBeRestricted;

	/**
		 * \brief If the node is removable by the user (using GUI).
		 * Used to make some nodes not removable (e.g. nodes used for manipulating layout restrictions - their
		 * existence is bound to the existence of the layout restriction and their removal is managed
		 * elsewhere, so they can not be removed by user).
		 */
	bool removableByUser;

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
		*  \fn private static  createNode(const float & scale, osg::StateSet* bbState)
		*  \brief Creates node drawable
		*  \param      scale    node scale
		*  \param  bbState    node stateset
		*  \return osg::ref_ptr node drawable
		*/
	static osg::ref_ptr<osg::Drawable> createNode(const float & scale, osg::StateSet* bbState);

	/**
		*  \fn private static  createStateSet(Data::Type * type = 0)
		*  \brief Creates node stateset
		*  \param   type     node type
		*  \return osg::ref_ptr node stateset
		*/
	static osg::ref_ptr<osg::StateSet> createStateSet(Data::Type * type = 0);

	/**
		*  \fn private static  createLabel(const float & scale, QString name)
		*  \brief Creates node label from name
		*  \param      scale     label scale
		*  \param       name     label text
		*  \return osg::ref_ptr node label
		*/
	static osg::ref_ptr<osg::Drawable> createLabel(const float & scale, QString name);

	/**
		*  \fn private static  createSquare
		*  \brief Creates square around node
		*  \param  scale   square scale
		*  \param  bbState     square stateset
		*  \return osg::ref_ptr square drawable
		*/
	static osg::ref_ptr<osg::Drawable> createSquare(const float & scale, osg::StateSet* bbState);


	/**
		*  osg::Vec4 color
		*  \brief Color of the Node
		*/
	osg::Vec4 color;


	/**
		*  \fn private  setDrawableColor(int pos, osg::Vec4 color)
		*  \brief Sets drawble color
		*  \param     pos     drawable position
		*  \param     color     drawable color
		*/
	void setDrawableColor(int pos, osg::Vec4 color);

	/**
		*  QString labelText
		*  \brief Text show in the label
		*/
	QString labelText;


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
		*  osg::ref_ptr focusedSquare
		*  \brief Focused square drawable
		*/
	osg::ref_ptr<osg::Drawable> focusedSquare;

	float nodeDegree;
	float nodeCloseness;
	float nodeBetweeness;
	bool nodeMatched;

	float overallWeight;

protected:

	/**
		*  QMap<QString,QString> * settings
		*  \brief Settings of the Node
		*/
	QMap<QString, QString> * settings;
};
}

#endif
