/**
*  PickHandler.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_PICK_HANDLER_DEF
#define VIEWER_PICK_HANDLER_DEF 1

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/Viewer>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Vec3f>

#include "Data/Node.h"
#include "Data/Edge.h"
#include "Data/Cluster.h"

#include "SelectionObserver.h"

#include <QLinkedList>
#include <QTimer>

namespace Util {
class ApplicationConfig;
}


namespace Vwr {

class CoreGraph;
class CameraManipulator;

/**
	*  \class PickHandler
	*  \brief Handles picking events
	*  \author Michal Paprcka
	*  \date 29. 4. 2010
	*/
class PickHandler : public QObject,	public osgGA::GUIEventHandler
{
	Q_OBJECT

public:

	/**
		*  \class PickMode
		*  \brief Picking modes
		*  \author Michal Paprcka
		*  \date 29. 4. 2010
		*/
	class PickMode
	{
	public:

		/**
			*  const int NONE
			*  \brief no picking
			*/
		static const int NONE = 0;

		/**
			*  const int SINGLE
			*  \brief picking single object
			*/
		static const int SINGLE = 1;

		/**
			*  const int MULTI
			*  \brief multiobject picking
			*/
		static const int MULTI = 2;
	};

	/**
		*  \class SelectionType
		*  \brief Selection types
		*  \author Michal Paprcka
		*  \date 29. 4. 2010
		*/
	class SelectionType
	{
	public:

		/**
			*  const int ALL
			*  \brief picking all
			*/
		static const int ALL = 0;

		/**
			*  const int NODE
			*  \brief picking nodes
			*/
		static const int NODE = 1;

		/**
			*  const int EDGE
			*  \brief picking edges
			*/
		static const int EDGE = 2;

		static const int CLUSTER = 3;
	};


	/**
		*  \fn public constructor  PickHandler(Vwr::CameraManipulator * cameraManipulator, Vwr::CoreGraph * coreGraph)
		*  \brief Creates picking handler
		*  \param  cameraManipulator    camera manipulator
		*  \param  coreGraph     core graph
		*/
	PickHandler( Vwr::CameraManipulator* cameraManipulator, Vwr::CoreGraph* coreGraph );


	/**
		*  \fn public virtual  handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief Handles events
		*  \param   ea const     event adapter
		*  \param   aa    action adapter
		*  \return bool true, if event was handled
		*/
	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	using osgGA::GUIEventHandler::handle;

	/**
		*  \fn public  toggleSelectedNodesFixedState(bool isFixed)
		*  \brief Sets fixed state to picked nodes
		*  \param     isFixed     fixed state
		*/
	void toggleSelectedNodesFixedState( bool isFixed );

	/**
				*  \fn public  getPickedNodeWithMaxEdgeCount()
				*  \brief Find picked node from pickedNodes with max edge count
				*  \return osg::ref_ptr<Data::Node>
				*/
	osg::ref_ptr<Data::Node> getPickedNodeWithMaxEdgeCount();

	//volovar zac
	/**
	 * @brief getPickedNodeWithMinEdgeCount
	 *  Find picked node from pickedNodes with min edge count for radial layout purpose
	 * @return
	 */
	osg::ref_ptr<Data::Node>  getPickedNodeWithMinEdgeCount();
	//volovar kon

	/**
		*  \fn inline public  setPickMode( int pickMode )
		*  \brief Sets pick mode
		*  \param     pickMode
		*/
	void setPickMode( int pickMode )
	{
		this->pickMode = pickMode;
	}


	/**
		*  \fn inline public  setSelectionType( int selectionType )
		*  \brief Sets selection type
		*  \param      selectionType
		*/
	void setSelectionType( int selectionType )
	{
		this->selectionType = selectionType;
	}

	/**
		*  \fn inline public constant  getSelectionType
		*  \brief Returns selection type
		*  \return int selection type
		*/
	int getSelectionType() const
	{
		return selectionType;
	}


	/**
		*  \fn public  getSelectionCenter(bool nodesOnly)
		*  \brief Returns selection mass center
		*  \param       nodesOnly     if true, mass will be calculated form nodes only
		*  \return osg::Vec3
		*/
	osg::Vec3 getSelectionCenter( bool nodesOnly );

	/**
		 * @author Viktor Vinczler
		 * @brief getSelectionCenterNnE Computes center of nodes/edges selection.
		 * @return osg::Vec3 Selection center,
		 * if are all nodes and edges unselected, returns center of graph.
		 */
	osg::Vec3 getSelectionCenterNnE();

	/**
		*  \fn inline public  getSelectedNodes
		*  \brief Returns selected nodes
		*  \return QLinkedList<osg::ref_ptr<Data::Node> > * selected nodes
		*/
	QLinkedList<osg::ref_ptr<Data::Node> >* getSelectedNodes()
	{
		return &pickedNodes;
	}

	/**
		*  \fn inline public  getSelectedEdges
		*  \brief Returns selected edges
		*  \return QLinkedList<osg::ref_ptr<Data::Edge> > * selected edges
		*/
	QLinkedList<osg::ref_ptr<Data::Edge> >* getSelectedEdges()
	{
		return &pickedEdges;
	}

	/**
		*  \fn public  unselectPickedNodes(osg::ref_ptr<Data::Node> node = 0)
		*  \brief unselects picked nodes. If null, all nodes will be unselected.
		*  \param     node     nodes to unselect
		*/
	void unselectPickedNodes( osg::ref_ptr<Data::Node> node = 0 );

	/**
		*  \fn public  unselectPickedEdges(osg::ref_ptr<Data::Edge> edge = 0)
		*  \brief unselects picked edges. If null, all edges will be unselected.
		*  \param     edge   edges to unselect
		*/
	void unselectPickedEdges( osg::ref_ptr<Data::Edge> edge = 0 );

	void addPickedNode( osg::ref_ptr<Data::Node> node )
	{
		pickedNodes.append( node );
	}
	void addPickedEdge( osg::ref_ptr<Data::Edge> edge )
	{
		pickedEdges.append( edge );
	}

	void unselectPickedClusters();

	QLinkedList<osg::ref_ptr<Data::Cluster> > getPickedClusters()
	{
		return pickedClusters;
	}

	SelectionObserver* getSelectionObserver() const;
	void setSelectionObserver( SelectionObserver* value );

protected:
	// Store mouse xy location for button press & move events.
	float _mX,_mY;
	float origin_mX, origin_mY, origin_mX_normalized, origin_mY_normalized;

	/**
		*  bool leftButtonPressed
		*  \brief true, if left mouse button pressed
		*/
	bool leftButtonPressed;

	// Perform a pick operation.
	bool pick( const double xMin, const double yMin, const double xMax, const double yMax, osgViewer::Viewer* viewer );

	SelectionObserver* selectionObserver;

private:

	/**
		*  Vwr::CameraManipulator * cameraManipulator
		*  \brief camera manipulator
		*/
	Vwr::CameraManipulator* cameraManipulator;

	/**
		*  Vwr::CoreGraph * coreGraph
		*  \brief core graph
		*/
	Vwr::CoreGraph* coreGraph;


	/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
	Util::ApplicationConfig* appConf;

	/**
	*  osg::ref_ptr<Data::Node> lastAutoMovementNode
	*  \brief the last approaching node in automatic camera movement
	*/
	osg::ref_ptr<Data::Node> lastAutoMovementNode;

	/**
		*  QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes
		*  \brief picked nodes list
		*/
	QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

	/**
		*  QLinkedList<osg::ref_ptr<Data::Edge> > pickedEdges
		*  \brief picked edges list
		*/
	QLinkedList<osg::ref_ptr<Data::Edge> > pickedEdges;

	/**
		*  QLinkedList<osg::ref_ptr<Data::Edge> > pickedNeighborsNodes
		*  \brief picked neighbors nodes list
		*/
	QLinkedList<osg::ref_ptr<Data::Node> > pickedNeighborsNodes;

	/**
		*  QLinkedList<osg::ref_ptr<Data::Edge> > pickedNeighborsEdges
		*  \brief picked neighbors edges list
		*/
	QLinkedList<osg::ref_ptr<Data::Edge> > pickedNeighborsEdges;

	/**
		*  osg::ref_ptr group
		*  \brief custom node group
		*/
	osg::ref_ptr<osg::Group> group;


	/**
		*  bool isCtrlPressed
		*  \brief true, if ctrl was pressed
		*/
	bool isCtrlPressed;

	/**
		*  bool isShiftPressed
		*  \brief true, if shift was pressed
		*/
	bool isShiftPressed;

	/**
		*  bool isAltPressed
		*  \brief true, if alt was pressed
		*/
	bool isAltPressed;

	/**
		*  bool isDrawingSelectionQuad
		*  \brief true, if selection quad is benign drawn
		*/
	bool isDrawingSelectionQuad;

	/**
		*  bool isDragging
		*  \brief true, if user is dragging mouse
		*/
	bool isDragging;

	/**
		*  bool isManipulatingNodes
		*  \brief true, if user is manipulating nodes
		*/
	bool isManipulatingNodes;

	/**
		*  bool isNeighborsSelection
		*  \brief true, if are Neighbors selected
		*/
	bool isNeighborsSelection;

	/**
		*  osg::ref_ptr<osg::Geode> selectionQuad
		*  \brief selection quad geode
		*/
	osg::ref_ptr<osg::Geode> selectionQuad;


	/**
		*  \fn private  doSinglePick(osg::NodePath nodePath)
		*  \brief Picks single object on screen
		*  \param    nodePath   pick nodepath
		*  \return bool true, if object was picked
		*/
	bool doSinglePick( osg::NodePath nodePath );

	/**
		*  \fn private  doNodePick(osg::NodePath nodePath)
		*  \brief Picks single node
		*  \param   nodePath     pick nodepath
		*  \return bool true, if node was picked
		*/
	bool doNodePick( osg::NodePath nodePath );

	/**
		*  \fn private  doEdgePick(osg::NodePath nodePath)
		*  \brief Picks single edge
		*  \param     nodePath   pick nodepath
		*  \return bool
		*/
	bool doEdgePick( osg::NodePath nodePath );

	bool doClusterPick( osg::NodePath nodePath );

	/**
		*  \fn private  selectAllNeighbors(QLinkedList<osg::ref_ptr<Data::Node>> nodes)
		*  \brief Select all neighbors edges and theirs nodes of nodes collection in parameter
		*  \param     nodes   nodes for neighbors selection
		*/
	void selectAllNeighbors( QLinkedList<osg::ref_ptr<Data::Node > > nodes );

	/**
		*  \fn private  dragNode(osgViewer::Viewer * viewer)
		*  \brief Drags selected nodes
		*  \param viewer  current viewer
		*  \return bool true, if nodes were dragged
		*/
	bool dragNode( osgViewer::Viewer* viewer );


	/**
		*  int pickMode
		*  \brief current pick mode
		*/
	int pickMode;

	/**
		*  int selectionType
		*  \brief current selection type
		*/
	int selectionType;


	/**
		*  \fn private  drawSelectionQuad(float origin_mX, float origin_mY, osgViewer::Viewer * viewer)
		*  \brief draws selection quad
		*  \param   origin_mX     star position
		*  \param   origin_mY     end position
		*  \param   viewer    current viewer
		*/
	void drawSelectionQuad( float origin_mX, float origin_mY, osgViewer::Viewer* viewer );

	/**
		*  \fn private  setSelectedNodesInterpolation(bool state)
		*  \brief sets interpolation state to selected nodes
		*  \param      state     interpolation state
		*/
	void setSelectedNodesInterpolation( bool state );

	/**
		*  \fn private  handlePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles push event
		*  \param  ea    event adapter
		*  \param   aa    action adapter
		*  \return bool true, if event was handled
		*/
	bool handlePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

	/**
		*  \fn private  handleDoubleclick( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles doubleclick event
		*  \param  ea     event adapter
		*  \param  aa    action adapter
		*  \return bool true, if event was handled
		*/
	bool handleDoubleclick( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

	/**
		*  \fn private  handleDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles drag event
		*  \param   ea   event adapter
		*  \param   aa   action adapter
		*  \return bool true, if event was handled
		*/
	bool handleDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );


	/**
		*  \fn private  handleMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles move event
		*  \param   ea   event adapter
		*  \param   aa   action adapter
		*  \return bool true, if event was handled
		*/
	bool handleMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );


	/**
		*  \fn private  handleRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles release event
		*  \param    ea   event adapter
		*  \param  aa    action adapter
		*  \return bool true, if event was handled
		*/
	bool handleRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );


	/**
		*  \fn private  handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles keyup event
		*  \param  ea     event adapter
		*  \param  aa    action adapter
		*  \return bool true, if event was handled
		*/

	bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );


	/**
		*  \fn private  handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
		*  \brief handles keydown event
		*  \param  ea    event adapter
		*  \param  aa    action adapter
		*  \return bool true, if event was handled
		*/
	bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

	/**
		*  const osgGA::GUIEventAdapter * eaPush
		*  \brief	variable for storing event for detecting double click
		*/
	const osgGA::GUIEventAdapter* eaPush;

	/**
		*  const osgGA::GUIEventAdapter * eaRel
		*  \brief	variable for storing event for detecting double click
		*/
	const osgGA::GUIEventAdapter* eaRel;

	/**
		*  osgGA::GUIActionAdapter * aaPush
		*  \brief	variable for storing event for detecting double click
		*/
	osgGA::GUIActionAdapter* aaPush;

	/**
		*  osgGA::GUIActionAdapter * aaRel
		*  \brief	variable for storing event for detecting double click
		*/
	osgGA::GUIActionAdapter* aaRel;

	/**
		*  QTimer timer
		*  \brief timer for detecting double click
		*/
	QTimer* timer;

	QLinkedList<osg::ref_ptr<Data::Cluster> > pickedClusters;
	bool dragCluster( osgViewer::Viewer* viewer );
	bool handleScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

	bool isXPressed;
	bool isYPressed;
	bool isZPressed;

public slots:
	/**
		*	\fn public mouseTimerTimeout
		*	\brief called when user don't double click
		*/
	void mouseTimerTimeout();
};
}

#endif
