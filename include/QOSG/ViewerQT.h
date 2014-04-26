/**
*  ViewerQT.h
*  Projekt 3DVisual
*/
#ifndef QOSG_VIEWER_QT_DEF
#define QOSG_VIEWER_QT_DEF 1

#include "QOSG/AdapterWidget.h"
#include <osgViewer/ViewerBase>
#include <osgViewer/ViewerEventHandlers>
#include <qtimer.h>
#include <QObject>

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


namespace Vwr
{
class PickHandler;
class CoreGraph;
}

namespace Util
{
class ApplicationConfig;
}


namespace Vwr
{
class CameraManipulator;
}


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

	ViewerQT(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0, Vwr::CoreGraph* cg = 0);




	/**
		  *  \fn inline public constant  getPickHandler
		  *  \brief Returns pick handler
		  *  \return Vwr::PickHandler * pick handler
		  */
	Vwr::PickHandler * getPickHandler() const ;


	/**
		  *  \fn inline public constant  getCameraManipulator
		  *  \brief Returns camera manipulator
		  *  \return Vwr::CameraManipulator * camera manipulator
		  */
	Vwr::CameraManipulator * getCameraManipulator() const;



	/**
		  *  \fn inline public  reloadConfig
		  *  \brief Reloads configuration
		  */
	void reloadConfig();

public slots:

	void moveMouseAruco(double positionX,double positionY,bool isClick, Qt::MouseButton button );

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

	virtual void paintGL();



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
