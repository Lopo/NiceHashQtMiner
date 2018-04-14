#ifndef QT_CLICKABLELABEL_H_
#define QT_CLICKABLELABEL_H_

#include <QLabel>


class ClickableLabel
	: public QLabel
{
	Q_OBJECT

public:
	explicit ClickableLabel(QWidget* parent=Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags());
	~ClickableLabel();

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event);

};

#endif
