#include "Viewer/EdgeGroup.h"

#include "Viewer/TextureWrapper.h"
#include "Data/Node.h"

#include <osg/Depth>
#include <osg/BlendFunc>

namespace Vwr {

EdgeGroup::EdgeGroup( QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges )
{
	this->edges = edges;

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

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

	while ( i != edges->end() ) {
		allEdges->addChild( i.value() );

		++i;
	}

	this->edgeGroup = allEdges;
}

void EdgeGroup::updateEdgeCoords()
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

	while ( i != edges->end() ) {
		osg::Vec3 srcNodePosition = i.value()->getSrcNode()->getCurrentPosition();
		osg::Vec3 dstNodePosition = i.value()->getDstNode()->getCurrentPosition();

		i.value()->updateCoordinates( srcNodePosition, dstNodePosition );
		++i;
	}
}

void EdgeGroup::synchronizeEdges()
{
	for ( unsigned int i = 0; i < edgeGroup->getNumChildren(); i++ ) {
		if ( edges->key( static_cast<Data::Edge*>( edgeGroup->getChild( i )->asSwitch() ), -1 ) == -1 ) {
			edgeGroup->removeChild( i );
		}
	}

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator ie = edges->begin();

	while ( ie != edges->end() ) {
		if ( !edgeGroup->containsNode( ie.value() ) ) {
			edgeGroup->addChild( ie.value() );
		}

		++ie;
	}
}

} // namespace Vwr
