/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - Hand Viewer Sample                                   *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef _HISTORY_BUFFER_H_
#define _HISTORY_BUFFER_H_

#ifdef NITE2_FOUND

#include "NiTE.h"

namespace Kinect {

template <int BufferSize>
/**
 * @brief The HistoryBuffer class
 * @brief Representing history of position, in Kinect is using for history of Hand position
 */
class HistoryBuffer
{
public:
	HistoryBuffer() : m_currentHead( -1 ), m_size( 0 )
	{}
	/**
	 * @brief add point to history buffer
	 * @param point representation of point in 3D space - float type
	 */
	void AddPoint( const nite::Point3f& point )
	{
		++m_currentHead;
		if ( m_currentHead == BufferSize ) {
			m_currentHead = 0;
		}
		m_points[m_currentHead] = point;
		if ( m_size < BufferSize ) {
			++m_size;
		}
	}

	/**
	 * @brief get count of point in history buffer
	 * @return size of history buffer
	 */
	int GetSize()
	{
		return m_size;
	}

	/**
	 * @brief operator []
	 * @param index
	 * @return Point
	 */
	const nite::Point3f& operator[]( int index )
	{
		return m_points[( m_currentHead - index + BufferSize ) % BufferSize];
	}

private:
	friend class Iterator;

	/**
	 * @brief buffer size
	 */
	static const int m_bufferSize = BufferSize;

	/**
	 * @brief array of points
	 */
	nite::Point3f m_points[BufferSize];

	/**
	 * @brief current head in History buffer
	 */
	int m_currentHead;

	/**
	 * @brief current size of History buffer
	 */
	int m_size;
};

}

#endif // NITE2_FOUND

#endif
