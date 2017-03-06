#include "Layout/FRAlgorithm.h"

#include <climits>



#include "Data/Edge.h"
#include "Data/Node.h"
#include "Data/Cluster.h"
#include "Data/Graph.h"

//volovar_zac
#include "Layout/RadialLayout.h"
//volovar_kon

#include <stdio.h>
#include <math.h>
#include <ctime>
#include <sstream>
#include <iostream>
#include <QDebug>

namespace Layout {

//Konstruktor pre vlakno s algoritmom
FRAlgorithm::FRAlgorithm() :
	rl( nullptr ),
	graph( nullptr ),
	PI( acos( -1.0 ) ),
	ALPHA( 0.005f ),
	MIN_MOVEMENT( 0.05f ),
	MAX_MOVEMENT( 30 ),
	MAX_DISTANCE( 400 ),
	MIN_MOVEMENT_EDGEBUNDLING( 0.05f ),
	ALPHA_EDGEBUNDLING( 100 ),
	flexibility( 0 ),
	sizeFactor( 0 ),
	K( 0 ),
	center( osg::Vec3f( 0, 0, 0 ) ),
	state( RUNNING ),
	stateEdgeBundling( PAUSED ),
	// Moznost odpudiveho posobenia limitovaneho vzdialenostou
	useMaxDistance( false ),
	notEnd( true ),
	fv( osg::Vec3f() ),
	last( osg::Vec3f() ),
	newLoc( osg::Vec3f() ),
	up( osg::Vec3f() ),
	vp( osg::Vec3f() ),
	dist( 0 ),
	// No node is focused on the beginning
	mLastFocusedNode( 0 )
{
	// Duransky start - pociatocne nastavenie nasobica odpudivych sil na rovnakej rovine na hodnotu 1
	setRepulsiveForceVertigo( 1 );
	// Duransky end - pociatocne nastavenie nasobica odpudivych sil na rovnakej rovine na hodnotu 1
}

FRAlgorithm::FRAlgorithm( Data::Graph* graph ) :
	graph( graph ),
	PI( acos( -1.0 ) ),
	ALPHA( 0.005f ),
	MIN_MOVEMENT( 0.05f ),
	MAX_MOVEMENT( 30 ),
	MAX_DISTANCE( 400 ),
	MIN_MOVEMENT_EDGEBUNDLING( 1.0f ),
	ALPHA_EDGEBUNDLING( 100 ),
	center( osg::Vec3f( 0, 0, 0 ) ),
	state( RUNNING ),
	stateEdgeBundling( PAUSED ),
	// Moznost odpudiveho posobenia limitovaneho vzdialenostou
	useMaxDistance( false ),
	notEnd( true ),
	fv( osg::Vec3f() ),
	last( osg::Vec3f() ),
	newLoc( osg::Vec3f() ),
	up( osg::Vec3f() ),
	vp( osg::Vec3f() )
{
	this->Randomize();
}

void FRAlgorithm::SetGraph( Data::Graph* graph, bool fixedPositions )
{
	//pociatocne nahodne rozdelenie pozicii uzlov
	notEnd = true;
	this->graph = graph;
	if ( !fixedPositions ) {
		this->Randomize();
	}
}

void FRAlgorithm::SetParameters( double sizeFactor,float flexibility,bool useMaxDistance )
{
	this->sizeFactor = sizeFactor;
	this->flexibility = flexibility;
	this->useMaxDistance = useMaxDistance;

	if ( this->graph != NULL ) {
		K = computeCalm();
		graph->setFrozen( false );
	}
	else {
		qDebug() << "Nenastaveny graf. Pouzi metodu SetGraph(Data::Graph graph).";
	}
}

/* Urci pokojovu dlzku strun */
double FRAlgorithm::computeCalm()
{
	double R = 300;
	double n = static_cast<double>( graph->getNodes()->count() );
	return sizeFactor* pow( ( 4*R*R*R*PI )/( n*3 ), 1/3 );
}

/* Rozmiestni uzly na nahodne pozicie */
void FRAlgorithm::Randomize()
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
	j = graph->getNodes()->begin();

	for ( int i = 0; i < graph->getNodes()->count(); ++i,++j ) {
		if ( !j.value()->isFixed() ) {
			osg::Vec3f randPos = getRandomLocation();
			j.value()->setTargetPosition( randPos );
			j.value()->setRestrictedTargetPosition( randPos );
		}
	}
	graph->setFrozen( false );
}

osg::Vec3f FRAlgorithm::getRandomLocation()
{
	double l = getRandomDouble() * 300;
	double alpha = getRandomDouble() * 2 * PI;
	double beta = getRandomDouble() * 2 * PI;
	osg::Vec3f newPos =  osg::Vec3f( static_cast<float>( l * sin( alpha ) ), static_cast<float>( l * cos( alpha ) * cos( beta ) ), static_cast<float>( l	* cos( alpha ) * sin( beta ) ) );
	return newPos;
}
double FRAlgorithm::getRandomDouble()
{

	return static_cast<double>( qrand() ) / static_cast<double>( RAND_MAX );
}

void FRAlgorithm::PauseAlg()
{
	state = PAUSED;
	isIterating_mutex.lock();
	isIterating_mutex.unlock();
}

void FRAlgorithm::WakeUpAlg()
{
	if ( graph != NULL && state == RUNNING && graph->isFrozen() ) {
		graph->setFrozen( false );
	}
}

void FRAlgorithm::RunAlg()
{
	if ( graph != NULL ) {
		K = computeCalm();
		graph->setFrozen( false );
		state = RUNNING;
	}
}

bool FRAlgorithm::IsRunning()
{
	return ( state == RUNNING );
}

void FRAlgorithm::RequestEnd()
{
	notEnd = false;
}

void FRAlgorithm::Run()
{
	if ( this->graph != NULL ) {
		isIterating_mutex.lock();
		while ( notEnd ) {
			// slucka pozastavenia - ak je pauza
			// alebo je graf zmrazeny (spravidla pocas editacie)
			while ( notEnd && ( state != RUNNING || graph->isFrozen() ) && ( stateEdgeBundling != RUNNING || graph->isFrozen() ) ) {
				// [GrafIT][!] not 100% OK (e.g. msleep(100) remains here), but we have fixed the most obvious multithreading issues of the original code
				isIterating_mutex.unlock();
				QThread::msleep( 100 );
				isIterating_mutex.lock();
			}
			if ( !iterate() ) {
				graph->setFrozen( true );
			}
		}

		isIterating_mutex.unlock();
	}
	else {
		std::cout << "Nenastaveny graf. Pouzi metodu SetGraph(Data::Graph graph).";

	}
}

bool FRAlgorithm::iterate()
{
	bool changed = false;
	{
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getNodes()->begin();
		for ( int i = 0; i < graph->getNodes()->count(); ++i,++j ) {
			// pre vsetky uzly..
			Data::Node* node = j.value();
			node->resetForce(); // vynulovanie posobiacej sily
		}
	}
	{
		//meta uzly

		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator k;
		j = graph->getMetaNodes()->begin();
		for ( int i = 0; i < graph->getMetaNodes()->count(); ++i,++j ) {
			// pre vsetky metauzly..
			j.value()->resetForce(); // vynulovanie posobiacej sily

			if ( stateEdgeBundling == RUNNING ) {
				if ( j.value()->Data::AbsNode::getName().compare( "metaNode" ) == 0 ) {
					continue;
				}
				//pritazlive sily medzi meta uzlom a jeho susedmi
				QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdge = j.value()->getEdges()->begin();
				while ( iEdge != j.value()->getEdges()->end() ) {
					if ( !j.value()->equals( ( *iEdge )->getSrcNode() ) ) {
						addNeighbourAttractive( j.value(), ( *iEdge )->getSrcNode(), 1 );
					}
					else {
						addNeighbourAttractive( j.value(), ( *iEdge )->getDstNode(), 1 );
					}
					++iEdge;
				}

				//pritazliva sila medzi meta uzlom a ostatnymi metauzlami s rovnakym indexom
				QString jIndex = ( *j )->Data::AbsNode::getName();
				jIndex = jIndex.right( jIndex.length() - jIndex.indexOf( ' ' ) - 1 );
				k = graph->getMetaNodes()->begin();
				while ( k != graph->getMetaNodes()->end() ) {
					if ( k.value()->Data::AbsNode::getName().compare( "metaNode" ) != 0 && !j.value()->equals( k.value() ) ) {

						//porovnanie ci maju rovnaky index
						QString kIndex = ( *k )->Data::AbsNode::getName();
						kIndex = kIndex.right( kIndex.length() - kIndex.indexOf( ' ' ) - 1 );
						if ( QString::compare( jIndex, kIndex, Qt::CaseInsensitive ) == 0 ) {

							//uhol medzi hranami, na ktorych su pomocne uzly
							if ( ( getAngleCompatibility( j.value(), k.value() ) > 0.5 )
									&& ( getScaleCompatibility( j.value(), k.value() ) > 0.5 )
									&& ( getPositionCompatibility( j.value(), k.value() ) > 0.5 ) ) {

								addSameIndexAttractive( j.value(), k.value(), 1 );
							}
						}
					}
					++k;
				}

			}
			else {
				k = graph->getMetaNodes()->begin();
				for ( int h = 0; h < graph->getMetaNodes()->count(); ++h,++k ) {
					// pre vsetky metauzly..
					if ( !j.value()->equals( k.value() ) ) { //Duransky - Bug (j == null zapricini pad programu) pri pridani alebo odstraneni vertigo roviny
						// odpudiva sila medzi metauzlami
						addRepulsive( j.value(), k.value(), Data::Graph::getMetaStrength() );
					}
				}
			}
		}
	}
	{
		//meta hrany

		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator j;
		j = graph->getMetaEdges()->begin();
		for ( int i = 0; i < graph->getMetaEdges()->count(); ++i,++j ) {
			// pre vsetky metahrany..
			Data::Node* u = j.value()->getSrcNode();
			Data::Node* v = j.value()->getDstNode();
			// [GrafIT][-] ignored value has not been used, so setting it here did not have any effect
			// uzly nikdy nebudu ignorovane
			/*
			u->setIgnored(false);
			v->setIgnored(false);
			*/
			// [GrafIT]
			if ( graph->getMetaNodes()->contains( u->getId() ) ) {
				// pritazliva sila, posobi na v
				addMetaAttractive( v, u, Data::Graph::getMetaStrength() );
			}
			if ( graph->getMetaNodes()->contains( v->getId() ) ) {
				// pritazliva sila, posobi na u
				addMetaAttractive( u, v, Data::Graph::getMetaStrength() );
			}
		}
	}
	{
		//uzly
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator k;
		j = graph->getNodes()->begin();
		for ( int i = 0; i < graph->getNodes()->count(); ++i,++j ) {
			// pre vsetky uzly..
			k = graph->getNodes()->begin();
			for ( int h = 0; h < graph->getNodes()->count(); ++h,++k ) { // pre vsetky uzly..
				if ( !j.value()->equals( k.value() ) ) {
					// odpudiva sila beznej velkosti

					//JMA ignore node cond
					//   if( !j.value()->isIgnoredByLayout() && !k.value()->isIgnoredByLayout() ){
					addRepulsive( j.value(), k.value(), 1 );
					//   }
				}
			}
		}
	}
	{
		//hrany
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator j;
		j = graph->getEdges()->begin();
		for ( int i = 0; i < graph->getEdges()->count(); ++i,++j ) {
			// pre vsetky hrany..
			// pritazliva sila beznej velkosti

			//JMA ignore node cond
			// if( !j.value()->getSrcNode()->isIgnoredByLayout() && !j.value()->getDstNode()->isIgnoredByLayout() ){
			addAttractive( j.value(), 1 );
			//  }
		}
	}
	if ( state == PAUSED && stateEdgeBundling == PAUSED ) {
		return false;
	}

	// Find out the focused node according to current restriction (if any)
	{
		float minimalDistanceFromFocus = FLT_MAX;
		Data::Node* focusedNode = 0;

		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getNodes()->begin();
		for ( int i = 0; i < graph->getNodes()->count(); ++i,++j ) {
			float distanceFromFocus = graph->getRestrictionsManager().distanceFromFocus( *j.value() );

			bool testDistance = !qFuzzyCompare( distanceFromFocus,-1.0f );
			if ( ( testDistance ) && ( distanceFromFocus < minimalDistanceFromFocus ) ) {
				minimalDistanceFromFocus = distanceFromFocus;
				focusedNode = j.value();
			}
		}

		// If another node than the last one is focused at the moment
		if ( focusedNode != mLastFocusedNode ) {
			if ( focusedNode != 0 ) {
				focusedNode->setIsFocused( true );
			}

			if ( mLastFocusedNode != 0 ) {
				mLastFocusedNode->setIsFocused( false );
			}
		}

		mLastFocusedNode = focusedNode;
	}

	// aplikuj sily na uzly
	{
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getNodes()->begin();
		for ( int i = 0; i < graph->getNodes()->count(); ++i,++j ) {
			// pre vsetky uzly..
			if ( !j.value()->isFixed() ) {
				last = j.value()->targetPosition();
				bool fo = applyForces( j.value() );
				changed = changed || fo;
			}
			else {
				changed = false;
			}
		}
	}
	// aplikuj sily na metauzly
	{
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getMetaNodes()->begin();
		for ( int i = 0; i < graph->getMetaNodes()->count(); ++i,++j ) {
			// pre vsetky metauzly..
			if ( !j.value()->isFixed() ) {
				bool fo = applyForces( j.value() );
				changed = changed || fo;
			}
		}
	}

	// Vracia true ak sa ma pokracovat dalsou iteraciou
	return changed;
}

bool FRAlgorithm::applyForces( Data::Node* node )
{

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator edgeIt;
	for ( edgeIt=node->getEdges()->begin(); edgeIt!=node->getEdges()->end(); ++edgeIt ) {
		if ( ( *edgeIt )->isShared_X() || ( *edgeIt )->isShared_Y() || ( *edgeIt )->isShared_Z() ) {
			osg::ref_ptr<Data::Node> secondNode = ( *edgeIt )->getSecondNode( node );
			if ( secondNode->isFixed() ) {
				return false;
			}
		}
	}

	// nakumulovana sila
	osg::Vec3f fv = node->getForce();

	// zmensenie pri edge bundlingu
	if ( stateEdgeBundling == RUNNING ) {
		float l = fv.length();
		if ( l > MIN_MOVEMENT_EDGEBUNDLING ) {
			if ( l > MAX_MOVEMENT ) {
				fv.normalize();
				fv *= 5;
			}
			fv += node->getVelocity();    // pricitame aktualnu rychlost
		}
		else {
			fv = osg::Vec3( 0,0,0 );
		}
	} // zmensenie pri FRAlg
	else {
		fv *= ALPHA;
		float l = fv.length();
		if ( ( l > MIN_MOVEMENT && stateEdgeBundling == PAUSED ) ) {
			// nie je sila primala?
			if ( l > MAX_MOVEMENT ) {
				// je sila privelka?
				fv.normalize();
				fv *= 5;
			}

			// pricitame aktualnu rychlost
			fv += node->getVelocity();
		}
		else {
			// [GrafIT][.] this has been a separate case when resetVelocity() has been called and nothing with the target position has been done;
			//             we needed to compute and maybe restrict the target position even if this case; setting the velocity to null vector
			//             and setting it as a velocity has the same effect as resetVelocity()
			// reset velocity
			fv = osg::Vec3( 0,0,0 );
			// [GrafIT]
		}
	}

	// [GrafIT][.] using restrictions (modified and optimized for speed by Peter Sivak)
	//JMA ignore cond
	if ( !node->isIgnoredByLayout() ) {
		node->setTargetPosition( node->targetPositionConstRef() + fv );   // Compute target position
	}

	//node->setTargetPosition( node->targetPositionConstRef() + fv );   // Compute target position
	graph->getRestrictionsManager().applyRestriction( *node );        // Compute restricted target position

	for ( edgeIt=node->getEdges()->begin(); edgeIt!=node->getEdges()->end(); ++edgeIt ) {
		if ( ( *edgeIt )->isShared_X() ) {
			osg::ref_ptr<Data::Node> secondNode = ( *edgeIt )->getSecondNode( node );
			osg::Vec3f secondOriginalTargetPosition = secondNode->getTargetPosition();
			osg::Vec3f x = osg::Vec3f( node->restrictedTargetPosition().x(), secondOriginalTargetPosition.y(), secondOriginalTargetPosition.z() );
			secondNode->setTargetPosition( x );
		}
		if ( ( *edgeIt )->isShared_Y() ) {
			osg::ref_ptr<Data::Node> secondNode = ( *edgeIt )->getSecondNode( node );
			osg::Vec3f secondOriginalTargetPosition = secondNode->getTargetPosition();
			osg::Vec3f y = osg::Vec3f( secondOriginalTargetPosition.x(), node->restrictedTargetPosition().y(), secondOriginalTargetPosition.z() );
			secondNode->setTargetPosition( y );
		}
		if ( ( *edgeIt )->isShared_Z() ) {
			osg::ref_ptr<Data::Node> secondNode = ( *edgeIt )->getSecondNode( node );
			osg::Vec3f secondOriginalTargetPosition = secondNode->getTargetPosition();
			osg::Vec3f z = osg::Vec3f( secondOriginalTargetPosition.x(), secondOriginalTargetPosition.y(), node->restrictedTargetPosition().z() );
			secondNode->setTargetPosition( z );
		}
	}

	// [GrafIT]

	// energeticka strata = 1-flexibilita
	fv *= flexibility;

	node->setVelocity( fv ); // ulozime novu rychlost

	return true;
}

/* Pricitanie pritazlivych sil */
void FRAlgorithm::addAttractive( Data::Edge* edge, float factor )
{
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if ( !areForcesBetween( edge->getSrcNode(), edge->getDstNode() ) ) {
		return;
	}
	// [GrafIT]
	up = edge->getSrcNode()->targetPosition();
	vp = edge->getDstNode()->targetPosition();
	dist = distance( up,vp );
	if ( qFuzzyCompare( dist,0.0 ) ) {
		return;
	}
	fv = vp - up; // smer sily
	fv.normalize();

	if ( edge->getSrcNode()->getCluster() != NULL && edge->getDstNode()->getCluster() && edge->getSrcNode()->getCluster()->getId() == edge->getDstNode()->getCluster()->getId() ) {
		float clusterForce = static_cast<float>( edge->getSrcNode()->getCluster()->getRepulsiveForceInside() );
		clusterForce = qFuzzyCompare( clusterForce, 0.0f ) ? 1 : clusterForce;
		fv *= attr( dist ) * factor / clusterForce; // velkost sily medzi uzlami zhluku
	}
	else {
		fv *= attr( dist ) * factor; // velkost sily
	}

	edge->getSrcNode()->addForce( fv );
	fv = center - fv;
	edge->getDstNode()->addForce( fv );
}

/* Pricitanie pritazlivych sil od metauzla */
void FRAlgorithm::addMetaAttractive( Data::Node* u, Data::Node* meta, float factor )
{
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if ( !areForcesBetween( u, meta ) ) {
		return;
	}
	// [GrafIT]
	up = u->targetPosition();
	vp = meta->targetPosition();
	dist = distance( up,vp );
	if ( qFuzzyCompare( dist,0.0 ) ) {
		return;
	}
	fv = vp - up;// smer sily
	fv.normalize();
	fv *= attr( dist ) * factor; // velkost sily
	u->addForce( fv );
}

/* Pricitanie pritazlivych sil medzi metauzlom a jeho susedom */
void FRAlgorithm::addNeighbourAttractive( Data::Node* meta, Data::Node* neighbour, float factor )
{
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if ( !areForcesBetween( meta, neighbour ) ) {
		return;
	}
	// [GrafIT]
	up = meta->targetPosition();
	vp = neighbour->targetPosition();
	dist = distance( up,vp );
	if ( qFuzzyCompare( dist,0.0 ) ) {
		return;
	}
	fv = vp - up;// smer sily
	fv.normalize();
	fv *= attr( dist )* factor; // velkost sily
	meta->addForce( fv );
}

/* Pricitanie pritazlivych sil medzi dvoma metauzlami s rovnakym indexom */
void FRAlgorithm::addSameIndexAttractive( Data::Node* meta1, Data::Node* meta2, float factor )
{
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if ( !areForcesBetween( meta1, meta2 ) ) {
		return;
	}
	// [GrafIT]
	up = meta1->targetPosition();
	vp = meta2->targetPosition();
	dist = distance( up,vp );
	if ( qFuzzyCompare( dist,0.0 ) ) {
		return;
	}
	fv = vp - up;// smer sily
	fv.normalize();
	fv *= ( 1/attr( dist ) )*ALPHA_EDGEBUNDLING*factor; // velkost sily
	meta1->addForce( fv );
}

/* Pricitanie odpudivych sil */
void FRAlgorithm::addRepulsive( Data::Node* u, Data::Node* v, float factor )
{
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if ( !areForcesBetween( u, v ) ) {
		return;
	}
	// [GrafIT]
	up = u->targetPosition();
	vp = v->targetPosition();
	dist = distance( up,vp );
	if ( useMaxDistance && dist > MAX_DISTANCE ) {
		return;
	}
	//if(dist==0)
	if ( qFuzzyCompare( dist,0.0 ) ) {
		// pri splynuti uzlov medzi nimi vytvorime malu vzdialenost
		vp.set( ( vp.x() + static_cast<float>( qrand() % 10 ) ), ( vp.y() + static_cast<float>( qrand() % 10 ) ),( vp.z() + static_cast<float>( rand() % 10 ) ) );
		dist = distance( up,vp );
	}
	fv = ( vp - up ); // smer sily
	fv.normalize();

	if ( u->getCluster() != NULL && v->getCluster() && u->getCluster()->getId() == v->getCluster()->getId() ) {
		fv *= rep( dist ) * factor * static_cast<float>( u->getCluster()->getRepulsiveForceInside() ); // velkost sily medzi uzlami zhluku
	}
	else {
		fv *= rep( dist ) * factor; // velkost sily
	}


	//volovar zmena aby repulzivne sily posobili len na uzly s rovnakym layerID, ked nemaju radial layout tak ho maju 0
	if ( u->getRadialLayout() != NULL && ( u->getRadialLayout() == v->getRadialLayout() ) ) {
		if ( u->getLayerID() == v->getLayerID() ) {
			fv *= u->getRadialLayout()->getForceSphereScale();
		}
		else {
			fv *= u->getRadialLayout()->getForceScale();
		}
	}
	//volovar koniec zmeny

	// Duransky start - vynasobenie odpudivej sily medzi dvoma uzlami hodnotou zo spinboxu ak su na rovnakej vertigo rovine
	if ( u->getNumberOfVertigoPlane() == v->getNumberOfVertigoPlane() ) {
		fv *= static_cast<float>( repulsiveForceVertigo );
	}
	// Duransky end - vynasobenie odpudivej sily medzi dvoma uzlami hodnotou zo spinboxu ak su na rovnakej vertigo rovine

	u->addForce( fv );
}
/* Vzorec na vypocet odpudivej sily */
float FRAlgorithm::rep( double distance )
{
	return static_cast<float>( -( K * K ) / distance );
}

/* Vzorec na vypocet pritazlivej sily */
float FRAlgorithm::attr( double distance )
{
	return static_cast<float>( ( distance * distance ) / K );
}

/* Vzorec na vypocet dostredivej sily */
float FRAlgorithm::centr( double distance )
{
	return static_cast<float>( distance );
}

double FRAlgorithm::distance( osg::Vec3f u,osg::Vec3f v )
{
	osg::Vec3f x = u - v;
	return static_cast<double>( x.length() );
}

bool FRAlgorithm::areForcesBetween( Data::Node* u, Data::Node* v )
{
	// ak sa aspon 1 z nodov nachadza v zhluku, na ktorom je zaregistrovany obmedzovac, neposobia medzi nimi ziadne sily
	if (
		(
			( u->getCluster() != NULL ) && ( u->getCluster()->getShapeGetter() != NULL )
			//&& ( ( ( v->getCluster() == NULL ) || ( v->getCluster() != NULL ) ) && ( v->getCluster()->getShapeGetter() == NULL ) )
		)
		||
		(
			( v->getCluster() != NULL ) && ( v->getCluster()->getShapeGetter() != NULL )
			//&& ( ( ( u->getCluster() == NULL ) || ( u->getCluster() != NULL ) ) && ( u->getCluster()->getShapeGetter() == NULL ) )
		)
	) {
		return false;
	}
	return
		!( u->isIgnored() )
		&&
		!( v->isIgnored() )
		&&
		(
			graph->isInSameGraph( u, v )
			||
			u->getType()->isMeta()
			||
			v->getType()->isMeta()
		)
		;
}

double FRAlgorithm::getAngleCompatibility( Data::Node* u, Data::Node* v )
{
	osg::ref_ptr<Data::Edge> edge1 =  u->getEdges()->values().at( 0 )->getEdgeParent();
	osg::Vec3f srcPos1 = edge1->getSrcNode()->restrictedTargetPosition();
	osg::Vec3f dstPos1 = edge1->getDstNode()->restrictedTargetPosition();
	osg::Vec3f vector1 = dstPos1 - srcPos1;

	osg::ref_ptr<Data::Edge> edge2 =  v->getEdges()->values().at( 0 )->getEdgeParent();
	osg::Vec3f srcPos2 = edge2->getSrcNode()->restrictedTargetPosition();
	osg::Vec3f dstPos2 = edge2->getDstNode()->restrictedTargetPosition();
	osg::Vec3f vector2 = dstPos2 - srcPos2;

	double angle = acos( static_cast<double>( ( vector1 * vector2 )/ ( ( vector1.length() )*( vector2.length() ) ) ) );
	double angleCompatibility = fabs( cos( angle ) );
	//angle =  osg::RadiansToDegrees( angle );

	return angleCompatibility;
}

double FRAlgorithm::getScaleCompatibility( Data::Node* u, Data::Node* v )
{
	osg::ref_ptr<Data::Edge> edge1 =  u->getEdges()->values().at( 0 )->getEdgeParent();
	osg::ref_ptr<Data::Edge> edge2 =  v->getEdges()->values().at( 0 )->getEdgeParent();
	double length1 = edge1->getLength();
	double length2 = edge2->getLength();

	double minLength = ( length1 < length2 ) ? length1 : length2;
	double maxLength = ( length1 > length2 ) ? length1 : length2;

	double scaleCompatibility = minLength/maxLength;

	return scaleCompatibility;
}

double FRAlgorithm::getPositionCompatibility( Data::Node* u, Data::Node* v )
{
	osg::Vec3f pos1 = u->getCurrentPosition();
	osg::Vec3f pos2 = v->getCurrentPosition();
	double distance = static_cast<double>( ( pos2 - pos1 ).length() );

	osg::ref_ptr<Data::Edge> edge1 =  u->getEdges()->values().at( 0 )->getEdgeParent();
	osg::ref_ptr<Data::Edge> edge2 =  v->getEdges()->values().at( 0 )->getEdgeParent();
	double averageEdgeLength = ( edge1->getLength() + edge2->getLength() )/2;

	double positionCompatibility = averageEdgeLength/( averageEdgeLength + distance );

	return positionCompatibility;
}

void FRAlgorithm::setRepulsiveForceVertigo( int value )
{

	repulsiveForceVertigo = value;

}

void FRAlgorithm::RunAlgEdgeBundling()
{
	if ( graph != NULL ) {
		graph->setFrozen( false );
		stateEdgeBundling = RUNNING;
	}
}

void FRAlgorithm::StopAlgEdgeBundling()
{
	if ( graph != NULL ) {
		stateEdgeBundling = PAUSED;
	}
}

} // namespace Layout

//int getRepulsiveForceVertigo();
