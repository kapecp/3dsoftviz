#ifndef BROWSERGROUP_H
#define BROWSERGROUP_H

#include <osg/ref_ptr>
#include <osg/AutoTransform>
#include <osg/Group>
#include <QList>
#include <osg/Geometry>
#include <osg/Geode>

#include "OsgQtBrowser/QWebViewImage.h"
#include "Data/Node.h"
#include "LuaGraph/LuaNode.h"
#include "LuaTypes/LuaValueMap.h"
#include "LuaGraph/LuaGraphTreeModel.h"

namespace Vwr {
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
	~BrowserGroup( void );

	/**
	    *  \fn public  setSelectedNodes
	    *  \brief Adds only nodes not previously added to selectedNodesModels map
	    *  \param  selected
	    */
	void setSelectedNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selected );

	/**
	    *  \fn public  setBrowsersGrouping
	    *  \brief Changes browsersGrouping value
	    */
	void setBrowsersGrouping( bool browsersGrouping );

	/**
	    *  \fn public  updateBrowsers
	    *  \brief Animates browsers pop out interpolation
	    */
	void updateBrowsers();

	/**
	    *  \fn public  getGroup
	    *  \brief Returns wrapped browsers group
	    *  \return osg::ref_ptr browsers group
	    */
	osg::ref_ptr<osg::Group> getGroup()
	{
		return group;
	}

	/**
	    *  \fn public  getSelectedNodesModels
	    *  \return Selected nodes models map
	    */
	inline QMap<qlonglong, Lua::LuaGraphTreeModel* >* getSelectedNodesModels()
	{
		return selectedNodesModels;
	}

private:

	/**
	    *  \fn private  addBrowser
	    *  \brief Adds browser at position, passes supplied list of model data objects to js variable qData and calls js function qDataReady.
	    *  \param templateType type of the template route
	    *  \param  position
	    *  \param  models
	    */
	void addBrowser( const std::string& templateType, osg::Vec3 position, Lua::LuaValueMap models );

	/**
	    *  \fn private  createBorderGeode
	    *  \brief returns Geode containing line Geometry for webview border
	    *  \param  bl bottom left
	    *  \param  br bottom right
	    *  \param  tr top right
	    *  \param  tl top left
	    */
	osg::Geode* createBorderGeode( osg::Vec3 bl, osg::Vec3 br, osg::Vec3 tr, osg::Vec3 tl );

	/**
	    *  \fn private  createConnectorsGeode
	    *  \brief returns Geode containing line geometries connecting webview center to all supplied nodes positions (targets)
	    *  \param  center center of the webview
	    *  \param  targets array of targets. There will be targets.size lines created
	    */
	osg::Geode* createConnectorsGeode( osg::Vec3 center, osg::Vec3Array* targets );

	/**
	    *  \fn private  createLinesGeode
	    *  \brief returns geode created from supplied geometry
	    *  \param linesGeom lines geometry which will be added to geode
	    */
	osg::Geode* createLinesGeode( osg::Geometry* linesGeom );

	/**
	*  \fn public  updateTransforms
	*  \brief Animates browser transform scale
	*  \param transforms
	*/
	void updateTransforms( QList<osg::ref_ptr<osg::AutoTransform> >* transforms );

	/**
	    *  \fn private  initBrowsers
	    *  \brief Initializes sepparate browser for all selected nodes models
	    */
	void initBrowsers();

	/**
	    *  \fn private  initGroupedBrowser
	    *  \brief Initializes one grouped browser for all selected nodes models
	    */
	void initGroupedBrowser();

	/**
	    *  \fn private  clearBrowsers
	    *  \brief Removes all browsers from group
	    */
	void clearBrowsers();

	/**
	    *  \fn private  clearModels
	    *  \brief Removes all model
	    */
	void clearModels();

	/**
	    *  osg::ref_ptr group
	    *  \brief browsers group
	    */
	osg::ref_ptr<osg::Group> group;

	/**
	    *  bool browsersGrouping
	    *  \brief bool value indicating if browsers are grouping into one
	    */
	bool browsersGrouping;

	/**
	    *  \brief List of browsers transforms
	    */
	QList<osg::ref_ptr<osg::AutoTransform> >* browsersTransforms;

	/**
	    *  \brief List of connectros transforms (lines connecting browser and node)
	    */
	QList<osg::ref_ptr<osg::AutoTransform> >* connectorsTransforms;

	/**
	    *  \brief List of selected nodes
	    */
	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes;

	/**
	    *  \brief List of selected nodes
	    */
	QMap<qlonglong, Lua::LuaGraphTreeModel*>* selectedNodesModels;

	/**
	    *  \fn public  interpolate
	    *  \brief Calculates interpolation function
	    *  \param  currentFrame
	    *  \param  endFrame
	    *  \param  startValue
	    *  \param  endValue
	    *  \return Caculated interpolation value
	    */
	double interpolate( long currentFrame, long endFrame, double startValue, double endValue );
};
}

#endif // BROWSERGROUP_H
