/**
*  CoreWindow.h
*  Projekt 3DVisual
*/
#ifndef QOSG_CORE_WINDOW
#define QOSG_CORE_WINDOW 1

#include <QMainWindow>
#include <QToolBar>
#include <QApplication>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QtGui>
#include <Qt/qstringlist.h>

#include "QOSG/OptionsWindow.h"
#include "QOSG/LoadGraphWindow.h"
#include "Viewer/CoreGraph.h"
#include "QOSG/CheckBoxList.h"
#include "QOSG/ViewerQT.h"
#include "Layout/LayoutThread.h"
#include "Manager/Manager.h"
#include "QOSG/qtcolorpicker.h"

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

			public slots:

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
				*  \fn public  saveGraph
				*  \brief Save loaded graph to database
				*/
				void saveGraphToDB();

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
				*  \fn public  add_EdgeClick
				*  \brief create edge between selected node
				*/
				bool add_EdgeClick();

				/**
				*  \fn public  add_NodeClick
				*  \brief create new Node
				*/
				bool add_NodeClick();

				/**
				*  \fn public  removeClick
				*  \brief remove all selected nodes and edges
				*/
				bool removeClick();
	private:

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
		*  \fn private  createToolBar
		*  \brief Create toolBar
		*/
		void createToolBar();	

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

		/**
		*  \fn inline public  setLayoutThread
		*  \brief	Set the layout thread 
		*  \param  val layout thread
		*/
		void setLayoutThread(Layout::LayoutThread * val) { layout = val; }

	};
}

#endif