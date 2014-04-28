#include "QOSG/ViewerQT.h"

#include "Viewer/PickHandler.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Util/ApplicationConfig.h"

QOSG::ViewerQT::ViewerQT(QWidget * parent , const char * name , const QGLWidget * shareWidget , WindowFlags f , Vwr::CoreGraph* cg):
	AdapterWidget( parent, name, shareWidget, f )
{
	this->cg = cg;
	cg->setCamera(this->getCamera());

	appConf = Util::ApplicationConfig::get();

	osg::DisplaySettings::instance()->setStereo(appConf->getValue("Viewer.Display.Stereoscopic").toInt());
	osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::ANAGLYPHIC);

	getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
	getCamera()->setProjectionMatrixAsPerspective(60, static_cast<double>(width())/static_cast<double>(height()), 0.01, appConf->getValue("Viewer.Display.ViewDistance").toFloat());
	getCamera()->setGraphicsContext(getGraphicsWindow());
	getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	manipulator = new Vwr::CameraManipulator(cg);
	QObject::connect( manipulator, SIGNAL(sendMouseRotation(osg::Quat)),
					  cg, SLOT(updateGraphRotByMouse(osg::Quat)) );
	QObject::connect( manipulator, SIGNAL(sendFaceDetRotation(osg::Quat)),
					  cg, SLOT(updateGraphRotByFaceDet(osg::Quat)) );

	pickHandler = new Vwr::PickHandler(manipulator, cg);

	addEventHandler(new osgViewer::StatsHandler);
	addEventHandler(pickHandler);
	setCameraManipulator(manipulator);

	getCamera()->setClearColor(osg::Vec4(0, 0, 0, 1));
	getCamera()->setViewMatrixAsLookAt(osg::Vec3d(-10, 0, 0), osg::Vec3d(0, 0, 0), osg::Vec3d(0, 1, 0));

	setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));

	_timer.start(10);
}

Vwr::PickHandler * QOSG::ViewerQT::getPickHandler() const {
	return pickHandler;
}

Vwr::CameraManipulator * QOSG::ViewerQT::getCameraManipulator() const {
	return manipulator;
}

void QOSG::ViewerQT::reloadConfig()
{
	manipulator->setMaxSpeed(appConf->getValue("Viewer.CameraManipulator.MaxSpeed").toFloat());
	manipulator->setTrackballSize(appConf->getValue("Viewer.CameraManipulator.Sensitivity").toFloat());

	osg::DisplaySettings::instance()->setStereo(appConf->getValue("Viewer.Display.Stereoscopic").toInt());
}

void QOSG::ViewerQT::paintGL()
{
	frame();
	cg->update();
}
