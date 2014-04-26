/**
*  CoreWindow.h
*  Projekt 3DVisual
*/
#ifndef QOSG_CORE_WINDOW
#define QOSG_CORE_WINDOW 1

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

#include "Layout/ShapeGetter.h"
#include "Layout/RestrictionRemovalHandler.h"
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"
#include "Layout/ShapeGetter_Circle_ByThreeNodes.h"
#include "Layout/ShapeGetter_SpherePlane_ByThreeNodes.h"
//#include "Viewer/CameraManipulator.h"
#ifdef SPEECHSDK_FOUND
#include "Speech/KinectSpeechThread.h"
#endif

namespace Layout
{
class LayoutThread;
}

namespace Vwr
{
class CoreGraph;
class CameraManipulator;
}

namespace QOSG
{

class ViewerQT;
}

namespace Network
{
class Client;
//class Network;
}

namespace QOSG
{
/**
	*  \class CoreWindow
	*  \brief Main window of aplication
	*  \author Adam Pazitnaj
	*  \date 3. 5. 2010
	*/
class CoreWindow : public QMainWindow
{
	Q_OBJECT
private:
#ifdef SPEECHSDK_FOUND
	Speech::KinectSpeechThread *mSpeechThr;
#endif
public slots:

	void moveMouseAruco(double positionX,double positionY,bool isClick, Qt::MouseButton button );

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

	/**
				*  \fn public  saveGraphToDB
				*  \brief Save a current graph to database
				*/
	void saveGraphToDB();


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
	void noSelectClicked(bool checked);

	/**
				*  \fn public  singleSelectClicked(bool checked
				*  \brief Single-select mode selected
				*  \param checked   flag if button is checked
				*/
	void singleSelectClicked(bool checked);

	/**
				*  \fn public  multiSelectClicked(bool checked
				*  \brief Multi-select mode selected
				*  \param  checked     flag if button is checked
				*/
	void multiSelectClicked(bool checked);

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
	void centerView(bool checked);

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
				*  \fn public  labelOnOff(bool checked)
				*  \brief Show / hide labels
				*  \param  checked flag if button is checked
				*/
	void labelOnOff(bool checked);

	/**
				*  \fn public  sliderValueChanged(int value)
				*  \brief Slider value is changed
				*  \param value actual value of slider
				*/
	void sliderValueChanged(int value);

	/**
				*  \fn public  colorPickerChanged
				*  \brief Color in colorpicker changed
				*  \param color const    changed color
				*/
	void colorPickerChanged(const QColor & color);

	/**
				*  \fn public  nodeTypeComboBoxChanged(int index)
				*  \brief Type in combobox changed
				*  \param  index
				*/
	void nodeTypeComboBoxChanged(int index);

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
	void setRestriction_SphereSurface ();

	/**
				 * \brief Creates a new Sphere restriction (defined by positions of 2 nodes) and sets
				 * if to all selected nodes (replacing any previously attached restrictions to these nodes).
				 */
	void setRestriction_Sphere ();

	/**
				 * \brief Creates a new ConeSurface restriction (defined by positions of 2 nodes) and sets
				 * if to all selected nodes (replacing any previously attached restrictions to these nodes).
				 */
	void setRestriction_ConeSurface();

	/**
				 * \brief Creates a new Plane restriction (defined by positions of 3 nodes) and sets
				 * if to all selected nodes (replacing any previously attached restrictions to these nodes).
				 */
	void setRestriction_Plane (QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict = NULL);

	/**
								 * \brief Creates a new restriction - composition sphere and plane (defined by positions of 3 nodes) and sets
								 * if to all selected nodes (replacing any previously attached restrictions to these nodes).
								 */
	void setRestriction_SpherePlane (QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict = NULL);

	/**
								 * \brief Creates a new Circle restriction (defined by positions of 3 nodes) and sets
								 * if to all selected nodes (replacing any previously attached restrictions to these nodes).
								 */
	void setRestriction_Circle(QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict = NULL, osg::ref_ptr<Data::Node> node1 = NULL);

	/**
								 * \brief Creates a new Cone restriction with node with maximum edge count as root and sets
								 * if to all selected nodes (replacing any previously attached restrictions to these nodes).
								 */
	void setRestriction_Cone(QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict = NULL,
							 osg::ref_ptr<Data::Node> parentNode = NULL);

	/**
								 * \brief Creates a new Cone Tree restriction with selected node as root (all previously
								 * attached restrictions are removed).
								 */
	void setRestriction_ConeTree ();

	/**
				 * \brief Removes restrictions from all selected nodes (if any has been set). Destroys a
				 * restriction (including manipulation nodes) if the last usage has been removed.
				 */
	void unsetRestriction ();

	/**
				 * \brief Removes restrictions from all nodes (if any has been set). Destroys a
				 * restriction (including manipulation nodes) if the last usage has been removed.
				 */
	void unsetRestrictionFromAll();

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
#ifdef NITE2_FOUND
	/**
		*
		* @brief create Kinect Button
	 */
	void createKinectWindow();

	/**
	 * @brief calculateRansac
	 */
	void calculateRansac();

#endif
#endif
#endif

#ifdef SPEECHSDK_FOUND
	void startSpeech();
#endif

	void toggleSpyWatch();
	void toggleAttention();

	void setAvatarScale(int scale);
private:

	bool mIsClicAruco;

	/**
		*  QApplication * application
		*  \brief Pointer to aplication
		*/
	QApplication * application;

	/**
		*  QDockWidget * dock
		*  \brief Pointer to dock widget
		*/
	QDockWidget * dock;

	/**
		*  QLineEdit * lineEdit
		*  \brief Pointer to sql input
		*/
	QLineEdit * lineEdit;

	/**
		*  QToolBar * toolBar
		*  \brief Pointer to toolbar
		*/
	QToolBar * toolBar;

	/**
		*  QAction * quit
		*  \brief Action to quit aplication
		*/
	QAction * quit;

	/**
		*  QAction * options
		*  \brief Pointer to option dialog
		*/
	QAction * options;

	/**
		*  QAction * loadGraph
		*  \brief Pointer to dialog to load graph from database
		*/
	QAction * loadGraph;

	/**
		*  QAction * saveLayout
		*  \brief Pointer to save layout to database
		*/
	QAction * saveLayout;

	/**
		*  QAction * saveGraph
		*  \brief Pointer to save graph to database
		*/
	QAction * saveGraph;


	/**
		*  QPushButton * play
		*  \brief Action for play/pause layout
		*/
	QPushButton * play;

	/**
		*  QPushButton * addMeta
		*  \brief Action for adding meta node
		*/
	QPushButton * addMeta;

	/**
		*  QPushButton * fix
		*  \brief Action for fix selected nodes
		*/
	QPushButton * fix;

	/**
		*  QPushButton * unFix
		*  \brief Action for unfix nodes
		*/
	QPushButton * unFix;

	/**
		*  QPushButton * merge
		*  \brief Action for merge selected nodes
		*/
	QPushButton * merge;

	/**
		*  QPushButton * separate
		*  \brief Action for separate nodes
		*/
	QPushButton * separate;

	/**
		*  QPushButton * noSelect
		*  \brief Action for no-select mode
		*/
	QPushButton * noSelect;

	/**
		*  QPushButton * singleSelect
		*  \brief Action for single-select mode
		*/
	QPushButton * singleSelect;

	/**
		*  QPushButton * multiSelect
		*  \brief Action for multi-select mode
		*/
	QPushButton * multiSelect;

	/**
		*  QPushButton * center
		*  \brief Action for center view
		*/
	QPushButton * center;

	/**
		*  QPushButton * removeMeta
		*  \brief Action for removing meta nodes
		*/
	QPushButton * removeMeta;

	/**
		*  QPushButton * applyColor
		*  \brief Action for applying color to selected nodes
		*/
	QPushButton * applyColor;

	/**
		*  QPushButton * applyLabel
		*  \brief Action for applying label to selected nodes
		*/
	QPushButton * applyLabel;

	QLineEdit * le_applyLabel;

	/**
		 * \brief Button for adding SphereSurface restriction.
		 */
	QPushButton * b_SetRestriction_SphereSurface;

	/**
		 * \brief Button for adding Sphere restriction.
		 */
	QPushButton * b_SetRestriction_Sphere;

	/**
		 * \brief Button for adding Plane restriction.
		 */
	QPushButton * b_SetRestriction_Plane;

	/**
				 * \brief Button for adding Sphere and Plane restriction.
				 */
	QPushButton * b_SetRestriction_SpherePlane;

	/**
				 * \brief Button for adding Circle restriction.
				 */
	QPushButton * b_SetRestriction_Circle;

	/**
				 * \brief Button for adding Cone restriction.
				 */
	QPushButton * b_SetRestriction_Cone;

	/**
				 * \brief Button for adding Cone Tree restriction.
				 */
	QPushButton * b_SetRestriction_ConeTree;

	/**
		 * \brief Button for removing restrictions.
		 */
	QPushButton * b_UnsetRestriction;

	/**
		 * \brief Button for adding CylinderSurface restriction.
		 */
	QPushButton * b_SetRestriction_CylinderSurface;

	/**
		 * \brief Spinbox for modifying CylinderSurface restriction.
		 */
	QSlider *b_SetRestriction_CylinderSurface_Slider;

	/**
		 * \brief Button for adding ConeSurface restriction.
		 */
	QPushButton * b_SetRestriction_ConeSurface;

	/**
		 * \brief Spinbox for modifying ConeSurface restriction.
		 */
	QSlider *b_SetRestriction_ConeSurface_Slider;

	/**
		 * \brief Button for removing restrictions from all nodes.
		 */
	QPushButton *b_UnsetRestrictionFromAll;

	/**
		*  QAction * create new Edge
		*  \brief Action for adding Edge
		*/
	QPushButton * add_Edge;

	/**
		*  QAction * create new Node
		*  \brief Action for adding Node
		*/
	QPushButton * add_Node;

	/**
		*  QAction * remove
		*  \brief Action for removing Nodes and Edges
		*/
	QPushButton * remove_all;

	/**
	 * QPushButton start face and mark recognition
	 *@brief b_start_face
	 */

	QPushButton * b_start_face;


	/**
	 * QPushButton start kinect recognition
	 *@brief b_start_kinect
	 */
	QPushButton * b_start_kinect;

	/**
	 * @brief Button for start calculate Ransac Surface
	 */
	QPushButton * b_start_ransac;

	/**
	 * QPushButton start speech recognition
	 *@brief b_start_speech
	 */
	QPushButton * b_start_speech;


	/*
	 *CheckBox for mapinulation camera or object
	 *@brief chb_camera_rot
	 */
	QCheckBox * chb_camera_rot;




	/**
		*  QAction * load
		*  \brief Action for loading file
		*/
	QAction * load;

	/**
		*  QPushButton * label
		*  \brief Pointer to labelOn/labelOff button
		*/
	QPushButton * label;

	/**
		*  QSlider * slider
		*  \brief Pointer to slider
		*/
	QSlider * slider;

	/**
		*  QMenu * file
		*  \brief Pointer to file menu
		*/
	QMenu * file;

	/**
		*  QMenu * edit
		*  \brief Pointer to edit menu
		*/
	QMenu * edit;

	/**
		*  QOSG::ViewerQT * viewerWidget
		*  \brief Ponter to viewer widget
		*/
	ViewerQT * viewerWidget;

	/**
		*  Layout::LayoutThread * layout
		*  \brief Pointer to layout thread
		*/
	Layout::LayoutThread * layout;

	/**
		*  QComboBox * nodeTypeComboBox
		*  \brief Pointer to comobox of node types
		*/
	QComboBox * nodeTypeComboBox;

	/**
		*  int isPlaying
		*  \brief Flag if layout is running
		*/
	int isPlaying;

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
		*  \fn private  createRightToolBar
		*  \brief Create right toolBar
		*/
	void createRightToolBar();

	/**
		*  \fn private  createCollaborationToolBar
		*  \brief Create collaboration toolBar
		*/
	void createCollaborationToolBar();

	/**
		*  \fn private  createAugmentedRealityToolBar
		*  \brief Create Augmented Reality ToolBar
		*/
	void createAugmentedRealityToolBar();

	/**
		*  \fn private  createHorizontalFrame
		*  \brief Crate frame with horizontal label
		*  \return QFrame * created frame
		*/
	QFrame * createHorizontalFrame();

	/**
		*  Vwr::CoreGraph * coreGraph
		*  \brief Pointer to CoreGraph
		*/
	Vwr::CoreGraph * coreGraph;

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

	QPushButton * b_send_message;
	QLineEdit * le_message;
	QSlider * sl_avatarScale;

	Network::Client * client;

public:

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
	CoreWindow(QWidget *parent = 0, Vwr::CoreGraph* coreGraph = 0, QApplication* app = 0, Layout::LayoutThread * thread = 0 );

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
	Layout::LayoutThread * getLayoutThread() const { return layout; }
	bool playing() { return isPlaying; }
	Vwr::CameraManipulator * getCameraManipulator();

	QOSG::ViewerQT * GetViewerQt();


	/**
		*  \fn inline public  setLayoutThread
		*  \brief	Set the layout thread
		*  \param  val layout thread
		*/
	void setLayoutThread(Layout::LayoutThread * val) { layout = val; }

	QLineEdit * le_client_name;
	QLineEdit * le_server_addr;
	QPushButton * b_start_client;
	QPushButton * b_start_server;
	QListWidget * lw_users;
	QCheckBox *chb_spy;
	QCheckBox *chb_center;
	QCheckBox *chb_attention;


	/**
		 * \brief Gets selected nodes and sets the restriction defined by shapeGetter to these nodes.
		 */
	void setRestrictionToSelectedNodes (
			QSharedPointer<Layout::ShapeGetter> shapeGetter,
			Data::Graph * currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
			QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict = NULL
			);

	void setRestrictionToShape(
			QSharedPointer<Layout::ShapeGetter> shapeGetter,
			Data::Graph * currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
			QLinkedList<osg::ref_ptr<Data::Node> > nodesOfShapeGettersToRestrict
			);

	/**
		 * @author Autor: David Durcak
		 * @brief getChbCameraRot Return chb_camera_rot
		 */
	QCheckBox *getChbCameraRot() const;

protected:

	/**
		 * @author Autor: David Durcak
		 * @brief closeEvent Reimplement close event. Call destructor on OpenCVCore
		 * @param event Close event
		 */
	void closeEvent(QCloseEvent *event);

private:

	/**
		 * \brief Gets all nodes and sets the restriction defined by shapeGetter to these nodes.
		 */
	void setRestrictionToAllNodes (
			QSharedPointer<Layout::ShapeGetter> shapeGetter,
			Data::Graph * currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler
			);

};
}

#endif
