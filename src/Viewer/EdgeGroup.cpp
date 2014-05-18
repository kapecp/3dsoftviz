#include "Viewer/EdgeGroup.h"

#include "Viewer/TextureWrapper.h"
#include "Data/Node.h"

#include <osg/Depth>
#include <osg/BlendFunc>

using namespace Vwr;

EdgeGroup::EdgeGroup(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges, float scale)
{
	this->edges = edges;
	this->scale = scale;

	createEdgeStateSets();
	initEdges();
}

EdgeGroup::~EdgeGroup(void)
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

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

	while (i != edges->end())
	{
        allEdges->addChild(i.value());

		i++;
	}

	this->edgeGroup = allEdges;
}

void EdgeGroup::updateEdgeCoords()
{
    QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

    while (i != edges->end())
    {
        osg::Vec3 srcNodePosition = i.value()->getSrcNode()->getCurrentPosition();
        osg::Vec3 dstNodePosition = i.value()->getDstNode()->getCurrentPosition();

        i.value()->updateCoordinates(srcNodePosition, dstNodePosition);
        i++;
    }
}

void EdgeGroup::synchronizeEdges()
{
    for (unsigned int i = 0; i < edgeGroup->getNumChildren(); i++){
        if (edges->key(static_cast<Data::Edge*>(edgeGroup->getChild(i)->asGeode()), -1) == -1)
            edgeGroup->removeChild(i);
    }

    QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator ie = edges->begin();

    while (ie != edges->end())
    {
        if (!edgeGroup->containsNode(ie.value()))
            edgeGroup->addChild(ie.value());

        ie++;
    }
}

void EdgeGroup::createEdgeStateSets()
{
	edgeStateSet = new osg::StateSet;

	edgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	edgeStateSet->setTextureAttributeAndModes(0, TextureWrapper::getEdgeTexture(), osg::StateAttribute::ON);
	edgeStateSet->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	edgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	edgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	edgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	orientedEdgeStateSet = new osg::StateSet;

	orientedEdgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	orientedEdgeStateSet->setTextureAttributeAndModes(0, TextureWrapper::getOrientedEdgeTexture(), osg::StateAttribute::ON);
	orientedEdgeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	orientedEdgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	orientedEdgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	orientedEdgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
}
