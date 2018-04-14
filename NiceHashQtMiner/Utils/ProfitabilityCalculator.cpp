#define __ProfitabilityCalculator_cpp__
#include "Utils/ProfitabilityCalculator.h"


QMap<Enums::AlgorithmType, double> ProfitabilityCalculator::Div={
	{Enums::AlgorithmType::INVALID, 1},
	{Enums::AlgorithmType::NONE, 1},
	{Enums::AlgorithmType::Scrypt_UNUSED, MHs},
	{Enums::AlgorithmType::SHA256_UNUSED, THs},
	{Enums::AlgorithmType::ScryptNf_UNUSED, MHs},
	{Enums::AlgorithmType::X11_UNUSED, MHs},
	{Enums::AlgorithmType::X13_UNUSED, MHs},
	{Enums::AlgorithmType::Keccak, MHs},
	{Enums::AlgorithmType::X15_UNUSED, MHs},
	{Enums::AlgorithmType::Nist5, MHs},
	{Enums::AlgorithmType::NeoScrypt, MHs},
	{Enums::AlgorithmType::Lyra2RE, MHs},
	{Enums::AlgorithmType::WhirlpoolX_UNUSED, MHs},
	{Enums::AlgorithmType::Qubit_UNUSED, MHs},
	{Enums::AlgorithmType::Quark_UNUSED, MHs},
	{Enums::AlgorithmType::Axiom_UNUSED, kHs},
	{Enums::AlgorithmType::Lyra2REv2, MHs},
	{Enums::AlgorithmType::ScryptJaneNf_UNUSED, MHs},
	{Enums::AlgorithmType::Blake256r8_UNUSED, GHs},
	{Enums::AlgorithmType::Blake256r14_UNUSED, GHs},
	{Enums::AlgorithmType::Blake256r8vnl_UNUSED, GHs},
	{Enums::AlgorithmType::Hodl, kHs},
	{Enums::AlgorithmType::DaggerHashimoto, MHs},
	{Enums::AlgorithmType::Decred, GHs},
	{Enums::AlgorithmType::CryptoNight, kHs},
	{Enums::AlgorithmType::Lbry, GHs},
	{Enums::AlgorithmType::Equihash, 1},
	{Enums::AlgorithmType::Pascal, GHs},
	{Enums::AlgorithmType::X11Gost, MHs},
	{Enums::AlgorithmType::Sia, GHs},
	{Enums::AlgorithmType::Blake2s, GHs},
	{Enums::AlgorithmType::Skunk, MHs},
	{Enums::AlgorithmType::CryptoNightV7, kHs}
	};

double ProfitabilityCalculator::GetFormatedSpeed(double speed, Enums::AlgorithmType type)
{
	if (Div.contains(type)) {
		return speed/Div[type];
		}
	return speed; // should never happen
}
