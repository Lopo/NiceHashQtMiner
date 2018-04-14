#include "Utils/CommandLineParser.h"
#include "Utils/Helpers.h"


CommandLineParser::CommandLineParser(QStringList argv)
{
	QString tmpString="x";

	if (ParseCommandLine(argv, "-config", tmpString)) {
		Helpers::ConsolePrint("CommandLineParser", "-config parameter has been depreciated, run setting from GUI");
		}
	if (ParseCommandLine(argv, "-lang", tmpString)) {
		IsLang=true;
		// if parsing fails set to default
		bool ok;
		int tmp=tmpString.toInt(&ok);
		LangValue= ok
			? (Enums::LanguageType)tmp
			: Enums::LanguageType::En;
		}
}

bool CommandLineParser::ParseCommandLine(QStringList argv, QString find, QString& value)
{
	value="";
	for (int i=0; i<argv.length(); i++) {
		if (argv.at(i)==find) {
			if ((i+1)<argv.length() && argv.at(i+1).trimmed().at(0)!='-') {
				value=argv.at(i+1);
				}
			return true;
			}
		}
	return false;
}
