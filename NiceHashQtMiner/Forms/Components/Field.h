#ifndef FORMS_COMPONENTS_FIELD_H_
#define FORMS_COMPONENTS_FIELD_H_


#include <QtWidgets>


class FieldSignalSlots
	: public QWidget
{
	Q_OBJECT
public:
	FieldSignalSlots(QWidget *parent=Q_NULLPTR) : QWidget(parent) {}

Q_SIGNALS:
	void TextChanged();
	void TextLeave();
};


template <typename T>
class Field
	: public FieldSignalSlots
{
public:
	Field(QWidget* parent=Q_NULLPTR);

	inline QString LabelText() const { return labelFieldIndicator->text();};
	inline void LabelText(QString value) {if (value.length()) labelFieldIndicator->setText(value);};

	QString EntryText() const;
	void EntryText(QString value);

	void InitLocale(QString infolabel, QString infoMsg);

private:
	void InitializeComponent();

	QLabel* labelFieldIndicator=nullptr;
	QWidget* textBox=nullptr;
	QLabel* pictureBox1=nullptr;
};

#endif
