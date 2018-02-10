#include "Layout/LayoutAlgorithms.h"
#include <exception>
#include <QVector>

namespace Layout {
void LayoutAlgorithms::layoutInsideRegion( const osg::BoundingBox& elementDimension, uint elementCount, float groundTopPosition, float spacing, QList<osg::Vec3>* layouts, osg::BoundingBox* aroundRegion )
{
	layouts->clear();
	const float width = elementDimension.xMax() - elementDimension.xMin();
	const float depth = elementDimension.yMax() - elementDimension.yMin();
	const float offsetX = width + spacing;
	const float offsetY = depth + spacing;
	const float elementConstOffsetX = -elementDimension.xMin();
	const float elementConstOffsetY = -elementDimension.yMin();
	const float elementConstOffsetZ = -elementDimension.zMin() + groundTopPosition;
	uint limit = 1;
	uint xNum = 0;
	uint yNum = 0;
	float xMax;
	float yMax;
	if ( elementCount > 0 ) {
		xMax = -FLT_MAX;
		yMax = -FLT_MAX;
		for ( uint i = 0; i < elementCount; ++i ) {
			osg::Vec3 pos( xNum * offsetX + elementConstOffsetX, yNum * offsetY + elementConstOffsetY, elementConstOffsetZ );
			if ( xMax < pos.x() ) {
				xMax = pos.x();
			}
			if ( yMax < pos.y() ) {
				yMax = pos.y();
			}
			*layouts << pos;
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
	Element( const osg::BoundingBox& boundingBox, uint index )
	{
		this->boundingBox = boundingBox;
		this->index = index;
	}
	osg::BoundingBox boundingBox;
	uint index;
	ElementLayout layout;
};

struct Edge {
	float maxEdgeSize;
	bool full;
	float beginAlongPos;
	int alongOffsetCoef;
	QList<Element> elements;

	Edge()
	{
		this->maxEdgeSize = 0;
		this->full = false;
		this->beginAlongPos = 0;
		this->alongOffsetCoef = 0;
	}
};

struct IndentEdges : public QVector<Edge> {
	IndentEdges( int count ) : QVector<Edge>( count ) {}
};

void LayoutAlgorithms::layoutAroundRegion( const osg::BoundingBox& elementDimension, uint elementCount, const osg::BoundingBox& region, float spacing, QList<ElementLayout>* layouts, osg::BoundingBox* aroundRegion )
{
	static const uint EDGES_COUNT = 4;

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
//		throw std::exception( "Region is too small for layouting even one element" );
	}

	float curWidth = regionWidth + ( regionWidth <= regionDepth ? elementIndentOffset * 2 : 0 );
	float curDepth = regionDepth + ( regionWidth > regionDepth ? elementIndentOffset * 2 : 0 );
	QList<IndentEdges> indents;
	IndentEdges* curIndent = nullptr;
	IndentEdges* prevIndent = nullptr;
	uint remainedElementsCount = elementCount;
	uint elementIndex = 0;

	// analyza - na ktorej stene bude kolko elementov
	while ( remainedElementsCount > 0 ) {
		indents << IndentEdges( EDGES_COUNT ); // nove odsadenie od regionu (jedno obkolesenie kvadrov - 4 strany)
		curIndent = &indents.last();
		const uint maxCountOnWidth = static_cast<uint>( floorf( ( curWidth + spacing ) / ( elementWidth + spacing ) ) ); // maximalny pocet elementov na sirku a hlbku
		const uint maxCountOnDepth = static_cast<uint>( floorf( ( curDepth + spacing ) / ( elementWidth + spacing ) ) );
		const uint maxCountOnIndent = ( maxCountOnWidth + maxCountOnDepth ) * 2; // maximalny pocet elementov celkovo okolo regionu pre dane odsadenie
		const float fillCoef = 0.5; // threshold podla ktoreho sa rozhoduje ktory algoritmus sa pouzije
		const uint elementsCountToAddForIndent = std::min( remainedElementsCount, maxCountOnIndent ); // nesmie sa umiestnit viac ako sa zmesti a zaroven viac kolko ostava
		remainedElementsCount -= elementsCountToAddForIndent;
		if ( static_cast<float>( elementsCountToAddForIndent ) < static_cast<float>( maxCountOnIndent ) * fillCoef ) { // zaplna po stranach (spusta sa len raz pre posledne odsadenie) - najprv zaplni near, potom right, potom ...
			uint remainedElementsCountToAddForIndent = elementsCountToAddForIndent; // pocitadlo ostavajucich elementov
			const uint firstEdgeOffset = curWidth < curDepth ? 0 : 1;
			uint edgeIndex = firstEdgeOffset; // index strany, near = 0, right = 1, far = 2, left = 3
			while ( remainedElementsCountToAddForIndent > 0 ) { // ak este ostavaju nejake elementy na rozmiestnenie pre odsadenie
				auto& edge = ( *curIndent )[static_cast<int>( edgeIndex )]; // vypocet informacii pre rozmiestnovanie na danej hrane
				const int coefForAlong = ( edgeIndex / 2 ) % 2 == 0 ? -1 : 1; // znamienko pre posun popri hrane (ci sa ma hodnota pridavat alebo odoberat)
				const float defaultMaxEdgeSize = edgeIndex % 2 == 0 ? curWidth : curDepth; // max dlzka popri hrane
				const float defaultBeginAlongPos = ( defaultMaxEdgeSize / 2 - elementHalfWidth ) * coefForAlong; // zaciatok umiestovania na hrane
				const float maxEdgeResizer = ( edgeIndex - firstEdgeOffset ) == 1 ? -elementAlongOffset : ( ( edgeIndex - firstEdgeOffset ) == 2 ? elementAlongOffset : 0 );
				const float beginAlongPosOffset = ( edgeIndex - firstEdgeOffset ) == 2 ? elementAlongOffset : 0;
				edge.alongOffsetCoef = coefForAlong;
				edge.maxEdgeSize = defaultMaxEdgeSize + maxEdgeResizer;
				edge.beginAlongPos = defaultBeginAlongPos + beginAlongPosOffset;
				const uint maxCountOnCurrentEdge = static_cast<uint>( floorf( ( edge.maxEdgeSize + spacing ) / ( elementWidth + spacing ) ) ); // maximalny pocet elementov na hrane
				const uint elemenstCountOnEdge = std::min( maxCountOnCurrentEdge, remainedElementsCountToAddForIndent ); // vezme sa bud max pocet alebo zostavajuci - ten mensi
				edge.full = elemenstCountOnEdge == maxCountOnCurrentEdge; // ak sa vzalo tolko elementov kolko sa zmesti na hranu - oznaci sa ako full zaplnena
				for ( uint i = 0; i < elemenstCountOnEdge; ++i ) {
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
			uint countOnWidth = static_cast<uint>( roundf( elementsCountToAddForIndent / 2 * widthRatio ) );
			uint countOnDepth = static_cast<uint>( roundf( elementsCountToAddForIndent / 2 * depthRatio ) );
			uint countOnEdges[] = { countOnWidth, countOnDepth, countOnWidth, countOnDepth };
			const int diffCount = static_cast<int>( ( countOnWidth + countOnDepth ) * 2 - elementsCountToAddForIndent );
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
			for ( uint edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
				auto& edge = ( *curIndent )[static_cast<int>( edgeIndex )];
				const int coefForAlong = ( edgeIndex / 2 ) % 2 == 0 ? -1 : 1;
				edge.maxEdgeSize = edgeIndex % 2 == 0 ? curWidth : curDepth;
				edge.full = edgeIndex % 2 == 0 ? countOnEdges[edgeIndex] == maxCountOnWidth : countOnEdges[edgeIndex] == maxCountOnDepth;
				edge.alongOffsetCoef = coefForAlong;
				edge.beginAlongPos = ( edge.maxEdgeSize / 2 - elementHalfWidth ) * edge.alongOffsetCoef;
				for ( uint i = 0; i < countOnEdges[edgeIndex]; ++i ) {
					edge.elements << Element( elementDimension, elementIndex++ );
				}
			}
		}
		curWidth += elementIndentOffset * 2; // pre dalsie odsadenie sa zvacsi aj velkost popri stene kde sa mozu rozmiestnovat elementy
		curDepth += elementIndentOffset * 2;
	}

	// evaluacia pozicii
	for ( uint indentIndex = 0; indentIndex < static_cast<uint>( indents.count() ); ++indentIndex ) {
		auto& indent = indents[static_cast<int>( indentIndex )];
		for ( uint edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
			auto& edge = indent[static_cast<int>( edgeIndex )];
			const float rot = static_cast<float>( edgeIndex ) * static_cast<float>( osg::PI_2 );
			const int coefForIndent = edgeIndex == 0 || edgeIndex == 3 ? -1 : 1;
			float alongEdgeValue = edge.beginAlongPos;
			const float baseIndentPos = edgeIndex % 2 == 0 ? regionHalfDepth : regionHalfWidth;
			const float indentEdgeValue = ( baseIndentPos + elementHalfDepth + spacing + indentIndex * elementIndentOffset ) * coefForIndent;
			float spacingForUse;
			if ( edge.full ) { // ak je strana maximalne zaplnena elementami, vynecha sa prva a posledna medzera
				spacingForUse = ( edge.maxEdgeSize - edge.elements.count() * elementWidth ) / ( edge.elements.count() - 1 );
			}
			else {
				spacingForUse = ( edge.maxEdgeSize - edge.elements.count() * elementWidth ) / ( edge.elements.count() + 1 );
				alongEdgeValue += spacingForUse * -edge.alongOffsetCoef;
			}
			for ( auto& element : edge.elements ) {
				element.layout.position = osg::Vec3( ( edgeIndex % 2 == 0 ? alongEdgeValue : indentEdgeValue ) + offsetX, ( edgeIndex % 2 == 1 ? alongEdgeValue : indentEdgeValue ) + offsetY, elementVerticalOffset );
				element.layout.yawRotation = rot;
				alongEdgeValue += ( elementWidth + spacingForUse ) * -edge.alongOffsetCoef;
			}
		}
	}

	// naplnenie pozicii do output-u
	QVector<ElementLayout> tmpLayouts( static_cast<int>( elementCount ) );
	for ( uint indentIndex = 0; indentIndex < static_cast<uint>( indents.count() ); ++indentIndex ) {
		auto& indent = indents[static_cast<int>( indentIndex )];
		for ( uint edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
			auto& edge = indent[static_cast<int>( edgeIndex )];
			for ( auto& element : edge.elements ) {
				tmpLayouts[static_cast<int>( element.index )] = element.layout;
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
			for ( uint edgeIndex = 0; edgeIndex < EDGES_COUNT; ++edgeIndex ) {
				const uint indentCountForEdge = indents.last()[static_cast<int>( edgeIndex )].elements.count() > 0 ? static_cast<const uint>( indents.count() ) : static_cast<const uint>( indents.count() - 1 );
				const int coefForIndent = edgeIndex == 0 || edgeIndex == 3 ? -1 : 1;
				newAroundRegionValuesForEdges[edgeIndex] = origRegionValuesForEdges[edgeIndex] + ( indentCountForEdge * elementIndentOffset + spacing ) * coefForIndent;
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
