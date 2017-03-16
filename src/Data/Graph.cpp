/*!
 * Graph.cpp
 * Projekt 3DVisual
 */
#include "Data/Graph.h"

#include "Data/GraphLayout.h"
#include "Data/GraphSpanningTree.h"

#include "Model/GraphDAO.h"
#include "Model/GraphLayoutDAO.h"
#include "Model/TypeDAO.h"
#include "Model/NodeDAO.h"
#include "Model/EdgeDAO.h"

#include "Layout/ShapeGetter_Sphere_AroundNode.h"

#include "Util/ApplicationConfig.h"

#include <QSharedPointer>
#include <QTextStream>
#include <QDebug>

// Tento konstruktor je uz zastaraly a neda sa realne pouzit - uzly musia mat priradeny graph, ktory sa prave vytvarat, rovnako edge, type, metatype (ten musi mat naviac aj layout, ktory opat musi mat graph)
Data::Graph::Graph( qlonglong graph_id, QString name, QSqlDatabase* conn, QMap<qlonglong,osg::ref_ptr<Data::Node> >* nodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> >* edges,QMap<qlonglong,osg::ref_ptr<Data::Node> >* metaNodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> >* metaEdges, QMap<qlonglong,Data::Type*>* types ) :
	graph_id( graph_id ),
	name( name ),
	ele_id_counter( this->getMaxEleIdFromElements() ),
//    ele_id_counter( std::numeric_limits<qlonglong>::max() - 2 ),
	//POZOR toto asi treba inak poriesit, teraz to predpoklada ze ziadne layouty nemame co je spravne, lenze bacha na metatypy, ktore layout mat musia !
	layout_id_counter( 0 ),
	inDB( false ),
	selectedLayout( nullptr ),
	conn( conn ),
	typesByName( new QMultiMap<QString, Data::Type*>() ),
	nodes( nodes ),
	edges( edges ),
	metaNodes( metaNodes ),
	metaEdges( metaEdges ),
	types( types ),
	frozen( false ),
	nodeVisual( Data::Node::INDEX_SQUARE ),
	edgeVisual( Data::Edge::INDEX_QUAD )
{
	foreach ( qlonglong i,nodes->keys() ) {
		this->nodesByType.insert( nodes->value( i )->getType()->getId(),nodes->value( i ) );
	}

	foreach ( qlonglong i,edges->keys() ) {
		this->edgesByType.insert( edges->value( i )->getType()->getId(),edges->value( i ) );
	}

	foreach ( qlonglong i,metaEdges->keys() ) {
		this->metaEdgesByType.insert( metaEdges->value( i )->getType()->getId(),metaEdges->value( i ) );
	}

	foreach ( qlonglong i,metaNodes->keys() ) {
		this->metaNodesByType.insert( metaNodes->value( i )->getType()->getId(),metaNodes->value( i ) );
	}

	if ( this->types!=nullptr && this->types->size()>0 ) {
		foreach ( qlonglong i, this->types->keys() ) {
			this->typesByName->insert( this->types->value( i )->getName(), this->types->value( i ) );
		}
	}

	this->edgeOccurence = QMap<QString, int>();
	this->currentVersion = 0;
}

Data::Graph::Graph( qlonglong graph_id, QString name, qlonglong layout_id_counter, qlonglong ele_id_counter, QSqlDatabase* conn ) :
	graph_id( graph_id ),
	name( name ),
	ele_id_counter( ele_id_counter ),
	layout_id_counter( layout_id_counter ),
	inDB( false ),
	selectedLayout( nullptr ),
	conn( conn ),
	typesByName( new QMultiMap<QString, Data::Type*>() ),
	nodes( new QMap<qlonglong,osg::ref_ptr<Data::Node> >() ),
	edges( new QMap<qlonglong,osg::ref_ptr<Data::Edge> >() ),
	metaNodes( new QMap<qlonglong,osg::ref_ptr<Data::Node> >() ),
	metaEdges( new QMap<qlonglong,osg::ref_ptr<Data::Edge> >() ),
	types( new QMap<qlonglong,Data::Type*>() ),
	frozen( false ),
	nodeVisual( Data::Node::INDEX_SQUARE ),
	edgeVisual( Data::Edge::INDEX_QUAD ),
	restrictionsManager_()
{
	this->edgeOccurence = QMap<QString, int>();
	this->currentVersion = 0;
}

Data::Graph::~Graph( void )
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

	while ( it!=this->types->end() ) {
		Data::Type* type;
		type = it.value();
		it = this->types->erase( it );
		delete type;
		type = NULL;
	}

	this->types->clear();
	delete this->types;
	this->types = NULL;

	//uvolnime vsetky layouty
	QMap<qlonglong, Data::GraphLayout*>::iterator it2 = this->layouts.begin();

	this->selectedLayout = NULL;
	while ( it2!=this->layouts.end() ) {
		Data::GraphLayout* l;
		l = it2.value();
		it2 = this->layouts.erase( it2 );
		delete l;
		l = NULL;
	}
	this->layouts.clear();

	//DB konekcia sa deletovat nebude (kedze tu riesi Manager)
	this->conn = NULL;
}

bool Data::Graph::saveGraphToDB( QSqlDatabase* conn, Data::Graph* graph )
{
	//ukladame cely graf do DB
	if ( Model::NodeDAO::addNodesToDB( conn, graph->nodes )
			&& Model::EdgeDAO::addEdgesToDB( conn, graph->edges ) ) {
		//uspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveGraphToDB] Graph was saved to DB.";
		return true;
	}
	else {
		//neuspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveGraphToDB] Graph wasn't saved to DB.";
		return false;
	}

	return false;
}

bool Data::Graph::saveLayoutToDB( QSqlDatabase* conn, Data::Graph* graph )
{
	//ukladame leyout do DB
	QMap<qlonglong, qlonglong> newMetaNodeID;
	QMap<qlonglong, qlonglong> newMetaEdgeID;

	newMetaNodeID = Model::NodeDAO::getNewMetaNodesId( conn, graph->getId(), graph->metaNodes );
	newMetaEdgeID = Model::EdgeDAO::getNewMetaEdgesId( conn, graph->getId(), graph->metaEdges );

	if ( Model::NodeDAO::addMetaNodesToDB( conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID )
			&& Model::NodeDAO::addNodesPositionsToDB( conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true )
			&& Model::NodeDAO::addNodesColorToDB( conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true )
			&& Model::NodeDAO::addNodesPositionsToDB( conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false )
			&& Model::NodeDAO::addNodesColorToDB( conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false )
			&& Model::EdgeDAO::addMetaEdgesToDB( conn, graph->metaEdges, graph->selectedLayout, newMetaNodeID, newMetaEdgeID )

			&& Model::EdgeDAO::addEdgesColorToDB( conn, graph->edges, graph->selectedLayout, newMetaEdgeID, false )
			&& Model::EdgeDAO::addEdgesColorToDB( conn, graph->metaEdges, graph->selectedLayout, newMetaEdgeID, true )
			&& Model::NodeDAO::addNodesScaleToDB( conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false, graph->getNodeScale() )
			&& Model::NodeDAO::addNodesScaleToDB( conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true, graph->getNodeScale() )
			&& Model::EdgeDAO::addEdgesScaleToDB( conn, graph->edges, graph->selectedLayout,  newMetaEdgeID, false, graph->getEdgeScale() )
			&& Model::EdgeDAO::addEdgesScaleToDB( conn, graph->metaEdges, graph->selectedLayout,  newMetaEdgeID, true, graph->getEdgeScale() )
			&& Model::NodeDAO::addNodesMaskToDB( conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false )
			&& Model::NodeDAO::addNodesMaskToDB( conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true )
			&& Model::NodeDAO::addNodesParentToDB( conn, graph->nodes, graph->selectedLayout, newMetaNodeID, false )
			&& Model::NodeDAO::addNodesParentToDB( conn, graph->metaNodes, graph->selectedLayout, newMetaNodeID, true ) ) {
		//uspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveLayoutToDB] Layout was saved to DB.";
		return true;
	}
	else {
		//neuspesne ulozenie do DB
		qDebug() << "[Data::Graph::saveLayoutToDB] Layout wasn't saved to DB.";
		return false;
	}

	return false;
}

Data::GraphLayout* Data::Graph::addLayout( QString layout_name )
{
	//Vytvarame novy layout pre graf

	// nie je to potrebne a zdrziava to a pouzivatel to nemusi chciet, na to je funkcionalita v menu pre loadovanie grafu z databazy pri starte.
	/*
	if(this->layouts.isEmpty()) { //na zaciatok ak ziadne ine layouty nemame, sa pokusime nacitat layouty z DB
		this->layouts = this->getLayouts(&error);
	}
	*/


	//layouty bude do DB pridavat user, nebudu sa pridavat automaticky
	//layout_id nebudeme brat z DB, lebo layout tam nemusi byt, preto vzdy nastavime na 1
	Data::GraphLayout* layout = new Data::GraphLayout( 1,this,layout_name,this->conn );

	if ( layout==NULL && ( this->conn==NULL || !this->conn->isOpen() ) ) { //nepodarilo sa vytvorit GraphLayout - nejaky problem s pripojenim na DB;
		//vytvorime si nahradny layout aby sme mohli bezat aj bez pripojenia k DB

		layout = new Data::GraphLayout( this->incLayoutIdCounter(),this,layout_name,this->conn );
	}
	else if ( layout==NULL ) { //nepodarilo sa vytvorit Layout, nejaky problem s pridavanim do DB
		return NULL;
	}
	else {   //vytvorili sme si GraphLayout a je v DB
		this->layout_id_counter = layout->getId(); //zvysenie counter ID layoutov (counter je pre pripad, ze by nam vypadlo spojenie ked sa budeme pokusat urobit novy layout)
	}

	this->layouts.insert( layout->getId(),layout );

	return layout;
}

QMap<qlonglong, Data::GraphLayout*> Data::Graph::getLayouts( bool* error )
{
	if ( this->layouts.isEmpty() ) { //ak ziadne layouty nemame, skusime ich nacitat z DB, ak uz nejake mame, tak sme tento krok uz vykonali
		this->layouts = Model::GraphLayoutDAO::getLayouts( this, this->conn, error );

		if ( !this->layouts.isEmpty() ) { //nastavime spravne layout_id_counter
			QMapIterator<qlonglong, Data::GraphLayout*> i( this->layouts );
			i.toBack();
			if ( i.hasPrevious() ) {
				this->layout_id_counter = i.previous().value()->getId();
			}
		}
	}

	return this->layouts;
}

bool Data::Graph::removeLayout( Data::GraphLayout* layout )
{
	if ( Model::GraphLayoutDAO::removeLayout( layout, this->conn ) || !layout->isInDB() ) {
		//layout sa podarilo odstranit z DB alebo v DB vobec nebol -> odstranime vsetky MetaType, ktore patria danemu layout

		if ( layout==this->selectedLayout ) {
			this->selectLayout( NULL );
		}
		this->layouts.remove( layout->getId() );
	}

	return false;
}

QString Data::Graph::setName( QString name )
{
	//nastavyme nazov grafu
	QString newName = Model::GraphDAO::setName( name,this,this->conn );

	if ( newName!=NULL || ( conn==NULL || !conn->isOpen() ) ) { //nastavime nove meno ak sa nam ho podarilo nastavit v DB, alebo ak nemame vobec pripojenie k DB (pre offline verziu)
		this->name = newName;
	}

	return this->name;
}

bool Data::Graph::isInSameGraph( Data::Node* nodeA, Data::Node* nodeB )
{
	if ( nodeA->getNestedParent()==nodeB->getNestedParent() ) {
		//urcie ze uzly su v rovnakom podgrafe ak maju rovnaky nadradeny uzol
		return true;
	}
	return false;
}

osg::ref_ptr<Data::Node> Data::Graph::addNode( QString name, Data::Type* type, osg::Vec3f position )
{
	Data::Type* metype;

	QList<Data::Type*> mtypes = getTypesByName( Data::GraphLayout::NESTED_NODE_TYPE );

	if ( this->parent_id.count()>0 ) {

		if ( mtypes.isEmpty() ) {
			//pridame metatype ak este nie je vytvoreny
			QMap<QString, QString>* settings = new QMap<QString, QString>;

			settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
			settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
			settings->insert( "color.R", "1" );
			settings->insert( "color.G", "0" );
			settings->insert( "color.B", "0" );
			settings->insert( "color.A", "1" );

			metype = this->addType( Data::GraphLayout::NESTED_NODE_TYPE, settings );
		}
		else {
			metype = mtypes[0];
		}

		type = metype;
	}

	osg::ref_ptr<Data::Node> node = new Data::Node( this->incEleIdCounter(), name, type, this->getNodeScaleByType( type ), this, position );

	node->setNestedParent( NULL );

	//pridanie do zoznamu vnorenych uzlov
	if ( this->parent_id.count()>0 ) {
		this->nestedNodes.insert( node.get() );
		this->nestetSubGraphs.last().insert( node.get() );
	}

	//pridame vnoreny uzol
	this->addNestedNode( node );

	this->newNodes.insert( node->getId(),node );
	if ( type!=NULL && type->isMeta() ) {
		//pridame meta uzol do zoznamu
		this->metaNodes->insert( node->getId(),node );
		this->metaNodesByType.insert( type->getId(),node );
	}
	else {
		//pridame vseobecny uzol do zoznamu
		this->nodes->insert( node->getId(),node );
		this->nodesByType.insert( type->getId(),node );
	}

	//pridame metatyp pre vnoreny graf
	if ( this->parent_id.count()>0 ) {
		QList<Data::Type*> metypes = getTypesByName( Data::GraphLayout::MULTI_EDGE_TYPE );
	}

	return node;
}

osg::ref_ptr<Data::Node> Data::Graph::replaceNodeId( int oldId, int newId )
{
	osg::ref_ptr<Data::Node> node = this->nodes->value( oldId );
	Data::Node* newNode = nullptr;
	if ( this->nodes->remove( oldId ) ) {
		newNode = node.release();
	}
	else {
		return nullptr;
	}

//    this->nodes->


	if ( this->nodes->contains( newId ) ) {
		return nullptr;
	}
	else {
		osg::ref_ptr<Data::Node> result = newNode;
		this->nodes->insert( newId, result );
		return result;
	}
}

osg::ref_ptr<Data::Node> Data::Graph::addNode( qlonglong id, QString name, Data::Type* type, osg::Vec3f position )
{
	//vytvorime novy objekt uzla
	osg::ref_ptr<Data::Node> node = new Data::Node( id, name, type, this->getNodeScaleByType( type ), this, position );

	this->newNodes.insert( node->getId(), node );

	//podla typu ho priradime danemu zoznamu
	if ( type!=NULL && type->isMeta() ) {
		this->metaNodes->insert( node->getId(),node );
		this->metaNodesByType.insert( type->getId(),node );
	}
	else {
		this->nodes->insert( node->getId(),node );
		this->nodesByType.insert( type->getId(),node );
	}

	//pridame vnoreny uzol do zoznamu
	this->addNestedNode( node );

	if ( this->parent_id.count()>0 ) {
		this->nestedNodes.insert( node.get() );
	}

	return node;
}

osg::ref_ptr<Data::Node> Data::Graph::mergeNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, osg::Vec3f position, qlonglong mergeNodeId )
{
	//vyratame velkost zluceneho uzla podla velkosti zlucovanych uzlov
	float scale = this->getNodeScale() + ( static_cast<float>( selectedNodes->count() ) / 2.f );

	//vytvorime novy zluceny uzol
	osg::ref_ptr<Data::Node> mergedNode = new Data::Node( ( mergeNodeId != -1 ) ? mergeNodeId : this->incEleIdCounter(), "mergedNode", this->getNodeMetaType(), scale, this, position );
	mergedNode->setColor( osg::Vec4( 0, 0, 1, 1 ) );

	QList<qlonglong> connectedNodes;

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator iAdd = selectedNodes->constBegin();

	//pridavame do zluceneho uzla zlucovane uzly
	while ( iAdd != selectedNodes->constEnd() ) {
		( *iAdd )->setCurrentPosition( mergedNode->getCurrentPosition() );
		( *iAdd )->setFixed( false );
		( *iAdd )->setNodeMask( 0 );

		Data::Edge* e = this->addEdge( "mergedEdge", ( *iAdd ), mergedNode, this->getEdgeMetaType(), true );
		e->setScale( 0 );

		connectedNodes << ( *iAdd )->getId();

		++iAdd;
	}

	//spajame povodne uzly s novovytvorenym zlucenym uzlom
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();
	while ( i != selectedNodes->constEnd() ) {
		QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iedge = ( *i )->getEdges()->constBegin();
		while ( iedge != ( *i )->getEdges()->constEnd() ) {
			iedge.value().get()->setScale( 0 );
			osg::ref_ptr<Data::Node> srcNode = iedge.value().get()->getSrcNode();
			osg::ref_ptr<Data::Node> dstNode = iedge.value().get()->getDstNode();
			osg::ref_ptr<Data::Node> connectNode;

			if ( dstNode->getId() == ( *i )->getId() ) {
				connectNode = srcNode;
			}
			else {
				connectNode = dstNode;
			}

			if ( !connectedNodes.contains( connectNode->getId() ) && ( connectNode->getNodeMask() != 0 ) ) {
				connectedNodes << connectNode->getId();
			}

			++iedge;
		}

		++i;
	}

	//pridame zluceny uzol medzi metauzly
	this->metaNodes->insert( mergedNode->getId(), mergedNode );
	this->metaNodesByType.insert( mergedNode->getId(), mergedNode );

	return mergedNode;
}

void Data::Graph::separateNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes )
{
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();

	//zo zluceneho uzla extrahujeme v nom zahrnute uzly
	while ( i != selectedNodes->constEnd() ) {
		//TODO zatial merged node identifikujeme len podla nazvu nodu - "mergedNode" - treba dokoncit
		if ( ( *i )->getType()->isMeta() && ( static_cast<Data::AbsNode*>( *i ) )->getName() == "mergedNode" ) {
			//najdeme vsetky uzly a zrusime mask
			//najdeme vsetky hrany spojene s tymto uzlom a nastavime im scale
			//nastavime poziciu na mergeNode a zrusime tento uzol
			osg::Vec3f position = ( *i )->getCurrentPosition();

			QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iedge = ( *i )->getEdges()->constBegin();
			while ( iedge != ( *i )->getEdges()->constEnd() ) {
				osg::ref_ptr<Data::Node> connectedNode;
				double scale = this->getEdgeScale();

				//mergedEdge ide vzdy z povodneho nodu do mergedNodu
				connectedNode = iedge.value().get()->getSrcNode();

				//ak je node zamaskovany, tak ho odmaskujeme
				if ( connectedNode->getNodeMask() == 0 ) {
					connectedNode->setCurrentPosition( position );
					connectedNode->setFixed( false );
					connectedNode->setNodeMask( static_cast<unsigned int>( ~0 ) );

					QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iedgeIn = connectedNode->getEdges()->constBegin();

					//nastavujeme velkost hranam
					while ( iedgeIn != connectedNode->getEdges()->constEnd() ) {
						osg::ref_ptr<Data::Node> srcNode = iedgeIn.value().get()->getSrcNode();
						osg::ref_ptr<Data::Node> dstNode = iedgeIn.value().get()->getDstNode();

						//iedgeIn.value().get()->getScale() == 0;
						if ( iedgeIn.value().get()->getIsInvisible() && ( srcNode->getNodeMask() != 0 && dstNode->getNodeMask() != 0 ) ) {
							//TODO - nastavime velkost hranam - vsetkym default - treba prerobit ak budu mat hrany inu velkost
							iedgeIn.value().get()->setScale( scale );
						}

						++iedgeIn;
					}
				}

				//TODO - nastavime velkost hranam - vsetkym default - treba prerobit ak budu mat hrany inu velkost
				iedge.value().get()->setScale( scale );

				++iedge;
			}

			this->removeNode( ( *i ) ); //zmaze uzol aj jeho hrany
		}
		++i;
	}

}


void Data::Graph::addNestedNode( Data::Node* node )
{
	if ( this->parent_id.count()>0 ) {
		//pridame vnoerny uzol do posledneho nacitaneho uzla len pri importe
		node->setParentNode( this->parent_id.last() );
	}
}


void Data::Graph::createNestedGraph( osg::ref_ptr<Data::Node> srcNode )
{
	//vytvorime vnoreny graf do posledneho nacitaneho uzla pri importe
	this->parent_id.append( srcNode );
	srcNode->setAsParentNode();
	QSet<Data::Node*> nn;
	nestetSubGraphs.append( nn );
}

void Data::Graph::closeNestedGraph()
{
	//ukoncime vytvaranie daneho vnoreneho grafu pri importe
	QSharedPointer<Layout::ShapeGetter> shapeGetter(
		new Layout::ShapeGetter_Sphere_AroundNode(
			this->parent_id.last(),
			getNodeScale()*4,
			Layout::Shape_Sphere::RANDOM_DISTANCE_FROM_CENTER,
			Layout::ShapeGetter_Sphere_AroundNode::NODE_CURRENT_POSITION
		)
	);
	//nastavime obmedzovac pre zobrazenie vnoreneho grafu
	restrictionsManager_.setRestrictions( this->nestetSubGraphs.last(), shapeGetter );

	this->nestedNodes.clear();
	this->nestetSubGraphs.removeLast();
	this->parent_id.removeLast();
}

bool Data::Graph::isNestedGraph()
{
	//urcenie ci je dany graf (this) vnoreny
	if ( this->parent_id.size()==0 ) {
		return false;
	}
	return true;
}

osg::ref_ptr<Data::Edge> Data::Graph::addEdge( QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented )
{
	if ( isParralel( srcNode, dstNode ) ) {
		//pridame multihranu fo grafu
		addMultiEdge( name, srcNode, dstNode, type, isOriented, NULL );
	}
	else {
		//pridame hranu do grafu

		if ( this->nestetSubGraphs.count()>0 ) { //parent_id.count()>0)
			type = getNestedEdgeType();
		}
		osg::ref_ptr<Data::Edge> edge = new Data::Edge( this->incEleIdCounter(), name, this, srcNode, dstNode, type, isOriented, getEdgeScale() );

		edge->linkNodes( &this->newEdges );
		if ( ( type!=NULL && type->isMeta() ) || ( ( srcNode->getType()!=NULL && srcNode->getType()->isMeta() ) || ( dstNode->getType()!=NULL && dstNode->getType()->isMeta() ) ) ) {
			//ak je type meta, alebo je meta jeden z uzlov (ma type meta)
			edge->linkNodes( this->metaEdges );
			this->metaEdgesByType.insert( type->getId(),edge );
		}
		else {
			edge->linkNodes( this->edges );
		}
		return edge;
	}

	return NULL;
}

osg::ref_ptr<Data::Edge> Data::Graph::addEdge( qlonglong id, QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented )
{
	if ( isParralel( srcNode, dstNode ) ) {
		//pridame multihranu do grafu
		addMultiEdge( name, srcNode, dstNode, type, isOriented, NULL );
	}
	else {
		//pridame hranu do grafu

		osg::ref_ptr<Data::Edge> edge = new Data::Edge( id, name, this, srcNode, dstNode, type, isOriented, getEdgeScale() );

		edge->linkNodes( &this->newEdges );
		if ( ( type!=NULL && type->isMeta() ) || ( ( srcNode->getType()!=NULL && srcNode->getType()->isMeta() ) || ( dstNode->getType()!=NULL && dstNode->getType()->isMeta() ) ) ) {
			//ak je type meta, alebo je meta jeden z uzlov (ma type meta)
			edge->linkNodes( this->metaEdges );
			this->metaEdgesByType.insert( type->getId(),edge );
		}
		else {
			edge->linkNodes( this->edges );
			this->edgesByType.insert( type->getId(),edge );
		}

		return edge;
	}

	return NULL;
}

Data::Type* Data::Graph::getNestedEdgeType()
{
	Data::Type* metype;

	//vraciame typ vnorenej hrany s nastavenymi parametrami

	QList<Data::Type*> metypes = getTypesByName( Data::GraphLayout::NESTED_EDGE_TYPE );

	if ( metypes.isEmpty() ) {
		//adding NESTED_EDGE_TYPE settings if necessary
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", "50" ); //this->getEdgeScale());
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
		settings->insert( "color.R", "1" );
		settings->insert( "color.G", "0" );
		settings->insert( "color.B", "0" );
		settings->insert( "color.A", "1" );

		metype = this->addType( Data::GraphLayout::NESTED_EDGE_TYPE, settings );
	}
	else {
		metype = metypes[0];
	}
	return metype;
}

Data::Type* Data::Graph::getNestedMetaEdgeType()
{
	Data::Type* metype;

	//vraciame typ vnorenej meta hrany s nastavenymi parametrami

	QList<Data::Type*> metypes = getTypesByName( Data::GraphLayout::NESTED_META_EDGE_TYPE );

	if ( metypes.isEmpty() ) {
		//adding NESTED_EDGE_TYPE settings if necessary
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", "Viewer.Textures.EdgeScale" );
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
		settings->insert( "color.R", "0" );
		settings->insert( "color.G", "1" );
		settings->insert( "color.B", "0" );
		settings->insert( "color.A", "0" );

		metype = this->addType( Data::GraphLayout::NESTED_META_EDGE_TYPE, settings );
	}
	else {
		metype = metypes[0];
	}
	return metype;
}

void Data::Graph::addMultiEdge( QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, osg::ref_ptr<Data::Edge> replacedSingleEdge )
{
	Data::Type* mNodeType;
	QList<Data::Type*> mNodeTypes = getTypesByName( Data::GraphLayout::MULTI_NODE_TYPE );
	//osetrime typ
	if ( mNodeTypes.isEmpty() ) {
		//adding META_NODE_TYPE settings if necessary
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", "20" );
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
		settings->insert( "color.R", "1" );
		settings->insert( "color.G", "1" );
		settings->insert( "color.B", "1" );
		settings->insert( "color.A", "1" );

		mNodeType = this->addType( Data::GraphLayout::MULTI_NODE_TYPE, settings );
	}
	else {
		mNodeType = mNodeTypes[0];
	}

	Data::Type* mEdgeType;
	QList<Data::Type*> mEdgetypes = getTypesByName( Data::GraphLayout::MULTI_EDGE_TYPE );

	if ( mNodeTypes.isEmpty() ) { // really mNodeTypes?
		//adding META_EDGE_TYPE settings if necessary
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", "Viewer.Textures.EdgeScale" );
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
		settings->insert( "color.R", "1" );
		settings->insert( "color.G", "1" );
		settings->insert( "color.B", "1" );
		settings->insert( "color.A", "1" );

		mEdgeType = this->addType( Data::GraphLayout::MULTI_EDGE_TYPE, settings );
	}
	else {
		mEdgeType = mEdgetypes[0];
	}

	QList<osg::ref_ptr<Data::Edge> > newEdgeList = splitEdge( name, srcNode, dstNode, isOriented, mNodeType, mEdgeType, 2 );
	QList<osg::ref_ptr<Data::Edge> >::iterator iEdge = newEdgeList.begin();

	while ( iEdge != newEdgeList.end() ) {
		( *iEdge )->linkNodes( edges );
		edgesByType.insert( type->getId(),( *iEdge ) );
		++iEdge;
	}

	if ( replacedSingleEdge!= NULL ) {
		removeEdge( replacedSingleEdge );
	}
}

QList<osg::ref_ptr<Data::Edge> > Data::Graph::splitEdge( QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, bool isOriented, Data::Type* nodeType, Data::Type* edgeType, int splitCount )
{
	//creating meta nodes to split edge
	QList<osg::ref_ptr<Data::Node> > splitNodeList;
	splitNodeList.push_back( srcNode );
	osg::Vec3f srcPosition = srcNode->getTargetPosition();
	osg::Vec3f dstPosition = dstNode->getTargetPosition();
	osg::Vec3f diffPosition = ( dstPosition - srcPosition )/static_cast<osg::Vec3f::value_type>( splitCount );
	osg::Vec3f metaPosition = srcPosition + diffPosition;
	for ( int i = 1; i < splitCount; i++ ) {
		splitNodeList.push_back( addNode( "SNode " + QString::number( i ), nodeType, metaPosition ) );
		metaPosition += diffPosition;
	}
	splitNodeList.push_back( dstNode );

	if ( this->parent_id.count()>0 ) {
		edgeType = getNestedEdgeType();
	}

	QList<osg::ref_ptr<Data::Edge> > newEdgeList;
	for ( int i = 0; i < splitCount; i++ ) {
		newEdgeList.push_back( new Data::Edge( this->incEleIdCounter(), name, this, splitNodeList.at( i ), splitNodeList.at( i+1 ), edgeType, isOriented, this->getEdgeScale() ) );
	}

	return newEdgeList;
}

void Data::Graph::splitAllEdges( int splitCount )
{
	ele_id_counter += 100000;

	QList<osg::ref_ptr<Data::Edge> > createdEdgeList;

	Data::Type* nodeType = getNodeSplitterType();
	Data::Type* edgeType = getEdgePieceType();

	//split all visible edges
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdge = edges->begin();
	while ( iEdge != edges->end() ) {
		if ( ( *iEdge )->getIsInvisible() ) {
			++iEdge;
			continue;
		}
		QList<osg::ref_ptr<Data::Edge> > edgePieces = splitEdge( "", ( *iEdge )->getSrcNode(), ( *iEdge )->getDstNode(), ( *iEdge )->isOriented(), nodeType, ( *iEdge )->getType(), splitCount ) ;
		createdEdgeList.append( edgePieces );
		( *iEdge )->setEdgePieces( edgePieces );
		++iEdge;
	}

	//add new edges
	QList<osg::ref_ptr<Data::Edge> >::iterator iNewEdge = createdEdgeList.begin();
	while ( iNewEdge != createdEdgeList.end() ) {
		( *iNewEdge )->linkNodes( edges );
		edgesByType.insert( edgeType->getId(),( *iNewEdge ) );
		++iNewEdge;
	}

	//split all visible meta edges
	createdEdgeList.clear();
	iEdge = metaEdges->begin();
	while ( iEdge != metaEdges->end() ) {
		if ( ( *iEdge )->getIsInvisible() ) {
			++iEdge;
			continue;
		}
		QList<osg::ref_ptr<Data::Edge> > edgePieces = splitEdge( "", ( *iEdge )->getSrcNode(), ( *iEdge )->getDstNode(), ( *iEdge )->isOriented(), nodeType, ( *iEdge )->getType(), splitCount ) ;
		createdEdgeList.append( edgePieces );
		( *iEdge )->setEdgePieces( edgePieces );
		++iEdge;
	}

	//add new meta edges
	iNewEdge = createdEdgeList.begin();
	while ( iNewEdge != createdEdgeList.end() ) {
		( *iNewEdge )->linkNodes( metaEdges );
		metaEdgesByType.insert( edgeType->getId(),( *iNewEdge ) );
		++iNewEdge;
	}
}

void Data::Graph::restoreSplittedEdges( )
{
	Data::Type* nodeType = typesByName->value( Data::GraphLayout::SPLITTER_NODE_TYPE );
	removeAllNodesOfType( nodeType );

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdge = edges->begin();
	while ( iEdge != edges->end() ) {
		( *iEdge )->clearEdgePieces();
		++iEdge;
	}

//    iEdge = metaEdges->begin();
//    while ( iEdge != metaEdges->end() ) {
//        ( *iEdge )->clearEdgePieces();
//        iEdge++;
//    }
}


osg::ref_ptr<Data::Node> Data   ::Graph::getMultiEdgeNeighbour( osg::ref_ptr<Data::Edge> multiEdge )
{
	//zistujeme uzly pospajane multihranou
	osg::ref_ptr<Data::Node> multiNode = NULL;

	if ( multiEdge->getSrcNode()->getType()->getName()=="multiNode" ) {
		multiNode = multiEdge->getSrcNode();
	}
	if ( multiEdge->getDstNode()->getType()->getName()=="multiNode" ) {
		multiNode = multiEdge->getDstNode();
	}
	if ( multiNode == NULL ) {
		return NULL;
	}

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = multiNode->getEdges()->begin();

	//prechadzame uzly pospajane multihranou
	while ( i != multiNode->getEdges()->end() ) {
		if ( ( *i )->getId()!= multiEdge->getId() ) {
			if ( ( *i )->getSrcNode()->getType()->getName()!= "multiNode" ) {
				return ( *i )->getSrcNode();
			}
			else {
				return ( *i )->getDstNode();
			}
		}
		++i;
	}

	return NULL;
}

osg::ref_ptr<Data::Node> Data::Graph::addHyperEdge( QString name, osg::Vec3f position )
{
	Data::Type* mtype;

	//pridavame hyper hranu

	QList<Data::Type*> mtypes = getTypesByName( Data::GraphLayout::HYPER_NODE_TYPE );

	if ( mtypes.isEmpty() ) {
		//osetrime typ
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", "5" );
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
		settings->insert( "color.R", "1" );
		settings->insert( "color.G", "1" );
		settings->insert( "color.B", "1" );
		settings->insert( "color.A", "1" );

		mtype = this->addType( Data::GraphLayout::HYPER_NODE_TYPE, settings );
	}
	else {
		mtype = mtypes[0];
	}

	return addNode( name, mtype, position );
}

bool Data::Graph::isParralel( osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode )
{
	//zistujeme ci dva uzly su spojene viacerymi hranami
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = srcNode->getEdges()->begin();
	bool isMulti = false;

	//prechadzame hranami vychadzajucimi z daneho uzla, zistujeme kolko z nich spaja druhy uzol
	while ( i != srcNode->getEdges()->end() ) {
		if ( ( srcNode->getId() == ( *i )->getSrcNode()->getId() && dstNode->getId() == ( *i )->getDstNode()->getId() ) || ( srcNode->getId() == ( *i )->getDstNode()->getId() && dstNode->getId() == ( *i )->getSrcNode()->getId() ) ) {
			isMulti= true;

			this->addMultiEdge( ( static_cast<Data::AbsEdge*>( *i ) )->getName(), ( *i )->getSrcNode(), ( *i )->getDstNode(), ( *i )->getType(), ( *i )->isOriented(), ( *i ) );

			break;
		}
		else {
			osg::ref_ptr<Data::Node> neghbourNode = getMultiEdgeNeighbour( ( *i ) );
			if ( neghbourNode!= NULL && ( neghbourNode->getId() == ( *i )->getSrcNode()->getId() || neghbourNode->getId() == ( *i )->getDstNode()->getId() ) ) {
				isMulti= true;
				break;
			}
		}
		++i;
	}
	return isMulti;
}

Data::Type* Data::Graph::addType( QString name, QMap <QString, QString>* settings )
{
	//pridanie typu
	Data::Type* type = new Data::Type( this->incEleIdCounter(),name, this, settings );

	this->newTypes.insert( type->getId(),type );
	this->types->insert( type->getId(),type );
	this->typesByName->insert( type->getName(),type );

	return type;
}

Data::MetaType* Data::Graph::addMetaType( QString name, QMap <QString, QString>* settings )
{
	//pridanie metatypu
	if ( this->selectedLayout == NULL ) {
		qDebug() << "[Data::Graph::addMetaType] Could not create MetaType. No GraphLayout selected.";
		return NULL;
	}

	Data::MetaType* type = new Data::MetaType( this->incEleIdCounter(),name, this, this->selectedLayout, settings );

	this->newTypes.insert( type->getId(),type );
	this->types->insert( type->getId(),type );
	this->typesByName->insert( type->getName(),type );

	return type;
}

Data::GraphLayout* Data::Graph::selectLayout( Data::GraphLayout* layout )
{
	//nastavime layout pre graf
	if ( layout==NULL || ( layout!=NULL && layout->getGraph()!=NULL && layout->getGraph()==this ) ) {
		if ( this->selectedLayout!=layout ) {
			this->selectedLayout = layout;

			QMap<qlonglong, Data::Type*>::iterator it = this->types->begin();

			while ( it!=this->types->end() ) {
				Data::Type* t;
				t=it.value();
				if ( t->isMeta() && ( static_cast<Data::MetaType*>( t ) )->getLayout()!=layout ) {
					it = this->types->erase( it );
					this->newTypes.remove( t->getId() );
					delete t;
					t = NULL;
				}
				else {
					++it;
				}
			}
		}
	}

	return this->selectedLayout;
}

Data::GraphSpanningTree* Data::Graph::getSpanningTree( qlonglong rootId )
{
	Data::GraphSpanningTree* spanningTree = new Data::GraphSpanningTree();
	QList<qlonglong> queue;
	QList<int> depthQueue;
	QMap<qlonglong, bool> visited;
	QList<qlonglong> pickedNodes;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator itNode;
	for ( itNode = nodes->constBegin(); itNode != nodes->constEnd(); ++itNode ) {
		visited[itNode.key()] = false;
	}

	visited[rootId] = true;
	queue.push_front( rootId );
	int rootDepth = 0;
	depthQueue.push_front( rootDepth );
	pickedNodes.append( rootId );

	spanningTree->addGroup( pickedNodes,rootDepth, static_cast<qlonglong>( 0 ) );


	while ( !queue.empty() ) {
		pickedNodes.clear();
		osg::ref_ptr<Data::Node> parent = *nodes->find( queue.back() );
		int parentDepth = depthQueue.back();
		queue.pop_back();
		depthQueue.pop_back();

		QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator itEdge;
		for ( itEdge = parent->getEdges()->constBegin();  itEdge != parent->getEdges()->constEnd(); ++itEdge ) {

			osg::ref_ptr<Data::Node> srcNode = itEdge.value().get()->getSrcNode();
			osg::ref_ptr<Data::Node> dstNode = itEdge.value().get()->getDstNode();
			qlonglong childNodeId;
			if ( dstNode->getId() == parent->getId() ) {
				childNodeId = srcNode->getId();
			}
			else {
				childNodeId = dstNode->getId();
			}

			if ( !visited[childNodeId] ) {
				visited[childNodeId] = true;
				queue.push_front( childNodeId );
				pickedNodes.append( childNodeId );
			}
		}
		if ( ! pickedNodes.empty() ) {
			int actDepth = parentDepth+1;
			for ( int i = 0; i<pickedNodes.size(); i++ ) {
				depthQueue.push_front( actDepth );
			}
			spanningTree->addGroup( pickedNodes,actDepth, parent->getId() );
		}
	}

	return spanningTree;
}

qlonglong Data::Graph::getMaxEleIdFromElements()
{
	//hladame najvacsi element podla kluca
	qlonglong max = 0;

	if ( this->nodes!=NULL && !this->nodes->isEmpty() ) {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iNodes =  this->nodes->end();
		--iNodes;

		if ( iNodes.key()>max ) {
			max = iNodes.key();
		}
	}

	if ( this->nodes!=NULL && !this->types->isEmpty() ) {
		QMap<qlonglong, Data::Type*>::iterator iTypes =  this->types->end();
		--iTypes;

		if ( iTypes.key()>max ) {
			max = iTypes.key();
		}
	}

	if ( this->nodes!=NULL && !this->edges->isEmpty() ) {
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdges =  this->edges->end();
		--iEdges;

		if ( iEdges.key()>max ) {
			max = iEdges.key();
		}
	}

	if ( this->nodes!=NULL && !this->metaNodes->isEmpty() ) {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iMetaNodes =  this->metaNodes->end();
		--iMetaNodes;

		if ( iMetaNodes.key()>max ) {
			max = iMetaNodes.key();
		}
	}

	if ( this->nodes!=NULL && !this->metaEdges->isEmpty() ) {
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iMetaEdges =  this->metaEdges->end();
		--iMetaEdges;

		if ( iMetaEdges.key()>max ) {
			max = iMetaEdges.key();
		}
	}

	return max;
}

QString Data::Graph::toString() const
{
	QString str;
	QTextStream( &str ) << "graph id:" << graph_id << " name:" << name << " max current ele ID:" << ele_id_counter;
	return str;
}

QList<Data::Type*> Data::Graph::getTypesByName( QString name )
{
	return this->typesByName->values( name );
}

Data::Type* Data::Graph::getNodeMetaType()
{
	//vraciame typ meta uzla s nastavenymi hodnotami
	if ( this->selectedLayout==NULL ) {
		return NULL;
	}

	if ( this->selectedLayout->getMetaSetting( Data::GraphLayout::META_NODE_TYPE ) == NULL ) {
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.MetaNode" ) );
		settings->insert( "color.R", "0.8" );
		settings->insert( "color.G", "0.1" );
		settings->insert( "color.B", "0.1" );
		settings->insert( "color.A", "0.8" );

		Data::MetaType* type = this->addMetaType( Data::GraphLayout::META_NODE_TYPE, settings );
		this->selectedLayout->setMetaSetting( Data::GraphLayout::META_NODE_TYPE,QString::number( type->getId() ) );
		return type;
	}
	else {
		qlonglong typeId = this->selectedLayout->getMetaSetting( Data::GraphLayout::META_NODE_TYPE ).toLongLong();
		if ( this->types->contains( typeId ) ) {
			return this->types->value( typeId );
		}
		return NULL;
	}
}

Data::Type* Data::Graph::getNodeMultiType()
{
	//vraciame typ multi uzla s nastavenymi hodnotami
	QMap<QString, QString>* settings = new QMap<QString, QString>;

	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.MetaNode" ) );
	settings->insert( "color.R", "0.8" );
	settings->insert( "color.G", "0.1" );
	settings->insert( "color.B", "0.1" );
	settings->insert( "color.A", "0.8" );

	Data::MetaType* type = this->addMetaType( Data::GraphLayout::MULTI_NODE_TYPE, settings );
	//this->selectedLayout->setMetaSetting(Data::GraphLayout::META_NODE_TYPE,QString::number(type->getId()));
	return type;
}

Data::Type* Data::Graph::getNodeSplitterType()
{
	//vraciame typ multi uzla s nastavenymi hodnotami
	QMap<QString, QString>* settings = new QMap<QString, QString>;

	double scale = Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ).toDouble();
	settings->insert( "scale", QString::number( scale / 4.0 ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.MetaNode" ) );
	settings->insert( "color.R", "1.0" );
	settings->insert( "color.G", "1.0" );
	settings->insert( "color.B", "1.0" );
	settings->insert( "color.A", "0.8" );

	Data::MetaType* type = this->addMetaType( Data::GraphLayout::SPLITTER_NODE_TYPE, settings );
	return type;
}

float Data::Graph::getNodeScale()
{
	//vraciame skalu grafu potrebnu pre vnorene grafy
	int level;
	level = this->parent_id.size();

	float offset = Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ).toFloat();

	for ( int i=0; i<level; i++ ) {
		//kazda dalsia uroven vnoreneho grafu je 3 krat mensia
		offset = offset/3;
	}
	return offset;
}

float Data::Graph::getNodeScaleByType( Data::Type* type )
{
	//vraciame skalu grafu potrebnu pre vnorene grafy
	int level;
	level = this->parent_id.size();

	float offset = type->getSettings()->value( "scale" ).toFloat();

	for ( int i=0; i<level; i++ ) {
		//kazda dalsia uroven vnoreneho grafu je 3 krat mensia
		offset = offset/3;
	}
	return offset;
}


double Data::Graph::getEdgeScale()
{
	//vraciame skalu grafu potrebnu pre vnorene grafy
	int level;
	level = this->parent_id.size();
	double offset = Util::ApplicationConfig::get()->getValue( "Viewer.Textures.EdgeScale" ).toDouble();

	for ( int i=0; i<level; i++ ) {
		//kazda dalsia uroven vnoreneho grafu je 3 krat mensia
		offset = offset/3;
	}
	return offset;
}

Data::Type* Data::Graph::getEdgeMetaType()
{
	//vraciame typ pre meta hranu
	if ( this->selectedLayout==NULL ) {
		return NULL;
	}

	if ( this->selectedLayout->getMetaSetting( Data::GraphLayout::META_EDGE_TYPE )==NULL ) {
		Data::MetaType* type = this->addMetaType( Data::GraphLayout::META_EDGE_TYPE );
		this->selectedLayout->setMetaSetting( Data::GraphLayout::META_EDGE_TYPE,QString::number( type->getId() ) );
		return type;
	}
	else {
		qlonglong typeId =  this->selectedLayout->getMetaSetting( Data::GraphLayout::META_EDGE_TYPE ).toLongLong();
		if ( this->types->contains( typeId ) ) {
			return this->types->value( typeId );
		}
		return NULL;
	}
}

Data::Type* Data::Graph::getEdgePieceType()
{
	//vraciame typ pre meta hranu
	if ( this->selectedLayout==NULL ) {
		return NULL;
	}

	if ( this->selectedLayout->getMetaSetting( Data::GraphLayout::EDGE_PIECE_TYPE )==NULL ) {
		Data::MetaType* type = this->addMetaType( Data::GraphLayout::EDGE_PIECE_TYPE );
		this->selectedLayout->setMetaSetting( Data::GraphLayout::EDGE_PIECE_TYPE,QString::number( type->getId() ) );
		return type;
	}
	else {
		qlonglong typeId =  this->selectedLayout->getMetaSetting( Data::GraphLayout::EDGE_PIECE_TYPE ).toLongLong();
		if ( this->types->contains( typeId ) ) {
			return this->types->value( typeId );
		}
		return NULL;
	}
}

Data::Type* Data::Graph::getRestrictionNodeMetaType()
{
	//vraciame nastaveny typ pre restriction node (obmedzovac pre vnoreny graf) s nastavenymi hodnotami
	if ( this->selectedLayout==NULL ) {
		return NULL;
	}

	if ( this->selectedLayout->getMetaSetting( Data::GraphLayout::RESTRICTION_NODE_TYPE ) == NULL ) {
		QMap<QString, QString>* settings = new QMap<QString, QString>;

		settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
		settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.MetaNode" ) );
		settings->insert( "color.R", "0" );
		settings->insert( "color.G", "1" );
		settings->insert( "color.B", "1" );
		settings->insert( "color.A", "0.8" );

		Data::MetaType* type = this->addMetaType( Data::GraphLayout::RESTRICTION_NODE_TYPE, settings );
		this->selectedLayout->setMetaSetting( Data::GraphLayout::RESTRICTION_NODE_TYPE,QString::number( type->getId() ) );
		return type;
	}
	else {
		qlonglong typeId = this->selectedLayout->getMetaSetting( Data::GraphLayout::RESTRICTION_NODE_TYPE ).toLongLong();
		if ( this->types->contains( typeId ) ) {
			return this->types->value( typeId );
		}
		return NULL;
	}
}

void Data::Graph::removeType( Data::Type* type )
{
	//odstranujeme typ
	if ( type!=NULL && type->getGraph()==this ) {
		if ( !type->isInDB() || Model::TypeDAO::removeType( type, this->conn ) ) {
			this->types->remove( type->getId() );
			this->newTypes.remove( type->getId() );
			this->typesByName->remove( type->getName() );

			if ( type->isMeta() ) {
				if ( this->getNodeMetaType()==type ) { //type je MetaTypom uzlov layoutu
					this->selectedLayout->removeMetaSetting( Data::GraphLayout::META_NODE_TYPE );
				}
				if ( this->getEdgeMetaType()==type ) { //type je MetaTypom hran layoutu
					this->selectedLayout->removeMetaSetting( Data::GraphLayout::META_EDGE_TYPE );
				}
			}

			//vymazeme vsetky hrany resp. metahrany daneho typu
			this->removeAllEdgesOfType( type );

			//vymazeme vsetky uzly daneho typu
			this->removeAllNodesOfType( type );
		}
	}
}

void Data::Graph::removeAllEdgesOfType( Data::Type* type )
{
	//ostranujeme vsetky hrany nejakeho typu
	QList<osg::ref_ptr<Data::Edge> > edgesToKill;
	if ( type->isMeta() ) {
		edgesToKill = this->metaEdgesByType.values( type->getId() );
	}
	else {
		edgesToKill = this->edgesByType.values( type->getId() );
	}
	if ( !edgesToKill.isEmpty() ) {

		for ( int i=0; i<edgesToKill.size(); i++ ) {
			this->removeEdge( edgesToKill.at( i ) );
		}

		edgesToKill.clear();
	}
}

void Data::Graph::removeAllNodesOfType( Data::Type* type )
{
	//odstranujeme vsetky uzly nejakeho typu
	QList<osg::ref_ptr<Data::Node> > nodesToKill;
	if ( type->isMeta() ) {
		nodesToKill = this->metaNodesByType.values( type->getId() );    //vyberieme vsetky uzly daneho typu
	}
	else {
		nodesToKill = this->nodesByType.values( type->getId() );
	}

	if ( !nodesToKill.isEmpty() ) {

		for ( int i=0; i<nodesToKill.size(); i++ ) { //prejdeme kazdy jeden uzol
			this->removeNode( nodesToKill.at( i ) );
		}

		nodesToKill.clear();
	}
}

void Data::Graph::removeEdge( osg::ref_ptr<Data::Edge> edge )
{
	//odstranenie danej hrany vratane relevantnych zoznamov
	if ( edge!=NULL && edge->getGraph()==this ) {
		if ( !edge->isInDB() || Model::EdgeDAO::removeEdge( edge, this->conn ) ) {
			this->edges->remove( edge->getId() );
			this->metaEdges->remove( edge->getId() );
			this->newEdges.remove( edge->getId() );
			this->edgesByType.remove( edge->getType()->getId(),edge );
			this->metaEdgesByType.remove( edge->getType()->getId(),edge );

			edge->unlinkNodes();
		}
	}
}

void Data::Graph::removeNode( osg::ref_ptr<Data::Node> node )
{
	//odstranenie uzla vratane relevantnych zoznamov
	if ( node!=NULL && node->getGraph()==this ) {
		if ( !node->isInDB() || Model::NodeDAO::removeNode( node, this->conn ) ) {

			// remove restrictions:
			{
				QSet<Data::Node*> nodes;
				nodes.insert( node.get() );
				restrictionsManager_.setRestrictions(
					nodes,
					QSharedPointer<Layout::ShapeGetter> ( NULL )
				);
			}

			if ( !this->nodes->remove( node->getId() ) ) {
				if ( !removeNodeByLuaIdentifier( node->getLuaIdentifier() ) ) {
					qDebug() << "Nepodarilo sa z grafu odstanit node ->" << node->getLuaIdentifier();
				}

			}

			this->metaNodes->remove( node->getId() );
			this->newNodes.remove( node->getId() );
			this->nodesByType.remove( node->getType()->getId(),node );
			this->metaNodesByType.remove( node->getType()->getId(),node );

			node->removeAllEdges();

			//zistime ci nahodou dany uzol nie je aj typom a osetrime specialny pripad ked uzol je sam sebe typom (v DB to znamena, ze uzol je ROOT uzlom/typom, teda uz nemoze mat ziaden iny typ)
			if ( this->types->contains( node->getId() ) ) {
				this->removeType( this->types->value( node->getId() ) );
			}

			node->setInvisible( true );
			node->showLabel( false, false );
			//-spravit odstranovanie poriadne
		}
	}
}

Layout::RestrictionsManager& Data::Graph::getRestrictionsManager( void )
{
	return restrictionsManager_;
}

osg::ref_ptr<Data::Node> Data::Graph::addRestrictionNode( QString name, osg::Vec3f position, int nodeId )
{
	osg::ref_ptr<Data::Node> node;

	//pridame obmedzovac reprezentovany uzlom
	if ( nodeId > -1 ) {
		node = addNode( nodeId, name, getRestrictionNodeMetaType(), position );
	}
	else {
		node = addNode( name, getRestrictionNodeMetaType(), position );
	}
	node->setIgnored( true );
	node->setPositionCanBeRestricted( false );
	node->setRemovableByUser( false );

	return node;
}

osg::ref_ptr<Data::Node> Data::Graph::addFloatingRestrictionNode( QString name, osg::Vec3f position, int nodeId )
{
	osg::ref_ptr<Data::Node> node;

	//pridame obmedzovac reprezentovany uzlom
	if ( nodeId > -1 ) {
		node = addNode( nodeId, name, getRestrictionNodeMetaType(), position );
	}
	else {
		node = addNode( name, getRestrictionNodeMetaType(), position );
	}
	//node->setIgnored (true);
	node->setIgnored( false );
	node->setPositionCanBeRestricted( true );
	node->setRemovableByUser( false );

	return node;
}

Data::Node* Data::Graph::findNodeByName( QString nodeName )
{
	Data::Node* lNode = nullptr;
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* lNodes = this->getNodes();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator it;
	for ( it = lNodes->begin(); it != lNodes->end(); ++it ) {
		lNode = it.value();

		if ( ( QString::compare( lNode->Data::AbsNode::getName(), nodeName ) ) == 0 ) {
			break;
		}
	}
	if ( it == lNodes->end() ) {
		lNode = nullptr;
	}
	return lNode;
}

Data::Node* Data::Graph::findNodeByLuaIdentifier( QString identifier )
{
	Data::Node* lNode = nullptr;
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* lNodes = this->getNodes();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator it;
	for ( it = lNodes->begin(); it != lNodes->end(); ++it ) {
		lNode = it.value();

		if ( !QString::compare( lNode->getLuaIdentifier(), identifier ) ) {
			break;
		}
	}
	if ( it == lNodes->end() ) {
		lNode =  nullptr;
	}

	return lNode;
}

Data::Edge* Data::Graph::findEdgeByName( QString edgeName )
{
	Data::Edge* lEdge = nullptr;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* lEdges = this->getEdges();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator it;
	for ( it = lEdges->begin(); it != lEdges->end(); ++it ) {
		lEdge = it.value();

		if ( ( QString::compare( lEdge->Data::AbsEdge::getName(), edgeName ) ) == 0 ) {
			break;
		}
	}

	if ( it == lEdges->end() ) {
		lEdge = nullptr;
	}
	return lEdge;
}

Data::Edge* Data::Graph::findEdgeByLuaIdentifier( QString identifier )
{
	QStringList nodes = identifier.split( "+" );
	QString newIdentifier = nodes.at( 1 ) + "+" + nodes.at( 0 );

	Data::Edge* lEdge = nullptr;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* lEdges = this->getEdges();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator it;
	for ( it = lEdges->begin(); it != lEdges->end(); ++it ) {
		lEdge = it.value();

		if ( !QString::compare( lEdge->getLuaIdentifier(), identifier ) ) {
			break;
		}
		if ( !QString::compare( lEdge->getLuaIdentifier(), newIdentifier ) ) {
			break;
		}
	}

	if ( it == lEdges->end() ) {
		lEdge = nullptr;
	}
	return lEdge;
}

osg::ref_ptr<Data::Node> Data::Graph::findNodeById( qlonglong nodeId )
{
	osg::ref_ptr<Data::Node> lNode;
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = this->getNodes();

	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *allNodes );
	while ( i.hasNext() ) {
		i.next();
		if ( i.value()->getId() == nodeId ) {
			return i.value();
		}
	}
	return nullptr;
}

void Data::Graph::addEdgeOccurence( QString key )
{
	int count = 0;
	if ( this->edgeOccurence.contains( key ) ) {
		count = this->edgeOccurence.value( key );
	}

	count++;
	this->edgeOccurence.insert( key, count );
}

bool Data::Graph::removeEdgeOccurence( QString key )
{
	if ( this->edgeOccurence.contains( key ) ) {
		int count  = this->edgeOccurence.value( key );
		count--;
		this->edgeOccurence.insert( key, count );
		if ( count != 0 ) {
			return true;
		}
		else {
			return false;
		}

	}
	qDebug() << this->getCurrentVersion() << "CHYBA V RAMCI Data::Graph::removeEdgeOccurence pre key" << key ;
	return false;
}

int Data::Graph::removeNodeByLuaIdentifier( QString identifier )
{
	int count = 0;
	QList<qlonglong> nodes = QList<qlonglong>();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Node>>::iterator iterator = this->nodes->begin(); iterator != this->nodes->end(); ++iterator ) {
		if ( !QString::compare( iterator.value()->getLuaIdentifier(), identifier ) ) {
			nodes.append( iterator.key() );
			count++;
		}
	}

	foreach ( qlonglong id, nodes ) {
		this->nodes->remove( id );
	}

	return count;
}
