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
	virtual void accept( ShapeVisitor& visitor ) = 0;

	void setInvisible( bool invisible )
	{
		this->invisible = invisible;
	}

	bool isInvisible()
	{
		return invisible;
	}

	//volovar zac, kvoli radial layoutu na manipuláciu zobrazenia shapu
	enum RenderType {
		WIREFRAME,
		SOLID,
		CIRCLE
	};
	void setAlpha( float alpha )
	{
		alpha_ = alpha;   //kvoli zobrazeniu priehladnosti
	}
	float getAlpha()
	{
		return alpha_;
	}
	void setRenderType( RenderType rt )
	{
		renderType_ = rt;   //zmena modu vykreslenia (droteny model, plny)
	}
	RenderType getRenderType()
	{
		return renderType_;
	}
	//volovar_kon


private:
	bool invisible;
	RenderType renderType_;
	float alpha_;

}; // class

} // namespace

#endif // LAYOUT_SHAPE_H
