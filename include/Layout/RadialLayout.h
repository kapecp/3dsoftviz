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
    private:
        void update();
        osg::Vec3f mCenterPosition;
        qlonglong mSphereCount;
        float mSize;
        osg::ref_ptr<Data::Node> mCenterNode;
        QSet<Data::Node *> mSelectedNodes;
        static RadialLayout* selectedRadialLayout;
        static int lastSphereID; //need for generating unique sphere id
        float mAlpha;
        Data::Graph * mCurrentGraph;
    };
}


#endif
