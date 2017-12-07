/**
 * Created by Patrik Berger on 13.11.2016.
 */

#include "Leap/HandModule/Model/HandPalm.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Shape>

#include <easylogging++.h>

const int TYPE_MIDDLE = 2; /*< The middle finger */
const int TYPE_RING = 3; /*< The ring finger */
const int BONE_COUNT = 4; /*< The ring finger */

Leap::HandPalm::HandPalm( float radius = 0.1f, osg::ref_ptr<osg::Group> handsGroup = nullptr, HandColors colorSwitch = HandColors::NONE )
	: fingerGroup( new osg::Group() ), interFingerBoneGroup( new osg::Group() ), colorSwitch( colorSwitch )
{
	this->generateGeometry( radius, colorSwitch );
	this->initStructure();

	if ( handsGroup != nullptr ) {
		// pridanie nodu dlane a groupy prstov do sceny,
		// ak je handsGroup null, je to trieda na posileanie hodnot.
		handsGroup->addChild( static_cast<osg::Node*>( this ) );
		handsGroup->addChild( this->fingerGroup );
		handsGroup->addChild( this->interFingerBoneGroup );
	}
}

void Leap::HandPalm::initStructure()
{
	QMutexLocker locker(&updateLock);

//	LOG( INFO ) << "Leap/HandleModule/HandPalm/initStrucure()";
	if ( this->fingerGroup.get()->getNumChildren() == 0 ) {
		int i = 0;
		int j = 0;

		// Create finger joint and bone groups
		for ( i = 0; i < 5; i++ ) {
			osg::ref_ptr<osg::Group> fingerJointGroup = new osg::Group();
			osg::ref_ptr<osg::Group> fingerBoneGroup = new osg::Group();

			// vynechanie klbov a kosti zapestia stredneho prstu a prstennika
			if ( i == TYPE_MIDDLE || i == TYPE_RING ) {
				Joint* joint = new Joint( 1, i, fingerJointGroup, this->colorSwitch );
				for ( j = 0; j < BONE_COUNT-1; j++ ) {
					HandBone* handBone = new HandBone( j, fingerBoneGroup );
				}
			}
			else {
				Joint* joint = new Joint( 0, i, fingerJointGroup, this->colorSwitch );
				// vygeneruje 4 kosti pre dany prst
				for ( j = 0; j < BONE_COUNT; j++ ) {
					HandBone* handBone = new HandBone( j, fingerBoneGroup );
				}
			}
			this->fingerGroup->insertChild( static_cast<unsigned int>( i ), fingerJointGroup );

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

void Leap::HandPalm::generateGeometry( float radius, HandColors colorSwitch )
{
	QMutexLocker locker(&updateLock);

	osg::ref_ptr<osg::Geode> handGeode( new osg::Geode );
	osg::ref_ptr<osg::Sphere> handSphere = new osg::Sphere( osg::Vec3f( 0.0f,0.0f,0.0f ), radius );

	osg::ref_ptr<osg::ShapeDrawable> handDrawable( new osg::ShapeDrawable( handSphere.get() ) );
	this->setColor( colorSwitch, handDrawable );

	handGeode->addDrawable( handDrawable.get() );

	this->addChild( handGeode.get() );
}

void Leap::HandPalm::addToStream( QDataStream* stream )
{
	QMutexLocker locker(&updateLock);

	Leap::HandNode::addToStream( stream );

	// add all fingers joints into stream
	for ( unsigned int i = 0; i < 5; i++ ) {
		osg::Group* joints = static_cast<osg::Group*>( this->fingerGroup->getChild( i ) );
		for ( unsigned int j = 0; j < joints->getNumChildren(); j++ ) {
			auto joint = static_cast<Joint*>( joints->getChild( j ) );
			joint->addToStream( stream );
		}
	}

	// add all fingers bones into stream
	for ( unsigned int i = 5; i < 10; i++ ) {
		osg::Group* bones = static_cast<osg::Group*>( this->fingerGroup->getChild( i ) );
		for ( unsigned int j = 0; j < bones->getNumChildren(); j++ ) {
			auto bone = static_cast<HandBone*>( bones->getChild( j ) );
			bone->addToStream( stream );
		}
	}

	// add inter finger bones into steam
	for ( unsigned int i = 0; i < this->interFingerBoneGroup->getNumChildren(); i++ ) {
		auto bone = static_cast<HandBone*>( this->interFingerBoneGroup->getChild( i ) );
		bone->addToStream( stream );
	}
}

void Leap::HandPalm::setFromStream( QDataStream* stream )
{
	QMutexLocker locker(&updateLock);

	Leap::HandNode::setFromStream( stream );

	// add all fingers joints into stream
	for ( unsigned int i = 0; i < 5; i++ ) {
		osg::Group* joints = static_cast<osg::Group*>( this->fingerGroup->getChild( i ) );
		for ( unsigned int j = 0; j < joints->getNumChildren(); j++ ) {
			auto joint = static_cast<Joint*>( joints->getChild( j ) );
			joint->setFromStream( stream );
		}
	}

	// add all fingers bones into stream
	for ( unsigned int i = 5; i < 10; i++ ) {
		osg::Group* bones = static_cast<osg::Group*>( this->fingerGroup->getChild( i ) );
		for ( unsigned int j = 0; j < bones->getNumChildren(); j++ ) {
			auto bone = static_cast<HandBone*>( bones->getChild( j ) );
			bone->setFromStream( stream );
		}
	}

	// add inter finger bones into steam
	for ( unsigned int i = 0; i < this->interFingerBoneGroup->getNumChildren(); i++ ) {
		auto bone = static_cast<HandBone*>( this->interFingerBoneGroup->getChild( i ) );
		bone->setFromStream( stream );
	}
}

void Leap::HandPalm::setFromPalm( HandPalm* palm )
{
	QMutexLocker locker(&updateLock);
	unsigned int minCount = 0;

	if ( palm != nullptr ) {

		osg::Group* palmFingerGroup = palm->fingerGroup;
		osg::Group* handFingerGroup = this->fingerGroup;
		if ( handFingerGroup != nullptr && palmFingerGroup != nullptr) {
			minCount = std::min<unsigned int>( std::min<unsigned int>( handFingerGroup->getNumChildren(), palmFingerGroup->getNumChildren() ), 10 );
			for ( unsigned int i = 0; i < minCount; i++ ) {
				osg::MatrixTransform* handFinger = static_cast<osg::MatrixTransform*>( handFingerGroup->getChild( i ) );
				osg::MatrixTransform* palmFinger = static_cast<osg::MatrixTransform*>( palmFingerGroup->getChild( i ) );
				handFinger->setMatrix( palmFinger->getMatrix() );
			}
		}

		osg::Group* palmFingerBoneGroup = palm->interFingerBoneGroup;
		osg::Group* handFingerBoneGroup = this->interFingerBoneGroup;
		if ( handFingerBoneGroup != nullptr && palmFingerBoneGroup != nullptr ) {
			minCount = std::min<unsigned int>( std::min<unsigned int>( palmFingerBoneGroup->getNumChildren(), handFingerBoneGroup->getNumChildren() ), 10);

			for ( unsigned int i = 0; i < minCount; i++ ) {
				osg::MatrixTransform* handBone = static_cast<osg::MatrixTransform*>( handFingerBoneGroup->getChild( i ) );
				osg::MatrixTransform* palmBone = static_cast<osg::MatrixTransform*>( palmFingerBoneGroup->getChild( i ) );
				handBone->setMatrix( palmBone->getMatrix() );
			}
		}

		this->setMatrix( palm->getMatrix() );
	}
}
