#ifndef UTILS_MINERSDOWNLOADMANAGER_H_
#define UTILS_MINERSDOWNLOADMANAGER_H_

#include "Utils/DownloadSetup.h"


class MinersDownloadManager
{
public:
	static DownloadSetup* StandardDlSetup;
	static DownloadSetup* ThirdPartyDlSetup;
};

DownloadSetup* MinersDownloadManager::StandardDlSetup=new DownloadSetup("http://temp.losys.sk/nhqtmbin.zip", "bins.zip", "bin");
DownloadSetup* MinersDownloadManager::ThirdPartyDlSetup=new DownloadSetup("http://temp.losys.sk/nhqtmbin3.zip", "bins3.zip", "bin_3rdparty");

#endif
