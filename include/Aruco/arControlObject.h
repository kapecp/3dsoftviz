#ifndef ARCONTROLOBJECT_H
#define ARCONTROLOBJECT_H

#include <osg/Vec3f>
#include "Data/Node.h"
#include "Core/Core.h"
#include <QTimer>
#include "Aruco/ArAssignmentStrategy.h"
#include "Aruco/ArSelectionLayoutStrategy.h"

namespace Data {
class Graph;
}
namespace ArucoModul {

//concrete objects
class ArControlObject : public QObject
{
	Q_OBJECT

public:
	ArControlObject( int id, osg::Vec3f position, ArAssignmentStrategy* _assignmentStrategy, ArSelectionLayoutStrategy* _selectionLayoutStrategy );
	void updatePosition( osg::Vec3f position );
	bool isFocused()
	{
		return this->focused;
	}
	void setNodeAssignmentStrategy( ArAssignmentStrategy* strategy )
	{
		this->_assignmentStrategy = strategy;
	}
	void setNodeBehaviourStrategy( ArSelectionLayoutStrategy* strategy )
	{
		if ( this->isFocused() ) {
			_selectionLayoutStrategy->resetSelectionLayout( this->focusedNode, false );
			this->_selectionLayoutStrategy = strategy;
			_selectionLayoutStrategy->setSelectionLayout( this->focusedNode );
		}
	}

public slots:
	void timerEvent();

private:
	int id;
	osg::Vec3f position;
	bool focused;

	QTimer* timer;

	osg::ref_ptr<Data::Node> focusedNode;


	ArAssignmentStrategy* _assignmentStrategy;
	ArSelectionLayoutStrategy* _selectionLayoutStrategy;
};


//object controller
class ArControlClass : public QObject
{

public:
	ArControlClass();
	void updateObjectPositionAruco( qlonglong object_id, QMatrix4x4 modelViewMatrix, bool reverse );
	void setNodeAssignmentStrategy( int strategy );
	void setNodeBehaviourStrategy( int strategy );
private:

	QOSG::ViewerQT* viewer;

	Vwr::CoreGraph* coreGraph;

	QMap<qlonglong, ArucoModul::ArControlObject*> controlObjects;

	ArAssignmentStrategy* _assignmentStrategy;
	ArSelectionLayoutStrategy* _selectionLayoutStrategy;
};

} // end ArucoModul namespace

#endif // ARCONTROLOBJECT_H
