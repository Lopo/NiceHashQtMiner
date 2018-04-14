#include "Qt/ClickableLabel.h"


ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
	: QLabel(parent)
{
	setStyleSheet("QLabel { color: blue; text-decoration: underline; }");
	setCursor(Qt::PointingHandCursor);
}

ClickableLabel::~ClickableLabel() { }

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
	emit clicked();
}
