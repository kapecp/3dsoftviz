#ifndef BROWSERGROUP_H
#define BROWSERGROUP_H

#include <osg/ref_ptr>
#include <osg/AutoTransform>
#include <osg/Group>
#include <QList>

#include "Data/Node.h"
#include "Util/ApplicationConfig.h"
#include "LuaGraph/LuaGraphTreeModel.h"

namespace Vwr
{
/**
    *  \class BrowserGroup
    *  \brief
    *  \author Michael Gloger
    *  \date 16. 11. 2014
    */
class BrowserGroup
{
public:
    /**
        *  \fn public constructor  BrowserGroup()
        *  \brief Creates browser group
        */
    BrowserGroup();

    /**
        *  \fn public destructor  ~BrowserGroup
        *  \brief destructor
        */
    ~BrowserGroup(void);

    /**
        *  \fn inline public  getGroup
        *  \brief Returns wrapped browsers group
        *  \return osg::ref_ptr browsers group
        */
    osg::ref_ptr<osg::Group> getGroup()	{return group;}

    /**
        *  \fn inline public  addBrowser
        *  \brief Adds browser at position with data model to group
        *  \param  position
        *  \param  model
        */
    void addBrowser(osg::Vec3 position, Lua::LuaGraphTreeModel *model);

    /**
        *  \fn inline public  clearBrowsers
        *  \brief Removes all browsers from group
        */
    void clearBrowsers();


private:
    /**
        *  osg::ref_ptr group
        *  \brief browsers group
        */
    osg::ref_ptr<osg::Group> group;

    /**
        *  Util::ApplicationConfig * appConf
        *  \brief application configuration
        */
    Util::ApplicationConfig* appConf;
};
}

#endif // BROWSERGROUP_H
