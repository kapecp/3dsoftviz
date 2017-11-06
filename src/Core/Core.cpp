
/*!
 * Core.cpp
 * Projekt 3DVisual
 */

#include "Core/Core.h"

#include "QOSG/CoreWindow.h"
#include "QOSG/MessageWindows.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include "Manager/Manager.h"
#include "Util/ApplicationConfig.h"


AppCore::Core* AppCore::Core::core;

AppCore::Core::Core( QApplication* app )
{
	//Application initialization
	core = this;

	Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();

	messageWindows = new QOSG::MessageWindows();

	//Counting forces for layout algorithm, init layout, viewer and window
	this->alg = new Layout::FRAlgorithm();

	this->thr = new Layout::LayoutThread( this->alg );
	this->cg = new Vwr::CoreGraph();
	this->cw = new QOSG::CoreWindow( 0, this->cg, app, this->thr );

	int width =appConf->getValue( "UI.MainWindow.DefaultWidth" ).toInt();
	int height= appConf->getValue( "UI.MainWindow.DefaultHeight" ).toInt();

	if ( appConf->getValue( "Viewer.Display.Multiview" ).toInt() ) {
		int screenNum = appConf->getValue( "Viewer.Display.ScreenNum" ).toInt();
		width = appConf->getValue( "Viewer.Display.MaxScreenWidth" ).toInt() * screenNum;
		height = appConf->getValue( "Viewer.Display.MaxScreenHeight" ).toInt();
	}



	this->cw->resize( width, height );
	this->cw->show();

	app->exec();
}

AppCore::Core::~Core()
{
}

void AppCore::Core::restartLayout( bool fixedPositions )
{
	// [GrafIT][!] the layout algorithm did not end correctly, what caused more instances
	// to be running, fixed it here + made modifications in FRAlgorithm to make correct ending possible
	//this->thr->requestEnd();
	//this->thr->wait();
	//delete this->thr;

	this->alg->SetGraph( Manager::GraphManager::getInstance()->getActiveGraph(), fixedPositions );

	this->alg->SetParameters( 50,0.7f,true );
	this->thr = new Layout::LayoutThread( this->alg );
	this->cw->setLayoutThread( thr );
	this->cg->reload( Manager::GraphManager::getInstance()->getActiveGraph() );
	this->thr->start();
	//this->thr->play();
	this->messageWindows->closeLoadingDialog();
}

void AppCore::Core::restartLayoutForMatrix()
{
	this->alg->SetGraph( Manager::GraphManager::getInstance()->getActiveGraph(), true );

	this->alg->SetParameters( 50,0.7f,true );
	this->thr = new Layout::LayoutThread( this->alg );
	this->cw->setLayoutThread( thr );
	this->cg->reload( Manager::GraphManager::getInstance()->getActiveGraph() );

	this->messageWindows->closeLoadingDialog();
}

AppCore::Core* AppCore::Core::getInstance( QApplication* app )
{
	if ( core == NULL ) {
		if ( app != NULL ) {
			core = new AppCore::Core( app );
		}
		else {
			qDebug() << "Internal error.";
			return NULL;
		}
	}

	return core;
}

QOSG::CoreWindow* AppCore::Core::getCoreWindow() const
{
	return this->cw;
}

Vwr::CoreGraph* AppCore::Core::getCoreGraph() const
{
	return this->cg;
}



