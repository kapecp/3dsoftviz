#ifndef DBEDGE_H
#define DBEDGE_H

#include "Data/VizEdge.h"
namespace Data {


class DbEdge : public VizEdge
{
public:
	DbEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, float scaling );

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

protected:
	/**
	    *  bool inDB
	    *  \brief Flag if the Edge is already added to database or not
	    */
	bool inDB;

};
}

#endif // DBEDGE_H
