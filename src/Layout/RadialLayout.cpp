#include "Layout/RadialLayout.h"
#include "Data/GraphSpanningTree.h"
#include "Layout/ShapeGetter_Sphere_AroundNode.h"
#include "Layout/ShapeGetter_Circle_ByThreeNodes.h"
#include "Layout/RestrictionsManager.h"
#include "Data/Graph.h"
#include "Data/Node.h"
#include "QDebug"

RadialLayout* RadialLayout::selectedRadialLayout = NULL;
int RadialLayout::lastLayerID = 0;

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
    unselectNodes();
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

    //return, when inputs are incorrect
    if (mSelectedNodes.empty())
           return;
    if (mCenterNode == NULL)
        return;

    Data::GraphSpanningTree* spanningTree = mCurrentGraph->getSpanningTree(mCenterNode->getId());

    int maxDepth = spanningTree->getMaxDepth();
    if (maxDepth == 0)
        return;

    mSphereCount = maxDepth;
    osg::ref_ptr<Data::Node> node;
    float radius = 0;
    osg::Vec4 color(0.0, 0.0, 0.0, 1.0);
    osg::Vec4 d_color((float) (1.0/maxDepth), (float) (1.0/maxDepth), 0.0, 1.0);

    QList<qlonglong> groups;
    QList<qlonglong>::iterator groupIt;
    QList<qlonglong> nodes;
    QList<qlonglong>::iterator nodeIt;

    //mCenterPosition = mCenterNode->getTargetPosition();  //because circle constrain
    mCenterNode->setFixed(true);     //don't move with radial layout

    for (int depth = 0; depth <= maxDepth; depth++)
    {
        radius = (float) depth*mSize/ (float) maxDepth;  //radius of sphere or circle
        groups = spanningTree->getGroupsInDepth(depth);
        QSet<Data::Node*>  pickedNodes; //nodes in same depth
        lastLayerID++; //because every layer need id, cause repulsive forces
        for(groupIt=groups.begin(); groupIt!=groups.end();groupIt++)  //groups are nodes with same parent
        {
            nodes = spanningTree->getNodesInGroup(*groupIt);
            for(nodeIt=nodes.begin(); nodeIt!=nodes.end();nodeIt++)
            {
                node = allNodes->value(*nodeIt);

                if (mSelectedNodes.contains(node))
                {
                    node->setColor(color);
                    node->setLayerID(lastLayerID); //for repulsive forces
                    node->setRadialLayout((RadialLayout*) this);
                    pickedNodes.insert(node);
                }
            }
        }
        Layout::ShapeGetter*  sphereSurfaceGetter;
        if (m2D == false) //sphere surface constrain, if mode is 3D
             sphereSurfaceGetter = new Layout::ShapeGetter_Sphere_AroundNode(mCenterNode, radius, Shape_Sphere::SURFACE, ShapeGetter_Sphere_AroundNode::NODE_TARGET_POSITION);
        else
        {
            //circle constrain, if mode is 2D
            osg::Vec3f positionNode1 = mCenterPosition;
            osg::Vec3f positionNode2 = mCenterPosition + osg::Vec3f (radius, 0.f, 0.f);
            osg::Vec3f positionNode3 = mCenterPosition + osg::Vec3f ( 0.f, radius, 0.f);

            mNode1 = mCurrentGraph->addRestrictionNode ("plane_node_1", positionNode1);
            mNode2 = mCurrentGraph->addRestrictionNode ("plane_node_2", positionNode2);
            mNode3 = mCurrentGraph->addRestrictionNode ("plane_node_2", positionNode3);

            mNode1->setInvisible();
            mNode2->setInvisible();
            mNode3->setInvisible();

            sphereSurfaceGetter = new Layout::ShapeGetter_Circle_ByThreeNodes(mNode1, mNode2, mNode3);
        }

        QSharedPointer <Layout::ShapeGetter> shapeGetter(sphereSurfaceGetter);
        QSharedPointer<Layout::Shape> shape = sphereSurfaceGetter->getShape();
        QSharedPointer<Layout::Shape_Sphere> sphere = shape.dynamicCast<Shape_Sphere>();
        QSharedPointer<Layout::Shape_Intersection> circle = shape.dynamicCast<Shape_Intersection>();
        if (sphere != NULL)
        {
            sphere->setRenderType(mRenderType);
            if ((float)depth/(float)maxDepth <= mVisibleSpheres and mVisibleSpheres > 0.0)
                sphere->setAlpha(mAlpha);
            else
                sphere->setAlpha(0);
        }
        if (circle != NULL)
        {
            circle->setRenderType(mRenderType);
            if ((float)depth/(float)maxDepth <= mVisibleSpheres and mVisibleSpheres > 0.0)
                circle->setAlpha(mAlpha);
            else
                circle->setAlpha(0);
        }
        mCurrentGraph->getRestrictionsManager().setRestrictions(pickedNodes, shapeGetter); //apply restriction
        pickedNodes.clear();
        groups.clear();
        color += d_color;
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
        //qDebug()<<"inserting: "<<it->get();
    }
    mSize = size;
    mCenterNode = centerNode;
    mCenterPosition = centerPosition;
    mCurrentGraph = currentGraph;
    mVisibleSpheres = 1.0;
    m2D = false;
    mForceScale = 1000.0;
    mForceSphereScale = 1000.0;
    mAlpha = 0.04f;
    mRenderType = Shape_AbstractSphere::WIREFRAME;
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

void RadialLayout::setVisibleSpheres(float visibleSpheres)
{
    mVisibleSpheres = visibleSpheres;
    update();
}

float RadialLayout::getVisibleSpheres()
{
    return mVisibleSpheres;
}

void RadialLayout::unselectNodes(QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes)
{
    osg::Vec4 color(1.0, 1.0, 1.0, 1.0);
    osg::ref_ptr<Data::Node> node;
    for (QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin (); it != selectedNodes->constEnd (); ++it)
    {
        QSet<Data::Node *>::iterator sit = qFind(mSelectedNodes.begin(), mSelectedNodes.end(), it->get());
        if (sit != mSelectedNodes.constEnd())
        {
            node = it->get();
            node->setColor(color);
            node->setLayerID(0);
            node->setRadialLayout(NULL);
            if (node == mCenterNode)
                node->setFixed(false);
            mSelectedNodes.erase(sit);
        }

    }
}


void RadialLayout::unselectNodes() //unselect all nodes
{
    osg::Vec4 color(1.0, 1.0, 1.0, 1.0);
    osg::ref_ptr<Data::Node> node;
    for (QSet<Data::Node *>::iterator it = mSelectedNodes.begin(); it != mSelectedNodes.end(); it = mSelectedNodes.erase(it))
    {
        node = *it;
        node->setColor(color);
        node->setLayerID(0);
        node->setRadialLayout(NULL);
    }
    if (mCenterNode != NULL)
        mCenterNode->setFixed(false);

}

void RadialLayout::setForceScale(float forceScale)
{
    mForceScale = forceScale;
}

float RadialLayout::getForceScale()
{
    return mForceScale;
}

void RadialLayout::setForceSphereScale(float forceSphereScale)
{
   mForceSphereScale = forceSphereScale;
}

float RadialLayout::getForceSphereScale()
{
    return mForceSphereScale;
}

void RadialLayout::setRenderType(Shape_AbstractSphere::RenderType RenderType)
{
    mRenderType = RenderType;
    update();
}

Shape_AbstractSphere::RenderType RadialLayout::getRenderType()
{
    return mRenderType;
}

void RadialLayout::changeRenderType()
{
    switch(mRenderType)
    {
    case Shape_AbstractSphere::WIREFRAME:
        mRenderType = Shape_AbstractSphere::SOLID;
        break;
    case Shape_AbstractSphere::SOLID:
        mRenderType = Shape_AbstractSphere::WIREFRAME;
        break;
    case Shape_AbstractSphere::CIRCLE:  //not implemented yet
        break;
    default:
            break;
    }
    update();
}

void RadialLayout::changeMode()
{
    m2D = !m2D;
    update();
}
