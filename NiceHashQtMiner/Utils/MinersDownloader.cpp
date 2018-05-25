#include "Utils/MinersDownloader.h"
#include "Utils/DownloadSetup.h"
#include "Utils/Helpers.h"
#include <QFile>
#include <QDir>
#include <QException>


MinersDownloader::MinersDownloader(DownloadSetup* downloadSetup)
{
	_downloadSetup=downloadSetup;

//	QList<IExtension> extensions;
	try {
//		extensions.add(new CoreExtension());
//		extensions.add(new HttpFtpProtocolExtension());
		}
	catch (...) {}
}

void MinersDownloader::Start(IMinerUpdateIndicator* minerUpdateIndicator)
{
	_minerUpdateIndicator=minerUpdateIndicator;
	// if something not right delete previous and download new
	try {
		if (QFile::exists(_downloadSetup->BinsZipLocation)) {
			QFile::remove(_downloadSetup->BinsZipLocation);
			}
		QDir d(_downloadSetup->ZipedFolderName);
		if (d.exists()) {
			d.removeRecursively();
			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint("MinersDownloader", e.what());
		}
	Downlaod();
}

void MinersDownloader::Downlaod()
{
	_lastProgress=0;
	_ticksSinceUpdate=0;

//	_minerUpdateIndicator.setTitle(GetText("MinersDownloadManager_Title_Downloading"));

//	DownloadManager
}
