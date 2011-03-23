#include "Importer/GraphMLImporter.h"
//-----------------------------------------------------------------------------
#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
//-----------------------------------------------------------------------------
#include <QtXml/QDomElement>
//-----------------------------------------------------------------------------
#include <memory>

namespace Importer {

bool GraphMLImporter::import (
	ImporterContext &context
) {
	GraphOperations graphOp (context.getGraph ());

	// ziskame graph element
	QDomElement rootElement;
	QDomNode graphElement;

	QDomDocument doc("graphMLDocument");
	if (doc.setContent(&(context.getStream())))
	{
			QDomElement docElem = doc.documentElement();
			if (!docElem.isNull() && docElem.nodeName() == "graphml")
			{
					QDomNodeList graphNodes = docElem.elementsByTagName("graph");
					if (graphNodes.length() > 0)
					{
							graphElement = graphNodes.item(0);
							if (!graphElement.isNull() && graphElement.parentNode() == docElem && graphElement.isElement())
							{
									rootElement = graphElement.toElement();
							}
					}
			}
	}

	// ak mame rootElement tak
	if(!rootElement.isNull())
	{
		QString graphname = "Graph "+rootElement.attribute("id");
		bool defaultDirection;
		if(rootElement.attribute("edgedefault") == "directed"){
			defaultDirection = true;
		} else {
			defaultDirection = false;
		}
		QDomNodeList nodes = rootElement.elementsByTagName("node");
		QDomNodeList edges = rootElement.elementsByTagName("edge");

		context.getGraph ().setName (graphname);

		// ziskame pristup ku nastaveniam
		Util::ApplicationConfig * appConf = Util::ApplicationConfig::get();
		QString edgeTypeAttribute = appConf->getValue("GraphMLParser.edgeTypeAttribute");
		QString nodeTypeAttribute = appConf->getValue("GraphMLParser.nodeTypeAttribute");

		// pridavame default typy
		Data::Type *edgeType = NULL;
		Data::Type *nodeType = NULL;
		(void)graphOp.addDefaultTypes (edgeType, nodeType);

		ReadNodesStore readNodes;

		// skusal som aj cez QList, ale vobec mi to neslo, tak som to spravil len takto jednoducho cez pole
		int colors = 6;
		// pole farieb FIXME prerobit cez nejaky QList alebo nieco take, oddelit farby hran od farieb uzlov
		qint8 nodeTypeSettings[6][4] = {
			{0, 1, 0, 1},
			{0, 1, 1, 1},
			{1, 0, 0, 1},
			{1, 0, 1, 1},
			{1, 1, 0, 1},
			{1, 1, 1, 1},
		};
		qint8 iColor = 0;

		// vypis % pri nacitavani grafu
		int step = 0;
		int stepLength = (int) (nodes.count()+edges.count())/100;
		if(stepLength == 0) {
			// zadame defaultnu hodnotu, aby to nezlyhavalo
			stepLength = 50;
		}

		// prechadzame uzlami
		for (unsigned int i = 0; i < nodes.length(); i++)
		{

				if(i % stepLength == 0){
					context.getInfoHandler ().setProgress (step++);
				}

				QDomNode nodeNode = nodes.item(i);
				if (!nodeNode.isNull() && nodeNode.isElement())
				{
					QDomElement nodeElement = nodeNode.toElement();
					if (nodeElement.parentNode() == graphElement)
					{
							QString nameId = nodeElement.attribute("id");
							QString name = NULL;
							// pozerame sa na data ktore nesie
							Data::Type *newNodeType;
							newNodeType = NULL;
							QDomNodeList nodeDataList = nodeElement.elementsByTagName("data");
							for (unsigned int j = 0; j < nodeDataList.length(); j++){
								QDomNode nodeData = nodeDataList.item(j);
								if (!nodeData.isNull() && nodeData.isElement())
								{
										QDomElement nodeDataElement = nodeData.toElement();
										QString dataName = nodeDataElement.attribute("key");
										QString dataValue = nodeDataElement.text();
										// rozpoznavame typy
										if(dataName == nodeTypeAttribute){
											// overime ci uz dany typ existuje v grafe
											QList<Data::Type*> types = context.getGraph().getTypesByName(dataValue);
											if(types.isEmpty()){
												QMap<QString, QString> *settings = new QMap<QString, QString>;

												settings->insert("color.R", QString::number(nodeTypeSettings[iColor][0]));
												settings->insert("color.G", QString::number(nodeTypeSettings[iColor][1]));
												settings->insert("color.B", QString::number(nodeTypeSettings[iColor][2]));
												settings->insert("color.A", QString::number(nodeTypeSettings[iColor][3]));
												settings->insert("scale",		Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
												settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));

												newNodeType = context.getGraph().addType(dataValue, settings);

												if(iColor == colors){
													iColor = 0;
												} else {
													iColor++;
												}
											} else {
												newNodeType = types.first();
											}

										} else {
											// kazde dalsie data nacitame do nosica dat - Node.name
											// FIXME potom prerobit cez Adamove Node.settings
											if(name == NULL){
												name = dataName+":"+dataValue;
											} else {
												name += " | "+dataName+":"+dataValue;
											}
										}
								}
							}

							// ak sme nenasli name, tak ako name pouzijeme aspon ID
							if(name == NULL){
								name = nameId;
							}

							// ak nebol najdeny ziaden typ, tak pouzijeme defaultny typ
							osg::ref_ptr<Data::Node> node;
							if(newNodeType == NULL)
								node = context.getGraph().addNode(name, nodeType);
							else
								node = context.getGraph().addNode(name, newNodeType);
							readNodes.addNode (nameId, node);
					}
			}
		}

		iColor = 0;

		// prechadzame hranami
		for (uint i = 0; i < edges.length(); i++)
		{

			if(i % stepLength == 0){
				context.getInfoHandler ().setProgress (step++);
			}

				QDomNode edgeNode = edges.item(i);

				if (!edgeNode.isNull() && edgeNode.isElement())
				{
						QDomElement edgeElement = edgeNode.toElement();
						if (edgeElement.parentNode() == rootElement)
						{
								QString sourceId = edgeElement.attribute("source");
								QString targetId = edgeElement.attribute("target");

								QString direction = NULL;
								bool directed = false;
								direction = edgeElement.attribute("directed");
								if(direction == NULL) {
									directed = defaultDirection;
									if(directed)
										direction = "_directed";
									else
										direction = "";
								} else {
									if(direction == "true"){
										direction = "_directed";
										directed = true;
									} else {
										direction = "";
										directed = false;
									}
								}



								// pozerame sa na data ktore hrana nesie
								Data::Type *newEdgeType;
								newEdgeType = NULL;
								QDomNodeList edgeDataList = edgeElement.elementsByTagName("data");
								for (unsigned int j = 0; j < edgeDataList.length(); j++){
									QDomNode edgeData = edgeDataList.item(j);
									if (!edgeData.isNull() && edgeData.isElement())
									{
											QDomElement edgeDataElement = edgeData.toElement();
											QString dataName = edgeDataElement.attribute("key");
											QString dataValue = edgeDataElement.text();
											// rozpoznavame typy deklarovane atributom relation
											if(dataName == edgeTypeAttribute){
												// overime ci uz dany typ existuje v grafe
												QList<Data::Type*> types = context.getGraph().getTypesByName(dataValue+direction);
												if(types.isEmpty()){
													QMap<QString, QString> *settings = new QMap<QString, QString>;

													// FIXME spravit tak, aby to rotovalo po tom poli - palo az to budes prerabat tak pre hrany pouzi ine pole, take co ma alfu na 0.5.. a to sa tyka aj uzlov s defaultnym typom
												   settings->insert("color.R", QString::number(nodeTypeSettings[iColor][0]));
												   settings->insert("color.G", QString::number(nodeTypeSettings[iColor][1]));
												   settings->insert("color.B", QString::number(nodeTypeSettings[iColor][2]));
												   settings->insert("color.A", QString::number(nodeTypeSettings[iColor][3]));
												   settings->insert("scale",		Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));

												   if (!directed)
														settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Edge"));
												   else
												   {
													   settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.OrientedEdgePrefix"));
													   settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.OrientedEdgeSuffix"));
												   }

													newEdgeType = context.getGraph().addType(dataValue+direction, settings);

													if(iColor == colors){
														iColor = 0;
													} else {
														iColor++;
													}
												} else {
													newEdgeType = types.first();
												}

											} else {
												// kazde dalsie data nacitame do nosica dat - Edge.name
												// FIXME potom prerobit cez Adamove Node.settings
											}
									}
								}

								// ak nebol najdeny typ, tak pouzijeme defaulty
								if(newEdgeType == NULL)
									newEdgeType = edgeType;

								context.getGraph().addEdge(sourceId+targetId, readNodes.get(sourceId), readNodes.get(targetId), newEdgeType, directed);
						}
				}
		}

		// pridame layout grafu
		Data::GraphLayout* gLay = context.getGraph().addLayout("new Layout");
		context.getGraph().selectLayout(gLay);
	} else {
		context.getInfoHandler ().reportError ("Zvoleny subor nie je validny GraphML subor.");
	}

	return true;
}

} // namespace