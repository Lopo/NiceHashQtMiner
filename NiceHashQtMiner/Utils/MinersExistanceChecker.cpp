#include "Utils/MinersExistanceChecker.h"
#include "Utils/BINS_CODEGEN.h"
#include "Utils/BINS_CODEGEN_3rd.h"
#include "Utils/Helpers.h"
#include <QFile>


bool MinersExistanceChecker::IsMinersBins_ALL_Init()
{
	foreach (QString filePath, Bins_Data::ALL_FILES_BINS) {
		if (!QFile::exists("bin"+filePath)) {
			Helpers::ConsolePrint("MinersExistanceChecker", QString("bin%1 doesn't exist! Warning").arg(filePath));
			return false;
			}
		}
	return true;
}

bool MinersExistanceChecker::IsMiners3rdPartyBinsInit()
{
	foreach (QString filePath, Bins_Data_3rd::ALL_FILES_BINS) {
		if (!QFile::exists("bin_3rdparty"+filePath)) {
			Helpers::ConsolePrint("MinersExistanceChecker", QString("bin_3rdparty%1 doesn't exist! Warning").arg(filePath));
			return false;
			}
		}
	return true;
}

bool MinersExistanceChecker::IsMinersBinsInit()
{
	return IsMinersBins_ALL_Init();
}
