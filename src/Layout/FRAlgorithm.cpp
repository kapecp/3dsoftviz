#include "Layout/FRAlgorithm.h"

#include <climits>

using namespace Layout;
using namespace Vwr;	



 //Konstruktor pre vlakno s algoritmom 
FRAlgorithm::FRAlgorithm() 
{
	//nastavenie konstant parametrov
	PI = acos((double) - 1);
	ALPHA = 0.005;
	MIN_MOVEMENT = 0.05;
	MAX_MOVEMENT = 30;
	MAX_DISTANCE = 400;	
	state = RUNNING;
	notEnd = true;
	center = osg::Vec3f (0,0,0);
	fv = osg::Vec3f();
	last = osg::Vec3f();
	newLoc = osg::Vec3f();
	up = osg::Vec3f();
	vp = osg::Vec3f();	
	
	/* moznost odpudiveho posobenia limitovaneho vzdialenostou*/
	useMaxDistance = false;
	this->graph = NULL;

    mLastFocusedNode = 0;   // No node is focused on the beginning
}
FRAlgorithm::FRAlgorithm(Data::Graph *graph) 
{
	PI = acos((double) - 1);
	ALPHA = 0.005;
	MIN_MOVEMENT = 0.05;
	MAX_MOVEMENT = 30;
	MAX_DISTANCE = 400;	
	state = RUNNING;
	notEnd = true;
	osg::Vec3f p(0,0,0);	
	center = p;	
	fv = osg::Vec3f();
	last = osg::Vec3f();
	newLoc = osg::Vec3f();
	up = osg::Vec3f();
	vp = osg::Vec3f();	
	
	/* moznost odpudiveho posobenia limitovaneho vzdialenostou*/
	useMaxDistance = false;
	this->graph = graph;
	this->Randomize();
}

void FRAlgorithm::SetGraph(Data::Graph *graph)
{
	//pociatocne nahodne rozdelenie pozicii uzlov
	notEnd = true;
	this->graph = graph;
	this->Randomize();
}
void FRAlgorithm::SetParameters(float sizeFactor,float flexibility,int animationSpeed,bool useMaxDistance) 
{
	this->sizeFactor = sizeFactor;
	this->flexibility = flexibility;
	this->useMaxDistance = useMaxDistance;

	if(this->graph != NULL)
	{
		K = computeCalm();
		graph->setFrozen(false);
	}
	else
	{
		cout << "Nenastaveny graf. Pouzi metodu SetGraph(Data::Graph graph).";
	}
}

/* Urci pokojovu dlzku strun */
double FRAlgorithm::computeCalm() {
	double R = 300;
	float n = graph->getNodes()->count();
	return sizeFactor* pow((4*R*R*R*PI)/(n*3), 1/3);
}
/* Rozmiestni uzly na nahodne pozicie */
void FRAlgorithm::Randomize() 
{
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
	j = graph->getNodes()->begin();

    for (int i = 0; i < graph->getNodes()->count(); ++i,++j)
	{		
		if(!j.value()->isFixed())
		{
			osg::Vec3f randPos = getRandomLocation();
            j.value()->setTargetPosition(randPos);
            j.value()->setRestrictedTargetPosition(randPos);
		}
	}	
	graph->setFrozen(false);
}

osg::Vec3f FRAlgorithm::getRandomLocation() 
{
	double l = getRandomDouble() * 300;	
	double alpha = getRandomDouble() * 2 * PI;
	double beta = getRandomDouble() * 2 * PI;
	osg::Vec3f newPos =  osg::Vec3f((float) (l * sin(alpha)), (float) (l * cos(alpha) * cos(beta)), (float) (l	* cos(alpha) * sin(beta)));
	return newPos;
}
double FRAlgorithm::getRandomDouble()
{
	int p = rand();

	return (double)rand() / (double)RAND_MAX;
}

void FRAlgorithm::PauseAlg() 
{	
	state = PAUSED;
	isIterating_mutex.lock();
	isIterating_mutex.unlock();
}

void FRAlgorithm::WakeUpAlg() 
{
	if(graph != NULL && state == RUNNING && graph->isFrozen())
	{
		graph->setFrozen(false);
	}
}

void FRAlgorithm::RunAlg()
{
	if(graph != NULL)
	{
		K = computeCalm();
		graph->setFrozen(false);
		state = RUNNING;
	}
}

bool FRAlgorithm::IsRunning() 
{
	return (state == RUNNING);
}

void FRAlgorithm::RequestEnd()
{
	notEnd = false;
}

void FRAlgorithm::Run() 
{
	if(this->graph != NULL)
	{
		isIterating_mutex.lock();
		while (notEnd) 
		{
			// slucka pozastavenia - ak je pauza
			// alebo je graf zmrazeny (spravidla pocas editacie)
			while (notEnd && (state != RUNNING || graph->isFrozen()))
			{
				// [GrafIT][!] not 100% OK (e.g. msleep(100) remains here), but we have fixed the most obvious multithreading issues of the original code
				isIterating_mutex.unlock();
				QThread::msleep(100);
				isIterating_mutex.lock();
			}
			if (!iterate()) {
				graph->setFrozen(true);
			}
		}

		isIterating_mutex.unlock();
	}
	else
	{
		cout << "Nenastaveny graf. Pouzi metodu SetGraph(Data::Graph graph).";

	}
}

bool FRAlgorithm::iterate()
{	
	bool changed = false;  		
	{			
        QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getNodes()->begin();
        for (int i = 0; i < graph->getNodes()->count(); ++i,++j)
		{ // pre vsetky uzly..
			Data::Node* node = j.value();
			node->resetForce(); // vynulovanie posobiacej sily			
		}		
	}
	{//meta uzly
		
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator k;	
		j = graph->getMetaNodes()->begin();
		for (int i = 0; i < graph->getMetaNodes()->count(); ++i,++j)
		{ // pre vsetky metauzly..
			j.value()->resetForce(); // vynulovanie posobiacej sily
			k = graph->getMetaNodes()->begin();
			for (int h = 0; h < graph->getMetaNodes()->count(); ++h,++k)
			{ // pre vsetky metauzly..
				if (!j.value()->equals(k.value())) 
				{
					// odpudiva sila medzi metauzlami
					addRepulsive(j.value(), k.value(), Data::Graph::getMetaStrength());
				}
			}
		}		
	}
	{//meta hrany
		
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator j;
		j = graph->getMetaEdges()->begin();
		for (int i = 0; i < graph->getMetaEdges()->count(); ++i,++j)
		{ // pre vsetky metahrany..
			Data::Node *u = j.value()->getSrcNode();
			Data::Node *v = j.value()->getDstNode();
			// [GrafIT][-] ignored value has not been used, so setting it here did not have any effect
			// uzly nikdy nebudu ignorovane
			/*
			u->setIgnored(false);
			v->setIgnored(false);
			*/
			// [GrafIT]
			if (graph->getMetaNodes()->contains(u->getId())) {
				// pritazliva sila, posobi na v
				addMetaAttractive(v, u, Data::Graph::getMetaStrength());
			}
			if (graph->getMetaNodes()->contains(v->getId())) {
				// pritazliva sila, posobi na u
				addMetaAttractive(u, v, Data::Graph::getMetaStrength());
			}
		}
	}
	{//uzly
        QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
        QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator k;
		j = graph->getNodes()->begin();
		for (int i = 0; i < graph->getNodes()->count(); ++i,++j)
		{ // pre vsetky uzly..
			k = graph->getNodes()->begin();
			for (int h = 0; h < graph->getNodes()->count(); ++h,++k) { // pre vsetky uzly..
				if (!j.value()->equals(k.value())) {
					// odpudiva sila beznej velkosti
					addRepulsive(j.value(), k.value(), 1);
				}
			}
		}
	}
	{//hrany
        QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator j;
		j = graph->getEdges()->begin();
		for (int i = 0; i < graph->getEdges()->count(); ++i,++j)
		{ // pre vsetky hrany..
			// pritazliva sila beznej velkosti
			addAttractive(j.value(), 1);
		}
	}	
	if(state == PAUSED) 
	{
        return false;
	}

    // Find out the focused node according to current restriction (if any)
    {
        float distanceFromFocus;
        float minimalDistanceFromFocus = FLT_MAX;
        Data::Node *focusedNode = 0;

        QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
        j = graph->getNodes()->begin();
        for (int i = 0; i < graph->getNodes()->count(); ++i,++j)
        {
            distanceFromFocus = graph->getRestrictionsManager().distanceFromFocus(*j.value());
            if ( (distanceFromFocus != -1) && (distanceFromFocus < minimalDistanceFromFocus) )
            {
                minimalDistanceFromFocus = distanceFromFocus;
                focusedNode = j.value();
            }
        }

        // If another node than the last one is focused at the moment
        if (focusedNode != mLastFocusedNode)
        {
            if (focusedNode != 0)
                focusedNode->setIsFocused(true);

            if (mLastFocusedNode != 0)
                mLastFocusedNode->setIsFocused(false);
        }

        mLastFocusedNode = focusedNode;
    }

	// aplikuj sily na uzly
	{	
        QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getNodes()->begin();
		for (int i = 0; i < graph->getNodes()->count(); ++i,++j)
		{ // pre vsetky uzly..
			if (!j.value()->isFixed()) {
				last = j.value()->targetPosition();
				bool fo = applyForces(j.value());
				changed = changed || fo;
			}
			else
			{
				changed = false;
			}
		}
	}
	// aplikuj sily na metauzly
	{
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
		j = graph->getMetaNodes()->begin();
		for (int i = 0; i < graph->getMetaNodes()->count(); ++i,++j)
		{ // pre vsetky metauzly..
			if (!j.value()->isFixed()) {
				bool fo = applyForces(j.value());
				changed = changed || fo;
			}
		}
	}

	// Vracia true ak sa ma pokracovat dalsou iteraciou
	return changed;
}

bool FRAlgorithm::applyForces(Data::Node* node) 
{

    QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator edgeIt;
    for (edgeIt=node->getEdges()->begin();edgeIt!=node->getEdges()->end(); edgeIt++){
        if ((*edgeIt)->isShared_X() || (*edgeIt)->isShared_Y() || (*edgeIt)->isShared_Z()){
            osg::ref_ptr<Data::Node> secondNode = (*edgeIt)->getSecondNode(node);
            if (secondNode->isFixed()) return false;
        }
    }

	// nakumulovana sila
	osg::Vec3f fv = node->getForce();
	// zmensenie
	fv *= ALPHA;
	float l = fv.length();
	if (l > MIN_MOVEMENT)
	{ // nie je sila primala?
		if (l > MAX_MOVEMENT)
		{ // je sila privelka?			
			fv.normalize();
			fv *= 5;
		}

		// pricitame aktualnu rychlost
		fv += node->getVelocity();
	} else {
		// [GrafIT][.] this has been a separate case when resetVelocity() has been called and nothing with the target position has been done;
		//             we needed to compute and maybe restrict the target position even if this case; setting the velocity to null vector
		//             and setting it as a velocity has the same effect as resetVelocity()
		// reset velocity
		fv = osg::Vec3(0,0,0);
		// [GrafIT]
	}

    // [GrafIT][.] using restrictions (modified and optimized for speed by Peter Sivak)
    node->setTargetPosition( node->targetPositionConstRef() + fv );   // Compute target position
    graph->getRestrictionsManager().applyRestriction(*node);          // Compute restricted target position
	
        for (edgeIt=node->getEdges()->begin();edgeIt!=node->getEdges()->end(); edgeIt++){
            if ((*edgeIt)->isShared_X()){
                osg::ref_ptr<Data::Node> secondNode = (*edgeIt)->getSecondNode(node);
                osg::Vec3f secondOriginalTargetPosition = secondNode->getTargetPosition();
                osg::Vec3f x = osg::Vec3f (node->restrictedTargetPosition().x(), secondOriginalTargetPosition.y(), secondOriginalTargetPosition.z());
                secondNode->setTargetPosition(x);
            }
            if ((*edgeIt)->isShared_Y()){
                osg::ref_ptr<Data::Node> secondNode = (*edgeIt)->getSecondNode(node);
                osg::Vec3f secondOriginalTargetPosition = secondNode->getTargetPosition();
                osg::Vec3f y = osg::Vec3f (secondOriginalTargetPosition.x(), node->restrictedTargetPosition().y(), secondOriginalTargetPosition.z());
                secondNode->setTargetPosition(y);
            }
            if ((*edgeIt)->isShared_Z()){
                osg::ref_ptr<Data::Node> secondNode = (*edgeIt)->getSecondNode(node);
                osg::Vec3f secondOriginalTargetPosition = secondNode->getTargetPosition();
                osg::Vec3f z = osg::Vec3f (secondOriginalTargetPosition.x(), secondOriginalTargetPosition.y(), node->restrictedTargetPosition().z());
                secondNode->setTargetPosition(z);
            }
        }

	// [GrafIT]

	// energeticka strata = 1-flexibilita
	fv *= flexibility;
	node->setVelocity(fv); // ulozime novu rychlost

    return true;
}

/* Pricitanie pritazlivych sil */
void FRAlgorithm::addAttractive(Data::Edge* edge, float factor) {
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if (!areForcesBetween (edge->getSrcNode(), edge->getDstNode())) {
		return;
	}
	// [GrafIT]
    up = edge->getSrcNode()->targetPosition();
    vp = edge->getDstNode()->targetPosition();
	dist = distance(up,vp);
	if (dist == 0)
		return;
	fv = vp - up; // smer sily
	fv.normalize();
	fv *= attr(dist) * factor;// velkost sily
	edge->getSrcNode()->addForce(fv);
	fv = center - fv;
	edge->getDstNode()->addForce(fv);
}

/* Pricitanie pritazlivych sil od metauzla */
void FRAlgorithm::addMetaAttractive(Data::Node* u, Data::Node* meta, float factor) {
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if (!areForcesBetween (u, meta)) {
		return;
	}
	// [GrafIT]
    up = u->targetPosition();
    vp = meta->targetPosition();
	dist = distance(up,vp);
	if (dist == 0)
		return;
	fv = vp - up;// smer sily
	fv.normalize();
	fv *= attr(dist) * factor;// velkost sily
	u->addForce(fv);
}

/* Pricitanie odpudivych sil */
void FRAlgorithm::addRepulsive(Data::Node* u, Data::Node* v, float factor) {
	// [GrafIT][+] forces are only between nodes which are in the same graph (or some of them is meta) AND are not ignored
	if (!areForcesBetween (u, v)) {
		return;
	}
	// [GrafIT]
    up = u->targetPosition();
    vp = v->targetPosition();
	dist = distance(up,vp);
	if (useMaxDistance && dist > MAX_DISTANCE) {
		return;
	}
	if (dist == 0) {
		// pri splynuti uzlov medzi nimi vytvorime malu vzdialenost
		vp.set(vp.x() + (rand() % 10), vp.y() + (rand() % 10), vp.z() + (rand() % 10));
		dist = distance(up,vp);
	}
	fv = (vp - up);// smer sily
	fv.normalize();
	fv *= rep(dist) * factor;// velkost sily
	u->addForce(fv);
}
/* Vzorec na vypocet odpudivej sily */
float FRAlgorithm::rep(double distance) {
	return (float) (-(K * K) / distance);
}

/* Vzorec na vypocet pritazlivej sily */
float FRAlgorithm::attr(double distance) {
	return (float) ((distance * distance) / K);
}

/* Vzorec na vypocet dostredivej sily */
float FRAlgorithm::centr(double distance) {
	return (float) distance;
}

double FRAlgorithm::distance(osg::Vec3f u,osg::Vec3f v)
{
	osg::Vec3f x = u - v;
	return (double) x.length();
}

bool FRAlgorithm::areForcesBetween (Data::Node * u, Data::Node * v) {
	return
		!(u->isIgnored ())
		&&
		!(v->isIgnored ())
		&&
		(
			graph->isInSameGraph (u, v)
			||
			u->getType ()->isMeta ()
			||
			v->getType ()->isMeta ()
		)
	;
}
