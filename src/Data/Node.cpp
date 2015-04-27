/*!
 * ::Node.cpp
 * Projekt 3DVisual
 *
 * TODO - reload configu sa da napisat aj efektivnejsie. Pri testoch na hranach priniesol vsak podobny prepis len male zvysenie vykonu. Teraz na to
 * nemam cas, takze sa raz k tomu vratim 8)
 */

#include "Data/Node.h"

#include "Data/Graph.h"
#include "Data/Cluster.h"

#include "Util/ApplicationConfig.h"

#include <osg/Geometry>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osgText/FadeText>
#include <osg/ShapeDrawable>

#include <QTextStream>

Data::Node::Node( qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position )
{
    //konstruktor
    //scaling je potrebne na zmensenie uzla ak je vnoreny
    this->id = id;
    this->name = name;
    this->type = type;
    this->mIsFocused = false;
    this->mTargetPosition = position;
    this->currentPosition = position * Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();
    this->graph = graph;
    this->inDB = false;
    this->edges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
    this->scale = scaling;
    this->setBall( NULL );
    this->setParentBall( NULL );
    this->hasNestedNodes = false;
    this->cluster = NULL;
    // Duransky start - Pociatocne nastavenie roznych cisiel vertigo rovin pre uzly
    this->numberOfVertigoPlane = id;
    // Duransky end - Pociatocne nastavenie roznych cisiel vertigo rovin pre uzly

    settings = new QMap<QString, QString>();
    //APA

    settings->insert( "Velkost","4242" );
    settings->insert( "Farba","ruzova" );
    //APA

    int pos = 0;
    int cnt = 0;

    labelText = this->name;

    while ( ( pos = labelText.indexOf( QString( " " ), pos + 1 ) ) != -1 ) {
        if ( ++cnt % 3 == 0 ) {
            labelText = labelText.replace( pos, 1, "\n" );
        }
    }

    insertChild( INDEX_SQUARE, createNodeSquare( this->scale, Node::createStateSet( this->type ) ) , !graph->getIs3D() );
    insertChild( INDEX_SPHERE, createNodeSphere( this->scale, Node::createStateSet( this->type ) ), graph->getIs3D() );
    insertChild( INDEX_LABEL, createLabel( this->scale, labelText ) , false );


    this->force = osg::Vec3f();
    this->velocity = osg::Vec3f( 0,0,0 );
    this->fixed = false;
    this->ignore = false;
    this->positionCanBeRestricted = true;
    this->removableByUser = true;
    this->selected = false;
    this->usingInterpolation = true;

    //nastavenie farebneho typu
    float r = type->getSettings()->value( "color.R" ).toFloat();
    float g = type->getSettings()->value( "color.G" ).toFloat();
    float b = type->getSettings()->value( "color.B" ).toFloat();
    float a = type->getSettings()->value( "color.A" ).toFloat();

    this->colorOfNode=osg::Vec4( r, g, b, a );
    this->setDrawableColor( colorOfNode );

    // merging Britvik: this was here
    //setDefaultColor();

    //volovar_zac
    layerID = 0;  //node is not on layer of radial layout
    radialLayout = NULL;  //node does not belong to radial layout
    //volovar_kon

}

Data::Node::~Node( void )
{
    foreach ( qlonglong i, edges->keys() ) {
        edges->value( i )->unlinkNodes();
    }
    edges->clear(); //staci to ?? netreba spravit delete/remove ??

    delete edges;
}

bool Data::Node::isFocused() const
{
    return mIsFocused;
}

void Data::Node::setIsFocused( bool value )
{
    mIsFocused = value;

    if ( mIsFocused ) {
        removeChild( INDEX_SQUARE, 1 );
        this->insertChild( INDEX_SQUARE, createNodeSquare( this->scale, Node::createStateSet( this->type ) ) , !graph->getIs3D() );
        removeChild( INDEX_SPHERE, 1 );
        this->insertChild( INDEX_SPHERE, createNodeSphere( this->scale, Node::createStateSet( this->type ) ) , graph->getIs3D() );
        setDrawableColor( osg::Vec4( 0.5f, 1.0f, 0.0f, 1.0 ) );
    }
    else {
        removeChild( INDEX_SQUARE, 1 );
        this->insertChild( INDEX_SQUARE, createNodeSquare( this->scale, Node::createStateSet( this->type ) ), !graph->getIs3D() );
        removeChild( INDEX_SPHERE, 1 );
        this->insertChild( INDEX_SPHERE, createNodeSphere( this->scale, Node::createStateSet( this->type ) ) , graph->getIs3D() );
        setDrawableColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0 ) );
    }
}
osg::Vec3f Data::Node::getTargetPosition() const
{
    return mTargetPosition;
}
osg::Vec3f Data::Node::targetPosition() const
{
    return mTargetPosition;
}
const osg::Vec3f& Data::Node::targetPositionConstRef() const
{
    return mTargetPosition;
}
void Data::Node::setTargetPosition( const osg::Vec3f& position )
{
    mTargetPosition = position;
}
osg::Vec3f Data::Node::restrictedTargetPosition() const
{
    return mRestrictedTargetPosition;
}
const osg::Vec3f& Data::Node::restrictedTargetPositionConstRef() const
{
    return mRestrictedTargetPosition;
}
void Data::Node::setRestrictedTargetPosition( const osg::Vec3f& position )
{
    mRestrictedTargetPosition = position;
}

void Data::Node::addEdge( osg::ref_ptr<Data::Edge> edge )
{
    //pridanie napojenej hrany na uzol
    edges->insert( edge->getId(), edge );
}

void Data::Node::removeEdge( osg::ref_ptr<Data::Edge> edge )
{
    //odobranie napojenej hrany zo zoznamu
    edges->remove( edge->getId() );
}

Data::Node* Data::Node::getParentNode()
{
    //nadradeny uzol
    return this->nested_parent;
}

void Data::Node::setParentNode( Node* parent )
{
    //nastavenie nadradeneho uzla ktoremu je tento vnoreny
    this->nested_parent = parent;
}

void Data::Node::removeAllEdges()
{
    //odpojenie od vsetkych hran
    foreach ( qlonglong i, edges->keys() ) {
        edges->value( i )->unlinkNodesAndRemoveFromGraph();
    }
    edges->clear();
}

osg::ref_ptr<osg::Geode> Data::Node::createNodeSquare( const float& scaling, osg::StateSet* bbState )
{
    //vytvorenie uzla, scaling urcuje jeho velkost
    float width = scaling;
    float height = scaling;

    osg::ref_ptr<osg::Geometry> nodeQuad = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array( 4 );

    //velkost uzla
    ( *nodeVerts )[0] = osg::Vec3( -width / 2.0f, -height / 2.0f, 0 );
    ( *nodeVerts )[1] = osg::Vec3( width / 2.0f, -height / 2.0f, 0 );
    ( *nodeVerts )[2] = osg::Vec3( width / 2.0f,    height / 2.0f, 0 );
    ( *nodeVerts )[3] = osg::Vec3( -width / 2.0f,  height / 2.0f, 0 );

    nodeQuad->setUseDisplayList( false );

    nodeQuad->setVertexArray( nodeVerts );
    nodeQuad->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS,0,4 ) );

    osg::ref_ptr<osg::Vec2Array> nodeTexCoords = new osg::Vec2Array( 4 );
    //umiestnenie popisu uzla
    ( *nodeTexCoords )[0].set( 0.0f,0.0f );
    ( *nodeTexCoords )[1].set( 1.0f,0.0f );
    ( *nodeTexCoords )[2].set( 1.0f,1.0f );
    ( *nodeTexCoords )[3].set( 0.0f,1.0f );

    nodeQuad->setTexCoordArray( 0, nodeTexCoords );

    osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
    colorArray->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    osg::ref_ptr<ColorIndexArray> colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>;
    colorIndexArray->push_back( 0 );

    nodeQuad->setColorArray( colorArray );
#ifdef BIND_PER_PRIMITIVE
    nodeQuad->setColorIndices( colorIndexArray );
#endif
    nodeQuad->setColorBinding( osg::Geometry::BIND_OVERALL );
    nodeQuad->setStateSet( bbState );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( nodeQuad );

    return geode;
}

osg::ref_ptr<osg::Geode> Data::Node::createNodeSphere( const float& scaling, osg::StateSet* bbState )
{
    //vytvorenie uzla, scaling urcuje jeho velkost
    float radius = scaling / 2.0f;

    osg::ref_ptr<osg::ShapeDrawable> nodeSphere = new osg::ShapeDrawable;
    osg::Sphere* sphere = new osg::Sphere;
//    if (type->isMeta()){
//        sphere->setRadius(this->scale*0.25);
//        (dynamic_cast<osg::ShapeDrawable*>(nodeSphere.get()))->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.5f));
//    }
//    else
    sphere->setRadius( radius );
    nodeSphere->setShape( sphere );

    nodeSphere->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    nodeSphere->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    nodeSphere->getStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
    nodeSphere->getStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    nodeSphere->getStateSet()->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
    nodeSphere->getStateSet()->setRenderBinDetails( 11, "RenderBin" );

    nodeSphere->setUseDisplayList( false );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( nodeSphere );

    return geode;
}

osg::ref_ptr<osg::Geode> Data::Node::createLabel( const float& scale, QString name )
{
    //vytvorenie popisu uzla
    osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
    label->setFadeSpeed( 0.03f );

    // Gloger: added Open Sans font
    QString fontPath = Util::ApplicationConfig::get()->getValue( "Viewer.Labels.Font" );

    // Gloger: added small offset to make label positioning better
    float yOffset = scale / 2 + 0.5f;
    float zOffset = 0.1f;

    // Calculate charSize from scale
    float charSize = (float)log(scale) * 1.8f + 10;

    // Gloger:
    if ( fontPath != NULL && !fontPath.isEmpty() ) {
        label->setFont( fontPath.toStdString() );
    }

    label->setText( name.toStdString() );
    label->setLineSpacing( 0 );
    label->setAxisAlignment( osgText::Text::SCREEN );
    label->setCharacterSize( charSize );
    label->setDrawMode( osgText::Text::TEXT );
    label->setAlignment( osgText::Text::CENTER_BOTTOM_BASE_LINE );
    label->setPosition( osg::Vec3( 0, yOffset, zOffset ) );
    label->setColor( Util::ApplicationConfig::get()->getColorValue("Label.Color") );
    label->setUseDisplayList( false );
//    label->setBackdropType(osgText::Text::OUTLINE);
//    label->setBackdropColor(Util::ApplicationConfig::get()->getColorValue( "Label.Outline.Color" ));
//    label->setBackdropOffset(0.05f);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( label );

    return geode;
}

osg::ref_ptr<osg::StateSet> Data::Node::createStateSet( Data::Type* type )
{
    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;

    stateSet->setDataVariance( osg::Object::DYNAMIC );
    stateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    if ( type != 0 ) {
        stateSet->setTextureAttributeAndModes( 0, type->getTypeTexture(), osg::StateAttribute::ON );
    }

    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    osg::ref_ptr<osg::Depth> depth = new osg::Depth;
    depth->setWriteMask( false );
    stateSet->setAttributeAndModes( depth, osg::StateAttribute::ON );

    osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
    cull->setMode( osg::CullFace::BACK );
    stateSet->setAttributeAndModes( cull, osg::StateAttribute::ON );

    return stateSet;
}

bool Data::Node::equals( Node* node )
{
    //porovnanie s inym uzlom
    if ( this == node ) {
        return true;
    }
    if ( node == NULL ) {
        return false;
    }
    if ( id != node->getId() ) {
        return false;
    }
    if ( ( graph==NULL && node->getGraph()!=NULL ) || ( graph!=NULL && node->getGraph()==NULL ) ) {
        return false;
    }
    if ( graph==NULL && node->getGraph()==NULL ) {
        return true;
    }
    if ( graph!=NULL && graph->getId() != node->getGraph()->getId() ) {
        return false;
    }
    return true;
}

void Data::Node::setDrawableColor( osg::Vec4 color )
{
    //nastavenie farby uzla
    osg::Geometry* geometry  = getChild( INDEX_SQUARE )->asGeode()->getDrawable( 0 )->asGeometry();

    if ( geometry != NULL ) {
        osg::Vec4Array* colorArray =  dynamic_cast<osg::Vec4Array*>( geometry->getColorArray() );

        colorArray->pop_back();
        colorArray->push_back( color );
    }

    ( dynamic_cast<osg::ShapeDrawable*>( getChild( INDEX_SPHERE )->asGeode()->getDrawable( 0 ) ) )->setColor( color );
}

void Data::Node::showLabel( bool visible )
{
    setValue( INDEX_LABEL, visible );
}

void Data::Node::reloadConfig()
{
    removeChildren( 0, 3 );
    this->insertChild( INDEX_SQUARE, createNodeSquare( this->scale, Node::createStateSet( this->type ) ), !graph->getIs3D() );
    this->insertChild( INDEX_SPHERE, createNodeSphere( this->scale, Node::createStateSet( this->type ) ), graph->getIs3D() );
    this->insertChild( INDEX_LABEL, createLabel( this->scale, labelText ) , true );
    setSelected( selected );
}

osg::Vec3f Data::Node::getCurrentPosition( bool calculateNew, float interpolationSpeed )
{
    //zisime aktualnu poziciu uzla v danom okamihu
    if ( calculateNew ) {
        float graphScale = Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

        //osg::Vec3 directionVector = osg::Vec3(targetPosition.x(), targetPosition.y(), targetPosition.z()) * graphScale - currentPosition;
        osg::Vec3 directionVector = osg::Vec3( mRestrictedTargetPosition.x(), mRestrictedTargetPosition.y(), mRestrictedTargetPosition.z() ) * graphScale - currentPosition;
        this->currentPosition = osg::Vec3( directionVector * ( usingInterpolation ? interpolationSpeed : 1 ) + this->currentPosition );
    }

    return osg::Vec3( this->currentPosition );
}

QSet<Data::Node*> Data::Node::getIncidentNodes() const
{
    QSet<Node*> nodes;

    QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i;
    for ( i = edges->begin(); i != edges->end(); ++i ) {
        osg::ref_ptr<Data::Edge> edge = i.value();
        nodes.insert( edge->getOtherNode( this ) );
    }
    return nodes;
    /*
        if (ignoreClusters) {
            return nodes;
        } else {
            QSet<Node*> visibleNodes;
            QSetIterator<Node*> nodeIt(nodes);
            while (nodeIt.hasNext()) {
                Node* node = nodeIt.next();
                Node* cluster = node->getTopCluster();
                if (cluster != NULL && cluster != this) {
                    visibleNodes.insert(cluster);
                } else {
                    visibleNodes.insert(node);
                }
            }
            return visibleNodes;
        }
        */
}

void Data::Node::setDefaultColor()
{
    float r = type->getSettings()->value( "color.R" ).toFloat();
    float g = type->getSettings()->value( "color.G" ).toFloat();
    float b = type->getSettings()->value( "color.B" ).toFloat();
    float a = type->getSettings()->value( "color.A" ).toFloat();

    this->setDrawableColor( osg::Vec4( r, g, b, a ) );
}

void Data::Node::set3D( bool value )
{
    setValue( INDEX_SQUARE, !value );
    setValue( INDEX_SPHERE, value );
}

// Duransky start - Funkcie na nastavenie a ziskanie cisla vertigo roviny, na ktorej sa uzol nachadza
void Data::Node::setNumberOfVertigoPlane( qlonglong value )
{
    this->numberOfVertigoPlane = value;
}

qlonglong Data::Node::getNumberOfVertigoPlane()
{
    return this->numberOfVertigoPlane;
}
// Duransky end - Funkcie na nastavenie a ziskanie cisla vertigo roviny, na ktorej salabel uzol nachadza

QString Data::Node::toString() const
{
    QString str;
    QTextStream( &str ) << "node id:" << id << " name:" << name << " pos:[" << mTargetPosition.x() << "," << mTargetPosition.y() << "," << mTargetPosition.z() << "]";
    return str;
}

Data::Cluster* Data::Node::getCluster() const
{
    return cluster;
}

void Data::Node::setCluster( Data::Cluster* cluster )
{
    this->cluster = cluster;
}


//volovar_zac
//pre potreby odpudivych sil v radialLayoute
void Data::Node::setLayerID( int id )
{
    layerID = id;
}

int Data::Node::getLayerID()
{
    return layerID;
}

Layout::RadialLayout* Data::Node::getRadialLayout()
{
    return radialLayout;
}
void Data::Node::setRadialLayout( Layout::RadialLayout* rl )
{
    radialLayout = rl;
}

//volovar_kon
