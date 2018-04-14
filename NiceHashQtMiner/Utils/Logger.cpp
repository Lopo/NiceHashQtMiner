#define __Utils_Logger_cpp__
#include "Utils/Logger.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include <log4qt/LogManager>
#include <log4qt/LoggerRepository>
#include <log4qt/RollingFileAppender>
#include <log4qt/ConsoleAppender>
#include <log4qt/PatternLayout>
#include <QDir>
#include <QTextCodec>


bool Logger::IsInit=false;
Log4Qt::Logger* Logger::log=Log4Qt::LogManager::logger("Logger");
const QString Logger::LogPath="logs/";


void Logger::ConfigureWithFile()
{
	try {
		if (!QDir(LogPath).exists()) {
			QDir().mkdir(LogPath);
			}
		}
	catch (...) { }

	IsInit=true;
	try {
		Log4Qt::LoggerRepository* h=log->loggerRepository();

		if (ConfigManager.generalConfig->LogToFile) {
			h->rootLogger()->setLevel(Log4Qt::Level::INFO_INT);
			}
		h->rootLogger()->addAppender(CreateFileAppender());
//		h->rootLogger()->addAppender(CreateConsoleAppender());
		}
	catch (...) {
		IsInit=false;
		}
}

Log4Qt::AppenderSharedPtr Logger::CreateFileAppender()
{
	Log4Qt::RollingFileAppender* appender=new Log4Qt::RollingFileAppender;
	appender->setName("RollingFileAppender");
	appender->setFile(LogPath+"log.txt");
	appender->setAppendFile(true);
//	appender.RollingStyle=size
//	appender.MaxSizeRollBackups=1
	appender->setMaximumFileSize(ConfigManager.generalConfig->LogMaxFileSize);
//	appender.PreserveLogFileNameExtension=true
	appender->setEncoding(QTextCodec::codecForLocale()); //unicode

	Log4Qt::PatternLayout* layout=new Log4Qt::PatternLayout;
	layout->setConversionPattern("[%d{yyyy-MM-dd HH:mm:ss}] [%p] %m%n");
	layout->activateOptions();

	appender->setLayout(layout);
	appender->activateOptions();

	return appender;
}

Log4Qt::AppenderSharedPtr Logger::CreateConsoleAppender()
{
	Log4Qt::ConsoleAppender* appender=new Log4Qt::ConsoleAppender;
	appender->setName("ConsoleAppender");
	appender->setEncoding(QTextCodec::codecForLocale()); //unicode

	Log4Qt::PatternLayout* layout=new Log4Qt::PatternLayout;
	layout->setConversionPattern("[%d{yyyy-MM-dd HH:mm:ss}] %m%n");
	layout->activateOptions();

	appender->setLayout(layout);
	appender->activateOptions();

	return appender;
}
