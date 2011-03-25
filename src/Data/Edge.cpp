/*!
 * Edge.cpp
 * Projekt 3DVisual
 */
#include "Data/Edge.h"

Data::Edge::Edge(qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, float scaling, int pos, osg::ref_ptr<osg::Camera> camera) : osg::DrawArrays(osg::PrimitiveSet::QUADS, pos, 4)
{
    this->id = id;
    this->name = name;
    this->graph = graph;
    this->srcNode = srcNode;
    this->dstNode = dstNode;
    this->type = type;
    this->oriented = isOriented;
    this->camera = camera;
    this->selected = false;
	this->inDB = false;
	this->scale = scaling;
    float r = type->getSettings()->value("color.R").toFloat();
    float g = type->getSettings()->value("color.G").toFloat();
    float b = type->getSettings()->value("color.B").toFloat();
    float a = type->getSettings()->value("color.A").toFloat();
    
    this->edgeColor = osg::Vec4(r, g, b, a);
    	
    this->appConf = Util::ApplicationConfig::get();
    coordinates = new osg::Vec3Array();
    edgeTexCoords = new osg::Vec2Array();
       
    updateCoordinates(getSrcNode()->getTargetPosition(), getDstNode()->getTargetPosition());
}


Data::Edge::~Edge(void)
{
    this->graph = NULL;
	if(this->srcNode!=NULL) {
		this->srcNode->removeEdge(this);
		this->srcNode = NULL;
	}
	
	if(this->dstNode!=NULL) {
		this->dstNode->removeEdge(this);
		this->dstNode = NULL;
	}
	
    this->type = NULL;
    this->appConf = NULL;
}

void Data::Edge::linkNodes(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges)
{
    edges->insert(this->id, this);
    this->dstNode->addEdge(this);
    this->srcNode->addEdge(this);
}

void Data::Edge::unlinkNodes()
{
	this->dstNode->removeEdge(this);
	this->srcNode->removeEdge(this);
	this->srcNode = NULL;
	this->dstNode = NULL;
}

void Data::Edge::unlinkNodesAndRemoveFromGraph() {
	//unlinkNodes will be called from graph->removeEdge !!
	this->graph->removeEdge(this);
}

void Data::Edge::updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstPos)
{
	coordinates->clear();
	edgeTexCoords->clear();

	osg::Vec3d viewVec(0, 0, 1);
	osg::Vec3d up;

	if (camera != 0)
	{
		osg::Vec3d eye;
		osg::Vec3d center;

		camera->getViewMatrixAsLookAt(eye,center,up);

		viewVec = eye - center;

	//	std::cout << eye.x() << " " << eye.y() << " " << eye.z() << "\n";
	//	std::cout << center.x() << " " << center.y() << " " << center.z() << "\n";
	}
	
	viewVec.normalize();

	float graphScale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat();

	osg::Vec3 x, y;
	x.set(srcPos);
	y.set(dstPos);

	osg::Vec3d edgeDir = x - y;
	length = edgeDir.length();

	up = edgeDir ^ viewVec;
	up.normalize();

	//up *= appConf->getValue("Viewer.Textures.EdgeScale").toFloat();
	up *= this->scale;

	coordinates->push_back(osg::Vec3(x.x() + up.x(), x.y() + up.y(), x.z() + up.z()));
	coordinates->push_back(osg::Vec3(x.x() - up.x(), x.y() - up.y(), x.z() - up.z()));
	coordinates->push_back(osg::Vec3(y.x() - up.x(), y.y() - up.y(), y.z() - up.z()));
	coordinates->push_back(osg::Vec3(y.x() + up.x(), y.y() + up.y(), y.z() + up.z()));

	/*std::cout << "Edge coord 1: " << x.x() + up.x() << " " << x.y() + up.y() << " " << x.z() + up.z() << "\n";
	std::cout << "Edge coord 2: " << x.x() - up.x() << " " << x.y() - up.y() << " " << x.z() - up.z() << "\n";
	std::cout << "Edge coord 3: " << y.x() - up.x() << " " << y.y() - up.y() << " " << y.z() - up.z() << "\n";
	std::cout << "Edge coord 4: " << y.x() + up.x() << " " << y.y() + up.y() << " " << y.z() + up.z() << "\n";*/

	//int repeatCnt = length / (2 * appConf->getValue("Viewer.Textures.EdgeScale").toFloat());
	int repeatCnt = length / (2 * this->scale);

	edgeTexCoords->push_back(osg::Vec2(0,1.0f));
	edgeTexCoords->push_back(osg::Vec2(0,0.0f));
	edgeTexCoords->push_back(osg::Vec2(repeatCnt,0.0f));
	edgeTexCoords->push_back(osg::Vec2(repeatCnt,1.0f));

	if (label != NULL)
		label->setPosition((srcPos + dstPos) / 2 );
}

osg::ref_ptr<osg::Drawable> Data::Edge::createLabel(QString name)
{
	label = new osgText::FadeText;
	label->setFadeSpeed(0.03);

	QString fontPath = Util::ApplicationConfig::get()->getValue("Viewer.Labels.Font");

	// experimental value
	float scale = 1.375f * this->type->getScale();

	if(fontPath != NULL && !fontPath.isEmpty())
		label->setFont(fontPath.toStdString());

	label->setText(name.toStdString());
	label->setLineSpacing(0);
	label->setAxisAlignment(osgText::Text::SCREEN);
	label->setCharacterSize(scale);
	label->setDrawMode(osgText::Text::TEXT);
	label->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
	label->setPosition((this->dstNode->getTargetPosition() + this->srcNode->getTargetPosition()) / 2 );
	label->setColor( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

	return label;
}

