#include "Leap/HandModule/Manipulator/HandObjectManipulator.h"

#include <vector>
#include <easylogging++.h>

Leap::HandObjectManipulator::HandObjectManipulator( HandMapper* mapper, char upDirectionAxis )
	:mapper( mapper ), upDirectionAxis( upDirectionAxis )
{

}

Leap::HandObjectManipulator::~HandObjectManipulator()
{

}

Leap::Vector Leap::HandObjectManipulator::changeHandUpDirectionAxis( Leap::Vector vector )
{
	float temp;
	if ( this->upDirectionAxis == 'y' ) {
		vector.x = - vector.x;
		vector.z = - vector.z;
	}
	else {
		//z-axis
		temp = vector.y;
		vector.y = -vector.z;
		vector.z = temp;
	}
	return vector;
}

void Leap::HandObjectManipulator::updateHands( Leap::Hand leftHand, Leap::Hand rightHand,
		HandPalm* leftPalm, HandPalm* rightPalm, osg::ref_ptr<osg::Camera> camera )
{
	float mid = 300;
	float diffLeftHand;
	float diffRightHand;

	this->center = Leap::Vector( 0.0f, -2.0f, 0.0f );
	this->direction = Leap::Vector( 0.0f, 5.0f, 0.0f );

	this->center = changeHandUpDirectionAxis( this->center );

	// update lavej ruky
	if ( leftHand.isValid() ) {
		Leap::Vector lVector = Leap::Vector( this->center[0]+0.5f,this->center[1],this->center[2] );
		//ziskanie pozicie dlane
		lVector = leftHand.palmPosition();



		diffLeftHand = lVector.y - mid;
		lVector = this->mapper->recalculateDepthNode( lVector, diffLeftHand );
		lVector = changeHandUpDirectionAxis( lVector );



		leftPalm->setMatrix(
			osg::Matrix::translate( static_cast<double>( this->center[0] ) + this->direction[0] + static_cast<double>( lVector.x )/100.0,
									static_cast<double>( this->center[1] ) + this->direction[1] + static_cast<double>( lVector.y )/100.0,
									static_cast<double>( this->center[2] ) + this->direction[2] + static_cast<double>( lVector.z )/100.0 ) );

		osg::Viewport* viewport = camera->getViewport();
		osg::Matrix win = camera->getViewport()->computeWindowMatrix();
		osg::Matrix view = camera->getViewMatrix();
		osg::Matrix proj = camera->getProjectionMatrix();
		osg::Matrix model = leftPalm->getWorldMatrices()[0];
		osg::Vec3 world_coords = osg::Vec3( static_cast<double>( this->center[0] ) + this->direction[0] + static_cast<double>( lVector.x )/100.0,
											static_cast<double>( this->center[1] ) + this->direction[1] + static_cast<double>( lVector.y )/100.0,
											static_cast<double>( this->center[2] ) + this->direction[2] + static_cast<double>( lVector.z )/100.0 );
		osg::Vec3 screenCoords = world_coords * view * proj * win;
		screenCoords.set( ( ( screenCoords.x() / viewport->width() ) * 640 ), ( screenCoords.y() / viewport->height() ) * 480, screenCoords.z() );
		this->mapper->setNodeScreenCoords( screenCoords );

		// update prstov lavej ruky
		this->updateFingers( leftPalm, leftHand.fingers(), diffLeftHand );
		// update kosti medzi prstamu
		this->updateInterFingerBones( leftPalm->interFingerBoneGroup, leftHand.fingers(), diffLeftHand );
	}

	// update pravej ruky
	if ( rightHand.isValid() ) {
		Leap::Vector rVector = Leap::Vector( this->center[0]-0.5,this->center[1],this->center[2] );
		//ziskanie pozicie dlane
		rVector = rightHand.palmPosition();

		diffRightHand = rVector.y - mid;
		rVector = this->mapper->recalculateDepthNode( rVector, diffRightHand );

		rVector = changeHandUpDirectionAxis( rVector );
		rightPalm->setMatrix(
			osg::Matrix::translate( this->center[0] + this->direction[0] + static_cast<double>( rVector.x )/100.0,
									this->center[1] + this->direction[1] + static_cast<double>( rVector.y )/100.0,
									this->center[2] + this->direction[2] + static_cast<double>( rVector.z )/100.0 ) );
		// update prstov pravej ruky
		this->updateFingers( rightPalm, rightHand.fingers(), diffRightHand );
		// update kosti medzi prstamu
		this->updateInterFingerBones( rightPalm->interFingerBoneGroup, rightHand.fingers(), diffRightHand );
	}

}

void Leap::HandObjectManipulator::updateFingers( HandPalm* palm, Leap::FingerList fingers, float diff )
{
	int i = 0;
	// update jointov vsetkych prstov
	for ( i = 0; i < 5; i++ ) {
		updateJoints( static_cast<osg::Group*>( palm->fingerGroup->getChild( static_cast<unsigned int>( i ) )->asGroup() ),
					  fingers[i], i, diff );
	}
	// update kosti vsetkych prstov
	for ( i = 5; i < 10; i++ ) {
		updateFingerBones( static_cast<osg::Group*>( palm->fingerGroup->getChild( static_cast<unsigned int>( i ) )->asGroup() ),
						   fingers[i-5], diff );
	}
}


void Leap::HandObjectManipulator::updateJoints( osg::Group* fingerJointGroup, Leap::Finger fingerLeap, int fingerPosition, float diff )
{
	// vykreslenie klbov zapastia ( klby v scene su ratene 0,1,2,3 s tym ze 4-ty je klb zapestia )
	if ( fingerPosition !=  2 && fingerPosition!= 3 ) {
		Joint* joint = static_cast<Joint*>( fingerJointGroup->getChild( 4 ) );
		Leap::Vector posVector = Leap::Vector( 0.0f,0.0f,0.0f );
		if ( fingerLeap.bone( static_cast<Leap::Bone::Type>( 0 ) ).isValid() ) {
			posVector = fingerLeap.bone( static_cast<Leap::Bone::Type>( 0 ) ).prevJoint();


			posVector = this->mapper->recalculateDepthNode( posVector, diff );
			posVector = changeHandUpDirectionAxis( posVector );
			joint->setMatrix( osg::Matrix::translate(
								  this->center[0] + this->direction[0] + static_cast<double>( posVector.x )/100.0,
								  this->center[1] + this->direction[1] + static_cast<double>( posVector.y )/100.0,
								  this->center[2] + this->direction[2] + static_cast<double>( posVector.z )/100.0 ) );
		}
	}
	// vykreslenie klbov prstov
	unsigned  int i = 0;
	for ( i= 0; i < 4; i++ ) {
		Leap::Joint* joint = static_cast<Leap::Joint*>( fingerJointGroup->getChild( i ) );

		Leap::Vector posVector = Leap::Vector( 0.0f,0.0f,0.0f );
		if ( fingerLeap.bone( static_cast<Leap::Bone::Type>( i ) ).isValid() ) {
			posVector = fingerLeap.bone( static_cast<Leap::Bone::Type>( i ) ).nextJoint();

			posVector = this->mapper->recalculateDepthNode( posVector, diff );
			posVector = changeHandUpDirectionAxis( posVector );

			joint->setMatrix( osg::Matrix::translate(
								  this->center[0] +this->direction[0] + static_cast<double>( posVector.x )/100.0,
								  this->center[1] +this->direction[1] + static_cast<double>( posVector.y )/100.0,
								  this->center[2] +this->direction[2] + static_cast<double>( posVector.z )/100.0 ) );
		}

	}
}

void Leap::HandObjectManipulator::updateFingerBones( osg::Group*  fingerBoneGroup, Leap::Finger fingerLeap, float diff )
{
	unsigned int i = 0;
	// ak ma prst 3 kosti (middle a  ring), tak je treba pouzit offset na data z leapu (leap 4 kosti, my 3)
	unsigned int offset = 0;
	if ( fingerBoneGroup->getNumChildren() == 3 ) {
		offset = 1;
	}

	for ( i = 0; i < fingerBoneGroup->getNumChildren(); i++ ) {
		Leap::HandBone* bone = static_cast<Leap::HandBone*>( fingerBoneGroup->getChild( i ) );

		Leap::Vector posVector = Leap::Vector( 0.0f,0.0f,0.0f );
		Leap::Vector dirVector = Leap::Vector( 0.0f,0.0f,0.0f );


		if ( fingerLeap.bone( static_cast<Leap::Bone::Type>( i + offset ) ).isValid() ) {
			float length;
			Leap::HandBone* bone = static_cast<Leap::HandBone*>( fingerBoneGroup->getChild( i ) );
			// ziskanie dat z Leap senzoru
			posVector = fingerLeap.bone( static_cast<Leap::Bone::Type>( i + offset ) ).center();
//			dirVector = fingerLeap.bone( static_cast<Leap::Bone::Type>( i + offset ) ).direction();

			length = fingerLeap.bone( static_cast<Leap::Bone::Type>( i + offset ) ).length();

			osg::RefMatrixd* boneMatrix = new osg::RefMatrixd();
			boneMatrix->makeIdentity();

			// position of bone
			posVector = this->mapper->recalculateDepthNode( posVector, diff );
			posVector = changeHandUpDirectionAxis( posVector );
			Leap::Vector prevVector = fingerLeap.bone( static_cast<Leap::Bone::Type>( i + offset ) ).prevJoint();
			prevVector = this->mapper->recalculateDepthNode( prevVector, diff );
			prevVector = changeHandUpDirectionAxis( prevVector );
			Leap::Vector nextVector = fingerLeap.bone( static_cast<Leap::Bone::Type>( i + offset ) ).nextJoint();
			nextVector = this->mapper->recalculateDepthNode( nextVector, diff );
			nextVector = changeHandUpDirectionAxis( nextVector );
			dirVector = nextVector - prevVector;

			boneMatrix->preMult( osg::Matrix::translate(
									 this->center[0] +this->direction[0] + static_cast<double>( posVector.x )/100.0,
									 this->center[1] +this->direction[1] + static_cast<double>( posVector.y )/100.0,
									 this->center[2] +this->direction[2] + static_cast<double>( posVector.z )/100.0 ) );

			// rotation of bone
			if ( dirVector.x != 0 || dirVector.y !=0 || dirVector.z !=0 ) {
				boneMatrix->preMult( osg::Matrix::rotate( osg::Vec3f( 0.0f,0.0f,1.0f ),
									 osg::Vec3f( dirVector.x/100.0f,dirVector.y/100.0f,dirVector.z/100.0f ) ) );
			}
			// scaling of bone
			boneMatrix->preMult( osg::Matrix::scale( 1.0,1.0,( static_cast<double>( length )/100.0 )/static_cast<double>( bone->HEIGHT ) ) );

			bone->setMatrix( *boneMatrix );
		}
	}
}

void Leap::HandObjectManipulator::updateInterFingerBones( osg::Group*  interFingerBoneGroup, Leap::FingerList fingers, float diff )
{
	int i;
	Leap::Vector arrayJoints [4];

	// update kosti v zapasti
	this->updateInterFingerWristBone( interFingerBoneGroup, fingers, diff );

	// inicializuju sa pozocie klbov medzi prstami
	for ( i = 1; i < 5; i++ ) {
		arrayJoints[i-1] = fingers[i].bone( static_cast<Leap::Bone::Type>( 0 ) ).nextJoint();
		arrayJoints[i-1] = this->mapper->recalculateDepthNode( arrayJoints[i-1], diff );
		arrayJoints[i-1] = changeHandUpDirectionAxis( arrayJoints[i-1] );
	}

	// ziskanie pozicii kosti medzi prstami
	Leap::Vector arrayInterFingerBonesPositions [3];
	for ( i = 0; i < 3; i++ ) {
		arrayInterFingerBonesPositions[i] = ( arrayJoints[i] + arrayJoints[i+1] ) / 2;
	}

	// ziskanie smeru kosti medzi prstami
	Leap::Vector arrayOfInterFingerBonesRotations [3];
	for ( i = 0; i < 3; i++ ) {
		arrayOfInterFingerBonesRotations[i] = ( arrayJoints[i+1] - arrayJoints[i] );
	}

	// ziskanie dlzky kosti medzi prstami
	float arrayInterFingerBonesLengths [3];
	for ( i = 0; i < 3; i++ ) {
		float tempLength = sqrtf( powf( ( arrayJoints[i+1].x - arrayJoints[i].x ),2 ) + powf( ( arrayJoints[i+1].y - arrayJoints[i].y ),2 )+ powf( ( arrayJoints[i+1].z - arrayJoints[i].z ),2 ) );
		arrayInterFingerBonesLengths[i] = ( tempLength/100.0f )/0.2f;

	}

	for ( i = 0; i < 3; i++ ) {
		Leap::HandBone* bone = static_cast<Leap::HandBone*>( interFingerBoneGroup->getChild( static_cast<unsigned int>( i ) ) );

		osg::RefMatrixd* boneMatrix = new osg::RefMatrixd();
		boneMatrix->makeIdentity();

		// position of bone
//        arrayInterFingerBonesPositions[i] = recalculateDepthNode(arrayInterFingerBonesPositions[i]);
		boneMatrix->preMult( osg::Matrix::translate(
								 this->center[0] +this->direction[0] + static_cast<double>( arrayInterFingerBonesPositions[i].x )/100.0,
								 this->center[1] +this->direction[1] + static_cast<double>( arrayInterFingerBonesPositions[i].y )/100.0,
								 this->center[2] +this->direction[2] + static_cast<double>( arrayInterFingerBonesPositions[i].z )/100.0 ) );

		// rotation of bone
		if ( arrayOfInterFingerBonesRotations[i].x != 0 || arrayOfInterFingerBonesRotations[i].y !=0 || arrayOfInterFingerBonesRotations[i].z !=0 ) {

			boneMatrix->preMult( osg::Matrix::rotate( osg::Vec3f( 0.0f,0.0f,1.0f ),
								 osg::Vec3f( arrayOfInterFingerBonesRotations[i].x/100.0f,
											 arrayOfInterFingerBonesRotations[i].y/100.0f,
											 arrayOfInterFingerBonesRotations[i].z/100.0f ) ) );
		}
		// scaling of bone
		boneMatrix->preMult( osg::Matrix::scale( 1.0,1.0,( static_cast<double>( arrayInterFingerBonesLengths[i] ) ) ) );

		bone->setMatrix( *boneMatrix );

	}
}

void Leap::HandObjectManipulator::updateInterFingerWristBone( osg::Group*  interFingerBoneGroup, Leap::FingerList fingers, float diff )
{
	Leap::Vector positionOfInnerJoint;
	Leap::Vector positionOfOuterJoint;
	Leap::Vector bonePosition;
	Leap::Vector boneDirection;
	float boneLength;

	//ukazovak, najspodnejsia kost
	positionOfInnerJoint = fingers[1].bone( static_cast<Leap::Bone::Type>( 0 ) ).prevJoint();
	positionOfInnerJoint = this->mapper->recalculateDepthNode( positionOfInnerJoint, diff );
	positionOfInnerJoint = changeHandUpDirectionAxis( positionOfInnerJoint );
	//malicek, najspodnejsia kost
	positionOfOuterJoint = fingers[4].bone( static_cast<Leap::Bone::Type>( 0 ) ).prevJoint();
	positionOfOuterJoint = this->mapper->recalculateDepthNode( positionOfOuterJoint, diff );
	positionOfOuterJoint = changeHandUpDirectionAxis( positionOfOuterJoint );

	bonePosition = ( positionOfInnerJoint + positionOfOuterJoint ) / 2;

	boneDirection = ( positionOfInnerJoint - positionOfOuterJoint );

	boneLength = sqrtf( powf( ( positionOfInnerJoint.x - positionOfOuterJoint.x ),2 )
						+ powf( ( positionOfInnerJoint.y - positionOfOuterJoint.y ),2 )
						+ powf( ( positionOfInnerJoint.z - positionOfOuterJoint.z ),2 ) );
	boneLength = ( boneLength/100.0f )/0.2f;

	// 4-ta kost v interFingerBoneGroup
	Leap::HandBone* bone = static_cast<Leap::HandBone*>( interFingerBoneGroup->getChild( 3 ) );

	osg::RefMatrixd* boneMatrix = new osg::RefMatrixd();
	boneMatrix->makeIdentity();

	// position of bone
//    bonePosition = recalculateDepthNode(bonePosition);
	boneMatrix->preMult( osg::Matrix::translate(
							 this->center[0] +this->direction[0] + static_cast<double>( bonePosition.x )/100.0,
							 this->center[1] +this->direction[1] + static_cast<double>( bonePosition.y )/100.0,
							 this->center[2] +this->direction[2] + static_cast<double>( bonePosition.z )/100.0 ) );

	// rotation of bone
	boneMatrix->preMult( osg::Matrix::rotate( osg::Vec3f( 0.0f,0.0f,1.0f ),
						 osg::Vec3f( boneDirection.x/100.0f, boneDirection.y/100.0f, boneDirection.z/100.0f ) ) );
	// scaling of bone
	boneMatrix->preMult( osg::Matrix::scale( 1.0,1.0,static_cast<double>( boneLength ) ) );

	bone->setMatrix( *boneMatrix );

}
