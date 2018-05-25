#ifndef ENUMS_H_
#define ENUMS_H_

#include <QObject>


class Enums
	: public QObject
{
	Q_OBJECT
public:
	Enums();
	~Enums();

	enum class AlgorithmBenchmarkSettingsType {
		SelectedUnbenchmarkedAlgorithms,
		UnbenchmarkedAlgorithms,
		ReBenchSelectedAlgorithms,
		AllAlgorithms
		};
	Q_ENUM(AlgorithmBenchmarkSettingsType)

	enum AlgorithmType {
		DaggerKeccak=-8,
		DaggerBlake2s=-7,
		DaggerSia=-6,
		DaggerDecred=-5,
		DaggerLbry=-4,
		DaggerPascal=-3,
		INVALID=-2,
		NONE=-1,

		Keccak=5,
		Nist5=7,

		Scrypt_UNUSED=0,
		SHA256_UNUSED=1,
		ScryptNf_UNUSED=2,
		X11_UNUSED=3,
		X13_UNUSED=4,
//		Keccak_UNUSED=5,
		X15_UNUSED=6,
//		Nist5_UNUSED=7,

		WhirlpoolX_UNUSED=10,
		Qubit_UNUSED=11,
		Quark_UNUSED=12,
		Axiom_UNUSED=13,

		ScryptJaneNf_UNUSED=15,
		Blake256r8_UNUSED=16,
		Blake256r14_UNUSED=17,
		Blake256r8vnl_UNUSED=18,

		NeoScrypt=8,
		Lyra2RE=9,

		Lyra2REv2=14,

		Hodl=19,
		DaggerHashimoto=20,
		Decred=21,
		CryptoNight=22,
		Lbry=23,
		Equihash=24,
		Pascal=25,
		X11Gost=26,
		Sia=27,
		Blake2s=28,
		Skunk=29,
		CryptoNightV7=30
		};
	Q_ENUM(AlgorithmType)

	enum class BenchmarkPerformanceType {
		Quick=0,
		Standard,
		Precise
		};
	Q_ENUM(BenchmarkPerformanceType)

	enum class BenchmarkProcessStatus {
		NONE,
		Idle,
		Running,
		Killing,
		DoneKilling,
		Finished,
		Success
		};
	Q_ENUM(BenchmarkProcessStatus)

	enum class CPUExtensionType {
		Automatic=0,
		AVX2_AES=1,
		AVX2=2,
		AVX_AES=3,
		AVX=4,
		AES=5,
		SSE2=6
		};
	Q_ENUM(CPUExtensionType)

	enum class DagGenerationType {
		SingleKeep=0,
		Single,
		Sequential,
		Parallel,
		END
		};
	Q_ENUM(DagGenerationType)

	enum class DeviceGroupType {
		NONE=-1,
		CPU=0,
		AMD_OpenCL,
		NVIDIA_2_1,
		NVIDIA_3_x,
		NVIDIA_5_x,
		NVIDIA_6_x,
		LAST
		};
	Q_ENUM(DeviceGroupType)

	enum class DeviceMiningStatus {
		Disabled,
		NoEnabledAlgorithms,
		DeviceNull,
		CanMine
		};
	Q_ENUM(DeviceMiningStatus)

	enum class DeviceType {
		CPU=0,
		NVIDIA,
		AMD
		};
	Q_ENUM(DeviceType)

	enum class LanguageType {
		En=0,
		Ru,
		Es,
		Pt,
		Bg,
		It,
		Pl,
		Zh_cn,
		Ro
		};
	Q_ENUM(LanguageType)

	enum class MinerApiReadStatus {
		NONE,
		WAIT,
		GOT_READ,
		READ_SPEED_ZERO,
		NETWORK_EXCEPTION,
		RESTART
		};
	Q_ENUM(MinerApiReadStatus)

	enum class MinerBaseType {
		NONE=0,
		cpuminer,
		ccminer,
		sgminer,
		nheqminer,
		eqm,
		ethminer,
		Claymore,
		OptiminerAMD,
		excavator,
		XmrStak,
		ccminer_alexis,
		experimental,
		EWBF,
		Prospector,
		Xmrig,
		XmrStakAMD,
		Claymore_old,
		dtsm,
		END
		};
	Q_ENUM(MinerBaseType)

	enum class MinerOptionFlagType {
		Uni,
		SingleParam,
		MultiParam,
		DuplicateMultiParam // the flag is multiplied
		};
	Q_ENUM(MinerOptionFlagType)

	enum class MinerStopType {
		SWITCH,
		END,
		FORCE_END
		};
	Q_ENUM(MinerStopType)

	enum class MinerType {
		NONE,
		ccminer,
		ccminer_CryptoNight,
		ethminer_OCL,
		ethminer_CUDA,
		sgminer,
		cpuminer_opt,
		nheqminer_CPU,
		nheqminer_CUDA,
		nheqminer_AMD,
		eqm_CPU,
		eqm_CUDA,
		ClaymoreZcash,
		ClaymoreCryptoNight,
		OptiminerZcash,
		excavator,
		ClaymoreDual,
		EWBF,
		Xmrig,
		dtsm,
		END
		};
	Q_ENUM(MinerType)

	enum class NhmConectionType {
		NONE,
		STRATUM_TCP,
		STRATUM_SSL,
		LOCKED // inhouse miners that are locked on NH (our eqm)
		};
	Q_ENUM(NhmConectionType)

	enum class TimeUnitType {
		Hour=0,
		Day,
		Week,
		Month,
		Year
		};
	Q_ENUM(TimeUnitType)

	enum class Use3rdPartyMiners {
		NOT_SET=0,
		NO,
		YES
		};
	Q_ENUM(Use3rdPartyMiners)
};

#endif
