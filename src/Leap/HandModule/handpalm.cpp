/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include <easylogging++.h>
#include "Leap/HandModule/HandPalm.h"

Leap::HandPalm::HandPalm(float radius, osg::ref_ptr<osg::Group> handsGroup, int colorSwitch) {
    this->fingerGroup = new osg::Group();
    this->colorSwitch = colorSwitch;
    this->generateGeometry(radius, colorSwitch);
    this->initStructure();

    // pridanie nodu dlane a groupy prstov do sceny
    handsGroup->addChild(static_cast<osg::Node*> (this));
    handsGroup->addChild(this->fingerGroup);
}

void Leap::HandPalm::initStructure() {
    LOG( INFO ) << "Leap/HandleModule/HandPalm/initStrucure()";
    if (this->fingerGroup.get()->getNumChildren() == 0) {
        int i = 0;
		int j = 0;

        // Create finger joint and bone groups
        for(i = 0; i < 5; i++) {
            osg::ref_ptr<osg::Group> fingerJointGroup = new osg::Group();
            Joint* joint = new Joint(0, i, fingerJointGroup, this->colorSwitch);
            this->fingerGroup->insertChild(i, fingerJointGroup);

			// elementy vo finger groupe v takomto poradi: {5x jointGroup, 5x boneGroup}
			osg::ref_ptr<osg::Group> fingerBoneGroup = new osg::Group();
			this->fingerGroup->insertChild(i+5, fingerBoneGroup);
			// vygeneruje 4 kosti pre dany prst
			for(j = 0; j < 4; j++) {
				HandBone* handBone = new HandBone(j, fingerBoneGroup);
			}
        }
    }
}

void Leap::HandPalm::generateGeometry(float radius, int colorSwitch) {
    osg::ref_ptr<osg::Geode> handGeode( new osg::Geode );
    osg::ref_ptr<osg::Sphere> handSphere = new osg::Sphere( osg::Vec3f( 0.0f,0.0f,0.0f ), radius );

    osg::ref_ptr<osg::ShapeDrawable> handDrawable( new osg::ShapeDrawable( handSphere.get() ) );
    this->setColor(colorSwitch, handDrawable);

    handGeode->addDrawable( handDrawable.get() );

    this->addChild( handGeode.get() );
}
