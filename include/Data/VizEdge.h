#ifndef VIZEDGE_H
#define VIZEDGE_H

#include "Data/AbsEdge.h"
#include "opencv/cv.h"

namespace Data {


class VizEdge : public AbsEdge
{
public:
	VizEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, float scaling );

	/**
	    * \fn inline public constant toString
	    * \brief  Returns human-readable string representation of the Edge
	    * \return QString
	    */
	QString toString() const ;

	/**
	    *  \fn inline public  getScale()
	    *  \brief Gets edge scale
	    *  \return float size of the scale
	    */
	float getScale()
	{
		return scale;
	}

	/**
	    *  \fn inline public  setScale(float val)
	    *  \brief Sets new scale
	    *  \param   val   new scale for the Edge
	    */
	void setScale( float val )
	{
		scale = val;
	}

	/**
	            *  \fn public   setInvisible()
	            */
	bool setInvisible( bool invisible )
	{

		if ( invisible ) {
			setScale( 0 );
			isInvisible=true;
		}
		else {
			setScale( 1 );
			isInvisible=false;
		}
		return true;
	}

	bool getIsInvisible()
	{
		return isInvisible;
	}

	/**
	            *  \fn public  setSharedCoordinates (bool x, bool y, bool z)
	            *  \brief sets shared coordinates
	            *  \param x true, if nodes share x coordinate
	            *  \param y true, if nodes share y coordinate
	            *  \param z true, if nodes share z coordinate
	            */
	void setSharedCoordinates( bool x, bool y, bool z )
	{
		this->shared_X = x;
		this->shared_Y = y;
		this->shared_Z = z;
	}

	/**
	            *  \fn public   isShared_X()
	            *  \return value of shared_X flag
	            */
	bool isShared_X()
	{
		return this->shared_X;
	}

	/**
	            *  \fn public   isShared_Y()
	            *  \return value of shared_Y flag
	            */
	bool isShared_Y()
	{
		return this->shared_Y;
	}

	/**
	            *  \fn public   isShared_Z()
	            *  \return value of shared_Z flag
	            */
	bool isShared_Z()
	{
		return this->shared_Z;
	}

protected:
	/**
	    *  float scale
	    *  \brief scale of edge
	    */
	float scale;

	bool isInvisible;

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

};
}

#endif // VIZEDGE_H
