#ifndef UTILS_DOWNLOADSETUP_H_
#define UTILS_DOWNLOADSETUP_H_

#include <QString>


class DownloadSetup
{
public:
	DownloadSetup(const QString url, const QString dlName, const QString inFolderName);

	QString BinsDownloadURL;
	QString BinsZipLocation;
	QString ZipedFolderName;
};

#endif
