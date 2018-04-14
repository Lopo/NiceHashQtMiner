#ifndef UTILS_MINERSDOWNLOADER_H_
#define UTILS_MINERSDOWNLOADER_H_

#include <QTimer>

class DownloadSetup;
class IMinerUpdateIndicator;
class Downloader;


class MinersDownloader
{
public:
	MinersDownloader(DownloadSetup* downloadSetup);
	void Start(IMinerUpdateIndicator* minerUpdateIndicator);

private:
	const QString Tag="MinersDownloader";
	DownloadSetup* _downloadSetup=nullptr;
	Downloader* _downloader=nullptr;
	QTimer* _timer=nullptr;
	int _ticksSinceUpdate=0;
	long _lastProgress=0;
	QThread* _unzipThread=nullptr;

	bool _isDownloadSizeInit=false;

	IMinerUpdateIndicator* _minerUpdateIndicator=nullptr;

	void Downlaod();
};

#endif
