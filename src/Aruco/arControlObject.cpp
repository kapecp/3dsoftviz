#include "Aruco/arControlObject.h"
#include "Util/ApplicationConfig.h"
#include <QDebug>
#include <QMap>
#include <QMatrix>

#include "Manager/Manager.h"
#include "Data/Graph.h"
#include "Data/Node.h"
#include "Core/Core.h"




namespace ArucoModul {


ArControlObject::ArControlObject( int id, osg::Vec3f position, qlonglong nodeToPick )
{
    this->id = id;
    this->nodeToPick = nodeToPick;
    this->position = position;
    this->focused = false;
}

void ArControlObject::updatePosition( osg::Vec3f position ){
    this->position = position;
   // qDebug() << position.x() << " / " << position.y() << " / " << position.z();

    if ( !this->focused ){
        Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
        QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();
        QList<qlonglong> keys = allNodes->keys();

        // JMA tmp doaround
        this->focusedNode = allNodes->value( keys.at(this->nodeToPick) );

        this->focused = true;
        this->focusedNode->setColor( osg::Vec4( 0.0f,1.0f,1.0f,0.5f ) );

        this->focusedNode -> setUsingInterpolation( false );
    }

    this->focusedNode->setTargetPosition(this->position);

}




ArControlClass::ArControlClass()
{
}

void ArControlClass::updateObjectPositionAruco( qlonglong object_id, QMatrix4x4 modelViewMatrix ){
    QOSG::ViewerQT* viewer;
    viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

    //transform matrix
    osg::Matrixd markerMVM( modelViewMatrix.operator()( 0,0 ),modelViewMatrix.operator()( 0,1 ),modelViewMatrix.operator()( 0,2 ),modelViewMatrix.operator()( 0,3 ),
                            modelViewMatrix.operator()( 1,0 ),modelViewMatrix.operator()( 1,1 ),modelViewMatrix.operator()( 1,2 ),modelViewMatrix.operator()( 1,3 ),
                            modelViewMatrix.operator()( 2,0 ),modelViewMatrix.operator()( 2,1 ),modelViewMatrix.operator()( 2,2 ),modelViewMatrix.operator()( 2,3 ),
                            modelViewMatrix.operator()( 3,0 ),modelViewMatrix.operator()( 3,1 ),modelViewMatrix.operator()( 3,2 ),modelViewMatrix.operator()( 3,3 ) );

    osg::Matrixd baseMVM = viewer->getCamera()->getViewMatrix();
    osg::Matrixd transMVM = markerMVM.operator *(baseMVM.inverse(baseMVM));

    osg::Vec3f targetPosition = transMVM.getTrans();

    // ani srnky netusia preco /2 ... ale takto to ide :D :D --- TMP JMA
    targetPosition.set( osg::Vec3f(targetPosition.x()/2, targetPosition.y()/2,targetPosition.z()/2) );

    if( controlObjects.value(object_id) == NULL ){
        controlObjects.insert(object_id, new ArControlObject(object_id, targetPosition, controlObjects.keys().length()));
    }
    else{
        controlObjects.value(object_id)->updatePosition( targetPosition );
    }
}

} // namespace ArucoModul
