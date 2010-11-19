#include "Util/Adapter_iostream_to_QIODevice.h"

namespace Util {

Adapter_iostream_to_QIODevice::Adapter_iostream_to_QIODevice (
	std::iostream &stream
) : stream_ (stream)
{
	setOpenMode (ReadWrite);
}

} // namespace
