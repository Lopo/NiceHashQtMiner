#include "Qt/LException.h"


LException::LException(const QByteArray msg)
{
	message=msg;
}

LException::~LException() _GLIBCXX_USE_NOEXCEPT
{ }

void LException::raise() const
{
	throw *this;
}

LException* LException::clone() const
{
	return new LException(*this);
}

const char* LException::what() const _GLIBCXX_USE_NOEXCEPT
{
	return message.data();
}
