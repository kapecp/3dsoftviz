#pragma once

#include <QList>
#include <osg/BoundingBox>
#include <QRectF>
#include <exception>

class LayoutAlgorithmException final : public std::exception
{
private:
	std::string errMsg;

public:
	explicit LayoutAlgorithmException( const std::string& errorMsg );
	const char* what() const throw();
};

namespace Layout {
struct ElementLayout {
	osg::Vec3 position;
	double yawRotation;
};

class LayoutAlgorithms
{
public:
	/**
	*  \fn layoutInsideRegion
	*  \brief Creates positions for required count of elements grouped near together to the most possible efective rectangle
	*  \param elementCount required count of elements to be layouted
	*  \param groundTopPosition top position of ground for elements to be placed
	*  \param spacing space among elements
	*  \return layouts positions for count elements
	*  \return aroundRegion region dimension to cover all elements with outside spacing
	*/
	static void layoutInsideRegion( const osg::BoundingBox& elementDimension, int elementCount, float groundTopPosition, float spacing, QList<osg::Vec3>* layouts, osg::BoundingBox* aroundRegion = nullptr );

	/**
	*  \fn layoutAroundRegion
	*  \brief Creates positions for required count of elements distributed around defined region to the most possible efective rectangle
	*  \param elementCount required count of elements to be layouted
	*  \param region dimension of the region around which the elements will be layouted
	*  \param spacing space among elements
	*  \return layouts positions for count elements
	*  \return aroundRegion region dimension to cover all elements with outside spacing
	*/
	static void layoutAroundRegion( const osg::BoundingBox& elementDimension, int elementCount, const osg::BoundingBox& region, float spacing, QList<ElementLayout>* layouts, osg::BoundingBox* aroundRegion = nullptr );
};
}
