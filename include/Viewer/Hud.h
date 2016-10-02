#pragma once

#include <osg/Projection>
#include <osgText/Text>
#include <QSize>

namespace Vwr {
class Hud : public osg::Projection
{
protected:
	osg::ref_ptr<osg::MatrixTransform> modelview;
	osg::ref_ptr<osg::Geode> geode;
	osg::ref_ptr<osgText::Text> text;
	osg::ref_ptr<osg::Geometry> background;
	QSize size;

public:
	Hud();
	void setText( const QString& text );
	void setSize( const QSize& size );
	void setWindowSize( const QSize& size );
	void refresh();
};
}
