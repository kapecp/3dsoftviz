/*!
 * Node.cpp
 * Projekt 3DVisual
 *
 * TODO - reload configu sa da napisat aj efektivnejsie. Pri testoch na hranach priniesol vsak podobny prepis len male zvysenie vykonu. Teraz na to
 * nemam cas, takze sa raz k tomu vratim 8)
 */
#include "Data/Node.h"
#include "Util/ApplicationConfig.h"
#include "Viewer/TextureWrapper.h"

#include <osgText/FadeText>

typedef osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,1> ColorIndexArray;

Data::Node::Node(qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position) 
{
	//konstruktor
	//scaling je potrebne na zmensenie uzla ak je vnoreny
    this->id = id;
	this->name = name;
	this->type = type;
	this->targetPosition = position;
	this->currentPosition = position * Util::ApplicationConfig::get()->getValue("Viewer.Display.NodeDistanceScale").toFloat();
	this->graph = graph;
	this->inDB = false;
	this->edges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
	this->scale = scaling;
	this->setBall(NULL);
	this->setParentBall(NULL);
	this->hasNestedNodes = false;



	settings = new QMap<QString, QString>();
	//APA

	settings->insert("Velkost","4242");
	settings->insert("Farba","ruzova");
	//APA

	int pos = 0;
	int cnt = 0;

	labelText = this->name;

	while ((pos = labelText.indexOf(QString(" "), pos + 1)) != -1)
	{
		if (++cnt % 3 == 0)
			labelText = labelText.replace(pos, 1, "\n");
	}

	this->addDrawable(createNode(this->scale, Node::createStateSet(this->type)));
	
	//vytvorenie grafickeho zobrazenia ako label
	this->square = createSquare(this->type->getScale(), Node::createStateSet());
	this->label = createLabel(this->type->getScale(), labelText);

	this->force = osg::Vec3f();
	this->velocity = osg::Vec3f(0,0,0);
	this->fixed = false;
	this->ignore = false;
	this->positionCanBeRestricted = true;
	this->removableByUser = true;
	this->selected = false;
	this->usingInterpolation = true;

	//nastavenie farebneho typu
	float r = type->getSettings()->value("color.R").toFloat();
	float g = type->getSettings()->value("color.G").toFloat();
	float b = type->getSettings()->value("color.B").toFloat();
	float a = type->getSettings()->value("color.A").toFloat();

	this->setColor(osg::Vec4(r, g, b, a));
}

Data::Node::~Node(void)
{
	foreach(qlonglong i, edges->keys()) {
		edges->value(i)->unlinkNodes();
	}
    edges->clear(); //staci to ?? netreba spravit delete/remove ??

	delete edges;
}

void Data::Node::addEdge(osg::ref_ptr<Data::Edge> edge) { 
	//pridanie napojenej hrany na uzol
	edges->insert(edge->getId(), edge);
}



void Data::Node::removeEdge( osg::ref_ptr<Data::Edge> edge )
{
	//odobranie napojenej hrany zo zoznamu
	edges->remove(edge->getId());
}

Data::Node* Data::Node::getParentNode()
{
	//nadradeny uzol
	return this->nested_parent;
}

void Data::Node::setParentNode(Node* parent)
{
	//nastavenie nadradeneho uzla ktoremu je tento vnoreny
	this->nested_parent = parent;
}

void Data::Node::removeAllEdges()
{
	//odpojenie od vsetkych hran
	foreach(qlonglong i, edges->keys()) {
		edges->value(i)->unlinkNodesAndRemoveFromGraph();
	}
	edges->clear();
}

osg::ref_ptr<osg::Drawable> Data::Node::createNode(const float & scaling, osg::StateSet* bbState) 
{
	//vytvorenie uzla, scaling urcuje jeho velkost
	float width = scaling;
	float height = scaling;

	osg::ref_ptr<osg::Geometry> nodeQuad = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array(4);

	//velkost uzla
	(*nodeVerts)[0] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[1] = osg::Vec3( width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[2] = osg::Vec3( width / 2.0f,	height / 2.0f, 0);
	(*nodeVerts)[3] = osg::Vec3(-width / 2.0f,  height / 2.0f, 0);

	nodeQuad->setUseDisplayList(false);

	nodeQuad->setVertexArray(nodeVerts);
	nodeQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

	osg::ref_ptr<osg::Vec2Array> nodeTexCoords = new osg::Vec2Array(4);
	//umiestnenie popisu uzla
	(*nodeTexCoords)[0].set(0.0f,0.0f);
	(*nodeTexCoords)[1].set(1.0f,0.0f);
	(*nodeTexCoords)[2].set(1.0f,1.0f);
	(*nodeTexCoords)[3].set(0.0f,1.0f);

	nodeQuad->setTexCoordArray(0, nodeTexCoords);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	osg::ref_ptr<ColorIndexArray> colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back(0);

	nodeQuad->setColorArray( colorArray);
	nodeQuad->setColorIndices(colorIndexArray);
	nodeQuad->setColorBinding(osg::Geometry::BIND_OVERALL);	
	nodeQuad->setStateSet(bbState); 

	return nodeQuad;
}

osg::ref_ptr<osg::Drawable> Data::Node::createSquare(const float & scale, osg::StateSet* bbState)
{
	//vytvorenie textury uzla
	float width = 2.0f;
	float height = 2.0f;

	width *= scale;
	height *= scale;

	osg::ref_ptr<osg::Geometry> nodeRect = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array(5);

	(*nodeVerts)[0] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[1] = osg::Vec3( width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[2] = osg::Vec3( width / 2.0f,	height / 2.0f, 0);
	(*nodeVerts)[3] = osg::Vec3(-width / 2.0f,  height / 2.0f, 0);
	(*nodeVerts)[4] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);

	nodeRect->setVertexArray(nodeVerts);
	nodeRect->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,5));

	osg::ref_ptr<ColorIndexArray> colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back(0);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));

	nodeRect->setColorArray(colorArray);
	nodeRect->setColorIndices(colorIndexArray);

	nodeRect->setColorArray( colorArray);
	nodeRect->setColorBinding(osg::Geometry::BIND_OVERALL);	

	nodeRect->setStateSet(bbState);

	return nodeRect;
}

osg::ref_ptr<osg::Drawable> Data::Node::createLabel(const float & scale, QString name)
{
	//vytvorenie popisu uzla
	osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
	label->setFadeSpeed(0.03);

	QString fontPath = Util::ApplicationConfig::get()->getValue("Viewer.Labels.Font");
	
	// experimental value
        float newScale = 1.375f * scale;

	if(fontPath != NULL && !fontPath.isEmpty())
		label->setFont(fontPath.toStdString());

	label->setText(name.toStdString());
	label->setLineSpacing(0);
	label->setAxisAlignment(osgText::Text::SCREEN);
	label->setCharacterSize(newScale);
	label->setDrawMode(osgText::Text::TEXT);
	label->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
	label->setPosition( osg::Vec3(0, newScale, 0) );
	label->setColor( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

	return label;
}

osg::ref_ptr<osg::StateSet> Data::Node::createStateSet(Data::Type * type)
{
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;

	stateSet->setDataVariance(osg::Object::DYNAMIC);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	if(type != 0)
		stateSet->setTextureAttributeAndModes(0, type->getTypeTexture(), osg::StateAttribute::ON);

	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); 
 
 	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
 	depth->setWriteMask(false);
 	stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON); 

	return stateSet;
}

bool Data::Node::equals(Node* node) 
{
	//porovnanie s inym uzlom
	if (this == node)
	{
		return true;
	}
	if (node == NULL)
	{
		return false;
	}		
	if (id != node->getId())
	{
		return false;
	}
	if((graph==NULL && node->getGraph()!=NULL) || (graph!=NULL && node->getGraph()==NULL)) {
	    return false;
	}
    if(graph==NULL && node->getGraph()==NULL) {
        return true;
    }
	if(graph!=NULL && graph->getId() != node->getGraph()->getId()) {
	    return false;
	}
	return true;
}

void Data::Node::setDrawableColor(int pos, osg::Vec4 color)
{
	//nastavenie farby uzla
	osg::Geometry * geometry  = dynamic_cast<osg::Geometry *>(this->getDrawable(pos)); 

	if (geometry != NULL)
	{
		osg::Vec4Array * colorArray =  dynamic_cast<osg::Vec4Array *>(geometry->getColorArray());

		colorArray->pop_back();
		colorArray->push_back(color);
	}
}

void Data::Node::showLabel(bool visible)
{
	//nastavenie zobrazenia popisku uzla
	if (visible && !this->containsDrawable(label))
		this->addDrawable(label);
	else if (!visible)
		this->removeDrawable(label);
}

void Data::Node::reloadConfig()
{
	this->setDrawable(0, createNode(this->scale, Node::createStateSet(this->type)));
	setSelected(selected);

	osg::ref_ptr<osg::Drawable> newRect = createSquare(this->type->getScale(), Node::createStateSet());
	osg::ref_ptr<osg::Drawable> newLabel = createLabel(this->type->getScale(), labelText);

	if (this->containsDrawable(label))
	{
		this->setDrawable(this->getDrawableIndex(label), newLabel);
	}

	if (this->containsDrawable(square))
	{
		this->setDrawable(this->getDrawableIndex(square), newRect);
	}

	label = newLabel;
	square = newRect;
}

osg::Vec3f Data::Node::getCurrentPosition(bool calculateNew, float interpolationSpeed)  
{ 
	//zisime aktualnu poziciu uzla v danom okamihu
	if (calculateNew)
	{
		float graphScale = Util::ApplicationConfig::get()->getValue("Viewer.Display.NodeDistanceScale").toFloat(); 

		osg::Vec3 directionVector = osg::Vec3(targetPosition.x(), targetPosition.y(), targetPosition.z()) * graphScale - currentPosition;
		this->currentPosition = osg::Vec3(directionVector * (usingInterpolation ? interpolationSpeed : 1) + this->currentPosition);
	}

	return osg::Vec3(this->currentPosition); 
}
