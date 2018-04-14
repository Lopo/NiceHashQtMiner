#ifndef UTILS_ALGORITHMNICEHASHNAMES_H_
#define UTILS_ALGORITHMNICEHASHNAMES_H_

#include <QString>

#include "Enums.h"


class AlgorithmNiceHashNames
{
public:
	static QString GetName(const Enums::AlgorithmType type);
};

#endif
