#include "Viewer/BrowserGroup.h"

#include "OsgQtBrowser/QWebViewImage.h"

using namespace Vwr;

BrowserGroup::BrowserGroup()
{
    this->group = new osg::Group;
    this->appConf = Util::ApplicationConfig::get();
}

BrowserGroup::~BrowserGroup(void)
{
}

void BrowserGroup::addBrowser(osg::Vec3 position, Lua::LuaGraphTreeModel *model)
{
    float graphScale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat();

    // Create webView
    osg::ref_ptr<QWebViewImage> webView = new QWebViewImage();
    webView->navigateTo("http://www.google.com");

    // Add it to browser
    osgWidget::GeometryHints hints(osg::Vec3(-50.0f,-50.0f,0.0f),
                                   osg::Vec3(100.0f,0.0f,0.0f),
                                   osg::Vec3(0.0f,100.0f,0.0f),
                                   osg::Vec4(1.0f,1.0f,1.0f,1.0f),
                                   osgWidget::GeometryHints::RESIZE_HEIGHT_TO_MAINTAINCE_ASPECT_RATIO);

    osg::ref_ptr<osgWidget::Browser> browser = new osgWidget::Browser;
    browser->assign(webView, hints);

    // Wrap browser to transform
    osg::ref_ptr<osg::AutoTransform> transform = new osg::AutoTransform;
    transform->setPosition(position * graphScale);
    transform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
    transform->addChild(browser);

    // Add transform to group
    this->group->addChild(transform);
}

void BrowserGroup::clearBrowsers()
{
    // Removes all children grom group
    // TODO make sure no memory is leaking ...
    this->group->removeChildren(0, this->group->getNumChildren());
}
