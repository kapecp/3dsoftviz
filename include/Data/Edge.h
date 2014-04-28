/*!
* Edge.h
* Projekt 3DVisual
*/
#ifndef DATA_EDGE_DEF
#define DATA_EDGE_DEF 1

#include "Data/Node.h"

#include <QString>
#include <QtCore/QMap>

#include <osg/Camera>
#include <osgText/FadeText>

namespace Util
{
class ApplicationConfig;
}

namespace Data
{
class Node;
class Graph;
class Type;

/**
	*  \class Edge
	*  \brief Object represents an edge in a Graph.
	*  \author Aurel Paulovic, Michal Paprcka
	*  \date 29. 4. 2010
	*/
class Edge : public osg::DrawArrays
{
public:

	/**
		* \fn public constructor Edge(qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0)
		* \brief  Creates new Edge object connecting two Nodes
		*
		* \param id	ID of the Edge
		* \param name 		name of the Edge
		* \param graph 	Graph to which the Edge belongs
		* \param srcNode 		starting Node
		* \param dstNode 	ending Node
		* \param type 		Type of the Edge.
		* \param isOriented 		true if the Edge is oriented.
		* \param pos int		first coordinate in Drawable coordinates array
		* \param camera 	current camera used in viewer
		*/
	Edge(qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, float scaling, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0);

	/**
		*  \fn public destructor  ~Edge
		*  \brief Destroys the Edge object
		*/
	~Edge(void);

	/**
		*  \fn inline public  getId
		*  \brief Returns ID of the Edge
		*  \return qlonglong ID of the Edge
		*/
	qlonglong getId() const { return id; }

	/**
		*  \fn inline public  getName
		*  \brief Returns the name of the Edge
		*  \return QString name of the Edge
		*/
	QString getName() const { return name; }

	/**
		*  \fn inline public  setName(QString val)
		*  \brief Sets new name
		*  \param   name   new Name for the Edge
		*  \return QString resultant name of the Edge
		*/
	void setName(QString val) { name = val; }

	/**
		*  \fn inline public  getScale()
		*  \brief Gets edge scale
		*  \return float size of the scale
		*/
	float getScale() { return scale; }

	/**
		*  \fn inline public  setScale(float val)
		*  \brief Sets new scale
		*  \param   val   new scale for the Edge
		*/
	void setScale(float val) { scale = val; }

	/**
		* \fn inline public constant getSrcNode
		* \brief  Returns the starting Node of the Edge
		* \return osg::ref_ptr<Data::Node> starting Node of the Edge
		*/
	osg::ref_ptr<Data::Node> getSrcNode() const { return srcNode; }

	/**
		* \fn inline public constant setSrcNode
		* \brief  Sets new starting Node of the Edge
		*
		* OBSOLETE - DO NOT USE IT
		*
		* \param val new starting Node of the Edge
		*/
	void setSrcNode(osg::ref_ptr<Data::Node> val) { srcNode = val; }

	/**
		* \fn inline public constant getDstNode
		* \brief  Returns ending Node of the Edge
		* \return osg::ref_ptr<Data::Node> ending Node of the Edge
		*/
	osg::ref_ptr<Data::Node> getDstNode() const { return dstNode; }

	/**
		* \fn inline public setDstNode(osg::ref_ptr<Data::Node> val)
		* \brief Sets new ending Node of the Edge
		*
		* OBSOLETE - DO NOT USE IT
		*
		* \param val new ending Node of the Edge
		*/
	void setDstNode(osg::ref_ptr<Data::Node> val) { dstNode = val; }

	/**
				* \fn public getSecondNode(osg::ref_ptr<Data::Node> firstNode)
				* \brief  Returns the second ending Node of the Edge
				* \return osg::ref_ptr<Data::Node> the second ending Node of the Edge
				*/
	osg::ref_ptr<Data::Node> getSecondNode(osg::ref_ptr<Data::Node> firstNode);

	/**
		* \fn inline public constant getType
		* \brief  Returns the Type of the Edge
		* \return Data::Type * Type of the Edge
		*/
	Data::Type* getType() const { return type; }

	/**
		* \fn void setType(Data::Type* val)
		* \brief  Sets new Type of the Edge
		*
		* OBSOLETE - DO NOT USE IT
		*
		* \param  val new Type of the Edge
		*/
	void setType(Data::Type* val) { type = val; }

	/**
		* \fn inline public constant isOriented
		* \brief Returns if the Edge is oriented or not
		* \return bool true, if the Edge is oriented
		*/
	bool isOriented() const { return oriented; }

	/**
		* \fn inline public setOriented(bool val)
		* \brief  Sets if the Edge is oriented
		* \param val 		true, if the Edge is oriented
		*/
	void setOriented(bool val) { oriented = val; }

	/**
		* \fn public linkNodes(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges)
		* \brief  Links the Edge to it's Nodes and adds itself to the edges
		* \param  edges
		*/
	void linkNodes(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges);

	/**
		* \fn public unlinkNodes
		* \brief Unlinks the Edge from the Nodes
		*/
	void unlinkNodes();

	/**
		* \fn public unlinkNodesAndRemoveFromGraph
		* \brief Unlinks the Edge from the Nodes and removes the Edge from it's Graph
		*/
	void unlinkNodesAndRemoveFromGraph();

	/**
		* \fn inline public constant toString
		* \brief  Returns human-readable string representation of the Edge
		* \return QString
		*/

	QString toString() const ;



	/**
		*  \fn inline public constant  getCooridnates
		*  \brief Returns coordinates of the Edge
		*  \return osg::ref_ptr coordinates of the Edge
		*/
	osg::ref_ptr<osg::Vec3Array> getCooridnates() const { return coordinates; }

	/**
		*  \fn inline public constant  getLength
		*  \brief Returns length of the Edge
		*  \return float length of the Edge
		*/

	double getLength() const { return length; }


	/**
		*  \fn public  updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode)
		*  \brief Updates coordinates of the Edge
		*  \param    srcPos    new coordinates of the starting Node
		*  \param   dstNode    new coordinates of the ending Node
		*/
	void updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode);


	/**
		*  \fn inline public constant  getEdgeColor
		*  \brief Returns the color of the Edge
		*  \return osg::Vec4 color of the Edge
		*/
	osg::Vec4 getEdgeColor() const
	{
		if (selected)
			return osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f); // color of selected node
		else
			return edgeColor;
	}


	/**
		*  \fn inline public  setEdgeColor(osg::Vec4 val)
		*  \brief Sets the color of the Edge
		*  \param     val   new color
		*/
	void setEdgeColor(osg::Vec4 val) { edgeColor = val; }


	/**
		*  \fn inline public constant  getEdgeTexCoords
		*  \brief Returns Texture coordinates array.
		*  \return osg::ref_ptr<osg::Vec2Array>
		*/
	osg::ref_ptr<osg::Vec2Array> getEdgeTexCoords() const { return edgeTexCoords; }


	/**
		*  \fn inline public constant  getCamera
		*  \brief returns current camera looking at edge
		*  \return osg::ref_ptr<osg::Camera>
		*/
	osg::ref_ptr<osg::Camera> getCamera() const { return camera; }


	/**
		*  \fn inline public  setCamera(osg::ref_ptr<osg::Camera> val)
		*  \brief sets current camera used in viewer.
		*  \param       val
		*/
	void setCamera(osg::ref_ptr<osg::Camera> val) { camera = val; }

	/**
		*  \fn inline public constant  isSelected
		*  \brief Returns selected flag
		*  \return bool true, if the Edge is selected
		*/
	bool isSelected() const { return selected; }


	/**
		*  \fn inline public  setSelected(bool val)
		*  \brief Sets the selected flag of the Edge
		*  \param       val   true, if the Edge is selected
		*/
	void setSelected(bool val)
	{
		selected = val;
	}


	/**
		*  \fn public  createLabel(QString name)
		*  \brief Creates new label
		*  \param      name     new label
		*  \return osg::ref_ptr added label
		*/
	osg::ref_ptr<osg::Drawable> createLabel(QString name);


	/**
		*  \fn inline public  isInDB
		*  \brief Returns true, if the Edge is already in database
		*  \return bool
		*/
	bool isInDB() { return inDB; }

	/**
		*  \fn inline public  setIsInDB
		*  \brief Sets the inDB flag of the Edge to true (meaning that the Edge is in database)
		*/
	void setIsInDB() { inDB = true; };


	/**
		*  \fn inline public  getGraph
		*  \brief Returns the Graph to which is the Edge assigned
		*  \return Data::Graph * Edge's Graph
		*/
	Data::Graph* getGraph() { return graph; }


	/**
		*  \fn inline public constant  getSettings
		*  \brief Returns current settings of the Edge
		*  \return QMap<QString,QString> * settings of the Edge
		*/
	QMap<QString, QString> * getSettings() const { return settings; }

	/**
		*  \fn inline public  setSettings(QMap<QString, QString> * val)
		*  \brief Sets new settings to the Edge
		*  \param [in, out]  val QMap<QString,QString> *    new settings
		*/
	void setSettings(QMap<QString, QString> * val) { settings = val; }

	int getWeight() { return 1; }

	/**
				*  \fn public  setSharedCoordinates (bool x, bool y, bool z)
				*  \brief sets shared coordinates
				*  \param x true, if nodes share x coordinate
				*  \param y true, if nodes share y coordinate
				*  \param z true, if nodes share z coordinate
				*/
	void setSharedCoordinates (bool x, bool y, bool z){
		this->shared_X = x;
		this->shared_Y = y;
		this->shared_Z = z;
	}

	/**
				*  \fn public   isShared_X()
				*  \return value of shared_X flag
				*/
	bool isShared_X(){return this->shared_X;}

	/**
				*  \fn public   isShared_Y()
				*  \return value of shared_Y flag
				*/
	bool isShared_Y(){return this->shared_Y;}

	/**
				*  \fn public   isShared_Z()
				*  \return value of shared_Z flag
				*/
	bool isShared_Z(){return this->shared_Z;}

	/**
				*  \fn public   setInvisible()
				*/
	bool setInvisible(bool invisible){

		if (invisible){
			setScale(0);
			isInvisible=true;
		}
		else {
			setScale(1);
			isInvisible=false;
		}
		return true;
	}

	bool getIsInvisible(){return isInvisible;}


private:

	bool isInvisible;
	/**
		*  Data::Graph * graph
		*  \brief Graph to which the Edge belongs
		*/
	Data::Graph* graph;

	/**
		*  bool inDB
		*  \brief Flag if the Edge is already added to database or not
		*/
	bool inDB;

	/**
		*  qlonglong id
		*  \brief ID of the Edge
		*/
	qlonglong id;

	//! Meno hrany.
	/**
		*  QString name
		*  \brief Name of the Edge
		*/
	QString name;

	/**
		*  osg::ref_ptr<Data::Node> srcNode
		*  \brief Starting Node of the Edge
		*/
	osg::ref_ptr<Data::Node> srcNode;

	/**
		*  osg::ref_ptr<Data::Node> dstNode
		*  \brief Ending Node of the Edge
		*/
	osg::ref_ptr<Data::Node> dstNode;

	/**
		*  Data::Type * type
		*  \brief Type of the Edge
		*/
	Data::Type* type;

	/**
		*  bool oriented
		*  \brief True, if the Edge is oriented
		*/
	bool oriented;


	/**
		*  float length
		*  \brief Length of the Edge
		*/

	double length;


	/**
		*  bool selected
		*  \brief True, if edge is picked
		*/
	bool selected;

	/**
		*  float scale
		*  \brief scale of edge
		*/
	float scale;

	/**
				*  bool shared_X
				*  \brief if true, then nodes of edge have shared X coordinate
				*/
	bool shared_X;

	/**
				*  bool shared_Y
				*  \brief if true, then nodes of edge have shared X coordinate
				*/
	bool shared_Y;

	/**
				*  bool shared_Z
				*  \brief if true, then nodes of edge have shared X coordinate
				*/
	bool shared_Z;

	/**
		*  osg::ref_ptr camera
		*  \brief Current camera used in viewer. It is used for billboarding purposes.
		*/
	osg::ref_ptr<osg::Camera> camera;

	/**
		*  osg::ref_ptr coordinates
		*  \brief Coordinates of the Edge
		*/
	osg::ref_ptr<osg::Vec3Array> coordinates;

	/**
		*  osg::ref_ptr<osg::Vec2Array> edgeTexCoords
		*  \brief Texture coordinates array.
		*/
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords;

	/**
		*  osg::ref_ptr label
		*  \brief Label of the Edge
		*/
	osg::ref_ptr<osgText::FadeText> label;


	/**
		*  Util::ApplicationConfig * appConf
		*  \brief ApplicationConfig
		*/
	Util::ApplicationConfig * appConf;


	/**
		*  osg::Vec4 edgeColor
		*  \brief Color of the Edge
		*/
	osg::Vec4 edgeColor;

protected:

	/**
		*  QMap<QString,QString> * settings
		*  \brief Settings of the Egde
		*/
	QMap<QString, QString> * settings;
};
}

#endif
