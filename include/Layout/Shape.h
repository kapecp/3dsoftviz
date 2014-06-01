#ifndef LAYOUT_SHAPE_H
#define LAYOUT_SHAPE_H

namespace Layout {

class ShapeVisitor;

/**
 * Base class of all shapes.
 */
class Shape
{
public:
	virtual ~Shape() {}

	/**
	 * [visitor pattern]
	 */
	virtual void accept(ShapeVisitor &visitor) = 0;

	void setInvisible(bool invisible){this->invisible = invisible;}

	bool isInvisible(){return invisible;}

    //volovar zac, kvoli radial layoutu na zvysenie citatelnosti, v triede abstract sphere, kvoli tomu, ze shapeVisitor_visualizerCreator berie tento typ
    enum RenderType {
        WIREFRAME,
        SOLID,
        CIRCLE
    };
    void setAlpha(float alpha) {alpha_ = alpha;}
    float getAlpha() {return alpha_;}
    void setRenderType(RenderType rt) {renderType_ = rt;}
    RenderType getRenderType() {return renderType_;}
    //volovar_kon

private:
	bool invisible;

    //volovar zac
    RenderType renderType_;
    float alpha_;
    //volovar kon

}; // class

} // namespace

#endif // LAYOUT_SHAPE_H
