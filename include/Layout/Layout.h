/**
*  Layout.h
*  Projekt 3DVisual
*/
#ifndef LAYOUT_LAYOUT_GENERATOR_DEF
#define LAYOUT_LAYOUT_GENERATOR_DEF 1

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <QString>

#include "igraph/igraph.h"
#include <osg/Vec3f>
#include "Viewer/DataHelper.h"
#include "Data/Edge.h"
#include "Data/Node.h"
#include "Data/Type.h"

#define COOL_EXP 1.5		//exponent chladnutia (teplota je maximalny posun uzla)
#define STEP_SIZE 1			//velkost kroku
#define STEP_COUNT 1000		//pocet krokov

namespace Layout
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \class	LayoutGenerator
	///
	/// \brief	Object generate graph layout by iGraph library. 
	///
	/// \author	Pavlik
	/// \date	13. 12. 2009
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class LayoutGenerator
	{
	public:		

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	LayoutGenerator(std::vector<Data::Node*> *in_nodes,std::vector<Data::Edge*> *in_edges,
		/// 	std::vector<Data::Type*> *in_types)
		///
		/// \brief	Constructor. 
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		///
		/// \param [in,out]	in_nodes	Nodes. 
		/// \param [in,out]	in_edges	Edges. 
		/// \param [in,out]	in_types	Types. 
		////////////////////////////////////////////////////////////////////////////////////////////////////

        LayoutGenerator(QMap<qlonglong, osg::ref_ptr<Data::Node> > *in_nodes,QMap<qlonglong, osg::ref_ptr<Data::Edge> > *in_edges,QMap<qlonglong, Data::Type*> *in_types);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	~LayoutGenerator(void)
		///
		/// \brief	Destructor.
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		////////////////////////////////////////////////////////////////////////////////////////////////////

		~LayoutGenerator(void);	

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	int InitFRLayout()
		///
		/// \brief	Initializies Fruchterman-Reingold algorithm. 
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		///
		/// \return	Zero, if initialization is succesfull. 
		////////////////////////////////////////////////////////////////////////////////////////////////////

		int InitFRLayout();	

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn	int InitTestCylinderFRLayout(int nodesInBottom, int height)
        ///
        /// \brief	Initializies FR algorithm and use graph of cylinder for testing.
        ///
        /// \author	Pavlik
        /// \date	13. 12. 2009
        ///
        /// \param	nodesInBottom	Number of nodes in bottom. 
		/// \param	height			Height of cylinder (number of nodes). 
        ///
        /// \return	Zero, if initialization is succesfull.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        int InitTestCylinderFRLayout(int nodesInBottom, int height);		        		

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	int GetNextStep()
		///
		/// \brief	Performs next iteration of algorithm.
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		///
		/// \return	Zero, if iteration is succesfull.
		////////////////////////////////////////////////////////////////////////////////////////////////////

		int GetNextStep();
	protected:
	private:

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * qnodes
		*  \brief Nodes.
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Node> > *qnodes;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * qedges
		*  \brief Edges.
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Edge> > *qedges;

		/**
		*  QMap<qlonglong,Data::Type*> * qtypes
		*  \brief Types.
		*/
		QMap<qlonglong, Data::Type*> *qtypes;


		/**
		*  igraph_matrix_t nodes_layout
		*  \brief iGraph nodes layout.
		*/
		igraph_matrix_t nodes_layout;

		/**
		*  igraph_t graph
		*  \brief iGraph data structure.
		*/
		igraph_t graph;	

		/**
		*  int step_index
		*  \brief Index of step.
		*/
		int step_index;		

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void createIgraph(igraph_t* graph)
		///
		/// \brief	Transforms data structure of application to iGraph data structure. 
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		///
		/// \param	graph	iGraph data structure. 
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void createIgraph(igraph_t* graph);	

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	int getFruchtermanReingoldLayout(bool isInit)
		///
		/// \brief	Performs next iteration of Fruchterman-Reingold algorithm.
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		///
		/// \param	isInit	True, if algorithm is initialized. 
		///
		/// \return	Zero, if iteration is succesfull.
		////////////////////////////////////////////////////////////////////////////////////////////////////

		int getFruchtermanReingoldLayout(bool isInit);		

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void printDataState()
		///
		/// \brief	Prints content of data structure. 
		///
		/// \author	Pavlik
		/// \date	13. 12. 2009
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void printDataState();
		
	};
}

# endif
