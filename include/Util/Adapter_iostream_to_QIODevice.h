#ifndef Util_Adapter_iostream_to_QIODevice_H
#define Util_Adapter_iostream_to_QIODevice_H
//-----------------------------------------------------------------------------
#include <QIODevice>
//-----------------------------------------------------------------------------
#include <iostream>
//-----------------------------------------------------------------------------

namespace Util {
class Adapter_iostream_to_QIODevice
	: public QIODevice {

public:

	Adapter_iostream_to_QIODevice (
		std::iostream &stream
	);

	virtual ~Adapter_iostream_to_QIODevice (void) {};

	virtual bool isSequential() const {
		return true;
	}

	virtual bool open(OpenMode mode) {
		return true;
	}

	virtual void close() {
		setOpenMode (NotOpen);
	}

	virtual qint64 pos() const {
		throw "not implemented";
	}
	virtual qint64 size() const {
		throw "not implemented";
	}
	virtual bool seek(qint64 pos) {
		throw "not implemented";
	}
	virtual bool atEnd() const {
		return stream_.eof();
	}
	virtual bool reset() {
		throw "not implemented";
	}

	virtual qint64 bytesAvailable() const {
		throw "not implemented";
	}
	virtual qint64 bytesToWrite() const {
		throw "not implemented";
	}

	virtual bool canReadLine() const {
		throw "not implemented";
	}

	virtual bool waitForReadyRead(int msecs) {
		return true;
	}
	virtual bool waitForBytesWritten(int msecs) {
		throw "not implemented";
	}

protected:

	virtual qint64 readData(char *data, qint64 maxlen) {
		stream_.read (data, maxlen);
		return stream_.gcount ();
	}
	virtual qint64 readLineData(char *data, qint64 maxlen) {
		throw "not implemented";
	}
	virtual qint64 writeData(const char *data, qint64 len) {
		throw "not implemented";
	}

private:

	std::iostream &stream_;

}; // class
} // namespace

#endif // Util_Adapter_iostream_to_QIODevice_H
