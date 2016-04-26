#ifndef MATRIXVIEWER_H
#define MATRIXVIEWER_H 1


#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgQt/GraphicsWindowQt>
#include <qtimer.h>
#include <QObject>
#include <QGridLayout>

#include "Data/Graph.h"
#include "Data/Node.h"
#include "Data/GraphLayout.h"
#include "Viewer/CoreGraph.h"

namespace SpecialMatrix {
/**
	*  \class SpacialMatrix
	*  \brief
	*  \author Denis Illes
	*  \date 13. 4. 2016
	*/
class MatrixViewer : public QWidget, public osgViewer::CompositeViewer
{


public:

	/*!
		*
		* \param parent
		* Rodic widgetu.
		*
		* \param f
		* Znacky pre vytvaranie okna.
		*
		*
		* Vytvori widget, ktory dokaze zobrazit specialne matice.
		*
		*/

	MatrixViewer( QWidget* parent = 0, Qt::WindowFlags f = 0, Vwr::CoreGraph* cg = 0 );

	QWidget* addViewWidget( osgQt::GraphicsWindowQt* gwQt, osg::Group* scene );

	osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );

	osg::Group* SpecialMatrix::MatrixViewer::createScene();

	osg::Drawable* SpecialMatrix::MatrixViewer::createAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir);

	virtual void paintEvent( QPaintEvent* event ) { frame(); }

protected:

	QTimer _timer;

	osg::ref_ptr<osgQt::GraphicsWindowQt> _gwQt;

	Data::Graph* simpleGraph;
};
}

#endif
