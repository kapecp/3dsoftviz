#ifndef Util_Adapter_iostream_to_QIODevice_H
#define Util_Adapter_iostream_to_QIODevice_H
//-----------------------------------------------------------------------------
#include <QIODevice>
//-----------------------------------------------------------------------------
#include <iostream>
#include "QDebug"
//-----------------------------------------------------------------------------

namespace Util {

/**
 * \brief Adapter providing the ability to use std::iostream as QIODevice.
 * Only reading is implemented.
 */
class Adapter_iostream_to_QIODevice
	: public QIODevice
{

public:

	/**
	 * \brief Creates a new adapter over the specified stream.
	 * \param[] stream Stream to use by the adapter.
	 */
	Adapter_iostream_to_QIODevice(
		std::iostream& stream
	) : stream_( stream )
	{
		setOpenMode( ReadWrite );
	}

	/***/
	virtual ~Adapter_iostream_to_QIODevice( void ) {}

	/**
	 * \brief Method provided for compatibility with QIODevice.
	 * Only returns true.
	 */
	virtual bool isSequential() const
	{
		return true;
	}

	/**
	 * \brief Open method provided for compatibility with QIODevice.
	 * Only returns true (encapsulated iostream is already opened by construction).
	 */
	virtual bool open()
	{
		return true;
	}
	using QIODevice::open;

	/**
	 * \brief Close method provided for compatibility with QIODevice.
	 * Only sets the open mode (the encapsulated iostream is closed later - by destruction).
	 */
	virtual void close()
	{
		setOpenMode( NotOpen );
	}

	/***/
	virtual qint64 pos() const
	{
		throw "not implemented";
	}

	/***/
	virtual qint64 size() const
	{
		throw "not implemented";
	}

	/***/
	virtual bool seek( qint64 pos )
	{
		throw "not implemented"+pos;
	}

	/***/
	virtual bool atEnd() const
	{
		return stream_.eof();
	}

	/***/
	virtual bool reset()
	{
		throw "not implemented";
	}

	/***/
	virtual qint64 bytesAvailable() const
	{
		throw "not implemented";
	}

	/***/
	virtual qint64 bytesToWrite() const
	{
		throw "not implemented";
	}

	/***/
	virtual bool canReadLine() const
	{
		throw "not implemented";
	}

	/**
	 * \brief Method provided for compatibility with QIODevice.
	 * Only returns true.
	 */
	virtual bool waitForReadyRead( int msecs )
	{
		qDebug() << "not implement "+msecs;
		return true;
	}

	/***/
	virtual bool waitForBytesWritten( int msecs )
	{
		throw "not implemented"+msecs;
	}

protected:

	/**
	 * \brief Reads up to maxlen bytes to the buffer.
	 * \param[out] data Buffer to read data to.
	 * \param[in] maxlen Max. length of the data read.
	 * \return Count of bytes read.
	 */
	virtual qint64 readData( char* data, qint64 maxlen )
	{
		stream_.read( data, maxlen );
		return stream_.gcount();
	}

	/***/
	virtual qint64 readLineData( char /* *data*/, qint64 /* maxlen */ )
	{
		throw "not implemented";

	}
	using QIODevice::readLineData;

	/***/
	virtual qint64 writeData( const char /* *data*/, qint64 /*len*/ )
	{
		throw "not implemented";
	}
	using QIODevice::writeData;

private:

	/**
	 * \brief Encapsulated stream.
	 */
	std::iostream& stream_;

}; // class

} // namespace

#endif // Util_Adapter_iostream_to_QIODevice_H
