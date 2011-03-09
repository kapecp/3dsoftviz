#include "Importer/RSFImporter.h"
#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"

#include <string>
#include <memory>
namespace Importer {

bool RSFImporter::import (
	ImporterContext &context
) {
	QTextStream stream (&(context.getStream ()));
	QString line;

	GraphOperations graphOp (context.getGraph ());
	ReadNodesStore readNodes;
	bool edgeOrientedDefault = false;
	bool edgeOrientedDefaultForce = false;
	Data::Type *edgeType = NULL;
	Data::Type *nodeType = NULL;
	(void)graphOp.addDefaultTypes (edgeType, nodeType);

	int i = 1;
	//QString s=context.getGraph ().setName ("Graph");
	while ( !stream.atEnd() ) {
		line = stream.readLine();
		QStringList words;
		words = line.split (QRegExp (QString ("[ \t]+")));
		int size = words.size ();

		osg::ref_ptr<Data::Node> node1, node2;
		
		if (size!=3){
			context.getInfoHandler ().reportError ("Zvoleny subor nie je validny RSF subor.");
			return false;
		}
		else{
			if (words[0]=="tagged")
			{
				printf("%s %s\n",words[1], words[2]);
			}else
			{
				if (!readNodes.contains(words[1]))
				{
					node1 = context.getGraph().addNode(words[1], nodeType);
					readNodes.addNode(words[1], node1);
				}else
					node1=readNodes.get(words[1]);
				if (!readNodes.contains(words[2]))
				{
					node2 = context.getGraph ().addNode(words[2], nodeType);
					readNodes.addNode(words[2], node2);
				}else
					node2=readNodes.get(words[2]);
				context.getGraph().addEdge(words[0], node1, node2, edgeType, true);
			
			}
		}
	}
	//system ("pause");

	return true;
}

} // namespace
