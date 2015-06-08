/*!
* Edge.h
* Projekt 3DVisual
*/
#ifndef DATA_EDGE_DEF
#define DATA_EDGE_DEF 1

#include "Data/Node.h"
#include "Data/OsgEdge.h"
#include "Data/AbsEdge.h"

#include <QString>
#include <osg/Switch>
#include <QtCore/QMap>

#include <osg/Camera>

#include <QTextStream>

namespace Util {
class ApplicationConfig;
}

namespace Data {
class Node;
class Graph;
class Type;

/**
    *  \class Edge
    *  \brief Object represents an edge in a Graph.
    *  \author Aurel Paulovic, Michal Paprcka
    *  \date 29. 4. 2010
    */
class Edge : public OsgEdge
{
public:
    // Marak start
    void setEdgeDefaultColor();
    // Marak end

    /**
        * \fn public constructor Edge(qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0)
        * \brief  Creates new Edge object connecting two Nodes
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
    Edge( qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, float scaling, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0 );

    /**
        *  \fn public destructor  ~Edge
        *  \brief Destroys the Edge object
        */
    ~Edge( void );

    /**
            *  \fn inline public  getName
            *  \brief Returns the name of the Edge
            *  \return QString name of the Edge
            */
            QString getName() const { return name; }

    /**
        * \fn public getSecondNode(osg::ref_ptr<Data::Node> firstNode)
        * \brief  Returns the second ending Node of the Edge
        * \return osg::ref_ptr<Data::Node> the second ending Node of the Edge
    */
    osg::ref_ptr<Data::Node> getSecondNode( osg::ref_ptr<Data::Node> firstNode );

    /**
        * \fn public linkNodes(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges)
        * \brief  Links the Edge to it's Nodes and adds itself to the edges
        * \param  edges
        */
    void linkNodes( QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges );

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
        *  \fn inline public constant  getCooridnates
        *  \brief Returns coordinates of the Edge
        *  \return osg::ref_ptr coordinates of the Edge
        */
    osg::ref_ptr<osg::Vec3Array> getCooridnates() const
    {
        return coordinates;
    }

    /**
        *  \fn inline public constant  getEdgeTexCoords
        *  \brief Returns Texture coordinates array.
        *  \return osg::ref_ptr<osg::Vec2Array>
        */
    osg::ref_ptr<osg::Vec2Array> getEdgeTexCoords() const
    {
        return edgeTexCoords;
    }

    /**
        *  \fn inline public constant  getCamera
        *  \brief returns current camera looking at edge
        *  \return osg::ref_ptr<osg::Camera>
        */
    osg::ref_ptr<osg::Camera> getCamera() const
    {
        return camera;
    }

    /**
            *  \fn public  updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode)
            *  \brief Updates coordinates of the Edge
            *  \param    srcPos    new coordinates of the starting Node
            *  \param   dstNode    new coordinates of the ending Node
            */
            void updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode);

    /**
        *  \fn inline public  setCamera(osg::ref_ptr<osg::Camera> val)
        *  \brief sets current camera used in viewer.
        *  \param       val
        */
    void setCamera( osg::ref_ptr<osg::Camera> val )
    {
        camera = val;
    }

    /**
        *  \fn inline public constant  isSelected
        *  \brief Returns selected flag
        *  \return bool true, if the Edge is selected
        */
    bool isSelected() const
    {
        return selected;
    }


    /**
        *  \fn inline public  setSelected(bool val)
        *  \brief Sets the selected flag of the Edge
        *  \param       val   true, if the Edge is selected
        */
    void setSelected( bool val )
    {
        selected = val;
    }


    /**
        *  \fn public  createLabel(QString name)
        *  \brief Creates new label
        *  \param      name     new label
        *  \return osg::ref_ptr added label
        */
    osg::ref_ptr<osg::Geode> createLabel( QString name );


    /**
        *  \fn inline public  isInDB
        *  \brief Returns true, if the Edge is already in database
        *  \return bool
        */
    bool isInDB()
    {
        return inDB;
    }

    /**
        *  \fn inline public  setIsInDB
        *  \brief Sets the inDB flag of the Edge to true (meaning that the Edge is in database)
        */
    void setIsInDB()
    {
        inDB = true;
    }

    /**
            * \fn inline public constant toString
            * \brief  Returns human-readable string representation of the Edge
            * \return QString
            */
            QString toString() const {
                QString str;
                QTextStream(&str) << "edge id:" << id << " name:" << name;
                return str;
            }

    /**
        *  \fn inline public  getGraph
        *  \brief Returns the Graph to which is the Edge assigned
        *  \return Data::Graph * Edge's Graph
        */
    Data::Graph* getGraph()
    {
        return graph;
    }


    /**
        *  \fn inline public constant  getSettings
        *  \brief Returns current settings of the Edge
        *  \return QMap<QString,QString> * settings of the Edge
        */
    QMap<QString, QString>* getSettings() const
    {
        return settings;
    }

    /**
        *  \fn inline public  setSettings(QMap<QString, QString> * val)
        *  \brief Sets new settings to the Edge
        *  \param [in, out]  val QMap<QString,QString> *    new settings
        */
    void setSettings( QMap<QString, QString>* val )
    {
        settings = val;
    }

    Data::Node* getOtherNode( const Data::Node* node ) const;

    void reloadLabel();
    void showLabel( bool visible );
    void set3D( bool value );
private:
    static const int INDEX_LABEL = 0;
    static const int INDEX_QUAD = 1;
    static const int INDEX_CYLINDER = 2;

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
        *  osg::Vec3 center
        *  \brief Center of the Edge
        */
    osg::ref_ptr<osg::Vec3Array> center;

    /**
        *  osg::Vec3 rotation
        *  \brief Rotation of the cylinder
        */
    osg::ref_ptr<osg::Vec3Array> rotation;

    /**
        *  double angle
        *  \brief Angle of the rotation of the cylinder
        */
    double angle;

    /**
        *  Util::ApplicationConfig * appConf
        *  \brief ApplicationConfig
        */
    Util::ApplicationConfig* appConf;


    /**
        *  osg::Vec4 edgeColor
        *  \brief Color of the Edge
        */
    osg::Vec4 edgeColor;

    float edgeStrength;

    osg::ref_ptr<osg::StateSet> createStateSet( Data::Type* type );
    osg::ref_ptr<osg::Geode> createEdgeQuad( osg::StateSet* bbState );
    osg::ref_ptr<osg::Geode> createEdgeCylinder( osg::StateSet* bbState );
protected:

    /**
        *  QMap<QString,QString> * settings
        *  \brief Settings of the Egde
        */
    QMap<QString, QString>* settings;
};
}

#endif
