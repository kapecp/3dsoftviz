/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include <easylogging++.h>
#include "Leap/HandModule/HandPalm.h"
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Shape>

const int TYPE_MIDDLE = 2; /*< The middle finger */
const int TYPE_RING = 3; /*< The ring finger */
const int BONE_COUNT = 4; /*< The ring finger */

Leap::HandPalm::HandPalm( float radius, osg::ref_ptr<osg::Group> handsGroup, int colorSwitch )
	: fingerGroup( new osg::Group() ), interFingerBoneGroup( new osg::Group() ), colorSwitch( colorSwitch )
{
	this->generateGeometry( radius, colorSwitch );
	this->initStructure();

	// pridanie nodu dlane a groupy prstov do sceny
	handsGroup->addChild( static_cast<osg::Node*>( this ) );
	handsGroup->addChild( this->fingerGroup );
	handsGroup->addChild( this->interFingerBoneGroup );
	\
}

void Leap::HandPalm::initStructure()
{
	LOG( INFO ) << "Leap/HandleModule/HandPalm/initStrucure()";
	if ( this->fingerGroup.get()->getNumChildren() == 0 ) {
		int i = 0;
		int j = 0;

		// Create finger joint and bone groups
		for ( i = 0; i < 5; i++ ) {
			osg::ref_ptr<osg::Group> fingerJointGroup = new osg::Group();
			osg::ref_ptr<osg::Group> fingerBoneGroup = new osg::Group();

			// vynechanie klbov a kosti zapestia stredneho prstu a prstennika
			if ( i == TYPE_MIDDLE || i == TYPE_RING ) {
				Joint* joint = new Joint( 1 , i, fingerJointGroup, this->colorSwitch );
				for ( j = 0; j < BONE_COUNT-1; j++ ) {
					HandBone* handBone = new HandBone( j, fingerBoneGroup );
				}
			}
			else {
				Joint* joint = new Joint( 0 , i, fingerJointGroup, this->colorSwitch );
				// vygeneruje 4 kosti pre dany prst
				for ( j = 0; j < BONE_COUNT; j++ ) {
					HandBone* handBone = new HandBone( j, fingerBoneGroup );
				}
			}
			this->fingerGroup->insertChild( static_cast<unsigned int>( i ) , fingerJointGroup );

			// elementy vo finger groupe v takomto poradi: {5x jointGroup, 5x boneGroup}
			this->fingerGroup->insertChild( static_cast<unsigned int>( i+5 ), fingerBoneGroup );

		}
		// pridanie kosti medzi prstami
		// 4-ta je kost v zapesti
		for ( i = 0; i < BONE_COUNT; i++ ) {
			HandBone* handBone = new HandBone( i, this->interFingerBoneGroup );
		}
	}
}

void Leap::HandPalm::generateGeometry( float radius, int colorSwitch )
{
	osg::ref_ptr<osg::Geode> handGeode( new osg::Geode );
	osg::ref_ptr<osg::Sphere> handSphere = new osg::Sphere( osg::Vec3f( 0.0f,0.0f,0.0f ), radius );

	osg::ref_ptr<osg::ShapeDrawable> handDrawable( new osg::ShapeDrawable( handSphere.get() ) );
	this->setColor( colorSwitch, handDrawable );

	handGeode->addDrawable( handDrawable.get() );

	this->addChild( handGeode.get() );
}
