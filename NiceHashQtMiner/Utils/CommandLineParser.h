#ifndef UTILS_COMMANDLINEPARSER_H_
#define UTILS_COMMANDLINEPARSER_H_

#include "Enums.h"


class CommandLineParser
{
public:
	CommandLineParser(QStringList argv);

	bool IsLang=false;
	Enums::LanguageType LangValue=Enums::LanguageType::En;

private:
	static bool ParseCommandLine(QStringList argv, QString find, QString& value);
};

#endif
