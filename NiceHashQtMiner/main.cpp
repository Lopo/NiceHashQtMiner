#include "config.h"
#include "version.h"
#include "Utils/Logger.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Stats/ExchangeRateAPI.h"
#include "Globals.h"
#include "Forms/Form_ChooseLanguage.h"
#include "Utils/Helpers.h"
#include "Forms/Form_Main.h"
#include "Configs/ConfigJsonFile/FOLDERS.h"
#include "International.h"
#include "Utils/CommandLineParser.h"
#include "Utils/Links.h"
#include <QtNetwork/QNetworkAccessManager>
#include <boost/filesystem.hpp>
#include <QtSingleApplication>
#include <QFontDatabase>


QNetworkAccessManager* qnam;


int main(int argc, char *argv[])
{
	// Set working directory to exe
	boost::filesystem::current_path(boost::filesystem::canonical(argv[0]).remove_filename());

	QCoreApplication::setApplicationName("NiceHash QtMiner");
	QCoreApplication::setApplicationVersion(NHQTM_VERSION_STRING);
	QCoreApplication::addLibraryPath("./");

	qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
#if QT_VERSION > 0x050100
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	// #1 first initialize config
	ConfigManager.InitializeConfig();

	QtSingleApplication a(argc, argv);
	a.setAttribute(Qt::AA_NativeWindows);
	QFontDatabase fntDb;
	QString curFam(QApplication::font().family());
	bool isCurScaleable=fntDb.isScalable(curFam);
	QString subFam(QFont::substitute(curFam));
	bool isSubScaleable=fntDb.isScalable(subFam);
	if (isCurScaleable && !isSubScaleable) {
		QFont::removeSubstitutions(curFam);
		}
//	float devicePixelRatio=1.0;
//	devicePixelRatio=a.devicePixelRatio();
	// #2 check if multiple instances are allowed
	if (!ConfigManager.generalConfig->AllowMultipleInstances) {
		try {
			if (a.isRunning()) {
				return 0;
				}
			}
		catch (...) {}
		}

	if (ConfigManager.generalConfig->LogToFile) {
		Logger.ConfigureWithFile();
		}
//	if (ConfigManager.generalConfig->DebugConsole) {
//		PInvokeHelpers::AllocConsole();
//		}

	// init active display currency after config load
	ExchangeRateApi::ActiveDisplayCurrency=ConfigManager.generalConfig->DisplayCurrency;

	// #2 then parse args
	CommandLineParser* commandLineArgs=new CommandLineParser(QCoreApplication::arguments());

	qnam=new QNetworkAccessManager;

	// Init languages
	International::Initialize(ConfigManager.generalConfig->Language);

	Helpers::ConsolePrint("NICEHASH", "Starting up NiceHashQtMiner v"+a.applicationVersion());
	bool tosChecked= ConfigManager.generalConfig->agreedWithTOS==Globals::CurrentTosVer;
	if (!tosChecked || !ConfigManager.GeneralConfigIsFileExist() && !commandLineArgs->IsLang) {
		Helpers::ConsolePrint("NICEHASH", "No config file found. Running NiceHash QtMiner for the first time. Choosing a default language.");
		Form_ChooseLanguage fcl;
		fcl.setGeometry(
			QStyle::alignedRect(
				Qt::LeftToRight,
				Qt::AlignCenter,
				fcl.size(),
				a.desktop()->availableGeometry()
				)
			);
		fcl.exec();
		}


	if (commandLineArgs->IsLang) {
		Helpers::ConsolePrint("NICEHASH", "Language is overwritten by command line parameter (-lang).");
		International::Initialize(commandLineArgs->LangValue);
		ConfigManager.generalConfig->Language=commandLineArgs->LangValue;
		}
	delete commandLineArgs;

	if (ConfigManager.generalConfig->agreedWithTOS!=Globals::CurrentTosVer) {
		return 0;
		}

	Form_Main w;
/*	w.setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			w.size(),
			a.desktop()->availableGeometry()
			)
		);*/
	w.show();
	return a.exec();
}
