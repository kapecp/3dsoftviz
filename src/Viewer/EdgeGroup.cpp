#include "Viewer/EdgeGroup.h"

#include "Viewer/TextureWrapper.h"
#include "Data/Node.h"

#include <osg/Depth>
#include <osg/BlendFunc>

namespace Vwr {

EdgeGroup::EdgeGroup( QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, float scale )
{
	this->edges = edges;
	this->scale = scale;

	createEdgeStateSets();
	initEdges();
}

EdgeGroup::~EdgeGroup( void )
{
}

/*!
 *
 * \param scale
 * Koeficient velkosti hran.
 *
 * \returns
 * Podgraf hran.
 *
 *
 * Vytvori podgraf s hranami, ktore spajaju uzly na zaklade ich aktualnej pozicie.
 *
 */
void EdgeGroup::initEdges()
{
	osg::ref_ptr<osg::Group> allEdges = new osg::Group;

/*
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();


	int edgePos = 0;

	while (i != edges->end())
	{
		float length = 0;
		float radius = 0;
		double angle = 0;
		osg::ref_ptr<osg::Vec3Array> rotation = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> center = new osg::Vec3Array;
		getEdgeCoordinatesAndColors(i.value(), edgePos, coordinates, edgeTexCoords, colors, orientedEdgeColors,&length, &radius, center, &angle, rotation);
		edgePos += 4;

		drawableList.push_back(new osg::ShapeDrawable());

		if (i.value()->isOriented())
			orientedGeometry->addPrimitiveSet(i.value());
		else
			geometry->addPrimitiveSet(i.value());

		i++;

	}
*/

QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

	while ( i != edges->end() ) {
		allEdges->addChild( i.value() );

		++i;
	}

/*
		osg::ref_ptr<osg::Geode> g1 = new osg::Geode;
		g1->addDrawable(geometry);
		g1->setStateSet(edgeStateSet);

		osg::ref_ptr<osg::Geode> g2 = new osg::Geode;
		g2->addDrawable(orientedGeometry);
		g2->setStateSet(orientedEdgeStateSet);

		QList<osg::ref_ptr<osg::ShapeDrawable> >::iterator il = drawableList.begin();
		osg::ref_ptr<osg::Group> cylinders = new osg::Group;
		while (il != drawableList.end())
		{
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			setCylinder(*il);
			geode->addDrawable(*il);
			cylinders->addChild(geode);
			il++;
		}

		allEdges->insertChild(INDEX_GEOMETRY, g1, true);
		allEdges->insertChild(INDEX_ORIENTED_GEOMETRY, g2, true);
		allEdges->insertChild(INDEX_CYLINDERS, cylinders, false);


	osg::ref_ptr<osg::Geode> g1 = new osg::Geode;

	g1->addDrawable(geometry);
	g1->setStateSet(edgeStateSet);

	osg::ref_ptr<osg::Geode> g2 = new osg::Geode;
	g2->addDrawable(orientedGeometry);
	g2->setStateSet(orientedEdgeStateSet);

	allEdges->addChild(g1);
	allEdges->addChild(g2);
*/

	this->edgeGroup = allEdges;
}

void EdgeGroup::updateEdgeCoords()
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();
	//QList<osg::ref_ptr<osg::ShapeDrawable> >::iterator ic = drawableList.begin();

	while ( i != edges->end() ) {
		osg::Vec3 srcNodePosition = i.value()->getSrcNode()->getCurrentPosition();
		osg::Vec3 dstNodePosition = i.value()->getDstNode()->getCurrentPosition();

		i.value()->updateCoordinates( srcNodePosition, dstNodePosition );
		i++;
	}
/*
	while (i != edges->end())
	{
		float length = 0;
		float radius = 0;
		double angle = 0;

		osg::ref_ptr<osg::Vec3Array> rotation = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> center = new osg::Vec3Array;
		getEdgeCoordinatesAndColors(i.value(), edgePos, coordinates, edgeTexCoords, colors, orientedEdgeColors,&length, &radius, center, &angle, rotation);

		((osg::Cylinder*)((*ic)->getShape()))->setHeight(length);
		((osg::Cylinder*)((*ic)->getShape()))->setRadius(radius);
		((osg::Cylinder*)((*ic)->getShape()))->setCenter(center->at(0));
		((osg::Cylinder*)((*ic)->getShape()))->setRotation(osg::Quat(angle, osg::Vec3(rotation->at(0).x(), rotation->at(0).y(), rotation->at(0).z())));
		(*ic)->dirtyDisplayList();

		edgePos += 4;

		ic++;
		i++;
	}
*/

}
/*
void EdgeGroup::getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first,
											osg::ref_ptr<osg::Vec3Array> coordinates,
											osg::ref_ptr<osg::Vec2Array> edgeTexCoords,
											osg::ref_ptr<osg::Vec4Array> colors,
											osg::ref_ptr<osg::Vec4Array> orientedEdgeColors,
											float* length,
											float* radius,
											osg::ref_ptr<osg::Vec3Array> center,
											double* angle,
											osg::ref_ptr<osg::Vec3Array> rotation)
{
	osg::Vec3 srcNodePosition = edge->getSrcNode()->getCurrentPosition();
	osg::Vec3 dstNodePosition = edge->getDstNode()->getCurrentPosition();

	edge->updateCoordinates(srcNodePosition, dstNodePosition);
	edge->setFirst(first);

	//calculation of properties of cylinder
	osg::Vec3 cor1 = edge->getCooridnates()->at(0);
	osg::Vec3 cor2 = edge->getCooridnates()->at(1);
	osg::Vec3 cor3 = edge->getCooridnates()->at(2);
	osg::Vec3 cor4 = edge->getCooridnates()->at(3);

	//center between coordinates 1 and 2 / 3 and 4
	osg::Vec3 cor12 = osg::Vec3((cor1.x() + cor2.x())/2, (cor1.y() + cor2.y())/2, (cor1.z() + cor2.z())/2);
	osg::Vec3 cor34 = osg::Vec3((cor3.x() + cor4.x())/2, (cor3.y() + cor4.y())/2, (cor3.z() + cor4.z())/2);

	//center of cylinder as center between coordinates 1-4
	center->push_back(osg::Vec3((cor12.x() + cor34.x())/2, (cor12.y() + cor34.y())/2, (cor12.z() + cor34.z())/2));

	//length of cylinder as length between cor12 and cor34
	(*length) = (float) sqrt(pow((cor34.x() - cor12.x()),2) + pow((cor34.y() - cor12.y()),2) + pow((cor34.z() - cor12.z()),2));
	(*radius) = 2;

	//default direction for the cylinders
	osg::Vec3 direction = osg::Vec3(0,0,1);

	// diff between cor12 and cor34
	osg::Vec3 diff = (cor34 - cor12);

	// CROSS product (the axis of rotation)
	rotation->push_back (direction ^ diff);
	(*angle) = acos( (direction * diff)/ diff.length());

	coordinates->push_back(edge->getCooridnates()->at(0));
	coordinates->push_back(edge->getCooridnates()->at(1));
	coordinates->push_back(edge->getCooridnates()->at(2));
	coordinates->push_back(edge->getCooridnates()->at(3));

	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(0));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(1));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(2));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(3));

	if (edge->isOriented())
		orientedEdgeColors->push_back(edge->getEdgeColor());
	else
		colors->push_back(edge->getEdgeColor());

||||||| merged common ancestors

	geometry->setVertexArray(coordinates);
	geometry->setTexCoordArray(0, edgeTexCoords);
	geometry->setColorArray(colors);

	orientedGeometry->setVertexArray(coordinates);
	orientedGeometry->setTexCoordArray(0, edgeTexCoords);
	orientedGeometry->setColorArray(orientedEdgeColors);
}

void EdgeGroup::getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first,
											osg::ref_ptr<osg::Vec3Array> coordinates,
											osg::ref_ptr<osg::Vec2Array> edgeTexCoords,
											osg::ref_ptr<osg::Vec4Array> colors,
											osg::ref_ptr<osg::Vec4Array> orientedEdgeColors)
{
	osg::Vec3 srcNodePosition = edge->getSrcNode()->getCurrentPosition();
	osg::Vec3 dstNodePosition = edge->getDstNode()->getCurrentPosition();

	edge->updateCoordinates(srcNodePosition, dstNodePosition);
	edge->setFirst(first);

	coordinates->push_back(edge->getCooridnates()->at(0));
	coordinates->push_back(edge->getCooridnates()->at(1));
	coordinates->push_back(edge->getCooridnates()->at(2));
	coordinates->push_back(edge->getCooridnates()->at(3));

	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(0));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(1));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(2));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(3));

	if (edge->isOriented())
		orientedEdgeColors->push_back(edge->getEdgeColor());
	else
		colors->push_back(edge->getEdgeColor());
}
*/
void EdgeGroup::synchronizeEdges()
{
	for ( unsigned int i = 0; i < edgeGroup->getNumChildren(); i++ ) {
		if ( edges->key( static_cast<Data::Edge*>( edgeGroup->getChild( i )->asGeode() ), -1 ) == -1 ) {
			edgeGroup->removeChild( i );
		}
	}

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator ie = edges->begin();

	while ( ie != edges->end() ) {
		if ( !edgeGroup->containsNode( ie.value() ) ) {
			edgeGroup->addChild( ie.value() );
		}

		ie++;
	}


/*
	QList<qlonglong> edgeKeys = edges->keys();

	for (int i = 0; i < 2; i++)
	{
		osg::ref_ptr<osg::Geometry> geometry = edgeGroup->getChild(i)->asGeode()->getDrawable(0)->asGeometry();
		if (geometry != 0){
			const osg::Geometry::PrimitiveSetList primitives = geometry->getPrimitiveSetList();

			for (unsigned int x = 0; x < primitives.size() ; x++)
			{
				Data::Edge * e = dynamic_cast<Data::Edge * >(primitives.at(x).get());

				if (!edgeKeys.contains(e->getId()))
				{
					geometry->removePrimitiveSet(geometry->getPrimitiveSetIndex((e)));
				}
			}

		}
	}

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator ie = edges->begin();

	while ( ie != edges->end() ) {
		if ( !edgeGroup->containsNode( ie.value() ) ) {
			edgeGroup->addChild( ie.value() );
		}

		ie++;
	}

	while (edges->count() > drawableList.size()){
		drawableList.push_back(new osg::ShapeDrawable());

		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::ShapeDrawable> drawable = drawableList.last();
		setCylinder(drawable);
		geode->addDrawable(drawable);
		edgeGroup->getChild(INDEX_CYLINDERS)->asGroup()->addChild(geode);
	}
*/

}

void EdgeGroup::createEdgeStateSets()
{
	edgeStateSet = new osg::StateSet;

	edgeStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	edgeStateSet->setTextureAttributeAndModes( 0, TextureWrapper::getEdgeTexture(), osg::StateAttribute::ON );
	edgeStateSet->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
	edgeStateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

	edgeStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask( false );
	edgeStateSet->setAttributeAndModes( depth, osg::StateAttribute::ON );

	orientedEdgeStateSet = new osg::StateSet;

	orientedEdgeStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	orientedEdgeStateSet->setTextureAttributeAndModes( 0, TextureWrapper::getOrientedEdgeTexture(), osg::StateAttribute::ON );
	orientedEdgeStateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
	orientedEdgeStateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

	orientedEdgeStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	orientedEdgeStateSet->setAttributeAndModes( depth, osg::StateAttribute::ON );
}

/*
void EdgeGroup::setCylinder(osg::ref_ptr<osg::ShapeDrawable> drawable){
	osg::ref_ptr<osg::Cylinder> c = new osg::Cylinder(osg::Vec3(0,0,0), 1.0, 1.0);
	drawable->setShape(c);
	drawable->setColor(osg::Vec4 (1.0f, 1.0f, 1.0f, 0.5f));
	drawable->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	drawable->getStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	drawable->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	drawable->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	drawable->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	drawable->getStateSet()->setRenderBinDetails(11, "RenderBin");
}
*/

} // namespace Vwr
