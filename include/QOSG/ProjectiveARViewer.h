/**
*  ProjectiveARViewer.h
*  Projekt 3DVisual
*/
#ifndef QOSG_PROJECTIVE_AR_VIEWER_DEF
#define QOSG_PROJECTIVE_AR_VIEWER_DEF 1

#include "QOSG/AdapterWidget.h"
#include <osgViewer/ViewerBase>
#include <osgViewer/ViewerEventHandlers>
#include <qtimer.h>
#include <QObject>

/*!
* \brief
* Widget viewing OSG scene for projective augmented reality.
*
* \author
* Viktor Kostan
* \version
* 1.0
* \date
* 21.4.2016
*/


namespace Vwr {
class PickHandler;
class CoreGraph;
}

namespace Util {
class ApplicationConfig;
}


namespace Vwr {
class CameraManipulator;
}


namespace QOSG {
/**
    *  \class ProjectiveARViewer
    *  \brief
    *  \author Viktor Kostan
    *  \date 21. 4. 2016
    */
class ProjectiveARViewer : public osgViewer::Viewer, public AdapterWidget
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

    ProjectiveARViewer( QWidget* parent = 0, const char* name = 0, const QGLWidget* shareWidget = 0, WindowFlags f = 0, osgViewer::Viewer* viewerPerspective = 0 );


    /**
          *  \fn inline public  reloadConfig
          *  \brief Reloads configuration
          */
    void reloadConfig();

    /**
        *  void updateScene()
        *  \brief update projector's and viewer's position, direction and field of view
        */
    void updateScene();


    // getters
    osg::Vec3d getViewerPos()
    {
        return viewerPos;
    }
    osg::Vec3d getViewerDir()
    {
        return viewerDir;
    }
    double getViewerFOV()
    {
        return viewerFOV;
    }
    osg::Vec3d getProjectorPos()
    {
        return projectorPos;
    }
    osg::Vec3d getProjectorDir()
    {
        return projectorDir;
    }
    double getProjectorFOV()
    {
        return projectorFOV;
    }
    // setters
    void setViewerPos(double x, double y, double z)
    {
        viewerPos.set(x, y, z);
    }
    void setViewerDir(double x, double y, double z)
    {
        viewerDir.set(x, y, z);
    }
    void setViewerFOV(double fov)
    {
        viewerFOV = fov;
    }
    void setProjectorPos(double x, double y, double z)
    {
        projectorPos.set(x, y, z);
    }
    void setProjectorDir(double x, double y, double z)
    {
        projectorDir.set(x, y, z);
    }
    void setProjectorFOV(double fov)
    {
        projectorFOV = fov;
    }

protected:


    /**
        *  QTimer _timer
        *  \brief
        */
    QTimer _timer;

    /**
        *  osgViewer::Viewer * cg
        *  \brief core graph
        */
    osgViewer::Viewer* viewerPerspective;


    /**
        *  \fn inline protected virtual  paintGL
        *  \brief Paints new frame
        */

    virtual void paintGL();



private:

    /**
        *  osg::Node* createModel()
        *  \brief camera that renders model to texture
        */
    osg::Camera* renderCamera;


    /**
        *  Util::ApplicationConfig * appConf
        *  \brief application configuration
        */
    Util::ApplicationConfig* appConf;

    osg::Vec3d projectorPos;
    osg::Vec3d projectorDir;
    double projectorFOV;

    osg::Vec3d viewerPos;
    osg::Vec3d viewerDir;
    double viewerFOV;


};
}

#endif
