/*!
 * Graph.cpp
 * Projekt 3DVisual
 */
#include "Data/Graph.h"
#include "Data/GraphLayout.h"
#include "Layout/ShapeGetter_Sphere_AroundNode.h"
#include <QSharedPointer>

Data::Graph::Graph(qlonglong graph_id, QString name, QSqlDatabase* conn, QMap<qlonglong,osg::ref_ptr<Data::Node> > *nodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> > *edges,QMap<qlonglong,osg::ref_ptr<Data::Node> > *metaNodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> > *metaEdges, QMap<qlonglong,Data::Type*> *types)
{
	//tento konstruktor je uz zastaraly a neda sa realne pouzit - uzly musia mat priradeny graph, ktory sa prave vytvarat, rovnako edge, type, metatype (ten musi mat naviac aj layout, ktory opat musi mat graph)
	this->inDB = false;

	this->graph_id = graph_id;
	this->name = name;
	this->conn = conn;

	this->nodes = nodes;
	foreach(qlonglong i,nodes->keys()) {
		this->nodesByType.insert(nodes->value(i)->getType()->getId(),nodes->value(i));
	}

	this->edges = edges;
	foreach(qlonglong i,edges->keys()) {
		this->edgesByType.insert(edges->value(i)->getType()->getId(),edges->value(i));
	}

	this->types = types;

	this->metaEdges = metaEdges;
	foreach(qlonglong i,metaEdges->keys()) {
		this->metaEdgesByType.insert(metaEdges->value(i)->getType()->getId(),metaEdges->value(i));
	}

	this->metaNodes = metaNodes;
	foreach(qlonglong i,metaNodes->keys()) {
		this->metaNodesByType.insert(metaNodes->value(i)->getType()->getId(),metaNodes->value(i));
	}

	this->selectedLayout = NULL;
	this->ele_id_counter = this->getMaxEleIdFromElements();
	this->layout_id_counter = 0; //POZOR toto asi treba inak poriesit, teraz to predpoklada ze ziadne layouty nemame co je spravne, lenze bacha na metatypy, ktore layout mat musia !

	this->frozen = false;

	this->typesByName = new QMultiMap<QString, Data::Type*>();

	if(this->types!=NULL && this->types->size()>0) {
		foreach(qlonglong i, this->types->keys()) {
			this->typesByName->insert(this->types->value(i)->getName(), this->types->value(i));
		}
	}
}

Data::Graph::Graph(qlonglong graph_id, QString name, qlonglong layout_id_counter, qlonglong ele_id_counter, QSqlDatabase* conn)
{
	this->inDB = false;

	this->graph_id = graph_id;
	this->name = name;
	this->ele_id_counter = ele_id_counter;
	this->layout_id_counter = layout_id_counter;
	this->conn = conn;
	this->selectedLayout = NULL;

	this->nodes = new QMap<qlonglong,osg::ref_ptr<Data::Node> >();
	this->edges = new QMap<qlonglong,osg::ref_ptr<Data::Edge> >();
	this->types = new QMap<qlonglong,Data::Type*>();
	this->metaEdges = new QMap<qlonglong,osg::ref_ptr<Data::Edge> >();
	this->metaNodes = new QMap<qlonglong,osg::ref_ptr<Data::Node> >();
	this->frozen = false;
	this->typesByName = new QMultiMap<QString, Data::Type*>();
}

Data::Graph::~Graph(void)
{
	//uvolnime vsetky Nodes, Edges, metaNodes, metaEdges... su cez osg::ref_ptr takze staci clearnut
	this->nodes->clear();
	delete this->nodes;
	this->nodes = NULL;

	this->metaNodes->clear();
	delete this->metaNodes;
	this->metaNodes = NULL;

	this->edges->clear();
	delete this->edges;
	this->edges = NULL;

	this->metaEdges->clear();
	delete this->metaEdges;
	this->metaEdges = NULL;

	//tieto zatial iba vyprazdnime - tu bude treba spavit logiku, ktora deletne tie, ktore nie su v nodes, metanodes, metaedges, edges a types  TODO!!!
	//v zasade, ak nam funguje DB pripojenie, tak tu nebudu ziadne uzly ktore uz mame v nodes, edges, metanodes ... treba ich deletnut
	//ak nam db nefunguje, tak uzly ktore su tu budu aj v nodes, metatypes, edges...
	this->newTypes.clear();
	this->newNodes.clear();
	this->newEdges.clear();
	this->metaEdgesByType.clear();
	this->metaNodesByType.clear();
	this->nodesByType.clear();
	this->edgesByType.clear();
	this->nestedNodes.clear();
	this->typesByName->clear();
	delete this->typesByName;
	this->typesByName = NULL;


	//uvolnime types - treba iterovat a kazde jedno deletnut samostatne
	QMap<qlonglong, Data::Type*>::iterator it = this->types->begin();

	Data::Type* type;
	while (it!=this->types->end()) {
		type = it.value();
		it = this->types->erase(it);
		delete type;
		type = NULL;
	}

	this->types->clear();
	delete this->types;
	this->types = NULL;

	//uvolnime vsetky layouty
	QMap<qlonglong, Data::GraphLayout*>::iterator it2 = this->layouts.begin();
	Data::GraphLayout* l;
	this->selectedLayout = NULL;
	while (it2!=this->layouts.end()) {
		l = it2.value();
		it2 = this->layouts.erase(it2);
		delete l;
		l = NULL;
	}
	this->layouts.clear();

	//DB konekcia sa deletovat nebude (kedze tu riesi Manager)
	this->conn = NULL;
}

bool Data::Graph::saveGraphToDB(QSqlDatabase* conn, Data::Graph * graph)
{
	//ukladame cely graf do DB
	if(Model::NodeDAO::addNodesToDB(conn, graph->nodes)
			&& Model::EdgeDAO::addEdgesToDB(conn, graph->edges))
	{
		//uspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveGraphToDB] Graph was saved to DB.";
		return true;
	}
	else
	{
		//neuspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveGraphToDB] Graph wasn't saved to DB.";
		return false;
	}

	return false;
}

bool Data::Graph::saveLayoutToDB(QSqlDatabase* conn, Data::Graph * graph)
{
	//ukladame leyout do DB
	QMap<qlonglong, qlonglong> newMetaNodeID;
	QMap<qlonglong, qlonglong> newMetaEdgeID;

	newMetaNodeID = Model::NodeDAO::getNewMetaNodesId(conn, graph->getId(), graph->metaNodes);
	newMetaEdgeID = Model::EdgeDAO::getNewMetaEdgesId(conn, graph->getId(), graph->metaEdges);

	if(Model::NodeDAO::addMetaNodesToDB(conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID)
			&& Model::NodeDAO::addNodesPositionsToDB(conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true)
			&& Model::NodeDAO::addNodesColorToDB(conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true)
			&& Model::NodeDAO::addNodesPositionsToDB(conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false)
			&& Model::NodeDAO::addNodesColorToDB(conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false)
			&& Model::EdgeDAO::addMetaEdgesToDB(conn, graph->metaEdges, graph->selectedLayout, newMetaNodeID, newMetaEdgeID)
			&& Model::EdgeDAO::addEdgesColorToDB(conn, graph->edges, graph->selectedLayout, newMetaNodeID, newMetaEdgeID, false)
			&& Model::EdgeDAO::addEdgesColorToDB(conn, graph->metaEdges, graph->selectedLayout, newMetaNodeID, newMetaEdgeID, true)
			&& Model::NodeDAO::addNodesScaleToDB(conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false, graph->getNodeScale())
			&& Model::NodeDAO::addNodesScaleToDB(conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true, graph->getNodeScale())
			&& Model::EdgeDAO::addEdgesScaleToDB(conn, graph->edges, graph->selectedLayout, newMetaNodeID, newMetaEdgeID, false, graph->getEdgeScale())
			&& Model::EdgeDAO::addEdgesScaleToDB(conn, graph->metaEdges, graph->selectedLayout, newMetaNodeID, newMetaEdgeID, true, graph->getEdgeScale())
			&& Model::NodeDAO::addNodesMaskToDB(conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false)
			&& Model::NodeDAO::addNodesMaskToDB(conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true)
			&& Model::NodeDAO::addNodesParentToDB(conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false)
			&& Model::NodeDAO::addNodesParentToDB(conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true))
	{
		//uspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveLayoutToDB] Layout was saved to DB.";
		return true;
	}
	else
	{
		//neuspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveLayoutToDB] Layout wasn't saved to DB.";
		return false;
	}

	return false;
}

Data::GraphLayout* Data::Graph::addLayout(QString layout_name)
{
	//Vytvarame novy layout pre graf
	bool error;
	if(this->layouts.isEmpty()) { //na zaciatok ak ziadne ine layouty nemame, sa pokusime nacitat layouty z DB
		this->layouts = this->getLayouts(&error);
	}

	//layouty bude do DB pridavat user, nebudu sa pridavat automaticky
	//layout_id nebudeme brat z DB, lebo layout tam nemusi byt, preto vzdy nastavime na 1
	Data::GraphLayout* layout = new Data::GraphLayout(1,this,layout_name,this->conn);

	if(layout==NULL && (this->conn==NULL || !this->conn->isOpen())) { //nepodarilo sa vytvorit GraphLayout - nejaky problem s pripojenim na DB;
		//vytvorime si nahradny layout aby sme mohli bezat aj bez pripojenia k DB

		layout = new Data::GraphLayout(this->incLayoutIdCounter(),this,layout_name,this->conn);
	} else if(layout==NULL) { //nepodarilo sa vytvorit Layout, nejaky problem s pridavanim do DB
		return NULL;
	} else { //vytvorili sme si GraphLayout a je v DB
		this->layout_id_counter = layout->getId(); //zvysenie counter ID layoutov (counter je pre pripad, ze by nam vypadlo spojenie ked sa budeme pokusat urobit novy layout)
	}

	this->layouts.insert(layout->getId(),layout);

	return layout;
}

QMap<qlonglong, Data::GraphLayout*> Data::Graph::getLayouts(bool* error)
{
	if(this->layouts.isEmpty()) { //ak ziadne layouty nemame, skusime ich nacitat z DB, ak uz nejake mame, tak sme tento krok uz vykonali
		this->layouts = Model::GraphLayoutDAO::getLayouts(this, this->conn, error);

		if(!this->layouts.isEmpty()) { //nastavime spravne layout_id_counter
			QMapIterator<qlonglong, Data::GraphLayout*> i(this->layouts);
			i.toBack();
			if(i.hasPrevious()) {
				this->layout_id_counter = i.previous().value()->getId();
			}
		}
	}

	return this->layouts;
}

bool Data::Graph::removeLayout(Data::GraphLayout* layout)
{
	if(Model::GraphLayoutDAO::removeLayout(layout, this->conn) || !layout->isInDB()) {
		//layout sa podarilo odstranit z DB alebo v DB vobec nebol -> odstranime vsetky MetaType, ktore patria danemu layout

		if(layout==this->selectedLayout) this->selectLayout(NULL);
		this->layouts.remove(layout->getId());
	}

	return false;
}

QString Data::Graph::setName(QString name)
{
	//nastavyme nazov grafu
	QString newName = Model::GraphDAO::setName(name,this,this->conn);

	if(newName!=NULL || (conn==NULL || !conn->isOpen())) { //nastavime nove meno ak sa nam ho podarilo nastavit v DB, alebo ak nemame vobec pripojenie k DB (pre offline verziu)
		this->name = newName;
	}

	return this->name;
}

bool Data::Graph::isInSameGraph(Data::Node * nodeA, Data::Node * nodeB)
{
	if(nodeA->getNestedParent()==nodeB->getNestedParent())
	{
		//urcie ze uzly su v rovnakom podgrafe ak maju rovnaky nadradeny uzol
		return true;
	}
	return false;
}

osg::ref_ptr<Data::Node> Data::Graph::addNode(QString name, Data::Type* type, osg::Vec3f position)
{
	Data::Type* metype;

	QList<Data::Type*> mtypes = getTypesByName(Data::GraphLayout::NESTED_NODE_TYPE);

	if(this->parent_id.count()>0)
	{

		if(mtypes.isEmpty())
		{
			//pridame metatype ak este nie je vytvoreny
			QMap<QString, QString> *settings = new QMap<QString, QString>;

			settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
			settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
			settings->insert("color.R", "1");
			settings->insert("color.G", "0");
			settings->insert("color.B", "0");
			settings->insert("color.A", "1");

			metype = this->addType(Data::GraphLayout::NESTED_NODE_TYPE, settings);
		}
		else
		{
			metype = mtypes[0];
		}

		type = metype;
	}

	osg::ref_ptr<Data::Node> node = new Data::Node(this->incEleIdCounter(), name, type, this->getNodeScale(), this, position);

	node->setNestedParent(NULL);

	//pridanie do zoznamu vnorenych uzlov
	if(this->parent_id.count()>0)
	{
		this->nestedNodes.insert(node.get());
		this->nestetSubGraphs.last().insert(node.get());
	}

	//pridame vnoreny uzol
	this->addNestedNode(node);

	this->newNodes.insert(node->getId(),node);
	if(type!=NULL && type->isMeta()) {
		//pridame meta uzol do zoznamu
		this->metaNodes->insert(node->getId(),node);
		this->metaNodesByType.insert(type->getId(),node);
	} else {
		//pridame vseobecny uzol do zoznamu
		this->nodes->insert(node->getId(),node);
		this->nodesByType.insert(type->getId(),node);
	}

	//pridame metatyp pre vnoreny graf
	if(this->parent_id.count()>0)
	{
		QList<Data::Type*> metypes = getTypesByName(Data::GraphLayout::MULTI_EDGE_TYPE);
	}

	return node;
}

osg::ref_ptr<Data::Node> Data::Graph::addNode(qlonglong id, QString name, Data::Type* type, osg::Vec3f position)
{
	//vytvorime novy objekt uzla
	osg::ref_ptr<Data::Node> node = new Data::Node(id, name, type, this->getNodeScale(), this, position);

	this->newNodes.insert(node->getId(),node);

	//podla typu ho priradime danemu zoznamu
	if(type!=NULL && type->isMeta()) {
		this->metaNodes->insert(node->getId(),node);
		this->metaNodesByType.insert(type->getId(),node);
	} else {
		this->nodes->insert(node->getId(),node);
		this->nodesByType.insert(type->getId(),node);
	}

	//pridame vnoreny uzol do zoznamu
	this->addNestedNode(node);

	if(this->parent_id.count()>0)
	{
		this->nestedNodes.insert(node.get());
	}

	return node;
}

osg::ref_ptr<Data::Node> Data::Graph::mergeNodes(QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes, osg::Vec3f position, qlonglong mergeNodeId)
{
	//vyratame velkost zluceneho uzla podla velkosti zlucovanych uzlov
	float scale = this->getNodeScale() + (selectedNodes->count() / 2);

	//vytvorime novy zluceny uzol
	osg::ref_ptr<Data::Node> mergedNode = new Data::Node((mergeNodeId != -1) ? mergeNodeId : this->incEleIdCounter(), "mergedNode", this->getNodeMetaType(), scale, this, position);
	mergedNode->setColor(osg::Vec4(0, 0, 1, 1));

	QList<qlonglong> connectedNodes;

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator iAdd = selectedNodes->constBegin();

	//pridavame do zluceneho uzla zlucovane uzly
	while (iAdd != selectedNodes->constEnd())
	{
		(*iAdd)->setCurrentPosition(mergedNode->getCurrentPosition());
		(*iAdd)->setFixed(false);
		(*iAdd)->setNodeMask(0);

		Data::Edge * e = this->addEdge("mergedEdge", (*iAdd), mergedNode, this->getEdgeMetaType(), true);
		e->setScale(0);

		connectedNodes << (*iAdd)->getId();

		++iAdd;
	}

	//spajame povodne uzly s novovytvorenym zlucenym uzlom
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();
	while (i != selectedNodes->constEnd())
	{
		QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iedge = (*i)->getEdges()->constBegin();
		while (iedge != (*i)->getEdges()->constEnd())
		{
			iedge.value().get()->setScale(0);
			osg::ref_ptr<Data::Node> srcNode = iedge.value().get()->getSrcNode();
			osg::ref_ptr<Data::Node> dstNode = iedge.value().get()->getDstNode();
			osg::ref_ptr<Data::Node> connectNode;

			if (dstNode->getId() == (*i)->getId()) {
				connectNode = srcNode;
			}
			else {
				connectNode = dstNode;
			}

			if(!connectedNodes.contains(connectNode->getId()) && connectNode->getNodeMask() != 0)
			{
				connectedNodes << connectNode->getId();
			}

			++iedge;
		}

		++i;
	}

	//pridame zluceny uzol medzi metauzly
	this->metaNodes->insert(mergedNode->getId(), mergedNode);
	this->metaNodesByType.insert(mergedNode->getId(), mergedNode);

	return mergedNode;
}

void Data::Graph::separateNodes(QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes)
{
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();

	//zo zluceneho uzla extrahujeme v nom zahrnute uzly
	while (i != selectedNodes->constEnd())
	{
		//TODO zatial merged node identifikujeme len podla nazvu nodu - "mergedNode" - treba dokoncit
		if ((*i)->getType()->isMeta() && (*i)->getName() == "mergedNode")
		{
			//najdeme vsetky uzly a zrusime mask
			//najdeme vsetky hrany spojene s tymto uzlom a nastavime im scale
			//nastavime poziciu na mergeNode a zrusime tento uzol
			osg::Vec3f position = (*i)->getCurrentPosition();

			QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iedge = (*i)->getEdges()->constBegin();
			while (iedge != (*i)->getEdges()->constEnd())
			{
				osg::ref_ptr<Data::Node> connectedNode;
				float scale = this->getEdgeScale();

				//mergedEdge ide vzdy z povodneho nodu do mergedNodu
				connectedNode = iedge.value().get()->getSrcNode();

				//ak je node zamaskovany, tak ho odmaskujeme
				if(connectedNode->getNodeMask() == 0)
				{
					connectedNode->setCurrentPosition(position);
					connectedNode->setFixed(false);
					connectedNode->setNodeMask(~0);

					QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iedgeIn = connectedNode->getEdges()->constBegin();

					//nastavujeme velkost hranam
					while (iedgeIn != connectedNode->getEdges()->constEnd())
					{
						osg::ref_ptr<Data::Node> srcNode = iedgeIn.value().get()->getSrcNode();
						osg::ref_ptr<Data::Node> dstNode = iedgeIn.value().get()->getDstNode();

						if(iedgeIn.value().get()->getScale() == 0 && (srcNode->getNodeMask() != 0 && dstNode->getNodeMask() != 0))
						{
							//TODO - nastavime velkost hranam - vsetkym default - treba prerobit ak budu mat hrany inu velkost
							iedgeIn.value().get()->setScale(scale);
						}

						++iedgeIn;
					}
				}

				//TODO - nastavime velkost hranam - vsetkym default - treba prerobit ak budu mat hrany inu velkost
				iedge.value().get()->setScale(scale);

				++iedge;
			}

			this->removeNode((*i)); //zmaze uzol aj jeho hrany
		}
		++i;
	}

}


void Data::Graph::addNestedNode(Data::Node * node)
{
	if(this->parent_id.count()>0)
	{
		//pridame vnoerny uzol do posledneho nacitaneho uzla len pri importe
		node->setParentNode(this->parent_id.last());
	}
}


void Data::Graph::createNestedGraph(osg::ref_ptr<Data::Node> srcNode)
{
	//vytvorime vnoreny graf do posledneho nacitaneho uzla pri importe
	this->parent_id.append(srcNode);
	srcNode->setAsParentNode();
	QSet<Data::Node *> nn;
	nestetSubGraphs.append(nn);
}

void Data::Graph::closeNestedGraph()
{
	//ukoncime vytvaranie daneho vnoreneho grafu pri importe
	QSharedPointer<Layout::ShapeGetter> shapeGetter (
				new Layout::ShapeGetter_Sphere_AroundNode (
					this->parent_id.last(),
					getNodeScale()*4,
					Layout::Shape_Sphere::RANDOM_DISTANCE_FROM_CENTER,
					Layout::ShapeGetter_Sphere_AroundNode::NODE_CURRENT_POSITION
					)
				);
	//nastavime obmedzovac pre zobrazenie vnoreneho grafu
	restrictionsManager_.setRestrictions (this->nestetSubGraphs.last(), shapeGetter);

	this->nestedNodes.clear();
	this->nestetSubGraphs.removeLast();
	this->parent_id.removeLast();
}

bool Data::Graph::isNestedGraph()
{
	//urcenie ci je dany graf (this) vnoreny
	if (this->parent_id.size()==0)
	{
		return false;
	}
	return true;
}

osg::ref_ptr<Data::Edge> Data::Graph::addEdge(QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented)
{
	if(isParralel(srcNode, dstNode))
	{
		//pridame multihranu fo grafu
		addMultiEdge(name, srcNode, dstNode, type, isOriented, NULL);
	}
	else
	{
		//pridame hranu do grafu

		if(this->nestetSubGraphs.count()>0) //parent_id.count()>0)
		{
			type = getNestedEdgeType();
		}
		osg::ref_ptr<Data::Edge> edge = new Data::Edge(this->incEleIdCounter(), name, this, srcNode, dstNode, type, isOriented, getEdgeScale());

		edge->linkNodes(&this->newEdges);
		if((type!=NULL && type->isMeta()) || ((srcNode->getType()!=NULL && srcNode->getType()->isMeta()) || (dstNode->getType()!=NULL && dstNode->getType()->isMeta())))
		{
			//ak je type meta, alebo je meta jeden z uzlov (ma type meta)
			edge->linkNodes(this->metaEdges);
			this->metaEdgesByType.insert(type->getId(),edge);
		} else
		{
			edge->linkNodes(this->edges);
		}
		return edge;
	}

	return NULL;
}

osg::ref_ptr<Data::Edge> Data::Graph::addEdge(qlonglong id, QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented)
{
	if(isParralel(srcNode, dstNode))
	{
		//pridame multihranu do grafu
		addMultiEdge(name, srcNode, dstNode, type, isOriented, NULL);
	}
	else
	{
		//pridame hranu do grafu

		osg::ref_ptr<Data::Edge> edge = new Data::Edge(id, name, this, srcNode, dstNode, type, isOriented, getEdgeScale());

		edge->linkNodes(&this->newEdges);
		if((type!=NULL && type->isMeta()) || ((srcNode->getType()!=NULL && srcNode->getType()->isMeta()) || (dstNode->getType()!=NULL && dstNode->getType()->isMeta())))
		{
			//ak je type meta, alebo je meta jeden z uzlov (ma type meta)
			edge->linkNodes(this->metaEdges);
			this->metaEdgesByType.insert(type->getId(),edge);
		} else
		{
			edge->linkNodes(this->edges);
		}

		return edge;
	}

	return NULL;
}

Data::Type* Data::Graph::getNestedEdgeType()
{
	Data::Type* metype;

	//vraciame typ vnorenej hrany s nastavenymi parametrami

	QList<Data::Type*> metypes = getTypesByName(Data::GraphLayout::NESTED_EDGE_TYPE);

	if(metypes.isEmpty())
	{
		//adding NESTED_EDGE_TYPE settings if necessary
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", "50");//this->getEdgeScale());
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Edge"));
		settings->insert("color.R", "1");
		settings->insert("color.G", "0");
		settings->insert("color.B", "0");
		settings->insert("color.A", "1");

		metype = this->addType(Data::GraphLayout::NESTED_EDGE_TYPE, settings);
	}
	else
	{
		metype = metypes[0];
	}
	return metype;
}

Data::Type* Data::Graph::getNestedMetaEdgeType()
{
	Data::Type* metype;

	//vraciame typ vnorenej meta hrany s nastavenymi parametrami

	QList<Data::Type*> metypes = getTypesByName(Data::GraphLayout::NESTED_META_EDGE_TYPE);

	if(metypes.isEmpty())
	{
		//adding NESTED_EDGE_TYPE settings if necessary
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", "Viewer.Textures.EdgeScale");
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Edge"));
		settings->insert("color.R", "0");
		settings->insert("color.G", "1");
		settings->insert("color.B", "0");
		settings->insert("color.A", "0");

		metype = this->addType(Data::GraphLayout::NESTED_META_EDGE_TYPE, settings);
	}
	else
	{
		metype = metypes[0];
	}
	return metype;
}

void Data::Graph::addMultiEdge(QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, osg::ref_ptr<Data::Edge> replacedSingleEdge)
{
	Data::Type* mtype;

	//pridavame multihranu

	Data::Type* metype;

	QList<Data::Type*> mtypes = getTypesByName(Data::GraphLayout::MULTI_NODE_TYPE);
	//osetrime typ
	if(mtypes.isEmpty())
	{
		//adding META_NODE_TYPE settings if necessary
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", "20");
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
		settings->insert("color.R", "1");
		settings->insert("color.G", "1");
		settings->insert("color.B", "1");
		settings->insert("color.A", "1");

		mtype = this->addType(Data::GraphLayout::MULTI_NODE_TYPE, settings);
	}
	else
	{
		mtype = mtypes[0];
	}

	QList<Data::Type*> metypes = getTypesByName(Data::GraphLayout::MULTI_EDGE_TYPE);

	if(mtypes.isEmpty())
	{
		//adding META_EDGE_TYPE settings if necessary
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", "Viewer.Textures.EdgeScale");
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Edge"));
		settings->insert("color.R", "1");
		settings->insert("color.G", "1");
		settings->insert("color.B", "1");
		settings->insert("color.A", "1");

		metype = this->addType(Data::GraphLayout::MULTI_EDGE_TYPE, settings);
	}
	else
	{
		metype = metypes[0];
	}

	//pridame multiranu tvorenu pomocnymi hranami a pomocnym uzlom
	osg::ref_ptr<Data::Node> parallelNode = addNode("PNode", mtype);

	if(this->parent_id.count()>0)
	{
		metype = getNestedEdgeType();
	}

	osg::ref_ptr<Data::Edge> edge1 = new Data::Edge(this->incEleIdCounter(), name, this, srcNode, parallelNode, metype, isOriented, this->getEdgeScale());
	edge1->linkNodes(this->edges);

	this->edgesByType.insert(type->getId(),edge1);

	osg::ref_ptr<Data::Edge> edge2 = new Data::Edge(this->incEleIdCounter(), name, this, parallelNode, dstNode, metype, isOriented, this->getEdgeScale());
	edge2->linkNodes(this->edges);

	this->edgesByType.insert(type->getId(),edge2);

	if(replacedSingleEdge!= NULL)
	{
		removeEdge(replacedSingleEdge);
	}
}

osg::ref_ptr<Data::Node> Data::Graph::getMultiEdgeNeighbour(osg::ref_ptr<Data::Edge> multiEdge)
{
	//zistujeme uzly pospajane multihranou
	osg::ref_ptr<Data::Node> multiNode = NULL;

	if(multiEdge->getSrcNode()->getType()->getName()=="multiNode")
		multiNode = multiEdge->getSrcNode();
	if(multiEdge->getDstNode()->getType()->getName()=="multiNode")
		multiNode = multiEdge->getDstNode();
	if(multiNode == NULL)
		return NULL;

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = multiNode->getEdges()->begin();

	//prechadzame uzly pospajane multihranou
	while (i != multiNode->getEdges()->end())
	{
		if((*i)->getId()!= multiEdge->getId())
		{
			if((*i)->getSrcNode()->getType()->getName()!= "multiNode")
			{
				return (*i)->getSrcNode();
			}
			else
			{
				return (*i)->getDstNode();
			}
		}
		i++;
	}

	return NULL;
}

osg::ref_ptr<Data::Node> Data::Graph::addHyperEdge(QString name, osg::Vec3f position) {
	Data::Type* mtype;

	//pridavame hyper hranu

	QList<Data::Type*> mtypes = getTypesByName(Data::GraphLayout::HYPER_NODE_TYPE);

	if(mtypes.isEmpty())
	{
		//osetrime typ
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", "5");
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
		settings->insert("color.R", "1");
		settings->insert("color.G", "1");
		settings->insert("color.B", "1");
		settings->insert("color.A", "1");

		mtype = this->addType(Data::GraphLayout::HYPER_NODE_TYPE, settings);
	}
	else
	{
		mtype = mtypes[0];
	}

	return addNode (name, mtype, position);
}

bool Data::Graph::isParralel(osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode)
{
	//zistujeme ci dva uzly su spojene viacerymi hranami
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = srcNode->getEdges()->begin();
	bool isMulti = false;

	//prechadzame hranami vychadzajucimi z daneho uzla, zistujeme kolko z nich spaja druhy uzol
	while (i != srcNode->getEdges()->end())
	{
		if ((srcNode->getId() == (*i)->getSrcNode()->getId() && dstNode->getId() == (*i)->getDstNode()->getId()) || (srcNode->getId() == (*i)->getDstNode()->getId() && dstNode->getId() == (*i)->getSrcNode()->getId()))
		{
			isMulti= true;

			this->addMultiEdge((*i)->getName(), (*i)->getSrcNode(), (*i)->getDstNode(), (*i)->getType(), (*i)->isOriented(), (*i));

			break;
		}
		else
		{
			osg::ref_ptr<Data::Node> neghbourNode = getMultiEdgeNeighbour((*i));
			if(neghbourNode!= NULL && (neghbourNode->getId() == (*i)->getSrcNode()->getId() || neghbourNode->getId() == (*i)->getDstNode()->getId()))
			{
				isMulti= true;
				break;
			}
		}
		i++;
	}
	return isMulti;
}

Data::Type* Data::Graph::addType(QString name, QMap <QString, QString> *settings)
{
	//pridanie typu
	Data::Type* type = new Data::Type(this->incEleIdCounter(),name, this, settings);

	this->newTypes.insert(type->getId(),type);
	this->types->insert(type->getId(),type);
	this->typesByName->insert(type->getName(),type);

	return type;
}

Data::MetaType* Data::Graph::addMetaType(QString name, QMap <QString, QString> *settings)
{
	//pridanie metatypu
	if(this->selectedLayout == NULL) {
		qDebug() << "[Data::Graph::addMetaType] Could not create MetaType. No GraphLayout selected.";
		return NULL;
	}

	Data::MetaType* type = new Data::MetaType(this->incEleIdCounter(),name, this, this->selectedLayout, settings);

	this->newTypes.insert(type->getId(),type);
	this->types->insert(type->getId(),type);
	this->typesByName->insert(type->getName(),type);

	return type;
}

Data::GraphLayout* Data::Graph::selectLayout( Data::GraphLayout* layout )
{
	//nastavime layout pre graf
	if(layout==NULL || (layout!=NULL && layout->getGraph()!=NULL && layout->getGraph()==this)) {
		if(this->selectedLayout!=layout) {
			this->selectedLayout = layout;

			QMap<qlonglong, Data::Type*>::iterator it = this->types->begin();
			Data::Type* t;

			while (it!=this->types->end()) {
				t=it.value();
				if(t->isMeta() && ((Data::MetaType* ) t)->getLayout()!=layout) {
					it = this->types->erase(it);
					this->newTypes.remove(t->getId());
					delete t;
					t = NULL;
				} else it++;
			}
		}
	}

	return this->selectedLayout;
}

Data::GraphSpanningTree* Data::Graph::getSpanningTree(qlonglong rootId){
	Data::GraphSpanningTree* spanningTree = new Data::GraphSpanningTree();
	QList<qlonglong> queue;
	QList<int> depthQueue;
	QMap<qlonglong, bool> visited;
	QList<qlonglong> pickedNodes;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator itNode;
	for(itNode = nodes->constBegin(); itNode != nodes->constEnd(); itNode++)
		visited[itNode.key()] = false;

	visited[rootId] = true;
	queue.push_front(rootId);
	int rootDepth = 0;
	depthQueue.push_front(rootDepth);
	pickedNodes.append(rootId);
	spanningTree->addGroup(pickedNodes,rootDepth, 0);


	while (!queue.empty()) {
		pickedNodes.clear();
		osg::ref_ptr<Data::Node> parent = *nodes->find(queue.back());
		int parentDepth = depthQueue.back();
		queue.pop_back();
		depthQueue.pop_back();

		QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator itEdge;
		for (itEdge = parent->getEdges()->constBegin();  itEdge != parent->getEdges()->constEnd(); itEdge++){

			osg::ref_ptr<Data::Node> srcNode = itEdge.value().get()->getSrcNode();
			osg::ref_ptr<Data::Node> dstNode = itEdge.value().get()->getDstNode();
			qlonglong childNodeId;
			if (dstNode->getId() == parent->getId()) {
				childNodeId = srcNode->getId();
			}
			else {
				childNodeId = dstNode->getId();
			}

			if (!visited[childNodeId]) {
				visited[childNodeId] = true;
				queue.push_front(childNodeId);
				pickedNodes.append(childNodeId);
			}
		}
		if (! pickedNodes.empty()){
			int actDepth = parentDepth+1;
			for (int i = 0; i<pickedNodes.size();i++) depthQueue.push_front(actDepth);
			spanningTree->addGroup(pickedNodes,actDepth, parent->getId());
		}
	}

	return spanningTree;
}

qlonglong Data::Graph::getMaxEleIdFromElements()
{
	//hladame najvacsi element podla kluca
	qlonglong max = 0;

	if(this->nodes!=NULL && !this->nodes->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iNodes =  this->nodes->end();
		iNodes--;

		if(iNodes.key()>max) max = iNodes.key();
	}

	if(this->nodes!=NULL && !this->types->isEmpty()) {
		QMap<qlonglong, Data::Type*>::iterator iTypes =  this->types->end();
		iTypes--;

		if(iTypes.key()>max) max = iTypes.key();
	}

	if(this->nodes!=NULL && !this->edges->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdges =  this->edges->end();
		iEdges--;

		if(iEdges.key()>max) max = iEdges.key();
	}

	if(this->nodes!=NULL && !this->metaNodes->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iMetaNodes =  this->metaNodes->end();
		iMetaNodes--;

		if(iMetaNodes.key()>max) max = iMetaNodes.key();
	}

	if(this->nodes!=NULL && !this->metaEdges->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iMetaEdges =  this->metaEdges->end();
		iMetaEdges--;

		if(iMetaEdges.key()>max) max = iMetaEdges.key();
	}

	return max;
}

QString Data::Graph::toString() const
{
	QString str;
	QTextStream(&str) << "graph id:" << graph_id << " name:" << name << " max current ele ID:" << ele_id_counter;
	return str;
}

QList<Data::Type*> Data::Graph::getTypesByName(QString name)
{
	return this->typesByName->values(name);
}

Data::Type* Data::Graph::getNodeMetaType()
{
	//vraciame typ meta uzla s nastavenymi hodnotami
	if(this->selectedLayout==NULL) return NULL;

	if(this->selectedLayout->getMetaSetting(Data::GraphLayout::META_NODE_TYPE) == NULL)
	{
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.MetaNode"));
		settings->insert("color.R", "0.8");
		settings->insert("color.G", "0.1");
		settings->insert("color.B", "0.1");
		settings->insert("color.A", "0.8");

		Data::MetaType* type = this->addMetaType(Data::GraphLayout::META_NODE_TYPE, settings);
		this->selectedLayout->setMetaSetting(Data::GraphLayout::META_NODE_TYPE,QString::number(type->getId()));
		return type;
	} else {
		qlonglong typeId = this->selectedLayout->getMetaSetting(Data::GraphLayout::META_NODE_TYPE).toLongLong();
		if(this->types->contains(typeId)) return this->types->value(typeId);
		return NULL;
	}
}

Data::Type* Data::Graph::getNodeMultiType()
{
	//vraciame typ multi uzla s nastavenymi hodnotami
	QMap<QString, QString> *settings = new QMap<QString, QString>;

	settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
	settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.MetaNode"));
	settings->insert("color.R", "0.8");
	settings->insert("color.G", "0.1");
	settings->insert("color.B", "0.1");
	settings->insert("color.A", "0.8");

	Data::MetaType* type = this->addMetaType(Data::GraphLayout::MULTI_NODE_TYPE, settings);
	//this->selectedLayout->setMetaSetting(Data::GraphLayout::META_NODE_TYPE,QString::number(type->getId()));
	return type;
}

float Data::Graph::getNodeScale()
{
	//vraciame skalu grafu potrebnu pre vnorene grafy
	int level;
	level = this->parent_id.size();
	float offset = Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale").toFloat();

	for(int i=0; i<level; i++)
	{
		//kazda dalsia uroven vnoreneho grafu je 3 krat mensia
		offset = offset/3;
	}
	return offset;
}

float Data::Graph::getEdgeScale()
{
	//vraciame skalu grafu potrebnu pre vnorene grafy
	int level;
	level = this->parent_id.size();
	float offset = Util::ApplicationConfig::get()->getValue("Viewer.Textures.EdgeScale").toFloat();

	for(int i=0; i<level; i++)
	{
		//kazda dalsia uroven vnoreneho grafu je 3 krat mensia
		offset = offset/3;
	}
	return offset;
}

Data::Type* Data::Graph::getEdgeMetaType()
{
	//vraciame typ pre meta hranu
	if(this->selectedLayout==NULL) return NULL;

	if(this->selectedLayout->getMetaSetting(Data::GraphLayout::META_EDGE_TYPE)==NULL) {
		Data::MetaType* type = this->addMetaType(Data::GraphLayout::META_EDGE_TYPE);
		this->selectedLayout->setMetaSetting(Data::GraphLayout::META_EDGE_TYPE,QString::number(type->getId()));
		return type;
	} else {
		qlonglong typeId =  this->selectedLayout->getMetaSetting(Data::GraphLayout::META_EDGE_TYPE).toLongLong();
		if(this->types->contains(typeId)) return this->types->value(typeId);
		return NULL;
	}
}

Data::Type* Data::Graph::getRestrictionNodeMetaType()
{
	//vraciame nastaveny typ pre restriction node (obmedzovac pre vnoreny graf) s nastavenymi hodnotami
	if(this->selectedLayout==NULL) return NULL;

	if(this->selectedLayout->getMetaSetting(Data::GraphLayout::RESTRICTION_NODE_TYPE) == NULL)
	{
		QMap<QString, QString> *settings = new QMap<QString, QString>;

		settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
		settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.MetaNode"));
		settings->insert("color.R", "0");
		settings->insert("color.G", "1");
		settings->insert("color.B", "1");
		settings->insert("color.A", "0.8");

		Data::MetaType* type = this->addMetaType(Data::GraphLayout::RESTRICTION_NODE_TYPE, settings);
		this->selectedLayout->setMetaSetting(Data::GraphLayout::RESTRICTION_NODE_TYPE,QString::number(type->getId()));
		return type;
	} else {
		qlonglong typeId = this->selectedLayout->getMetaSetting(Data::GraphLayout::RESTRICTION_NODE_TYPE).toLongLong();
		if(this->types->contains(typeId)) return this->types->value(typeId);
		return NULL;
	}
}

void Data::Graph::removeType( Data::Type* type )
{
	//odstranujeme typ
	if(type!=NULL && type->getGraph()==this) {
		if(!type->isInDB() || Model::TypeDAO::removeType(type, this->conn)) {
			this->types->remove(type->getId());
			this->newTypes.remove(type->getId());
			this->typesByName->remove(type->getName());

			if(type->isMeta()) {
				if(this->getNodeMetaType()==type) { //type je MetaTypom uzlov layoutu
					this->selectedLayout->removeMetaSetting(Data::GraphLayout::META_NODE_TYPE);
				}
				if(this->getEdgeMetaType()==type) { //type je MetaTypom hran layoutu
					this->selectedLayout->removeMetaSetting(Data::GraphLayout::META_EDGE_TYPE);
				}
			}

			//vymazeme vsetky hrany resp. metahrany daneho typu
			this->removeAllEdgesOfType(type);

			//vymazeme vsetky uzly daneho typu
			this->removeAllNodesOfType(type);
		}
	}
}

void Data::Graph::removeAllEdgesOfType(Data::Type* type )
{
	//ostranujeme vsetky hrany nejakeho typu
	QList<osg::ref_ptr<Data::Edge> > edgesToKill;
	if(type->isMeta()) edgesToKill = this->metaEdgesByType.values(type->getId());
	else edgesToKill = this->edgesByType.values(type->getId());
	if(!edgesToKill.isEmpty()) {
		for(qlonglong i=0;i<edgesToKill.size();i++) {
			this->removeEdge(edgesToKill.at(i));
		}

		edgesToKill.clear();
	}
}

void Data::Graph::removeAllNodesOfType( Data::Type* type )
{
	//odstranujeme vsetky uzly nejakeho typu
	QList<osg::ref_ptr<Data::Node> > nodesToKill;
	if(type->isMeta()) nodesToKill = this->metaNodesByType.values(type->getId()); //vyberieme vsetky uzly daneho typu
	else nodesToKill = this->nodesByType.values(type->getId());

	if(!nodesToKill.isEmpty()) {
		for(qlonglong i=0;i<nodesToKill.size();i++) { //prejdeme kazdy jeden uzol
			this->removeNode(nodesToKill.at(i));
		}

		nodesToKill.clear();
	}
}

void Data::Graph::removeEdge( osg::ref_ptr<Data::Edge> edge)
{
	//odstranenie danej hrany vratane relevantnych zoznamov
	if(edge!=NULL && edge->getGraph()==this) {
		if(!edge->isInDB() || Model::EdgeDAO::removeEdge(edge, this->conn)) {
			this->edges->remove(edge->getId());
			this->metaEdges->remove(edge->getId());
			this->newEdges.remove(edge->getId());
			this->edgesByType.remove(edge->getType()->getId(),edge);
			this->metaEdgesByType.remove(edge->getType()->getId(),edge);

			edge->unlinkNodes();
		}
	}
}

void Data::Graph::removeNode( osg::ref_ptr<Data::Node> node )
{
	//odstranenie uzla vratane relevantnych zoznamov
	if(node!=NULL && node->getGraph()==this) {
		if(!node->isInDB() || Model::NodeDAO::removeNode(node, this->conn)) {

			// remove restrictions:
			{
				QSet<Data::Node *> nodes;
				nodes.insert (node.get ());
				restrictionsManager_.setRestrictions (
							nodes,
							QSharedPointer<Layout::ShapeGetter> (NULL)
							);
			}

			this->nodes->remove(node->getId());
			this->metaNodes->remove(node->getId());
			this->newNodes.remove(node->getId());
			this->nodesByType.remove(node->getType()->getId(),node);
			this->metaNodesByType.remove(node->getType()->getId(),node);

			node->removeAllEdges();

			//zistime ci nahodou dany uzol nie je aj typom a osetrime specialny pripad ked uzol je sam sebe typom (v DB to znamena, ze uzol je ROOT uzlom/typom, teda uz nemoze mat ziaden iny typ)
			if(this->types->contains(node->getId())) {
				this->removeType(this->types->value(node->getId()));
			}
		}
	}
}

Layout::RestrictionsManager & Data::Graph::getRestrictionsManager (void) {
	return restrictionsManager_;
}

osg::ref_ptr<Data::Node> Data::Graph::addRestrictionNode(QString name, osg::Vec3f position, int nodeId) {
	osg::ref_ptr<Data::Node> node;

	//pridame obmedzovac reprezentovany uzlom
	if (nodeId > -1) {
		node = addNode (nodeId, name, getRestrictionNodeMetaType (), position);
	} else {
		node = addNode (name, getRestrictionNodeMetaType (), position);
	}
	node->setIgnored (true);
	node->setPositionCanBeRestricted (false);
	node->setRemovableByUser (false);

	return node;
}

osg::ref_ptr<Data::Node> Data::Graph::addFloatingRestrictionNode(QString name, osg::Vec3f position, int nodeId) {
	osg::ref_ptr<Data::Node> node;

	//pridame obmedzovac reprezentovany uzlom
	if (nodeId > -1) {
		node = addNode (nodeId, name, getRestrictionNodeMetaType (), position);
	} else {
		node = addNode (name, getRestrictionNodeMetaType (), position);
	}
	//node->setIgnored (true);
	node->setIgnored (false);
	node->setPositionCanBeRestricted (true);
	node->setRemovableByUser (false);

	return node;
}

