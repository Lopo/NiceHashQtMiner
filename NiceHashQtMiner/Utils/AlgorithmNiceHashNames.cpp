#include "Utils/AlgorithmNiceHashNames.h"
#include <QMetaEnum>


QString AlgorithmNiceHashNames::GetName(const Enums::AlgorithmType type)
{
	if ((Enums::AlgorithmType::INVALID<=type && type<=Enums::AlgorithmType::CryptoNightV7)
		|| (Enums::AlgorithmType::DaggerKeccak<=type && type<=Enums::AlgorithmType::DaggerPascal)
		) {
		return QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)type);
		}
	return "NameNotFound type not supported";
}
