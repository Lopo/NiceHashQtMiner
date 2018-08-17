#include "Devices/Algorithms/DefaultAlgorithms.h"
#include "Devices/AmdGpuDevice.h"


QMap<Enums::MinerBaseType, QList<Algorithm*>*>* DefaultAlgorithms::All()
{
	return new QMap<Enums::MinerBaseType, QList<Algorithm*>*>({
		{Enums::MinerBaseType::XmrStak, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::XmrStak, Enums::AlgorithmType::CryptoNightV7),
			new Algorithm(Enums::MinerBaseType::XmrStak, Enums::AlgorithmType::CryptoNightHeavy)
			})},
		{Enums::MinerBaseType::Xmrig, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::Xmrig, Enums::AlgorithmType::CryptoNightV7)
			})}
		});
}

QMap<Enums::MinerBaseType, QList<Algorithm*>*>* DefaultAlgorithms::Gpu()
{
	return new QMap<Enums::MinerBaseType, QList<Algorithm*>*>({
		{Enums::MinerBaseType::Claymore, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto),
			new DualAlgorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Decred),
			new DualAlgorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Lbry),
			new DualAlgorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Pascal),
			new DualAlgorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Sia),
			new DualAlgorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Blake2s),
			new DualAlgorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Keccak)
			})}
		});
}

QMap<Enums::MinerBaseType, QList<Algorithm*>*>* DefaultAlgorithms::Cpu()
{
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* ret=new QMap<Enums::MinerBaseType, QList<Algorithm*>*>({
		{Enums::MinerBaseType::cpuminer, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::cpuminer, Enums::AlgorithmType::Hodl, "hodl"),
			new Algorithm(Enums::MinerBaseType::cpuminer, Enums::AlgorithmType::Lyra2z, "lyra2z")
			})}
		});
	auto all=All();
	foreach (Enums::MinerBaseType mbt, all->keys()) {
		if (!ret->count(mbt)) {
			ret->insert(mbt, all->value(mbt));
			}
		else {
			foreach (Algorithm* a, *all->value(mbt)) {
				ret->value(mbt)->append(a);
				}
			}
		}
	return ret;
}

#if WITH_AMD
const QStringList DefaultAlgorithms::RemDis={"--remove-disabled"};
const QStringList DefaultAlgorithms::DefaultParam=QStringList(RemDis) << AmdGpuDevice::DefaultParam();

QMap<Enums::MinerBaseType, QList<Algorithm*>*>* DefaultAlgorithms::Amd()
{
	Algorithm* a;
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* ret=new QMap<Enums::MinerBaseType, QList<Algorithm*>*>;
	QList<Algorithm*>* l=new QList<Algorithm*>;
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::NeoScrypt, "neoscrypt", false);
	a->ExtraLaunchParameters=DefaultParam+QStringList({"--nfactor", "10", "--xintensity", "2", "--thread-concurrency", "8192", "--worksize", "64", "--gpu-threads", "4"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::DaggerHashimoto, "ethash");
	a->ExtraLaunchParameters=RemDis+QStringList({"--xintensity", "512", "-w", "192", "-g", "1"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::Decred, "decred", false);
	a->ExtraLaunchParameters=RemDis+QStringList({"--gpu-threads", "1", "--xintensity", "256", "--lookup-gap", "2", "--worksize", "64"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::Lbry, "lbry", false);
	a->ExtraLaunchParameters=RemDis+QStringList({"--xintensity", "512", "--worksize", "128", "--gpu-threads", "2"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::Pascal, "pascal", false);
	a->ExtraLaunchParameters=RemDis+QStringList({"--intensity", "21", "-w", "64", "-g", "2"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::X11Gost, "sibcoin-mod", false);
	a->ExtraLaunchParameters=RemDis+QStringList({"--intensity", "16", "-w", "64", "-g", "2"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::Keccak, "keccak");
	a->ExtraLaunchParameters=RemDis+QStringList({"--intensity", "15"});
	l->append(a);
	a=new Algorithm(Enums::MinerBaseType::sgminer, Enums::AlgorithmType::X16R, "x16r");
	a->ExtraLaunchParameters=RemDis+QStringList({"-X", "256"});
	l->append(a);
	ret->insert(Enums::MinerBaseType::sgminer, l);
	ret->insert(Enums::MinerBaseType::Claymore, new QList<Algorithm*>({
		new Algorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::CryptoNightV7),
		new Algorithm(Enums::MinerBaseType::Claymore, Enums::AlgorithmType::Equihash, "equihash")
		}));
	ret->insert(Enums::MinerBaseType::OptiminerAMD, new QList<Algorithm*>({
		new Algorithm(Enums::MinerBaseType::OptiminerAMD, Enums::AlgorithmType::Equihash, "equihash")
		}));
	ret->insert(Enums::MinerBaseType::Prospector, new QList<Algorithm*>({
		new Algorithm(Enums::MinerBaseType::Prospector, Enums::AlgorithmType::Skunk, "sigt", false),
		new Algorithm(Enums::MinerBaseType::Prospector, Enums::AlgorithmType::Sia, "sia", false)
		}));
	auto all=All();
	foreach (Enums::MinerBaseType mbt, all->keys()) {
		if (!ret->count(mbt)) {
			ret->insert(mbt, all->value(mbt));
			}
		else {
			foreach (Algorithm* a, *all->value(mbt)) {
				ret->value(mbt)->append(a);
				}
			}
		}
	auto gpu=Gpu();
	foreach (Enums::MinerBaseType mbt, gpu->keys()) {
		if (!ret->count(mbt)) {
			ret->insert(mbt, gpu->value(mbt));
			}
		else {
			foreach (Algorithm* a, *gpu->value(mbt)) {
				ret->value(mbt)->append(a);
				}
			}
		}
	return ret;
}
#endif
#if WITH_NVIDIA
QMap<Enums::MinerBaseType, QList<Algorithm*>*>* DefaultAlgorithms::Nvidia()
{
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* ret=new QMap<Enums::MinerBaseType, QList<Algorithm*>*>({
		{Enums::MinerBaseType::ccminer, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::NeoScrypt, "neoscrypt"),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Lyra2REv2, "lyra2v2"),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Decred, "decred", false),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Lbry, "lbry", false),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::X11Gost, "sib", false),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Blake2s, "blake2s"),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Sia, "sia", false),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Keccak, "keccak"),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Skunk, "skunk"),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::Lyra2z, "lyra2z"),
			new Algorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::X16R, "x16r")
			})},
		{Enums::MinerBaseType::ccminer_alexis, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::ccminer_alexis, Enums::AlgorithmType::X11Gost, "sib", false),
			new Algorithm(Enums::MinerBaseType::ccminer_alexis, Enums::AlgorithmType::Keccak, "keccak")
			})},
		{Enums::MinerBaseType::ethminer, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::ethminer, Enums::AlgorithmType::DaggerHashimoto, "daggerhashimoto")
			})},
		{Enums::MinerBaseType::nheqminer, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::nheqminer, Enums::AlgorithmType::Equihash, "equihash")
			})},
		{Enums::MinerBaseType::EWBF, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::EWBF, Enums::AlgorithmType::Equihash)
			})},
		{Enums::MinerBaseType::dtsm, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::dtsm, Enums::AlgorithmType::Equihash)
			})},
		{Enums::MinerBaseType::excavator, new QList<Algorithm*>({
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Equihash, "equihash"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Pascal, "pascal"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Decred, "decred"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Sia, "sia"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Lbry, "lbry"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Blake2s, "blake2s"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::DaggerHashimoto, "daggerhashimoto"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Lyra2REv2, "lyra2rev2"),
			new DualAlgorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Decred),
			new DualAlgorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Sia),
			new DualAlgorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::DaggerHashimoto, Enums::AlgorithmType::Pascal),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Keccak, "keccak"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::NeoScrypt, "neoscrypt"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::CryptoNightV7, "cryptonightV7"),
			new Algorithm(Enums::MinerBaseType::excavator, Enums::AlgorithmType::Lyra2z, "lyra2z")
			})}
		});

	auto all=All();
	foreach (Enums::MinerBaseType mbt, all->keys()) {
		if (!ret->count(mbt)) {
			ret->insert(mbt, all->value(mbt));
			}
		else {
			foreach (Algorithm* a, *all->value(mbt)) {
				ret->value(mbt)->append(a);
				}
			}
		}
	auto gpu=Gpu();
	foreach (Enums::MinerBaseType mbt, gpu->keys()) {
		if (!ret->count(mbt)) {
			ret->insert(mbt, gpu->value(mbt));
			}
		else {
			foreach (Algorithm* a, *gpu->value(mbt)) {
				ret->value(mbt)->append(a);
				}
			}
		}
	return ret;
}
#endif
