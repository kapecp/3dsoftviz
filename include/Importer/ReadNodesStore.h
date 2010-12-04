#ifndef Importer_ReadNodesStore_H
#define Importer_ReadNodesStore_H
//-----------------------------------------------------------------------------
#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <osg/ref_ptr>
#include <QString>
//-----------------------------------------------------------------------------
#include <map>

namespace Importer {

class ReadNodesStore {

public:

	/***/
	virtual ~ReadNodesStore (void) {};

	void addNode (
		const QString &name,
		osg::ref_ptr<Data::Node> node
	);

	bool contains (
		const QString &name
	);

	osg::ref_ptr<Data::Node> get (
		const QString &name
	);

private:

	typedef std::map<QString, osg::ref_ptr<Data::Node> > NodesMapType;
	NodesMapType nodes_;

}; // class

} // namespace

#endif // Importer_ReadNodesStore_H
