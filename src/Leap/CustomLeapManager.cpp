#include "Leap/CustomLeapManager.h"
#include <easylogging++.h>
#include <math.h>

Leap::CustomLeapManager::CustomLeapManager( Vwr::CameraManipulator* cameraManipulator, Layout::LayoutThread* layout,
		Vwr::CoreGraph* coreGraph , osg::ref_ptr<osg::Group> handsGroup )
{
	this->cameraManipulator = cameraManipulator;
	this->layout = layout;
	this->coreGraph = coreGraph;
	this->handsGroup = handsGroup;
	arMode = false;

    //init handPalms here
    if ( this->handsGroup != NULL ) {
        arMode = true;
        HandPalm *rightPalm = new HandPalm(0.1f, handsGroup, 1);
        HandPalm *leftPalm = new HandPalm(0.1f, handsGroup, 2);

        rightPalm->setMatrix(osg::Matrix::translate( -0.5,0,-0.5 ));
        leftPalm->setMatrix(osg::Matrix::translate( 0.5,0,-0.5 ));
    }

}

Leap::CustomLeapManager::~CustomLeapManager( void )
{
	//remove hands nodes here
	if ( handsGroup ) {
		handsGroup->removeChildren( 0,handsGroup->getNumChildren() );
	}
}

void Leap::CustomLeapManager::enableCameraMovement( Movement direction )
{
	this->cameraManipulator->enableCameraMovement( static_cast<Vwr::CameraManipulator::Movement>( direction ) );
}

void Leap::CustomLeapManager::disableCameraMovement()
{
	this->cameraManipulator->disableCameraMovement();
}

void Leap::CustomLeapManager::rotateCamera( float py0, float px0, double throwScale, float py1, float px1 )
{
	this->cameraManipulator->rotateCamera( py0,px0,throwScale,py1, px1 );
}

//jurik
void Leap::CustomLeapManager::graphRotateSwipe( int swipeDirection )
{
	//direction -1 = LEFT, 1 = RIGHT
	switch ( swipeDirection ) {
		case -1: {
			coreGraph->rotateGraph( swipeDirection );
			break;
		}
		case 1: {
			coreGraph->rotateGraph( swipeDirection );
			break;
		}
		default:
			break;
	}
}

void Leap::CustomLeapManager::rotateArucoLeft()
{
	coreGraph->rotateGraph( 1 );
}

void Leap::CustomLeapManager::rotateArucoRight()
{
	coreGraph->rotateGraph( -1 );
}

void Leap::CustomLeapManager::scaleEdgesUp()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 1.02f );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomLeapManager::scaleEdgesDown()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 0.98f );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomLeapManager::scaleNodes( bool scaleUp )
{
	if ( scaleUp ) {
		coreGraph->scaleNodes( true );
	}
	else {
		coreGraph->scaleNodes( false );
	}
}

//*****
void Leap::CustomLeapManager::updateHands( Leap::Hand leftHand, Leap::Hand rightHand )
{        
    Leap::Vector lVector = Leap::Vector(0.0f,0.0f,0.0f);
    if ( leftHand.isValid() ) {
        lVector = leftHand.palmPosition();
    }
    Leap::Vector rVector = Leap::Vector(0.0f,0.0f,0.0f);
    if ( rightHand.isValid() ) {
        rVector = rightHand.palmPosition();
    }
//    LOG (INFO) << lVector.toString() << ' ';

    if ( this->handsGroup != NULL ) {
        //0 a 2 z dovodu ze v grupe je palmNode, fingerGroup, palmNode, fingerGroup
        HandPalm* rightPalm = static_cast<HandPalm*>(handsGroup->getChild(0));
        HandPalm* leftPalm = static_cast<HandPalm*>(handsGroup->getChild(3));

        leftPalm->setMatrix(
                    osg::Matrix::translate( static_cast<double>( lVector.x )/100.0,
                                            static_cast<double>( -lVector.z )/100.0,
                                            static_cast<double>( lVector.y )/100.0 ));
        rightPalm->setMatrix(
                    osg::Matrix::translate( static_cast<double>( rVector.x )/100.0,
                                            static_cast<double>( -rVector.z )/100.0,
                                            static_cast<double>( rVector.y )/100.0 ) );

        this->updateFingers(rightPalm, rightHand.fingers());
        this->updateFingers(leftPalm, leftHand.fingers());
        this->updateInterFingerBones(rightPalm->interFingerBoneGroup, rightHand.fingers());
        this->updateInterFingerBones(leftPalm->interFingerBoneGroup, leftHand.fingers());
    }
}

void Leap::CustomLeapManager::updateFingers(HandPalm* palm, Leap::FingerList fingers) {
    int i = 0;
    // odstranene fingers.count(), aby sa pozicie klbov zobrazili na 0,0,0, ked nieje zachytavana ruka
    for(i = 0; i < 5; i++) {
        updateJoints(static_cast<osg::Group*>(palm->fingerGroup->getChild(i)->asGroup()), fingers[i]);
    }
    for(i = 5; i < 10; i++) {
        updateFingerBones(static_cast<osg::Group*>(palm->fingerGroup->getChild(i)->asGroup()), fingers[i-5]);
    }
}


void Leap::CustomLeapManager::updateJoints(osg::Group*  fingerGroup, Leap::Finger fingerLeap){

    // vykreslenie klbov zapastia ( klby v scene su ratene 0,1,2,3 s tym ze 4-ty je klb zapestia )
        Joint* joint = static_cast<Joint*>(fingerGroup->getChild(4));
        Leap::Vector posVector = Leap::Vector(0.0f,0.0f,0.0f);
        if(fingerLeap.bone(static_cast<Leap::Bone::Type>(0)).isValid()){
           posVector = fingerLeap.bone(static_cast<Leap::Bone::Type>(0)).prevJoint();

           joint->setMatrix(osg::Matrix::translate( static_cast<double>( posVector.x )/100.0,
                                                    static_cast<double>( -posVector.z )/100.0,
                                                    static_cast<double>( posVector.y )/100.0 ));
        }

    // vykreslenie klbov prstov
    int i = 0;
    for(i= 0; i < 4; i++) {
        Leap::Joint* joint = static_cast<Leap::Joint*>(fingerGroup->getChild(i));

        Leap::Vector posVector = Leap::Vector(0.0f,0.0f,0.0f);
        if(fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).isValid()){
           posVector = fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).nextJoint();

           joint->setMatrix(osg::Matrix::translate( static_cast<double>( posVector.x )/100.0,
                                                    static_cast<double>( -posVector.z )/100.0,
                                                static_cast<double>( posVector.y )/100.0 ));
        }

    }
}

void Leap::CustomLeapManager::updateFingerBones(osg::Group*  fingerGroup, Leap::Finger fingerLeap) {
    int i = 0;

    for(i = 0; i < 4; i++) {
        Leap::HandBone* bone = static_cast<Leap::HandBone*>(fingerGroup->getChild(i));

        Leap::Vector posVector = Leap::Vector(0.0f,0.0f,0.0f);
        Leap::Vector dirVector = Leap::Vector(0.0f,0.0f,0.0f);
        float length;

        if (fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).isValid()) {
            posVector = fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).center();
            dirVector = fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).direction();
            length = fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).length();

            osg::RefMatrixd *boneMatrix = new osg::RefMatrixd();
            boneMatrix->makeIdentity();

            // position of bone
            boneMatrix->preMult(osg::Matrix::translate(static_cast<double>( posVector.x )/100.0,
                                           static_cast<double>( -posVector.z )/100.0,
                                           static_cast<double>( posVector.y )/100.0 ));

            // rotation of bone
            boneMatrix->preMult(osg::Matrix::rotate(osg::Vec3f( 0.0f,0.0f,1.0f ) ,
                                                    osg::Vec3f( static_cast<double>( dirVector.x/100.0),
                                                    static_cast<double>( -(dirVector.z/100.0)) ,
                                                    static_cast<double>( dirVector.y/100.0 ))));
            // scaling of bone
            boneMatrix->preMult(osg::Matrix::scale(1.0,1.0,(length/100.0)/0.2));

            bone->setMatrix(*boneMatrix);
        }

    }
}

void Leap::CustomLeapManager::updateInterFingerBones(osg::Group*  interFingerBoneGroup, Leap::FingerList fingers){
    int i;
    Leap::Vector aJoints [4];
    // inicializuju sa pozocie klbov medzi prstami
    for(i = 1; i < 5; i++) {
     aJoints[i-1] = fingers[i].bone(static_cast<Leap::Bone::Type>(0)).nextJoint();
     }

    // ziskanie pozicii kosti medzi prstami
    Leap::Vector aInterFingerBonesPositions [3];
    for(i = 0; i < 3; i++) {
        aInterFingerBonesPositions[i] = (aJoints[i] + aJoints[i+1]) / 2;
    }

    // ziskanie rotacii kosti medzi prstami
    Leap::Vector aInterFingerBonesRotations [3];
    for(i = 0; i < 3; i++) {
        aInterFingerBonesRotations[i] = (aJoints[i+1] - aJoints[i]);
    }

    // ziskanie dlzky kosti medzi prstami
    float aInterFingerBonesLengths [3];
    for(i = 0; i < 3; i++) {
        float tempLength = sqrt (pow((aJoints[i+1].x - aJoints[i].x),2) + pow((aJoints[i+1].y - aJoints[i].y),2)+ pow((aJoints[i+1].z - aJoints[i].z),2));
        aInterFingerBonesLengths[i] = (tempLength/100.0)/0.2;

    }

    for(i = 0; i < 3; i++) {
        Leap::HandBone* bone = static_cast<Leap::HandBone*>(interFingerBoneGroup->getChild(i));

        osg::RefMatrixd *boneMatrix = new osg::RefMatrixd();
        boneMatrix->makeIdentity();

        // position of bone
        boneMatrix->preMult(osg::Matrix::translate(static_cast<double>( aInterFingerBonesPositions[i].x )/100.0,
                                       static_cast<double>( - aInterFingerBonesPositions[i].z )/100.0,
                                       static_cast<double>( aInterFingerBonesPositions[i].y )/100.0 ));

        // rotation of bone
        boneMatrix->preMult(osg::Matrix::rotate(osg::Vec3f( 0.0f,0.0f,1.0f ) ,
                                                osg::Vec3f( static_cast<double>( aInterFingerBonesRotations[i].x/100.0),
                                                static_cast<double>( -(aInterFingerBonesRotations[i].z/100.0)) ,
                                                static_cast<double>( aInterFingerBonesRotations[i].y/100.0 ))));
        // scaling of bone
        boneMatrix->preMult(osg::Matrix::scale(1.0,1.0,aInterFingerBonesLengths[i]));

        bone->setMatrix(*boneMatrix);

    }
}
