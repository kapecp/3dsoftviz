
/*!
 * Core.cpp
 * Projekt 3DVisual
 */

#include "Core/Core.h"
#include "Util/ApplicationConfig.h"
#include "Layout/RestrictionApplier.h"
#include "Layout/RestrictionApplier_Sphere.h"
#include "Layout/RestrictionApplier_SphereSurface.h"

AppCore::Core * AppCore::Core::core;

AppCore::Core::Core(QApplication * app)
{
    core = this;

    Util::ApplicationConfig *appConf = Util::ApplicationConfig::get();

    messageWindows = new QOSG::MessageWindows();

    this->alg = new Layout::FRAlgorithm();
    std::auto_ptr<Layout::RestrictionApplier> restrictionApplier (new Layout::RestrictionApplier_SphereSurface(osg::Vec3f (0, 0, 0), 50));
    this->alg->SetRestrictionApplier(restrictionApplier);

    this->thr = new Layout::LayoutThread(this->alg);
    this->cg = new Vwr::CoreGraph();
    this->cw = new QOSG::CoreWindow(0, this->cg, app, this->thr);
    this->cw->resize(
    	appConf->getNumericValue (
    		"UI.MainWindow.DefaultWidth",
    		std::auto_ptr<long> (new long(200)),
    		std::auto_ptr<long> (NULL),
    		1024
    	),
    	appConf->getNumericValue (
			"UI.MainWindow.DefaultHeight",
			std::auto_ptr<long> (new long(200)),
			std::auto_ptr<long> (NULL),
			768
		)
    );
    this->cw->show();

    app->exec();
}

AppCore::Core::~Core()
{
}

void AppCore::Core::restartLayout()
{
    // neviem preco sa tuto nejak dlho zastavi
    if(this->thr->isRunning())
    {
        this->thr->terminate();
        this->thr->wait();
    }
    delete this->thr;

    this->alg->SetGraph(Manager::GraphManager::getInstance()->getActiveGraph());
    this->alg->SetParameters(10,0.7,1,true);
    this->thr = new Layout::LayoutThread(this->alg);
    this->cw->setLayoutThread(thr);
    this->cg->reload(Manager::GraphManager::getInstance()->getActiveGraph());
    this->thr->start();
	this->thr->play();
    this->messageWindows->closeLoadingDialog();
}

AppCore::Core * AppCore::Core::getInstance(QApplication * app)
{
        if(core == NULL)
        {
                if (app != NULL)
					core = new AppCore::Core(app);
                else
                {
                        qDebug() << "Internal error.";
                        return NULL;
                }
        }

        return core;
}
