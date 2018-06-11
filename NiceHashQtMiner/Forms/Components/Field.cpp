#include "Forms/Components/Field.h"
#include "Properties/Resources.h"
#include <climits>
#include <float.h>


template<typename T>
Field<T>::Field(QWidget* parent/*=Q_NULLPTR*/)
	: FieldSignalSlots(parent)
{
	InitializeComponent();
}

template<typename T>
void Field<T>::InitializeComponent()
{
	Resources resources;

	labelFieldIndicator=new QLabel(this);
	labelFieldIndicator->setObjectName("labelFieldIndicator");
	labelFieldIndicator->setGeometry(3, 6, 190, 13);
	labelFieldIndicator->setText("Label field indicator");

	if (std::is_same<T, int>::value) {
		textBox=new QSpinBox(this);
		QSpinBox* rtb=(QSpinBox*)textBox;
		rtb->setButtonSymbols(QDoubleSpinBox::ButtonSymbols::NoButtons);
		rtb->setMaximum(INT_MAX);
		connect(rtb, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=]{Q_EMIT TextChanged();});
		connect(rtb, &QSpinBox::editingFinished, [=]{Q_EMIT TextLeave();});
		}
	else if (std::is_same<T, double>::value) {
		textBox=new QDoubleSpinBox(this);
		QDoubleSpinBox* rtb=(QDoubleSpinBox*)textBox;
		rtb->setButtonSymbols(QDoubleSpinBox::ButtonSymbols::NoButtons);
		rtb->setMaximum(DBL_MAX);
		connect(rtb, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=]{Q_EMIT TextChanged();});
		connect(rtb, &QDoubleSpinBox::editingFinished, [=]{Q_EMIT TextLeave();});
		}
	else if (std::is_same<T, QString>::value) {
		textBox=new QLineEdit(this);
		connect((QLineEdit*)textBox, &QLineEdit::textChanged, [=]{Q_EMIT TextChanged();});
		connect((QLineEdit*)textBox, &QLineEdit::editingFinished, [=]{Q_EMIT TextLeave();});
		}
	textBox->setObjectName("textBox");
	textBox->setGeometry(3, 24, 214, 20);

	pictureBox1=new QLabel(this);
	pictureBox1->setObjectName("pictureBox1");
	pictureBox1->setGeometry(197, 3, 20, 19);
	pictureBox1->setPixmap(resources.info_black_18);

	setObjectName("Field");
	resize(220, 48);
}

template<typename T>
void Field<T>::InitLocale(QString infolabel, QString infoMsg)
{
	labelFieldIndicator->setText(infolabel);
	labelFieldIndicator->setToolTip(infoMsg);
	textBox->setToolTip(infoMsg);
	pictureBox1->setToolTip(infoMsg);
}

template<typename T>
QString Field<T>::EntryText() const
{
	if (std::is_same<T, int>::value) {
		return QString::number(((QSpinBox*)textBox)->value());
		}
	if (std::is_same<T, double>::value) {
		return QString::number(((QDoubleSpinBox*)textBox)->value());
		}
	if (std::is_same<T, QString>::value) {
		return ((QLineEdit*)textBox)->text();
		}
}

template<typename T>
void Field<T>::EntryText(QString value)
{
	if (std::is_same<T, int>::value) {
		((QSpinBox*)textBox)->setValue(value.toInt());
		}
	else if (std::is_same<T, double>::value) {
		((QDoubleSpinBox*)textBox)->setValue(value.toDouble());
		}
	else if (std::is_same<T, QString>::value) {
		((QLineEdit*)textBox)->setText(value);
		}
}


template class Field<int>;
template class Field<double>;
template class Field<QString>;
