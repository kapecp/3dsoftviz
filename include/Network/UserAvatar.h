#ifndef UserAvatar_H
#define UserAvatar_H

#include <Leap/HandModule/Model/HandPalm.h>

#include <QString>
#include <osg/PositionAttitudeTransform>
#include <osg/Group>

namespace Network {

class UserAvatar : public osg::PositionAttitudeTransform
{
public:
	UserAvatar( QString label );
	void UpdateHands( QDataStream* stream );

protected:
	virtual void initStructure();

private:
	osg::ref_ptr<osg::MatrixTransform> handsGroup;
	osg::ref_ptr<Softviz::Leap::HandPalm> leftHand;
	osg::ref_ptr<Softviz::Leap::HandPalm> rightHand;

	QString label;
};

}

#endif
