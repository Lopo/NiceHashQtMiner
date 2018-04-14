#ifndef IFTTT_H
#define IFTTT_H

#include <QString>


class Ifttt
{
private:
	const QString ApiUrl="https://maker.ifttt.com/trigger/";

public:
	static void PostToIfttt(QString action, QString msg);
};


#endif
