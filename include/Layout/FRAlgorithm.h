/**
*  FRAlgorithm.h
*  Projekt 3DVisual
*/
#ifndef LAYOUT_FRALGORITHM_DEF
#define LAYOUT_FRALGORITHM_DEF 1


#include <osg/Vec3f>

#include <QMutex>
#include <QThread>

namespace Data {
class Graph;
class Node;
class Edge;
}

namespace Layout {
class RadialLayout;
/**
	*  \class FRAlgorithm
	*
	*  \brief Object represents Fruchterman-Reingold layout algorithm
	*
	*  \author Michal Pavlik
	*
	*  \date 28. 4. 2010
	*/
class FRAlgorithm : public QThread
{
public:

	/**
		*  \fn public overloaded constructor  FRAlgorithm
		*  \brief Creates new FRAlgorithm object
		*/
	FRAlgorithm();

	/**
		*  \fn public overloaded constructor  FRAlgorithm(Data::Graph *graph)
		*  \brief Creates new FRAlgorithm object and sets input graph
		*  \param  graph  Graph to which layout will be setted
		*/
	explicit FRAlgorithm( Data::Graph* graph );

	/**
		*  \fn public  SetParameters(float sizeFactor,float flexibility,int animationSpeed,bool useMaxDistance)
		*  \brief Sets parameters of layout algorithm
		*  \param       sizeFactor
		*  \param       flexibility
		*  \param       animationSpeed
		*  \param       useMaxDistance
		*/

	void SetParameters( double sizeFactor,float flexibility,bool useMaxDistance );

	/**
		*  \fn public  Randomize
		*  \brief Sets random position of nodes
		*/
	void Randomize();

	/**
		*  \fn inline public  SetAlphaValue(float val)
		*  \brief Sets multiplicity of forces
		*  \param      val  multipliciter of forces
		*/

	void SetAlphaValue( float val )
	{
		ALPHA = val;
	}

	/**
		*  \fn public  PauseAlg
		*  \brief Sets PAUSED state and waits until the current iteration ends.
		*/
	void PauseAlg();

	/**
		*  \fn public  RunAlg
		*  \brief Play paused layout algorithm
		*/
	void RunAlg();

	/**
		*  \fn public  WakeUpAlg
		*  \brief Wakes up frozen layout algorithm
		*/
	void WakeUpAlg();

	/**
		*  \fn public  IsRunning
		*  \brief Returns if layout algorithm is running or not
		*  \return bool true, if algorithm is running
		*/
	bool IsRunning();

	/**
		*  \fn public  Run
		*  \brief Starts layout algorithm process
		*/
	void Run();

	/**
		*  \fn public  SetGraph(Data::Graph *graph)
		*  \brief Sets graph data structure
		*  \param graph  data structure containing nodes, edges and types
		*/
	void SetGraph( Data::Graph* graph, bool fixedPositions = false );

	/**
		*  \brief Sets the end status (causing the loops in run method to end)
		*/
	void RequestEnd();

	/**
		*  \fn public  RunAlg
		*  \brief Play edge bundling algorithm
		*/
	void RunAlgEdgeBundling();

	/**
		*  \fn public  StopAlgEdgeBundling
		*  \brief stop edge bundling algorithm
		*/
	void StopAlgEdgeBundling();

	/**
		*  \fn inline public  SetAlphaEdgeBundlingValue(float val)
		*  \brief Sets multiplicity of forces in edge bundling algorithm
		*  \param	  val  multipliciter of forces
		*/
	void SetAlphaEdgeBundlingValue( float val )
	{
		ALPHA_EDGEBUNDLING = val;
	}

	/**
		*  \fn inline public  getAngle
		*  \brief returns  angle compatibility (0.0 - 1.0) of edges splitted by param nodes
		*/
	double getAngleCompatibility( Data::Node* u, Data::Node* v );

	/**
		*  \fn inline public  getScaleCompatibility
		*  \brief returns  scale compatibility (0.0 - 1.0) of edges splitted by param nodes (returns 1.0 if edges have equal length)
		*/
	double getScaleCompatibility( Data::Node* u, Data::Node* v );

	/**
		*  \fn inline public  getPositionCompatibility
		*  \brief returns  position compatibility (0.0 - 1.0) of edges splitted by param nodes (returns 1.0 if edges have equal length)
		*/
	double getPositionCompatibility( Data::Node* u, Data::Node* v );

	/**
		*  \brief Sets the repulsive force multiplier between two nodes in one vertigo plane
		*/
	void setRepulsiveForceVertigo( int value );

	//skalovanie dlzky hran
	void setMaxDistance( float max_distance )
	{
		this->MAX_DISTANCE = max_distance;
	}

	float getMaxDistance()
	{
		return MAX_DISTANCE;
	}

private:
	Layout::RadialLayout* rl;

	/**
		*  Data::Graph * graph
		*  \brief data structure containing nodes, edges and types
		*/
	Data::Graph* graph;

	/**
		*  double PI
		*  \brief circular constant
		*/
	double PI;

	/**
		*  float ALPHA
		*  \brief multipliciter of forces
		*/
	float ALPHA;

	/**
		*  float MIN_MOVEMENT
		*  \brief minimal distance between two nodes, when forces are aplified
		*/
	float MIN_MOVEMENT;

	/**
		*  float MAX_MOVEMENT
		*  \brief maximal distance between two nodes, when forces are aplified
		*/
	float MAX_MOVEMENT;

	/**
		*  float MAX_DISTANCE
		*  \brief maximal distance of nodes, when repulsive force is aplied
		*/
	float MAX_DISTANCE;

	/**
		*  float MIN_MOVEMENT_EDGEBUNDLING
		*  \brief minimal distance between two meta nodes during edge bundling alg, when forces are aplified
		*/
	float MIN_MOVEMENT_EDGEBUNDLING;

	/**
		*  float ALPHA_EDGEBUNDLING
		*  \brief multipliciter of forces
		*/
	float ALPHA_EDGEBUNDLING;

	/**
		*  enum State
		*  \brief maximal distance of nodes, when repulsive force is aplied
		*/


	enum State {
		RUNNING, PAUSED
	};

	/**
		*  float flexibility
		*  \brief flexibility of graph layouting
		*/
	float flexibility;

	/**
		*  float sizeFactor
		*  \brief size of graph layout
		*/
	double sizeFactor;

	/**
		*  double K
		*  \brief normal length of edge, rest mass of chord
		*/
	double K;

	/**
		*  osg::Vec3f center
		*  \brief center of cetripetal force
		*/
	osg::Vec3f center;

	/**
		*  FRAlgorithm::State state
		*  \brief actual status of the algorithm
		*/
	FRAlgorithm::State state;

	/**
		*  FRAlgorithm::State stateEdgeBundling
		*  \brief actual status of the edge bundling algorithm
		*/
	FRAlgorithm::State stateEdgeBundling;

	/**
		*  bool useMaxDistance
		*  \brief constaint using maximal distance of nodes, when repulsive force is aplied
		*/
	bool useMaxDistance;
	/**
		*  bool notEnd
		*  \brief algorithm end flag
		*/
	bool notEnd;

	/**
		*  \brief usage: waiting for current iteration end after pausing the algorithm
		*/
	QMutex isIterating_mutex;

	/**
		*  \fn private  computeCalm
		*  \brief computes rest mass chord
		*  \return double normal length of edge
		*/
	double computeCalm();

	/**
		*  \fn private  getRandomDouble
		*  \brief Returns random double number
		*  \return double Random number
		*/
	double getRandomDouble();

	/**
		*  \fn private  getRandomLocation
		*  \brief Gets random vector
		*  \return osg::Vec3f random vector
		*/
	osg::Vec3f getRandomLocation();

	/**
		*  osg::Vec3f fv
		*  \brief force vector
		*/
	osg::Vec3f fv;

	/**
		*  osg::Vec3f last
		*  \brief origin position of node
		*/
	osg::Vec3f last;

	/**
		*  osg::Vec3f newLoc
		*  \brief new position of node
		*/
	osg::Vec3f newLoc;

	/**
		*  osg::Vec3f up
		*  \brief vector U
		*/
	osg::Vec3f up;

	/**
		*  osg::Vec3f vp
		*  \brief vector V
		*/
	osg::Vec3f vp;

	/**
		*  osg::Vec3f barycenter
		*  \brief barycenter position
		*/
	osg::Vec3f barycenter;

	/**
		*  osg::Vec3f centripetal
		*  \brief centripetal position
		*/
	osg::Vec3f centripetal;

	/**
		*  double dist
		*  \brief distance between two positions
		*/
	double dist;

	/**
		*  int repulsiveForceVertigo
		*  \brief repulsive force multiplier between two nodes in one vertigo plane
		*/
	int repulsiveForceVertigo;

	/**
		*  \fn private  iterate
		*  \brief performs one iteration of the algorithm
		*  \return bool
		*/
	bool iterate();

	/**
		*  \fn private  applyForces(Data::Node* node)
		*  \brief Applyies forces to node
		*  \param   node node to which will be aplified forces
		*  \return bool
		*/
	bool applyForces( Data::Node* node );

	/**
		*  \fn private  addAttractive(Data::Edge* edge, float factor)
		*  \brief Adds attractive force to node
		*  \param  edge  which associate two nodes on which effect attractive force
		*  \param  factor  multiplicer of attaractive force
		*/
	void addAttractive( Data::Edge* edge, float factor );

	/**
		*  \fn private  addMetaAttractive(Data::Node* u, Data::Node* meta, float factor)
		*  \brief Adds attractive force between node U and meta node
		*  \param   u  node U
		*  \param   meta meta node
		*  \param   factor  multiplicer of attractive force
		*/
	void addMetaAttractive( Data::Node* u, Data::Node* meta, float factor );

	/**
		*  \fn private  addNeighbourAttractive(Data::Node* meta, Data::Node* neighbour, float factor)
		*  \brief Adds attractive force between meta node and his neighbour
		*  \param   meta	meta node
		*  \param   neighbour   neighbour node
		*  \param   factor  multiplicer of attractive force
		*/
	void addNeighbourAttractive( Data::Node* meta, Data::Node* neighbour, float factor );

	/**
		*  \fn private  addSameIndexAttractive(Data::Node* meta1, Data::Node* meta2, float factor)
		*  \brief Adds attractive force between two meta nodes with same index
		*  \param   meta1	first meta node
		*  \param   meta2   second meta node
		*  \param   factor  multiplicer of attractive force
		*/
	void addSameIndexAttractive( Data::Node* meta1, Data::Node* meta2, float factor );
	/**
		*  \fn private  addRepulsive(Data::Node* u, Data::Node* v, float factor)
		*  \brief Adds repulsive force between two nodes into node U
		*  \param  u  node U
		*  \param  v  node V
		*  \param  factor  multiplicer of repulsive force
		*/
	void addRepulsive( Data::Node* u, Data::Node* v, float factor );

	/**
		*  \fn private  rep(double distance)
		*  \brief Computes repulsive force
		*  \param  distance  distance between two vectors
		*  \return float size of repulsive force
		*/
	float rep( double distance );

	/**
		*  \fn private  attr(double distance)
		*  \brief Computes attractive force
		*  \param     distance  distance between two vectors
		*  \return float size of attractive force
		*/
	float attr( double distance );

	/**
		*  \fn private  centr(double distance)
		*  \brief Computes centripetal force
		*  \param      distance   distance between two vectors
		*  \return float size of centripetal force
		*/
	float centr( double distance );

	/**
		*  \fn private  distance(osg::Vec3f u,osg::Vec3f v)
		*  \brief Computes distance between vectors U and V.
		*  \param       u  vector U
		*  \param       v  vector V
		*  \return double distance between two vectors
		*/
	double distance( osg::Vec3f u,osg::Vec3f v );

	/**
		 * \brief If the nodes are not ignored and are from the same graph.
		 * If one of the nodes (or both) is a meta node, there are forces between these
		 * nodes even if they are from different graphs.
		 */
	bool areForcesBetween( Data::Node* u, Data::Node* v );

	/**
		 * \brief Last focused node (if any).
		 */
	Data::Node* mLastFocusedNode;

};
}

# endif
