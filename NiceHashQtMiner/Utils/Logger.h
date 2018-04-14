#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

#include <log4qt/Logger>
#include <log4qt/Appender>


class Logger
{
public:
	static bool IsInit;
	static Log4Qt::Logger* log;
	static const QString LogPath;

	static void ConfigureWithFile();
	static Log4Qt::AppenderSharedPtr CreateFileAppender();
	static Log4Qt::AppenderSharedPtr CreateConsoleAppender();
};

#ifdef __Utils_Logger_cpp__
	Logger Logger;
#else
	extern Logger Logger;
#endif

#endif
