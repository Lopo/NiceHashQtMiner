#ifndef QT_LEXCEPTION_H_
#define QT_LEXCEPTION_H_

#include <QException>


class LException
	: public QException
{
public:
	LException(const QByteArray msg);
	virtual ~LException() _GLIBCXX_USE_NOEXCEPT;

	void raise() const Q_DECL_OVERRIDE;
	LException *clone() const Q_DECL_OVERRIDE;
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;

private:
	QByteArray message;
};

#endif
