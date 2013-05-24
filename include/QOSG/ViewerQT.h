/**
*  ViewerQT.h
*  Projekt 3DVisual
*/
#ifndef QOSG_VIEWER_QT_DEF
#define QOSG_VIEWER_QT_DEF 1

#include "QOSG/AdapterWidget.h"
#include <osgViewer/ViewerBase>

#include "Viewer/CameraManipulator.h"
#include "Util/ApplicationConfig.h"
#include "Viewer/PickHandler.h"

#include "Viewer/CameraManipulator.h"

/*!
* \brief
* Widget, ktoryzobrazuje OSG sceny.
*
* \author
* Michal Paprcka
* \version 
* 1.0
* \date 
* 1.12.2009
*/

namespace QOSG
{
	/**
	*  \class ViewerQT
	*  \brief 
	*  \author Adam Pazitnaj 
	*  \date 29. 4. 2010
	*/
	class ViewerQT : public osgViewer::Viewer, public AdapterWidget
	{
	public:

		/*!
		* 
		* \param parent
		* Rodic widgetu.
		* 
		* \param name
		* Meno widgetu.
		* 
		* \param shareWidget
		* Zdielanie widgetu.
		* 
		* \param f
		* Znacky pre vytvaranie okna.
		* 
		* \param cg
		* CoreGraf, ktory sa bude vykreslovat.
		* 
		* 
		* Vytvori widget, ktory dokaze zobrazit OSG grafy.
		* 
		*/
		ViewerQT(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0, Vwr::CoreGraph* cg = 0):
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


		  /**
		  *  \fn inline public constant  getPickHandler 
		  *  \brief Returns pick handler
		  *  \return Vwr::PickHandler * pick handler
		  */
		  Vwr::PickHandler * getPickHandler() const {return pickHandler;}

		  /**
		  *  \fn inline public constant  getCameraManipulator
		  *  \brief Returns camera manipulator
		  *  \return Vwr::CameraManipulator * camera manipulator
		  */
		  Vwr::CameraManipulator * getCameraManipulator() const {return manipulator;}


		  /**
		  *  \fn inline public  reloadConfig
		  *  \brief Reloads configuration
		  */
		  void reloadConfig()
	  {
		  manipulator->setMaxSpeed(appConf->getValue("Viewer.CameraManipulator.MaxSpeed").toFloat());
		  manipulator->setTrackballSize(appConf->getValue("Viewer.CameraManipulator.Sensitivity").toFloat());

		  osg::DisplaySettings::instance()->setStereo(appConf->getValue("Viewer.Display.Stereoscopic").toInt());
	  }

	protected:


		/**
		*  QTimer _timer
		*  \brief 
		*/
		QTimer _timer;

		/**
		*  Vwr::CoreGraph * cg
		*  \brief core graph
		*/
		Vwr::CoreGraph* cg;


		/**
		*  \fn inline protected virtual  paintGL
		*  \brief Paints new frame
		*/
		virtual void paintGL()
		{
			frame();
			cg->update();
		}

	private:

		/**
		*  Vwr::CameraManipulator * manipulator
		*  \brief camera manipulator
		*/
		Vwr::CameraManipulator * manipulator;

		/**
		*  Vwr::PickHandler * pickHandler
		*  \brief pick handler
		*/
		Vwr::PickHandler * pickHandler;

		/**
		*  Util::ApplicationConfig * appConf
		*  \brief application configuration
		*/
		Util::ApplicationConfig * appConf;

	};
}

#endif