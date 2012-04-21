#include "Network/Helper.h"
#include "Util/ApplicationConfig.h"

#include <QDebug>

#include <osg/Matrix>
#include <osgDB/ReadFile>
#include <osgText/FadeText>
#include <osg/AutoTransform>

namespace Network {

osg::Quat Helper::lookAt(osg::Vec3 from, osg::Vec3 to){
    osg::Quat quad;
    osg::Matrix matrix;
    matrix.makeLookAt(from, to, osg::Z_AXIS);
    matrix.get(quad);
    return quad.inverse();
}

osg::PositionAttitudeTransform * Helper::generateAvatar(QString label) {
    osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("avatar.osg");
    if (!modelNode) {
        qDebug() << "could not find model";
        return NULL;
    }

    osg::ref_ptr<osgText::FadeText> labelNode = new osgText::FadeText;
    osg::Geode* textGeode = new osg::Geode();
    textGeode->addDrawable(labelNode);

    osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
    PAtransform->addChild(modelNode);

    osg::AutoTransform* autoTransformText = new osg::AutoTransform;
    autoTransformText->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
    autoTransformText->addChild(textGeode);

    PAtransform->addChild(autoTransformText);

    QString fontPath = Util::ApplicationConfig::get()->getValue("Viewer.Labels.Font");

    if(fontPath != NULL && !fontPath.isEmpty())
            labelNode->setFont(fontPath.toStdString());

    labelNode->setFadeSpeed(0.03);

    labelNode->setText(label.toStdString());
    labelNode->setLineSpacing(0);
    labelNode->setAxisAlignment(osgText::Text::XY_PLANE);
    labelNode->setCharacterSize(10);
    labelNode->setDrawMode(osgText::Text::TEXT);
    labelNode->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
    labelNode->setPosition( osg::Vec3(0, 7, -7) );
    labelNode->setColor( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

    return PAtransform;
}

}
