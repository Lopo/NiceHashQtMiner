#include "Utils/DownloadSetup.h"


DownloadSetup::DownloadSetup(const QString url, const QString dlName, const QString inFolderName)
{
	BinsDownloadURL=url;
	BinsZipLocation=dlName;
	ZipedFolderName=inFolderName;
}
