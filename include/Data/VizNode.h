#ifndef VIZNODE_H
#define VIZNODE_H

#include <Data/AbsNode.h>

namespace Data {
class Node;
class Graph;
class Type;


class VizNode : public AbsNode
{
public:

	VizNode( qlonglong id, QString name, Type* type, Graph* graph, float scaling );

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
	virtual void setScale( float val )
	{
		scale = val;
	}

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

};
}

#endif // VIZNODE_H
