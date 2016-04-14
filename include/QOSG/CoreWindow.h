/**
*  CoreWindow.h
*  Projekt 3DVisual
*/
#ifndef QOSG_CORE_WINDOW
#define QOSG_CORE_WINDOW 1

#include <Viewer/SelectionObserver.h>

#include "Layout/ShapeGetter.h"
#include "Layout/RestrictionRemovalHandler.h"
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"
#include "Layout/ShapeGetter_Circle_ByThreeNodes.h"
#include "Layout/ShapeGetter_SpherePlane_ByThreeNodes.h"
//#include "Viewer/CameraManipulator.h"

#include "Clustering/Clusterer.h"

#ifdef SPEECHSDK_FOUND
#include "Speech/KinectSpeechThread.h"
#endif

#ifdef FGLOVE_FOUND
#include "fglove.h"
#include "Fglove/FgloveThread.h"
#endif

#ifdef LEAP_FOUND
#include "Leap/LeapThread.h"
#endif

#include <qtcolorpicker.h>

#ifdef __APPLE__
#include <qstringlist.h>
#else
#include <Qt/qstringlist.h>
#endif
#include <QMainWindow>
#include <QToolBar>
#include <QApplication>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QtGui>
#include <QLineEdit>
#include <QToolBox>
#include <QString>

namespace Layout {
class LayoutThread;
}

namespace Vwr {
class CoreGraph;
class CameraManipulator;
}

namespace QOSG {

class ViewerQT;
}

namespace Network {
class Client;
//class Network;
}

namespace QOSG {
/**
    *  \class CoreWindow
    *  \brief Main window of aplication
    *  \author Adam Pazitnaj
    *  \date 3. 5. 2010
    */
class CoreWindow : public QMainWindow, public SelectionObserver
{
	Q_OBJECT
private:
#ifdef SPEECHSDK_FOUND
	Speech::KinectSpeechThread* mSpeechThr;
#endif

#ifdef FGLOVE_FOUND
	Fglove::FgloveThread* mGloveThr;
#endif

#ifdef LEAP_FOUND
	Leap::LeapThread* mLeapThr;
#endif

public slots:
	void moveMouseAruco( double positionX,double positionY,bool isClick, Qt::MouseButton button );

	/**
	            *  \fn public  showOptions
	            *  \brief Show the options of aplication
	            */
	void showOptions();

	/**
	            *  \fn public  showLoadGraph
	            *  \brief Show the dialog to load graph from database
	            */
	void showLoadGraph();

    void changeEvolutionVisualization( int  state );

    void changeEvolutionFilterOption( int state );

    void changeEvolutionFilterSpecificOption( int state );

	/**
	            *  \fn public  saveGraphToDB
	            *  \brief Save a current graph to database
	            */
	void saveGraphToDB();


	/**
	            *  \fn public  loadFunctionCall()
	            *  \brief load function call clicked
	            */
	void loadFunctionCall();

	/**
	            *  \fn public  browsersGroupingClicked()
	            *  \brief toggles between browsers grouping and sepparate browsers
	            *  \param checked    flag if button is checked
	            */
	void browsersGroupingClicked( bool checked );

	/**
	            *  \fn public  filterGraph()
	            *  \brief filter graph from query
	            */
	void filterGraph();

	/**
	            *  \fn public  saveLayoutToDB
	            *  \brief Save a current layout of current graph to database
	            */
	void saveLayoutToDB();

	/**
	            *  \fn public  sqlQuery
	            *  \brief Commit the sql in sql input
	            */
	void sqlQuery();

	void showMetrics();

	/**
	            *  \fn public  playPause
	            *  \brief Play or pause the layout algorithm
	            */
	void playPause();

	/**
	            *  \fn public  noSelectClicked(bool checked
	            *  \brief No-select mode selected
	            *  \param checked    flag if button is checked
	            */
	void noSelectClicked( bool checked );

	/**
	            *  \fn public  singleSelectClicked(bool checked
	            *  \brief Single-select mode selected
	            *  \param checked   flag if button is checked
	            */
	void singleSelectClicked( bool checked );

	/**
	            *  \fn public  multiSelectClicked(bool checked
	            *  \brief Multi-select mode selected
	            *  \param  checked     flag if button is checked
	            */
	void multiSelectClicked( bool checked );

	/**
	            *  \fn public  addMetaNode
	            *  \brief Add meta node
	            */
	void addMetaNode();

	/**
	            *  \fn public  centerView(bool checked
	            *  \brief Center the view
	            *  \param   checked     flag if button is checked
	            */
	void centerView( bool checked );

	/**
	            *  \fn public  fixNodes
	            *  \brief Fix selected nodes
	            */
	void fixNodes();

	/**
	            *  \fn public  unFixNodes
	            *  \brief Unfix all nodes
	            */
	void unFixNodes();

	/**
	            *  \fn public  mergeNodes
	            *  \brief Merge selected nodes together
	            */
	void mergeNodes();

	/**
	            *  \fn public  separateNodes
	            *  \brief separate selected merged nodes
	            */
	void separateNodes();

	/**
	            *  \fn public  removeMetaNodes
	            *  \brief Remove all meta nodes
	            */
	void removeMetaNodes();

	/**
	            *  \fn public  loadFile
	            *  \brief Show dialog to select file which will be opened
	            */
	void loadFile();

	/**
	            *  \fn public  loadFromGit
	            *  \brief Show dialog to write path to git repo which will be loaded
	            */
	void loadFromGit();

    /**
     * void loadLuaGraph()
     * @brief Load lua graph without visualization
     */
    void loadLuaGraph();

	/**
	            *  \fn public  labelOnOff(bool checked)
	            *  \brief Show / hide labels
	            *  \param  checked flag if button is checked
	            */
	void labelOnOff( bool checked );

	/**
	            *  \fn public  sliderValueChanged(int value)
	            *  \brief Slider value is changed
	            *  \param value actual value of slider
	            */
	void sliderValueChanged( int value );

	void clusteringDepthChanged( const QString& );
	void clusteringAlgorithmChanged( int value );
	void clustersOpacityCheckboxValueChanged( bool checked );
	void clusterSelectedOpacityCheckboxValueChanged( bool checked );
	void clustersOpacitySliderValueChanged( int value );
	void clustersShapeBoundarySliderValueChanged( int value );
	void repulsiveForceInsideClusterValueChanged( double value );
	void setRestriction_Cube_Selected();
	void cluster_nodes();
	// currentPosition nastavi na targetPosition - cim sposobi to, ze uzly vo vnutri obmedzovaca sa pekne nalayoutuju, pretoze uz nebudu tahane mimo neho kvoli targetPos
	void restartLayouting();

	//volovar_zac
	/**
	            *  \fn public  RadialLayoutSizeChanged
	            *  \brief size of radial layout is changed
	            *  \param size const
	            */
	void RadialLayoutSizeChanged( int value );

	/**
	            *  \fn public  RadialLayoutAlphaChanged
	            *  \brief alpha of radial layout is changed
	            *  \param alpha const
	            */
	void RadialLayoutAlphaChanged( int value );

	/**
	            *  \fn public  RadialLayoutSetVisibleSpheres
	            *  \brief number of visible spheres is changed
	            *  \param visibility const
	            */
	void RadialLayoutSetVisibleSpheres( int value );


	/**
	            *  \fn public  RadialLayoutSetForceScale
	            *  \brief multiplier of repulsive forces between two nodes in same radial layout and different layer
	            *  \param forceFactor const
	            */
	void RadialLayoutSetForceScale( int value );


	/**
	            *  \fn public  RadialLayoutSetForceSphereScale
	            *  \brief multiplier of repulsive forces between two nodes in same radial layout and layer
	            *  \param forceFactor const
	            */
	void RadialLayoutSetForceSphereScale( int value );

	//volovar_kon
	/**
	            *  \fn public  colorPickerChanged
	            *  \brief Color in colorpicker changed
	            *  \param color const    changed color
	            */
	void colorPickerChanged( const QColor& color );

	/**
	            *  \fn public  selectionTypeComboBoxChanged(int index)
	            *  \brief Type in combobox changed
	            *  \param  index
	            */
	void selectionTypeComboBoxChanged( int index );

	/**
	            *  \fn public  nodeTypeComboBoxChanged(int index)
	            *  \brief Type in combobox changed
	            *  \param  index
	            */
	void nodeTypeComboBoxChanged( int index );

	/**
	            *  \fn public  nodeTypeComboBoxChanged(int index)
	            *  \brief Type in combobox changed
	            *  \param  index
	            */
	void edgeTypeComboBoxChanged( int index );

	/**
	            *  \fn public  applyColorClick
	            *  \brief Apply selected color in colorpicker to selected node
	            */
	void applyColorClick();

	/**
	            *  \fn public  applyLabelClick
	            *  \brief Apply selected label to selected node
	            */
	void applyLabelClick();

	/**
	            * \brief Creates a new CylinderSurface restriction (defined by positions of 2 nodes) and sets
	            * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	            */
	void setRestriction_CylinderSurface();

	/**
	             * \brief Creates a new SphereSurface restriction (defined by positions of 2 nodes) and sets
	             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	             */
	void setRestriction_SphereSurface();

	/**
	             * \brief Creates a new Sphere restriction (defined by positions of 2 nodes) and sets
	             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	             */
	void setRestriction_Sphere();

	/**
	             * \brief Creates a new ConeSurface restriction (defined by positions of 2 nodes) and sets
	             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	             */
	void setRestriction_ConeSurface();

	/**
	             * \brief Creates a new Plane restriction (defined by positions of 3 nodes) and sets
	             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	             */
	void setRestriction_Plane( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = NULL );

	/**
	                             * \brief Creates a new Plane restriction (defined by positions of 3 nodes) with position according to nOfPlane and sets
	                             * it to all selected nodes (replacing any previously attached restrictions to these nodes).
	                             */
	Layout::ShapeGetter_Plane_ByThreeNodes* setRestriction_Plane_Vertigo( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = NULL, int nOfPlane = 0 );
	/**
	                             * \brief Creates a new restriction - composition sphere and plane (defined by positions of 3 nodes) and sets
	                             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	                             * \return Layout::ShapeGetter_Plane_ByThreeNodes *
	                             */
	void setRestriction_SpherePlane( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = NULL );

	/**
	                             * \brief Creates a new Circle restriction (defined by positions of 3 nodes) and sets
	                             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	                             */
	void setRestriction_Circle( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = NULL, osg::ref_ptr<Data::Node> node1 = NULL );

	/**
	                             * \brief Creates a new Cone restriction with node with maximum edge count as root and sets
	                             * if to all selected nodes (replacing any previously attached restrictions to these nodes).
	                             */
	void setRestriction_Cone( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = NULL,
							  osg::ref_ptr<Data::Node> parentNode = NULL );

	/**
	                             * \brief Creates a new Cone Tree restriction with selected node as root (all previously
	                             * attached restrictions are removed).
	                             */
	void setRestriction_ConeTree();

	/**
	             * \brief Removes restrictions from all selected nodes (if any has been set). Destroys a
	             * restriction (including manipulation nodes) if the last usage has been removed.
	             */
	//volovar_zac
	/**
	             * \brief Create a new Radial restriction for all selected nodes.
	 */
	void setRestriction_RadialLayout();

	/**
	 * @brief changeDrawMethod_RadialLayout change Drawing method of radial layout (SOLID, WIREFRAME)
	 */
	void changeDrawMethod_RadialLayout();

	/**
	 * @brief changeMode_RadialLayout switch between 2D and 3D mode of radial layout
	 */
	void changeMode_RadialLayout();
	//volovar_kon

	void unsetRestriction();

	/**
	             * \brief Removes restrictions from all nodes (if any has been set). Destroys a
	             * restriction (including manipulation nodes) if the last usage has been removed.
	             */
	void unsetRestrictionFromAll();

	/**
	             * \brief Start edge bundling.
	             */
	void startEdgeBundling();

	/**
	             * \brief Pause edge bundling.
	             */
	void pauseEdgeBundling();

	/**
	             * \brief Stop edge bundling.
	             */
	void stopEdgeBundling();

	/**
	            *  \fn public  add_EdgeClick
	            *  \brief create edge between selected node
	            */
	bool add_EdgeClick();

	/**
	            *  \fn public  add_NodeClick
	            *  \brief create new Node in GUI
	            */
	bool add_NodeClick();

	/**
	            *  \fn public  removeClick
	            *  \brief remove all selected nodes and edges
	            */
	bool removeClick();

	void start_server();
	void start_client();
	void send_message();
	void create_facewindow();

#ifdef OPENCV_FOUND
#ifdef OPENNI2_FOUND
	/**
	    *
	    * @brief create Kinect Button
	 */
	void createKinectWindow();
#endif
#endif

#ifdef SPEECHSDK_FOUND
	void startSpeech();
#endif

#ifdef LEAP_FOUND
	void startLeap();
#endif

#ifdef FGLOVE_FOUND
	void startGlovesRecognition();
#endif

	void toggleSpyWatch();
	void toggleAttention();
	void setAvatarScale( int scale );

	/**
	            *  \fn public  toggleVertigo
	            *  \brief toggles between normal and vertigo camera mode
	            */
	void toggleVertigo();
	/**
	            *  \fn public  create_Vertigo_Planes
	            *  \brief creates planes and restricts the nodes of current graph nodes according to their depth of spanning tree
	            */
	void create_Vertigo_Planes( int numberOfPlanes, int nOfDepthsInOnePlane, Data::GraphSpanningTree* spanningTree, int maxDepth, QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes );
	/**
	            *  \fn public  change_Vertigo_Planes_Distance
	            *  \brief changes the distance between the planes
	            */
	void change_Vertigo_Planes_Distance( int value );
	/**
	            *  \fn public  add_DistanceClick
	            *  \brief adds the distance between the planes
	            */
	void add_DistanceClick();
	/**
	            *  \fn public  subtract_DistanceClick
	            *  \brief subtracts the distance between the planes
	            */
	void subtract_DistanceClick();
	/**
	            *  \fn public  add_PlanesClick
	            *  \brief creates two more planes for vertigo mode
	            */
	void add_PlanesClick();
	/**
	            *  \fn public  remove_PlanesClick
	            *  \brief removes two planes for vertigo mode
	            */
	void remove_PlanesClick();

	void repulsive_Forces_ValueChanged();

	void setCameraEnable( bool enable );

	/**
	 * bool nextVersion()
	 * @brief Updates graph to the next version
	 * @return Returns true, if update was successful, otherwise false.
	 */
	bool nextVersion();

	/**
	 * bool previousVersion()
	 * @brief Updates graph to the previous version
	 * @return Returns true, if update was successful, otherwise false.
	 */
	bool previousVersion();

	/**
	 * void runEvolution()
	 * @brief Runs evolution of the graph
	 */
	void runEvolution();

	/**
	 * void move()
	 * @brief Moves the slider if evolution is running.
	 */
	void move();

    /**
     * void changeCommits( bool change );
     * @brief Change repository commit when changing versions
     * @param change True, if repository should change, otherwise false
     */
    void changeCommits( bool change );

	/**
	 * void fasterEvolution()
	 * @brief Sets up faster evolution
	 */
	void fasterEvolution();

	/**
	 * void slowerEvolution()
	 * @brief Sets up slower evolution
	 */
	void slowerEvolution();

	/**
	 * void getDiffInfo()
	 * @brief Get diff info for selected nodes
	 */
	void getDiffInfo();

	/**
	 * void showInfo()
	 * @brief Prints out info about current version in console
	 */
	void showInfo();

	/**
	 * void createEvolutionLuaGraph()
	 * @brief Creates evolution graph by using Lua graph
	 */
	void createEvolutionLuaGraph();

	/**
	 * void sliderVersionValueChanged( int value )
	 * @brief Changes graph version to the selected value on slider
	 * @param value Index of version to be vizualized
	 */
	void sliderVersionValueChanged( int value );

	/**
	 * void changeLifespan( int value )
	 * @brief Changes lifespan of removed nodes
	 * @param value Number of version removed nodes lives after repository removal.
	 */
	void changeLifespan( int value );

private:

	/**
	    *  QApplication * application
	    *  \brief Pointer to aplication
	    */
	QApplication* application;

	/**
	    *  QDockWidget * dock
	    *  \brief Pointer to dock widget
	    */
	QDockWidget* dock;

	/**
	    *  QLineEdit * lineEdit
	    *  \brief Pointer to sql input
	    */
	QLineEdit* lineEdit;

	/**
	    *  QToolBar * toolBar
	    *  \brief Pointer to toolbar
	    */
	QToolBar* toolBar;

	/**
	    *  QToolBar * metricsToolBar
	    *  \brief Pointer to toolbar
	    */
	QToolBar* metricsToolBar;

	/**
	    *  ToolBox * toolBox
	    *  \brief Pointer to toolbox
	    */
	QToolBox* toolBox;

	/**
	    *  QAction * quit
	    *  \brief Action to quit aplication
	    */
	QAction* quit;

	/**
	    *  QAction * options
	    *  \brief Pointer to option dialog
	    */
	QAction* options;

	/**
	    *  QAction * loadGraph
	    *  \brief Pointer to dialog to load graph from database
	    */
	QAction* loadGraph;

	/**
	    *  QAction * saveLayout
	    *  \brief Pointer to save layout to database
	    */
	QAction* saveLayout;

	/**
	    *  QAction * saveGraph
	    *  \brief Pointer to save graph to database
	    */
	QAction* saveGraph;

	/**
	     *  QAction * about
	    *  \brief Pointer to show info about software
	    */
	QAction* about;

	/**
	    *  QPushButton * play
	    *  \brief Action for play/pause layout
	    */
	QPushButton* play;

	QPushButton* showMetricsButton;

	/**
	    *  QPushButton * addMeta
	    *  \brief Action for adding meta node
	    */
	QPushButton* addMeta;

	/**
	    *  QPushButton * fix
	    *  \brief Action for fix selected nodes
	    */
	QPushButton* fix;

	/**
	    *  QPushButton * loadFunctionCallButton
	    *  \brief Action for loading function call graph from lua
	    */
	QPushButton* loadFunctionCallButton;

	/**
	    *  QPushButton * browsersGroupingButton
	    *  \brief Action for switching browsers grouping when nodes are selected
	    */
	QPushButton* browsersGroupingButton;

	/**
	    *  QLineEdit * filterNodesEdit
	    *  \brief Edit area for node filter query
	    */
	QLineEdit* filterNodesEdit;

	/**
	    *  QLineEdit * filterEdgesEdit
	    *  \brief Edit area for edge filter query
	    */
	QLineEdit* filterEdgesEdit;

	/**
	    *  QTreeView * luaGraphTreeView
	    *  \brief TreeView for exploring selected node of lua graph
	    */
	QTreeView* luaGraphTreeView;

	/**
	    *  QPushButton * unFix
	    *  \brief Action for unfix nodes
	    */
	QPushButton* unFix;

	/**
	    *  QPushButton * merge
	    *  \brief Action for merge selected nodes
	    */
	QPushButton* merge;

	/**
	    *  QPushButton * separate
	    *  \brief Action for separate nodes
	    */
	QPushButton* separate;

	/**
	    *  QPushButton * noSelect
	    *  \brief Action for no-select mode
	    */
	QPushButton* noSelect;

	/**
	    *  QPushButton * singleSelect
	    *  \brief Action for single-select mode
	    */
	QPushButton* singleSelect;

	/**
	    *  QPushButton * multiSelect
	    *  \brief Action for multi-select mode
	    */
	QPushButton* multiSelect;

	/**
	    *  QPushButton * center
	    *  \brief Action for center view
	    */
	QPushButton* center;

	/**
	    *  QPushButton * removeMeta
	    *  \brief Action for removing meta nodes
	    */
	QPushButton* removeMeta;

	/**
	    *  QPushButton * applyColor
	    *  \brief Action for applying color to selected nodes
	    */
	QPushButton* applyColor;

	/**
	    *  QPushButton * applyLabel
	    *  \brief Action for applying label to selected nodes
	    */
	QPushButton* applyLabel;

	QLineEdit* le_applyLabel;

	/**
	     * \brief Button for adding SphereSurface restriction.
	     */
	QPushButton* b_SetRestriction_SphereSurface;

	/**
	     * \brief Button for adding Sphere restriction.
	     */
	QPushButton* b_SetRestriction_Sphere;

	/**
	     * \brief Button for adding Plane restriction.
	     */
	QPushButton* b_SetRestriction_Plane;

	/**
	             * \brief Button for adding Sphere and Plane restriction.
	             */
	QPushButton* b_SetRestriction_SpherePlane;

	/**
	             * \brief Button for adding Circle restriction.
	             */
	QPushButton* b_SetRestriction_Circle;

	/**
	             * \brief Button for adding Cone restriction.
	             */
	QPushButton* b_SetRestriction_Cone;

	/**
	             * \brief Button for adding Cone Tree restriction.
	             */
	//volovar_zac
	/**
	 * @brief b_SetRestriction_RadialLayout Button for create new radial layout
	 */
	QPushButton* b_SetRestriction_RadialLayout;

	/**
	 * @brief b_drawMethod_RadialLayout Button for switch between drawing method (WIREFRAME, SOLID)
	 */

	QPushButton* b_drawMethod_RadialLayout;

	/**
	 * @brief b_mode_RadialLayout Button for switch between 2D and 3D mode
	 */
	QPushButton* b_mode_RadialLayout;
	//volovar_kon
	QPushButton* b_SetRestriction_ConeTree;
	/**
	     * \brief Button for removing restrictions.
	     */
	QPushButton* b_UnsetRestriction;

	/**
	     * \brief Button for adding CylinderSurface restriction.
	     */
	QPushButton* b_SetRestriction_CylinderSurface;

	/**
	     * \brief Spinbox for modifying CylinderSurface restriction.
	     */
	QSpinBox* b_SetRestriction_CylinderSurface_SpinBox;

	/**
	 * @brief Spin box for changing evolution lifespan of removed nodes
	 */
	QSpinBox* evolutionLifespanSpinBox;

	//volovar_zac
	/**
	 * @brief slider for change size of radial layout
	 */
	QSlider* b_SetRestriction_RadialLayout_Slider;

	/**
	 * @brief slider for change alpha of radial layout
	 */
	QSlider* b_SetAlpha_RadialLayout_Slider;

	/**
	 * @brief b_SetVisibleSpheres_RadialLayout_Slider change how many spheres are visible
	 */

	QSlider* b_SetVisibleSpheres_RadialLayout_Slider;

	/**
	 * @brief b_SetForceScale_RadialLayout_Slider slider for multiplier repulsive forces between two nodes in same radial layout, but different layer
	 */
	QSlider* b_SetForceScale_RadialLayout_Slider;

	/**
	 * @brief b_SetForceSphereScale_RadialLayout_Slider slider for multiplier repulsive forces between two nodes in same radial layout and layer
	 */
	QSlider* b_SetForceSphereScale_RadialLayout_Slider;

	//volovar_kon

	/**
	     * \brief Button for adding ConeSurface restriction.
	     */
	QPushButton* b_SetRestriction_ConeSurface;

	/**
	     * \brief Spinbox for modifying ConeSurface restriction.
	     */
	QSpinBox* b_SetRestriction_ConeSurface_SpinBox;

	/**
	     * \brief Button for removing restrictions from all nodes.
	     */
	QPushButton* b_UnsetRestrictionFromAll;

	/**
	    *  QAction * create new Edge
	    *  \brief Action for adding Edge
	    */
	QPushButton* add_Edge;

	/**
	    *  QAction * create new Node
	    *  \brief Action for adding Node
	    */
	QPushButton* add_Node;

	/**
	    *  QAction * remove
	    *  \brief Action for removing Nodes and Edges
	    */
	QPushButton* remove_all;

	/**
	 * QPushButton start face and mark recognition
	 *@brief b_start_face
	 */

	QPushButton* b_start_face;


	/**
	 * QPushButton start kinect recognition
	 *@brief b_start_kinect
	 */
	QPushButton* b_start_kinect;

	/**
	 * QPushButton start leap senzor
	 * @brief b_start_leap
	 */
	QPushButton* b_start_leap;

	/**
	 * QPushButton start speech recognition
	 *@brief b_start_speech
	 */
	QPushButton* b_start_speech;

	/**
	 * QPushButton start 5DT gloves recognition
	 *@brief b_start_gloves
	 */
	QPushButton* b_start_gloves;

	/**
	 * QPushButton* b_previous_version
	 * @brief Button which graph update to previous version
	 */
	QPushButton* b_previous_version;

	/**
	 * QPushButton* b_next_version
	 * @brief Button which signals graph update to next version
	 */
	QPushButton* b_next_version;

	/**
	 * QPushButton* b_run_evolution
	 * @brief Button which starts graph evolution
	 */
	QPushButton* b_run_evolution;

	/**
	 * QPushButton* b_info_version
	 * @brief Button which prints out info about current version
	 */
	QPushButton* b_info_version;

	/**
	 * QPushButton* b_faster_evolution
	 * @brief Button which sets up faster evolution
	 */
	QPushButton* b_faster_evolution;

	/**
	 * QPushButton* b_slower_evolution
	 * @brief Button which sets up slower evolution
	 */
	QPushButton* b_slower_evolution;

	/**
	 * QPushButton* b_git_diff;
	 * @brief Button which returns git diff about selected nodes
	 */
	QPushButton* b_git_diff;

	/**
	 * QLabel * labelEvolutionSlider
	 * @brief Shows current vizualized version
	 */
	QLabel* labelEvolutionSlider;

	/**
	 * QTimer * evolutionTimer
	 * @brief Timer which interval periodically invokes next version
	 */
	QTimer* evolutionTimer;

    /**
     *CheckBox for changes repository's commits
     *@brief chb_git_changeCommits
     */
    QCheckBox* chb_git_changeCommits;

    /**
     * QComboBox* cb_git_evoVisualizeMethod
     * @brief cb_git_evoVisualizeMethod
     */
    QComboBox* cb_git_evoVisualizeMethod;

    QComboBox* cb_git_authors;

    QComboBox* cb_git_files;

	bool isRunning;

	/**
	 *CheckBox for mapinulation camera or object
	 *@brief chb_camera_rot
	 */
	QCheckBox* chb_camera_rot;

	/**
	 *CheckBox for enabling camera
	 *@brief chb_camera_enable
	 */
	QCheckBox* chb_camera_enable;

	/**
	    *  QAction * load
	    *  \brief Action for loading file
	    */
	QAction* load;

	/**
	    * QAction* laodGit
	    * \brief Action for loading from git
	    */
	QAction* loadGit;

	/**
	    *  QPushButton * label
	    *  \brief Pointer to labelOn/labelOff button
	    */
	QPushButton* label;

	/**
	    *  QSlider * slider
	    *  \brief Pointer to slider
	    */
	QSlider* slider;

	/**
	 * QSlider* evolutionSlider
	 * @brief Slider for evolution graph
	 */
	QSlider* evolutionSlider;


	/**
	    *  QFrame * line1
	    *  \brief Pointer to first line in clustering tab
	    */
	QFrame* line1;

	/**
	    *  QFrame * line2
	    *  \brief Pointer to second line in clustering tab
	    */
	QFrame* line2;

	/**
	    *  QFrame * line3
	    *  \brief Pointer to third line in clustering tab
	    */
	QFrame* line3;

	/**
	    *  QMenu * file
	    *  \brief Pointer to file menu
	    */
	QMenu* file;

	/**
	    *  QMenu * edit
	    *  \brief Pointer to edit menu
	    */
	QMenu* edit;

	/**
	    *  QMenu * help
	    *  \brief Pointer to edit menu
	    */
	QMenu* help;

	/**
	    *  QOSG::ViewerQT * viewerWidget
	    *  \brief Ponter to viewer widget
	    */
	ViewerQT* viewerWidget;

	/**
	    *  Layout::LayoutThread * layout
	    *  \brief Pointer to layout thread
	    */
	Layout::LayoutThread* layout;

	/**
	    *  QComboBox * nodeTypeComboBox
	    *  \brief Pointer to comobox of node types
	    */
	QComboBox* nodeTypeComboBox;

	/**
		*  bool isPlaying
		*  \brief Flag if layout is running
		*/
	bool isPlaying;

	/**
	    *  \fn private  createActions
	    *  \brief Initialize all actions of aplication
	    */
	void createActions();

	/**
	    *  \fn private  createMenus
	    *  \brief Create menu of aplication
	    */
	void createMenus();

	/**
	    *  \fn private  createLeftToolBar
	    *  \brief Create left toolBar
	    */
	void createLeftToolBar();

	/**
	    *  \fn private  createMetricsToolBar
	    *  \brief Create metrics toolBar
	    */
	void createMetricsToolBar();

	/**
	    *  \fn private  createHorizontalFrame
	    *  \brief Crate frame with horizontal label
	    *  \return QFrame * created frame
	    */
	QFrame* createHorizontalFrame();

	/**
	    *  \fn private  createLine
	    *  \brief Creates horizontal line
	    *  \return QFrame * created line
	    */
	QFrame* createLine();

	/**
	    *  \fn private  createGroupLayout
	    *  \brief Creates layout for grouping elements
	    *  \return QFormLayout * created layout
	    */
	QFormLayout* createGroupLayout();

	/**
	    *  \fn private  createGroupBox
	    *  \brief Creates group box for grouping elements
	    *  \return createGroupBox * created group box
	    */
	QGroupBox* createGroupBox( QString name );

	/**
	    *  Vwr::CoreGraph * coreGraph
	    *  \brief Pointer to CoreGraph
	    */
	Vwr::CoreGraph* coreGraph;

	/**
	    *  bool edgeLabelsVisible
	    *  \brief Flag if edges are visible
	    */
	bool edgeLabelsVisible;

	/**
	    *  bool nodeLabelsVisible
	    *  \brief Flag if labels are visible
	    */
	bool nodeLabelsVisible;

	/**
	    *  QColor color
	    *  \brief Selected color in colorpicker
	    */
	QColor color;

	QPushButton* b_send_message;
	QLineEdit* le_message;
	QSlider* sl_avatarScale;

	Network::Client* client;

	/**
	    *  int numberOfPlanes
	    *  \brief number of paralell planes created for vertigo mode
	    */
	int numberOfPlanes;

	/**
	    *  int deltaVertigoDistance
	    *  \brief delta number for changing the plane distance in vertigo mode
	    */
	int deltaVertigoDistance;

	/**
	    *  int vertigoPlanesDistance
	    *  \brief distance between two planes in vertigo mode
	    */
	int vertigoPlanesDistance;

	/**
	    *  int vertigoPlanesMinDistance
	    *  \brief minimal distance between two planes in vertigo mode
	    */
	int vertigoPlanesMinDistance;

	/**
	    *  int vertigoPlanesMaxDistance
	    *  \brief maximal distance between two planes in vertigo mode
	    */
	int vertigoPlanesMaxDistance;

	QLabel* l_clustersOpacity;
	QCheckBox* chb_clustersOpacity;
	QCheckBox* chb_clusterSelectedOpacity;
	QSlider* b_clustersOpacity_Slider;
	QLabel* l_clustersShapes;
	QLabel* l_clusters1Min;
	QLabel* l_clusters1Max;
	QSlider* b_clustersShapeBoundary_Slider;
	QPushButton* b_cluster_nodes;
	QPushButton* b_restartLayouting;
	QComboBox* cb_clusteringAlgorithm;
	QLineEdit* le_clusteringDepth;
	QProgressDialog* clusteringProgressBar;

	QPushButton* b_SetRestriction_Cube_Selected;

	QLabel* l_repulsiveForceInsideCluster;
	QDoubleSpinBox* sb_repulsiveForceInsideCluster;

	void setVisibleClusterSection( bool visible );

	/**
	     * \brief Button for starting edge bundling.
	     */
	QPushButton* b_StartEdgeBundling;

	/**
	     * \brief Button for pausing edge bundling.
	     */
	QPushButton* b_PauseEdgeBundling;

	/**
	     * \brief Button for stoping edge bundling.
	     */
	QPushButton* b_StopEdgeBundling;

	/**
	     * \brief LineEdit for modifying edge bundling. It defines strength of edge bundling forces.
	     */
	QLineEdit* le_edgeBundlingalpha;

	/**
	    *  QComboBox * selectionTypeComboBox
	    *  \brief Pointer to comobox of selection types
	    */
	QComboBox* selectionTypeComboBox;

	/**
	    *  QComboBox * edgeTypeComboBox
	    *  \brief Pointer to comobox of edge types
	    */
	QComboBox* edgeTypeComboBox;

	/**
		*  bool isEBPlaying
		*  \brief Flag if edge bundling is running
		*/
	bool isEBPlaying;

public:

	void setRepulsiveForceInsideCluster( double repulsiveForceInsideCluster );
	void hideRepulsiveForceSpinBox();

	/*!
	    *
	    * \param parent
	    * Rodic okna.
	    *
	    * \param coreGraph
	    * Graf, ktory bude zobrazovany.
	    *
	    * \param app
	    * Qt aplikacia.
	    *
	    * Constructor
	    *
	    */
	CoreWindow( QWidget* parent = 0, Vwr::CoreGraph* coreGraph = 0, QApplication* app = 0, Layout::LayoutThread* thread = 0 );

	/**
	    *  \fn public  addSQLInput
	    *  \brief Show sql input
	    */
	void addSQLInput();

	/**
	    *  \fn inline public constant  getLayoutThread
	    *  \brief Get the layout thread
	    *  \return Layout::LayoutThread *
	    */
	Layout::LayoutThread* getLayoutThread() const
	{
		return layout;
	}
	bool playing()
	{
		return isPlaying;
	}
	bool playingEB()
	{
		return isEBPlaying;
	}
	Vwr::CameraManipulator* getCameraManipulator();

	QOSG::ViewerQT* GetViewerQt();


	/**
	    *  \fn inline public  setLayoutThread
	    *  \brief	Set the layout thread
	    *  \param  val layout thread
	    */
	void setLayoutThread( Layout::LayoutThread* val )
	{
		layout = val;
	}

	QLineEdit* le_client_name;
	QLineEdit* le_server_addr;
	QPushButton* b_start_client;
	QPushButton* b_start_server;
	QListWidget* lw_users;
	QCheckBox* chb_spy;
	QCheckBox* chb_center;
	QCheckBox* chb_attention;
	QCheckBox* chb_vertigo;
	QPushButton* add_Distance;
	QPushButton* subtract_Distance;
	QPushButton* add_Planes;
	QPushButton* remove_Planes;
	QSpinBox* change_Forces;

	static ViewerQT* getViewerWidget();

	QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*> planes_Vertigo;

	/**
	     * \brief Gets selected nodes and sets the restriction defined by shapeGetter to these nodes.
	     */
	void setRestrictionToSelectedNodes(
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		Data::Graph* currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
		QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = NULL
	);

	void setRestrictionToShape(
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		Data::Graph* currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
		QLinkedList<osg::ref_ptr<Data::Node> > nodesOfShapeGettersToRestrict
	);

	/**
	     * @author Autor: David Durcak
	     * @brief getChbCameraRot Return chb_camera_rot
	     */
	QCheckBox* getChbCameraRot() const;

	/**
	 * @author Peter Mendel
	 * @brief createGraphTab add elements to QWidget for graph functionality
	 * @param line pointer to add line
	 * @return QWidget for graph functionality
	 */
	QWidget* createGraphTab( QFrame* line );

	/**
	 * @author Peter Mendel
	 * @brief createConstraintsTab add elements to QWidget for constraints functionality
	 * @param line pointer to add line
	 * @return QWidget for constraints functionality
	 */
	QWidget* createConstraintsTab( QFrame* line );

	/**
	 * @author Peter Mendel
	 * @brief createConnectionsTab add elements to QWidget for connections functionality
	 * @param line pointer to add line
	 * @return QWidget for connections functionality
	 */
	QWidget* createConnectionsTab( QFrame* line );

	/**
	 * @author Peter Mendel
	 * @brief createClusteringTab add elements to QWidget for clustering functionality
	 * @param line pointer to add line
	 * @return QWidget for clustering functionality
	 */
	QWidget* createClusteringTab( QFrame* line );

	/**
	 * @author Peter Mendel
	 * @brief createMoreFeaturesTab add elements to QWidget for more features functionality
	 * @param line pointer to add line
	 * @return QWidget for clustering functionality
	 */
	QWidget* createMoreFeaturesTab( QFrame* line );

    /**
     * @author Michael Garaj
     * @brief createEvolutionTab add elements to QWidget for evolution functionality
     * @param line pointer to add line
     * @return QWidget for evolution functionality
     */
    QWidget* createEvolutionTab( QFrame* line );

	/**
	 * @author Peter Mendel
	 * @brief createColorPicker initialize color picker
	 * @return pointer of color picker
	 */
	QtColorPicker* createColorPicker();

	/**
	 * @brief createGraphSlider initialize slider
	 */
	void createGraphSlider();

	/**
	 * @brief createSelectionComboBox initialize combobox for selecting parts of graph
	 * available options are node,edge, cluster, all
	 */
	void createSelectionComboBox();

	/**
	 * @brief circleAndConeGroup creates group for control elements for circle and cone
	 * and inserts these elements into the group
	 * @return created group
	 */
	QGroupBox* circleAndConeGroup();

	/**
	 * @brief sphereAndPlaneGroup creates group for control elements for sphere and plane
	 * and inserts these elements into the group
	 * @return created group
	 */
	QGroupBox* sphereAndPlaneGroup();

	/**
	 * @brief cylinderAndConeSurfaceGroup creates group for control elements for cylinder and cone surface
	 * and inserts these elements into the group
	 * @return created group
	 */
	QGroupBox* cylinderAndConeSurfaceGroup();

	/**
	 * @brief radialLayoutGroup creates group for control elements for radial layout
	 * and inserts these elements into the group
	 * @return created group
	 */
	QGroupBox* radialLayoutGroup();

protected:

	/**
	     * @author Autor: David Durcak
	     * @brief closeEvent Reimplement close event. Call destructor on OpenCVCore
	     * @param event Close event
	     */
	void closeEvent( QCloseEvent* event );


private:

	/**
	     * \brief Gets all nodes and sets the restriction defined by shapeGetter to these nodes.
	     */
	void setRestrictionToAllNodes(
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		Data::Graph* currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler
	);

	void onChange();

};
}

#endif
