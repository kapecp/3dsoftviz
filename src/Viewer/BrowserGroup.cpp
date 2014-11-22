#include "Viewer/BrowserGroup.h"

#include "OsgQtBrowser/QWebViewImage.h"
#include <osg/ValueObject>
#include <math.h>

using namespace Vwr;

BrowserGroup::BrowserGroup()
{
	this->group = new osg::Group;
	this->browsersTransforms = new QList<osg::ref_ptr<osg::AutoTransform> >;
}

BrowserGroup::~BrowserGroup(void)
{
}

void BrowserGroup::addBrowser(osg::Vec3 position, Lua::LuaGraphTreeModel *model)
{
	// std::cout << "Adding browser to Lua Node\n";
	// std::cout << flush;

	// Create webView
	osg::ref_ptr<QWebViewImage> webView = new QWebViewImage();
	webView->navigateTo("http://www.google.com");

	// Add it to browser
	osgWidget::GeometryHints hints(osg::Vec3(30.0f,30.0f,0.0f),
								   osg::Vec3(100.0f,0.0f,0.0f),
								   osg::Vec3(0.0f,100.0f,0.0f),
								   osg::Vec4(1.0f,1.0f,1.0f,1.0f),
								   osgWidget::GeometryHints::RESIZE_HEIGHT_TO_MAINTAINCE_ASPECT_RATIO);

	osg::ref_ptr<osgWidget::Browser> browser = new osgWidget::Browser;
	browser->assign(webView, hints);

	// Wrap browser to transform
	osg::ref_ptr<osg::AutoTransform> transform = new osg::AutoTransform;
	transform->setPosition(position);
	transform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	transform->addChild(browser);

	// Set initial scale & set animation start frame to help us calculate interpolation value
	transform->setScale(0);
	transform->setUserValue("frame", 0);

	// Add transform to group
	this->group->addChild(transform);

	// Remember transform
	this->browsersTransforms->append(transform);
}

void BrowserGroup::clearBrowsers()
{
	// Removes all children grom group
	// TODO make sure no memory is leaking ...
	this->group->removeChildren(0, this->group->getNumChildren());
	this->browsersTransforms->clear();
}

double BrowserGroup::interpolate(long currentFrame, long endFrame, double startValue, double endValue)
{
	double value = endValue * (pow((currentFrame / (float)endFrame) - 1, 5) + 1 + startValue);

	// std::cout << value << " " << currentFrame << " " << endFrame << " " << startValue << " " << endValue << endl;
	// std::cout << flush;

	return value;
}

void BrowserGroup::updateBrowsers()
{
	QList<osg::ref_ptr<osg::AutoTransform> >::iterator i;
	osg::ref_ptr<osg::AutoTransform> transform;
	int frame;

	// Interpolate each browser transform scale using interpolation function
	for (i = this->browsersTransforms->begin(); i != this->browsersTransforms->end(); i++){

		transform = *i;

		if(transform->getScale().x() < 1){

			// Get animation frame for current transform
			transform->getUserValue("frame", frame);

			// Apply interpolation function
			transform->setScale(this->interpolate(frame, 20, 0, 1));

			// Increment transform animation time
			transform->setUserValue("frame", frame + 1);
		}
	}
}
