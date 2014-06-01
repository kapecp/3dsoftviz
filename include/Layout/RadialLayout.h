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
        void select();
        void unselect();
        static RadialLayout* getSelectedRadialLayout();
        void changeCenterPosition(osg::Vec3f centerPosition);
        void changeCenterNode(osg::ref_ptr<Data::Node> centerNode);
        osg::Vec3f getCenterPosition();
        osg::ref_ptr<Data::Node> getCenterNode();
        qlonglong getSpeheCount();
        void setAlpha(float alpha);
        float getAlpha();
        void setVisibleSpheres(float visibleSpheres);
        float getVisibleSpheres();

        void unselectNodes();
        void unselectNodes(QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes);

        //nastavi faktor, ktorym sa znasobi odpudiva sila, na uzloch, ktore nie su na rovnakej guli
        void setForceScale(float forceScale);
        float getForceScale();

        //nastavi faktor, ktorym sa znasobi odpudiva sila, na uzloch, ktore su na rovnakej guli
        void setForceSphereScale(float forceSphereScale);
        float getForceSphereScale();

        void setRenderType(Shape_AbstractSphere::RenderType RenderType);
        Shape_AbstractSphere::RenderType getRenderType();
        void changeRenderType();

        void changeMode(); //switch between 2D and 3D

    private:
        void update();
        osg::Vec3f mCenterPosition;
        qlonglong mSphereCount;
        float mSize;
        osg::ref_ptr<Data::Node> mCenterNode;
        QSet<Data::Node *> mSelectedNodes;
        static RadialLayout* selectedRadialLayout;
        static int lastLayerID; //need for generating unique sphere id
        float mAlpha;
        float mVisibleSpheres; //some spheres can be set as invisible, real number in interval 0,1
        Data::Graph * mCurrentGraph;
        bool m2D; //radial is in plane or 3d
        osg::ref_ptr<Data::Node> mNode1;  //restriction node 1 for plane
        osg::ref_ptr<Data::Node> mNode2;   //restriction node 2 for plane
        osg::ref_ptr<Data::Node> mNode3;
        float mForceScale;
        float mForceSphereScale;
        Shape_AbstractSphere::RenderType mRenderType;
    };
}


#endif
