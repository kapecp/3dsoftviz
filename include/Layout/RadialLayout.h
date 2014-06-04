#ifndef LAYOUT_RADIAL_LAYUOT_H
#define LAYOUT_RADIAL_LAYUOT_H

#include <osg/Vec3f>

namespace Layout
{
    class RadialLayout
    {
    public:
        RadialLayout(Data::Graph * currentGraph, QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes, float size, osg::ref_ptr<Data::Node> centerNode, osg::Vec3f centerPosition);
        ~RadialLayout();
        void changeSize(float size);
        void select(); //select this radial layout
        void unselect(); //unselect this radial layout
        static RadialLayout* getSelectedRadialLayout(); //pointer to selected radial layout
        void changeCenterPosition(osg::Vec3f centerPosition);
        void changeCenterNode(osg::ref_ptr<Data::Node> centerNode);

        osg::Vec3f getCenterPosition();
        osg::ref_ptr<Data::Node> getCenterNode();
        qlonglong getSpeheCount();

        void setAlpha(float alpha);
        float getAlpha();
        void setVisibleSpheres(float visibleSpheres); //change number of visible spheres, argument real number between 0 - 1,  0 no visibles spheres, 1 - all spheres are visible
        float getVisibleSpheres();

        void unselectNodes(); //unselect all nodes
        void unselectNodes(QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes); //unselect nodes from argumets

        //nastavi faktor, ktorym sa znasobi odpudiva sila, na uzloch, ktore nie su na rovnakej guli
        void setForceScale(float forceScale);
        float getForceScale();

        //nastavi faktor, ktorym sa znasobi odpudiva sila, na uzloch, ktore su na rovnakej guli
        void setForceSphereScale(float forceSphereScale);
        float getForceSphereScale();

        void setRenderType(Shape_AbstractSphere::RenderType RenderType);
        Shape_AbstractSphere::RenderType getRenderType();
        void changeRenderType();

        void changeMode(); //switch between 2D and 3D mode

    private:
        void update();
        osg::Vec3f mCenterPosition;  //position of central node
        qlonglong mSphereCount;
        float mSize;  //size of radial layout
        osg::ref_ptr<Data::Node> mCenterNode; //pointer to center node
        QSet<Data::Node *> mSelectedNodes; //set of pointers to selected nodes
        static RadialLayout* selectedRadialLayout;
        static int lastLayerID; //need for generating unique sphere id
        float mAlpha; //alpha channel
        float mVisibleSpheres; //some spheres can be set as invisible, real number between
        Data::Graph * mCurrentGraph; //pointer to graph
        bool m2D; //radial is in plane or 3d
        osg::ref_ptr<Data::Node> mNode1;  //restriction node 1 for plane
        osg::ref_ptr<Data::Node> mNode2;   //restriction node 2 for plane
        osg::ref_ptr<Data::Node> mNode3; //restriction node 3 for plane
        float mForceScale; //multiply repulsive forces between nodes which are not in same layer
        float mForceSphereScale; //multiply repulsive forces between nodes which are on same layer
        Shape_AbstractSphere::RenderType mRenderType; //render type (WIREFRAME, SOLID)
    };
}


#endif
