#ifndef UserAvatar_H
#define UserAvatar_H

#include <Leap/HandModule/Model/HandPalm.h>
#include <Leap/CustomLeapManager.h>

#include <QString>
#include <osg/PositionAttitudeTransform>
#include <osg/Group>

namespace Network {

class UserAvatar : public osg::PositionAttitudeTransform
{
public:
	UserAvatar( QString label );
	void UpdateHands( Leap::HandPalm* leftHand, Leap::HandPalm* rightHand );

protected:
	virtual void initStructure();

private:
	Leap::CustomLeapManager* handManager;
	osg::ref_ptr<osg::Group> handsGroup;
	QString label;
};

}

#endif
