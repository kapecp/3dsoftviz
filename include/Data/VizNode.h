#ifndef VIZNODE_H
#define VIZNODE_H

//#include "opencv/cv.h"
#include <Data/AbsNode.h>
#include "QVector3D"

namespace Data {
class Node;
class Graph;
class Type;


class VizNode : public AbsNode
{
public:

    VizNode(qlonglong id, QString name, Type *type, Graph *graph, float scaling);

    void setLabelText( QString label )
    {
        labelText = label;
    }

    /**
        *  \fn inline public  getScale()
        *  \brief Gets node scale
        *  \return float size of the scale
        */
    float getScale()
    {
        return scale;
    }

    /**
        *  \fn inline public  setScale(float val)
        *  \brief Sets new scale
        *  \param   val   new scale for the Node
        */
    void setScale( float val )
    {
        scale = val;
    }

    /**
        *  \fn inline public  setUsingInterpolation(bool val)
        *  \brief Sets if the Node is using interpolation or not
        *  \param      val
        */
    /*void setUsingInterpolation( bool val )
    {
        usingInterpolation = val;
    }*/

    float getOverallWeight()
    {
        return overallWeight;
    }

    void setOverallWeight( float overallWeight )
    {
        this->overallWeight = overallWeight;
    }

    /**
        *  \fn inline public  setIgnored(bool b)
        *  \brief Sets ignoring by layout algorithm.
        *  \param       b  True, if node is ignored.
        */
    void setIgnored( bool b )
    {
        ignore = b;
    }

    /**
        *  \fn inline public constant  isIgnored
        *  \brief Return if the Node is ignored
        *  \return bool true, if the Node is ignored
        */
    bool isIgnored() const
    {
        return ignore;
    }

    /**
        *  \fn inline public  setVelocity(osg::Vec3f v)
        *  \brief Sets node force for next iteration
        *  \param    v  Force in actual iteration
        */
    void setVelocity( QVector3D v )
    {
        velocity = v;
    }

    /**
        *  \fn inline public  resetVelocity
        *  \brief Reset node force for next iteration
        */
    void resetVelocity()
    {
        velocity = QVector3D( 0,0,0 );
    }

    /**
        *  \fn inline public constant  getVelocity
        *  \brief Sets node force for next iteration.
        *  \return osg::Vec3f Node force
        */
    QVector3D getVelocity() const
    {
        return velocity;
    }

    /**
        *  QString labelText
        *  \brief Text show in the label
        */
    QString labelText;

    /**
        *	float scale
        *	\brief scaling of the Node
        */
    float scale;

    float overallWeight;

    /**
        *  bool ignore
        *  \brief node ignoring flag
        */
    bool ignore;

    /**
        *  osg::Vec3f velocity
        *  \brief Size of node force in previous iteration
        */
    QVector3D velocity;

};
}

#endif // VIZNODE_H
