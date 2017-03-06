#include "Layout/LayoutAlgorithms.h"
#include <exception>
#include <QVector>

LayoutAlgorithmException::LayoutAlgorithmException( const std::string& errorMsg )
	: errMsg( errorMsg )
{
}

const char* LayoutAlgorithmException::what() const throw()
{
	return errMsg.c_str();
}

namespace Layout {
void LayoutAlgorithms::layoutInsideRegion( const osg::BoundingBox& elementDimension, int elementCount, float groundTopPosition, float spacing, QList<osg::Vec3>* layouts, osg::BoundingBox* aroundRegion )
{
	layouts->clear();
	const float width = elementDimension.xMax() - elementDimension.xMin();
	const float depth = elementDimension.yMax() - elementDimension.yMin();
	const float offsetX = width + spacing;
	const float offsetY = depth + spacing;
	const float elementConstOffsetX = -elementDimension.xMin();
	const float elementConstOffsetY = -elementDimension.yMin();
	const float elementConstOffsetZ = -elementDimension.zMin() + groundTopPosition;
	float xMax;
	float yMax;
	if ( elementCount > 0 ) {
		xMax = -FLT_MAX;
		yMax = -FLT_MAX;
		for ( int i = 0; i < elementCount; ++i ) {
			uint xNum = 0;
			uint yNum = 0;
			osg::Vec3 pos( static_cast<float>( xNum ) * offsetX + elementConstOffsetX, static_cast<float>( yNum ) * offsetY + elementConstOffsetY, elementConstOffsetZ );
			if ( xMax < pos.x() ) {
				xMax = pos.x();
			}
			if ( yMax < pos.y() ) {
				yMax = pos.y();
			}
			*layouts << pos;
			uint limit = 1;
			if ( xNum == limit ) {
				if ( yNum == limit ) {
					limit++;
					yNum = 0;
					xNum = limit;
				}
				else {
					yNum++;
					xNum = ( yNum == limit ) ? 0 : limit;
				}
			}
			else {
				xNum++;
			}
		}
		xMax += width / 2;
		yMax += depth / 2;
		const float centerCorrectionOffsetX = -xMax / 2;
		const float centerCorrectionOffsetY = -yMax / 2;
		for ( auto& l : *layouts ) {
			l.x() += centerCorrectionOffsetX;
			l.y() += centerCorrectionOffsetY;
		}
	}
	else {
		xMax = 0;
		yMax = 0;
	}

	if ( aroundRegion ) {
		const float regionX = xMax / 2 + spacing;
		const float regionY = yMax / 2 + spacing;
		*aroundRegion = osg::BoundingBox( -regionX, -regionY, 0, regionX, regionY, 0 );
	}
}

struct Element {
	Element( const osg::BoundingBox& boundingBox, int index )
	{
		this->boundingBox = boundingBox;
		this->index = index;
	}
	osg::BoundingBox boundingBox;
	int index;
	ElementLayout layout;
};

struct Edge {
	float maxEdgeSize;
	float beginAlongPos;
	QList<Element> elements;
	int alongOffsetCoef;
	bool full;

	Edge()
	{
		this->maxEdgeSize = 0;
		this->full = false;
		this->beginAlongPos = 0;
		this->alongOffsetCoef = 0;
	}
};

struct IndentEdges : public QVector<Edge> {
	explicit IndentEdges( int count ) : QVector<Edge>( count ) {}
};

void LayoutAlgorithms::layoutAroundRegion( const osg::BoundingBox& elementDimension, int elementCount, const osg::BoundingBox& region, float spacing, QList<ElementLayout>* layouts, osg::BoundingBox* aroundRegion )
{
	static const int EDGES_COUNT = 4;

	// rozmery regionu (oblast okolo ktorej sa umiestnuje)
	const float regionWidth = region.xMax() - region.xMin();
	const float regionHalfWidth = regionWidth / 2;
	const float regionDepth = region.yMax() - region.yMin();
	const float regionHalfDepth = regionDepth / 2;

	// rozmery elementu (kvader ktory je umiestnovany)
	const float elementWidth = elementDimension.xMax() - elementDimension.xMin();
	const float elementHalfWidth = elementWidth / 2;
	const float elementDepth = elementDimension.yMax() - elementDimension.yMin();
	const float elementHalfDepth = elementDepth / 2;

	// posuny
	const float elementAlongOffset = elementWidth + spacing;
	const float elementIndentOffset = elementDepth + spacing;
	const float elementVerticalOffset = -( region.zMin() + elementDimension.zMin() );
	const float origRegionValuesForEdges[] = { region.yMin(), region.xMax(), region.yMax(), region.xMin() };
	const float offsetX = region.center().x();
	const float offsetY = region.center().y();

	if ( regionWidth < elementWidth || regionDepth < elementDepth ) {
		throw LayoutAlgorithmException( std::string( "Region is too small for layouting even one element" ) );
	}

	float curWidth = regionWidth + ( regionWidth <= regionDepth ? elementIndentOffset * 2 : 0 );
	float curDepth = regionDepth + ( regionWidth > regionDepth ? elementIndentOffset * 2 : 0 );
	QList<IndentEdges> indents;
	IndentEdges* prevIndent = nullptr;
	int remainedElementsCount = elementCount;
	int elementIndex = 0;

	// analyza - na ktorej stene bude kolko elementov
	while ( remainedElementsCount > 0 ) {
		indents << IndentEdges( EDGES_COUNT ); // nove odsadenie od regionu (jedno obkolesenie kvadrov - 4 strany)
		IndentEdges* curIndent = nullptr;
		curIndent = &indents.last();
		const int maxCountOnWidth = static_cast<int>( floorf( ( curWidth + spacing ) / ( elementWidth + spacing ) ) ); // maximalny pocet elementov na sirku a hlbku
		const int maxCountOnDepth = static_cast<int>( floorf( ( curDepth + spacing ) / ( elementWidth + spacing ) ) );
		const int maxCountOnIndent = ( maxCountOnWidth + maxCountOnDepth ) * 2; // maximalny pocet elementov celkovo okolo regionu pre dane odsadenie
		const float fillCoef = 0.5f; // threshold podla ktoreho sa rozhoduje ktory algoritmus sa pouzije
		const int elementsCountToAddForIndent = std::min( remainedElementsCount, maxCountOnIndent ); // nesmie sa umiestnit viac ako sa zmesti a zaroven viac kolko ostava
		remainedElementsCount -= elementsCountToAddForIndent;
		if ( static_cast<float>( elementsCountToAddForIndent ) < static_cast<float>( maxCountOnIndent ) * fillCoef ) { // zaplna po stranach (spusta sa len raz pre posledne odsadenie) - najprv zaplni near, potom right, potom ...
			int remainedElementsCountToAddForIndent = elementsCountToAddForIndent; // pocitadlo ostavajucich elementov
			const int firstEdgeOffset = curWidth < curDepth ? 0 : 1;
			int edgeIndex = firstEdgeOffset; // index strany, near = 0, right = 1, far = 2, left = 3
			while ( remainedElementsCountToAddForIndent > 0 ) { // ak este ostavaju nejake elementy na rozmiestnenie pre odsadenie
				auto& edge = ( *curIndent )[edgeIndex]; // vypocet informacii pre rozmiestnovanie na danej hrane
				const int coefForAlong = ( edgeIndex / 2 ) % 2 == 0 ? -1 : 1; // znamienko pre posun popri hrane (ci sa ma hodnota pridavat alebo odoberat)
				const float defaultMaxEdgeSize = edgeIndex % 2 == 0 ? curWidth : curDepth; // max dlzka popri hrane
				const float defaultBeginAlongPos = ( defaultMaxEdgeSize / 2 - elementHalfWidth ) * static_cast<float>( coefForAlong ); // zaciatok umiestovania na hrane
				const float maxEdgeResizer = ( edgeIndex - firstEdgeOffset ) == 1 ? -elementAlongOffset : ( ( edgeIndex - firstEdgeOffset ) == 2 ? elementAlongOffset : 0 );
				const float beginAlongPosOffset = ( edgeIndex - firstEdgeOffset ) == 2 ? elementAlongOffset : 0;
				edge.alongOffsetCoef = coefForAlong;
				edge.maxEdgeSize = defaultMaxEdgeSize + maxEdgeResizer;
				edge.beginAlongPos = defaultBeginAlongPos + beginAlongPosOffset;
				const int maxCountOnCurrentEdge = static_cast<int>( floorf( ( edge.maxEdgeSize + spacing ) / ( elementWidth + spacing ) ) ); // maximalny pocet elementov na hrane
				const int elemenstCountOnEdge = std::min( maxCountOnCurrentEdge, remainedElementsCountToAddForIndent ); // vezme sa bud max pocet alebo zostavajuci - ten mensi
				edge.full = elemenstCountOnEdge == maxCountOnCurrentEdge; // ak sa vzalo tolko elementov kolko sa zmesti na hranu - oznaci sa ako full zaplnena
				for ( int i = 0; i < elemenstCountOnEdge; ++i ) {
					edge.elements << Element( elementDimension, elementIndex++ );
				}

				remainedElementsCountToAddForIndent -= elemenstCountOnEdge;
				edgeIndex = ( edgeIndex + 1 ) % EDGES_COUNT;
			}
		}
		else { // zaplna rovnomerne dokola - distribuuje elementy tak aby bolo na vsetkych stranach rovnako
			// pocitaju sa pomery, na sirsiu stranu ich pojde viac
			const float tmpBase = curWidth + curDepth;
			const float widthRatio = curWidth / tmpBase;
			const float depthRatio = curDepth / tmpBase;
			int countOnWidth = static_cast<int>( roundf( static_cast<float>( elementsCountToAddForIndent / 2 ) * widthRatio ) );
			int countOnDepth = static_cast<int>( roundf( static_cast<float>( elementsCountToAddForIndent / 2 ) * depthRatio ) );
			int countOnEdges[] = { countOnWidth, countOnDepth, countOnWidth, countOnDepth };
			const int diffCount = ( countOnWidth + countOnDepth ) * 2 - elementsCountToAddForIndent;
			if ( diffCount != 0 ) {
				if ( diffCount > 0 ) {
					if ( countOnWidth >= countOnDepth ) {
						countOnEdges[2]--;
					}
					else {
						countOnEdges[3]--;
					}
				}
				else {
					if ( countOnWidth <= countOnDepth ) {
						countOnEdges[0]++;
					}
					else {
						countOnEdges[1]++;
					}
				}
			}
			for ( int edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
				auto& edge = ( *curIndent )[edgeIndex];
				const int coefForAlong = ( edgeIndex / 2 ) % 2 == 0 ? -1 : 1;
				edge.maxEdgeSize = edgeIndex % 2 == 0 ? curWidth : curDepth;
				edge.full = edgeIndex % 2 == 0 ? countOnEdges[edgeIndex] == maxCountOnWidth : countOnEdges[edgeIndex] == maxCountOnDepth;
				edge.alongOffsetCoef = coefForAlong;
				edge.beginAlongPos = ( edge.maxEdgeSize / 2 - elementHalfWidth ) * static_cast<float>( edge.alongOffsetCoef );
				for ( int i = 0; i < countOnEdges[edgeIndex]; ++i ) {
					edge.elements << Element( elementDimension, elementIndex++ );
				}
			}
		}
		curWidth += elementIndentOffset * 2; // pre dalsie odsadenie sa zvacsi aj velkost popri stene kde sa mozu rozmiestnovat elementy
		curDepth += elementIndentOffset * 2;
	}

	// evaluacia pozicii
	for ( int indentIndex = 0; indentIndex < indents.count(); ++indentIndex ) {
		auto& indent = indents[indentIndex];
		for ( int edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
			auto& edge = indent[edgeIndex];
			const float rot = static_cast<float>( edgeIndex ) * static_cast<float>( osg::PI_2 );
			const int coefForIndent = edgeIndex == 0 || edgeIndex == 3 ? -1 : 1;
			float alongEdgeValue = edge.beginAlongPos;
			const float baseIndentPos = edgeIndex % 2 == 0 ? regionHalfDepth : regionHalfWidth;
			const float indentEdgeValue = ( baseIndentPos + elementHalfDepth + spacing + static_cast<float>( indentIndex ) * elementIndentOffset ) * static_cast<float>( coefForIndent );
			float spacingForUse;
			if ( edge.full ) { // ak je strana maximalne zaplnena elementami, vynecha sa prva a posledna medzera
				spacingForUse = ( edge.maxEdgeSize - static_cast<float>( edge.elements.count() ) * elementWidth ) / static_cast<float>( ( edge.elements.count() - 1 ) );
			}
			else {
				spacingForUse = ( edge.maxEdgeSize - static_cast<float>( edge.elements.count() ) * elementWidth ) / static_cast<float>( ( edge.elements.count() + 1 ) );
				alongEdgeValue += spacingForUse * -static_cast<float>( edge.alongOffsetCoef );
			}
			for ( auto& element : edge.elements ) {
				element.layout.position = osg::Vec3( ( edgeIndex % 2 == 0 ? alongEdgeValue : indentEdgeValue ) + offsetX, ( edgeIndex % 2 == 1 ? alongEdgeValue : indentEdgeValue ) + offsetY, elementVerticalOffset );
				element.layout.yawRotation = rot;
				alongEdgeValue += ( elementWidth + spacingForUse ) * -static_cast<float>( edge.alongOffsetCoef );
			}
		}
	}

	// naplnenie pozicii do output-u
	QVector<ElementLayout> tmpLayouts( elementCount );
	for ( int indentIndex = 0; indentIndex < indents.count(); ++indentIndex ) {
		auto& indent = indents[indentIndex];
		for ( int edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
			auto& edge = indent[edgeIndex];
			for ( auto& element : edge.elements ) {
				tmpLayouts[element.index] = element.layout;
			}
		}
	}
	*layouts = tmpLayouts.toList();

	// ak sa ma vratit aj region ktory pokryva vsetky rozmiestnene elementy - naplnia sa hodnoty
	if ( aroundRegion ) {
		if ( indents.empty() ) {
			*aroundRegion = osg::BoundingBox( region.xMin() - spacing, region.yMin() - spacing, 0, region.xMax() + spacing, region.yMax() + spacing, 0 );
		}
		else {
			float newAroundRegionValuesForEdges[4];
			for ( int edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
				const int indentCountForEdge = indents.last()[edgeIndex].elements.count() > 0 ? indents.count() : indents.count() - 1;
				const int coefForIndent = edgeIndex == 0 || edgeIndex == 3 ? -1 : 1;
				newAroundRegionValuesForEdges[edgeIndex] = origRegionValuesForEdges[edgeIndex] + ( static_cast<float>( indentCountForEdge ) * elementIndentOffset + spacing ) * static_cast<float>( coefForIndent );
			}

			const float& newNear = newAroundRegionValuesForEdges[0];
			const float& newRight = newAroundRegionValuesForEdges[1];
			const float& newFar = newAroundRegionValuesForEdges[2];
			const float& newLeft = newAroundRegionValuesForEdges[3];
			*aroundRegion = osg::BoundingBox( newLeft, newNear, 0, newRight, newFar, 0 );
		}
	}
}
}
