#include "Importer/RSFImporter.h"
#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"

#include <string>
#include <memory>
namespace Importer {

	osg::ref_ptr<Data::Node> RSFImporter::getHyperEdge(
		QString srcNodeName,
		QString edgeName,QMap<qlonglong, 
		osg::ref_ptr<Data::Edge> > *mapa)
	{
		osg::ref_ptr<Data::Node> hyperEdgeNode1;
		for (QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator it = mapa->begin (); it != mapa->end (); ++it) {
			osg::ref_ptr<Data::Edge> existingEdge = it.value ();
			if (
				existingEdge->getSrcNode ()->getName () == srcNodeName && 
				existingEdge->getDstNode ()->getName () == edgeName //&& 
				//existingEdge->getDstNode ()->getType ()->getName () == QString ("hyperNode")
			) {
				hyperEdgeNode1 = existingEdge->getDstNode ();
				break;
			}
		}
		return hyperEdgeNode1;
}

bool RSFImporter::import (
	ImporterContext &context
) {
	QTextStream stream (&(context.getStream ()));
	QString line, graphname="Graph";

	GraphOperations graphOp (context.getGraph ());
	ReadNodesStore readNodes;
	bool edgeOrientedDefault = false;
	bool edgeOrientedDefaultForce = false;
	Data::Type *edgeType = NULL;
	Data::Type *nodeType = NULL;
	context.getGraph ().setName (graphname);
	(void)graphOp.addDefaultTypes (edgeType, nodeType);
	
	int i = 0;
	while ( !stream.atEnd() ) {
		line = stream.readLine();
		QStringList words;
		words = line.split (QRegExp (QString ("[ \t]+")));
		int size = words.size ();
		Data::Type *newNodeType = NULL;
		osg::ref_ptr<Data::Node> node1, node2;
		osg::ref_ptr<Data::Edge> edge;
		if (size!=3){
			printf("%d",size);
			context.getInfoHandler ().reportError ("Zvoleny subor nie je validny RSF subor.");
			return false;
		}
		else{
			if (words[0]=="tagged")
			{
				//printf("%s %s\n",words[1], words[2]);
			}else
			{
				QString edgeName = words[0];
				QString srcNodeName = words[1];
				QString dstNodeName = words[2];

				// create nodes if not exist

				if (!readNodes.contains(srcNodeName))
				{
					node1 = context.getGraph().addNode(srcNodeName, nodeType);
					readNodes.addNode(srcNodeName, node1);
				} else {
					node1=readNodes.get(srcNodeName);
				}

				if (!readNodes.contains(dstNodeName))
				{
					node2 = context.getGraph ().addNode(dstNodeName, nodeType);
					readNodes.addNode(dstNodeName, node2);
				} else {
					node2=readNodes.get(dstNodeName);
				}
				//create hyperedge
				osg::ref_ptr<Data::Node> hyperEdgeNode;
				QMap<qlonglong, osg::ref_ptr<Data::Edge> > *mapa = context.getGraph().getEdges();

				hyperEdgeNode=RSFImporter().getHyperEdge(srcNodeName,edgeName,mapa);
				if (!hyperEdgeNode.valid ()) {
					hyperEdgeNode = context.getGraph ().addHyperEdge (edgeName);
					context.getGraph ().addEdge (QString (""), node1, hyperEdgeNode, edgeType, true);
				}
				
				context.getGraph ().addEdge (QString (""), hyperEdgeNode, node2, edgeType, true);
			}
		}

	}

	return true;
}

} // namespace
