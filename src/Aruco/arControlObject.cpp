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


ArControlObject::ArControlObject( int id, osg::Vec3f position )
{
    this->id = id;
    this->position = position;
    this->focused = false;
}

void ArControlObject::updatePosition( osg::Vec3f position ){
    this->position = position;
    qDebug() << position.x() << " / " << position.y() << " / " << position.z();

    if ( this->focused ){
        this->focusedNode->setTargetPosition(this->position);
    }
    else{
        Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
        QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();
        QList<qlonglong> keys = allNodes->keys();

        // JMA tmp doaround
        this->focusedNode = allNodes->value( keys.first() );
/*

        QOSG::ViewerQT* viewer;
        viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

        osg::Matrixd mwpv = ( viewer->getCamera()->getViewMatrix()
                              * viewer->getCamera()->getProjectionMatrix()
                              * viewer->getCamera()->getViewport()->computeWindowMatrix() );

        osg::Matrixd compositeMi = compositeMi.inverse( mwpv );

        osg::Vec3f tpos = this->focusedNode->getTargetPosition();
        tpos = tpos * mwpv;

      //  this->position = this->focusedNode->getTargetPosition();
     //   this->position.set(osg::Vec3f(this->position.x() + 0.1f, this->position.y(), this->position.z() ) );
*/
        this->focused = true;
     //   this->focusedNode->setFixed( true );
        this->focusedNode->setColor( osg::Vec4( 0.0f,1.0f,1.0f,0.5f ) );
        this->focusedNode->setTargetPosition(this->position);
    }
}




ArControlClass::ArControlClass()
{
    this->controlObject = NULL;
}

void ArControlClass::updateObjectPositionAruco( osg::Vec3f position, QMatrix4x4 modelViewMatrix ){
    QOSG::ViewerQT* viewer;
    viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

    //transform matrix
    osg::Matrixd markerMVM( modelViewMatrix.operator()( 0,0 ),modelViewMatrix.operator()( 0,1 ),modelViewMatrix.operator()( 0,2 ),modelViewMatrix.operator()( 0,3 ),
                            modelViewMatrix.operator()( 1,0 ),modelViewMatrix.operator()( 1,1 ),modelViewMatrix.operator()( 1,2 ),modelViewMatrix.operator()( 1,3 ),
                            modelViewMatrix.operator()( 2,0 ),modelViewMatrix.operator()( 2,1 ),modelViewMatrix.operator()( 2,2 ),modelViewMatrix.operator()( 2,3 ),
                            modelViewMatrix.operator()( 3,0 ),modelViewMatrix.operator()( 3,1 ),modelViewMatrix.operator()( 3,2 ),modelViewMatrix.operator()( 3,3 ) );

    // get matrices ... world->view->projection(device)->viewport(screen)
    osg::Matrixd mwpv = ( viewer->getCamera()->getViewMatrix()
                          * viewer->getCamera()->getProjectionMatrix()
                          * viewer->getCamera()->getViewport()->computeWindowMatrix() );

 /*   osg::Matrixd baseMVM = viewer->getCamera()->getViewMatrix();
    osg::Matrixd baseMVMi = baseMVMi.inverse( baseMVM );
    osg::Matrixd transformMVM = baseMVMi * markerMVM;
    osg::Vec3f ppp = transformMVM.getTrans();
   // osg::Vec3f ppp = osg::Vec3f(modelViewMatrix.operator()( 3,0 ),modelViewMatrix.operator()( 3,1 ),modelViewMatrix.operator()( 3,2 ));

    osg::Vec3f targetPosition = ppp * mwpv;
    //targetPosition.set( targetPosition.y(), targetPosition.z(), targetPosition.x() );
    //osg::Vec3f targetPosition = ppp;
*/

 /*
    osg::Matrixd baseMVM = viewer->getCamera()->getViewMatrix();
    osg::Vec3f bTransVec = baseMVM.getTrans();
    osg::Vec3f mTransVec = markerMVM.getTrans();

    osg::Vec3f targetPosition = osg::Vec3f(
                                    mTransVec.y() -  bTransVec.y(),
                mTransVec.x() -  bTransVec.x(),
                                    mTransVec.z() -  bTransVec.z()
                                );
    */

    osg::Matrixd baseMVM = viewer->getCamera()->getViewMatrix();
    osg::Matrixd transMVM = markerMVM.operator *(baseMVM.inverse(baseMVM));

    osg::Vec3f targetPosition = transMVM.getTrans();

    if( controlObject == NULL ){
        controlObject = new ArControlObject( 1, targetPosition);
    }
    else{
        controlObject->updatePosition( targetPosition );
    }
}

} // namespace ArucoModul
