#include "Viewer/RestrictionManipulatorsGroup.h"
//-----------------------------------------------------------------------------
#include <osg/ShapeDrawable>
#include <osg/Geode>
namespace Vwr {

RestrictionManipulatorsGroup::RestrictionManipulatorsGroup()
 : group_ (new osg::Group()) {
    TURN_ON = false;
}

osg::ref_ptr<osg::Group> RestrictionManipulatorsGroup::getGroup() {
        return group_;
}

void RestrictionManipulatorsGroup::restrictionAdded (
        QSharedPointer<Layout::ShapeGetter> shapeGetter
) {

        osg::Switch* rootM = new osg::Switch;
        group_->addChild(rootM);
        manipulator[shapeGetter] = rootM;
}

void RestrictionManipulatorsGroup::shapeChanged (
        QSharedPointer<Layout::ShapeGetter> shapeGetter,
        QSharedPointer<Layout::Shape> shape
) {
        manipulatorCreator.setType(ShapeVisitor_ManipulatorCreator::ManipulatorType::TRACKBALLDRAGGER);
        shape->accept (manipulatorCreator);
        osgManipulator::Dragger* dragger = manipulatorCreator.getCreatedDragger();
        osg::MatrixTransform* selection = manipulatorCreator.getCreatedSelection();

        osg::Switch* rootM = manipulator[shapeGetter];
        if (rootM->getNumChildren() == 0){
            rootM->addChild(dragger, TURN_ON);
            rootM->addChild(selection, TURN_ON);
        }else{
            rootM->setChild(0,dragger);
            rootM->setChild(1,selection);

        }
}

void RestrictionManipulatorsGroup::restrictionRemoved (
        QSharedPointer<Layout::ShapeGetter> shapeGetter
) {

        group_->removeChild (manipulator[shapeGetter]);
        manipulator.remove (shapeGetter);
}

void RestrictionManipulatorsGroup::addNodeToSelection(QSharedPointer<Layout::ShapeGetter> shapeGetter, osg::Node* node){
    osg::MatrixTransform* selection = dynamic_cast<osg::MatrixTransform*>(manipulator[shapeGetter]->getChild(1));
     int a = selection->getNumChildren();
    if (selection)
           selection->addChild(node);

}

osg::Geode* RestrictionManipulatorsGroup::test(){
    osg::Vec3 center (0,0, -0.8);
    osg::Box * box = new osg::Box;
    box->setCenter (center);
    box->setHalfLengths (osg::Vec3 (100, 100, 100));

    osg::ShapeDrawable * sd = new osg::ShapeDrawable;
    sd->setShape (box);
    sd->setColor (osg::Vec4 (0, 0, 1.0, 1.0));
    sd->getOrCreateStateSet()->setMode (GL_BLEND, osg::StateAttribute::ON);
    sd->getStateSet()->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

    osg::Geode * geode = new osg::Geode;
    geode->addDrawable (sd);
   return geode;
}





}
