#include "Layout/RadialLayout.h"
#include "Data/GraphSpanningTree.h"
#include "Layout/ShapeGetter_Sphere_AroundNode.h"
#include "Layout/RestrictionsManager.h"
#include "Data/Graph.h"
#include "QDebug"

RadialLayout* RadialLayout::selectedRadialLayout = NULL;
int RadialLayout::lastSphereID = 0;

void RadialLayout::changeSize(float size)
{
    RadialLayout::mSize = size;
    update();
}

void RadialLayout::select()
{
    selectedRadialLayout = this;
}

void RadialLayout::unselect()
{
    if (selectedRadialLayout == this)
        selectedRadialLayout = NULL;
}

RadialLayout::~RadialLayout()
{
    unselect();
}


RadialLayout* RadialLayout::getSelectedRadialLayout()
{
    return selectedRadialLayout;
}

void RadialLayout::changeCenterPosition(osg::Vec3f centerPosition)
{
    mCenterPosition = centerPosition;
    update();
}

void RadialLayout::changeCenterNode(osg::ref_ptr<Data::Node> centerNode)
{
    mCenterNode = centerNode;
    update();
}

void RadialLayout::update()
{
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = mCurrentGraph->getNodes();
    Data::GraphSpanningTree* spanningTree = mCurrentGraph->getSpanningTree(mCenterNode->getId());

    int maxDepth = spanningTree->getMaxDepth();
    if (maxDepth == 0)
        return;

    mSphereCount = maxDepth;
    osg::ref_ptr<Data::Node> node;
    float radius = 0;
    float dR = mSize/float(maxDepth);
    osg::Vec4 color(0.0, 0.0, 0.0, 1.0);
    osg::Vec4 d_color((float) (1.0/maxDepth), (float) (1.0/maxDepth), 0.0, 1.0);

    QList<qlonglong> groups;
    QList<qlonglong>::iterator groupIt;
    QList<qlonglong> nodes;
    QList<qlonglong>::iterator nodeIt;

    for (int depth = 1; depth <= maxDepth; depth++)
    {
        radius += dR;
        groups = spanningTree->getGroupsInDepth(depth);
        QSet<Data::Node*>  pickedNodes;
        lastSphereID++; //because every sphere need id, cause repulsive forces
        for(groupIt=groups.begin(); groupIt!=groups.end();groupIt++)  //groups are nodes with same parent
        {
            //qDebug()<<"groupIt";
            nodes = spanningTree->getNodesInGroup(*groupIt);
            for(nodeIt=nodes.begin(); nodeIt!=nodes.end();nodeIt++)
            {
                node = allNodes->value(*nodeIt);

                //qDebug()<<"node: "<<node<<"ID: "<<lastSphereID;
                if (mSelectedNodes.contains(node))
                {
                    node->setColor(color);
                    node->setSphereID(lastSphereID);
                    pickedNodes.insert(node);
                    //qDebug()<<"Picked node: "<< node;
                }
            }
        }
        Layout::ShapeGetter *shapeGet= new Layout::ShapeGetter_Sphere_AroundNode(mCenterNode, radius, Shape_Sphere::SURFACE, ShapeGetter_Sphere_AroundNode::NODE_TARGET_POSITION);
        QSharedPointer <Layout::ShapeGetter> shapeGetter(shapeGet);
        QSharedPointer<Layout::Shape> shape = shapeGet->getShape();
        QSharedPointer<Layout::Shape_Sphere> sphere = shape.dynamicCast<Shape_Sphere>();
        if (sphere != NULL)
        {
            sphere->setRenderType(Shape_AbstractSphere::WIREFRAME);
            sphere->setInvisible(0);
            sphere->setAlpha(mAlpha);
        }
        mCurrentGraph->getRestrictionsManager().setRestrictions(pickedNodes, shapeGetter);
        pickedNodes.clear();
        groups.clear();
        color += d_color;
        //qDebug()<<"Depth: "<< depth;
    }


}

osg::Vec3f RadialLayout::getCenterPosition()
{
    return mCenterPosition;
}

osg::ref_ptr<Data::Node> RadialLayout::getCenterNode()
{
    return mCenterNode;
}

qlonglong RadialLayout::getSpeheCount()
{
    return mSphereCount;
}

RadialLayout::RadialLayout(Data::Graph * currentGraph, QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, float size, osg::ref_ptr<Data::Node> centerNode, osg::Vec3f centerPosition)
{
    for (QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin (); it != selectedNodes->constEnd (); ++it)
    {
        mSelectedNodes.insert (it->get ());
        //qDebug()<<"iserting: "<<it->get();
    }
    mSize = size;
    mCenterNode = centerNode;
    mCenterPosition = centerPosition;
    mCurrentGraph = currentGraph;
    update();
}

void RadialLayout::setAlpha(float alpha)
{
    mAlpha = alpha;
    update();
}

float RadialLayout::getAlpha()
{
    return mAlpha;
}
