#define __ExtraLaunchParameters_cpp__
#include "Miners/Parsing/ExtraLaunchParameters.h"
#include "Configs/ConfigJsonFile/FOLDERS.h"
#include "Utils/Helpers.h"
#include "Miners/Parsing/MinerOption.h"
#include <QMetaEnum>


MinerOptionPackageFile::MinerOptionPackageFile(QString name)
	: ConfigFile(Folders::Internals, name+".json", name+".json")
{ }


QList<MinerOptionPackage*> ExtraLaunchParameters::Defaults={
	new MinerOptionPackage(
		Enums::MinerType::ccminer,
		new QList<MinerOption*>({
			new MinerOption("Intensity", "-i", "--intensity=", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ccminer_CUDA_Schedule", "--cuda-schedule=", "0", Enums::MinerOptionFlagType::Uni)
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::ccminer_CryptoNight,
		new QList<MinerOption*>({
			new MinerOption("Launch", "-l", "--launch=", "8x20", Enums::MinerOptionFlagType::MultiParam, ","), // default is 8x20
			new MinerOption("Bfactor", "", "--bfactor=", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("Bsleep", "", "--bsleep=", "0", Enums::MinerOptionFlagType::MultiParam, ",") // TODO check default
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::ethminer_OCL,
		new QList<MinerOption*>({
			new MinerOption("LocalWork", "", "--cl-local-work", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("GlobalWork", "", "--cl-global-work", "0", Enums::MinerOptionFlagType::MultiParam, ",")
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::ethminer_CUDA,
		new QList<MinerOption*>({
			new MinerOption("CudaBlockSize", "", "--cuda-block-size", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("CudaGridSize", "", "--cuda-grid-size", "0", Enums::MinerOptionFlagType::MultiParam, ",")
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::sgminer,
		new QList<MinerOption*>({
			// SingleParam
			new MinerOption("KeccakUnroll", "", "--keccak-unroll", "0", Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("HamsiExpandBig", "", "--hamsi-expand-big", "4", Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Nfactor", "", "--nfactor", "10", Enums::MinerOptionFlagType::SingleParam),
			// MultiParam TODO IMPORTANT check defaults
			new MinerOption("Intensity", "-I", "--intensity", "d", Enums::MinerOptionFlagType::MultiParam, ","), // default is "d" check if -1 works
			new MinerOption("Xintensity", "-X", "--xintensity", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("Rawintensity", "", "--rawintensity", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("ThreadConcurrency", "", "--thread-concurrency", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("Worksize", "-w", "--worksize", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("GpuThreads", "-g", "--gpu-threads", "1", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("LookupGap", "", "--lookup-gap", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			// Uni
			new MinerOption("RemoveDisabled", "--remove-disabled", "--remove-disabled", "", Enums::MinerOptionFlagType::Uni) // default none
			}),
		new QList<MinerOption*>({ // TemperatureOptions
			new MinerOption("GpuFan", "", "--gpu-fan", "30-60", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("TempCutoff", "", "--temp-cutoff", "95", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("TempOverheat", "", "--temp-overheat", "85", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("TempTarget", "", "--temp-target", "75", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("AutoFan", "", "--auto-fan", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("AutoGpu", "", "--auto-gpu", NULL, Enums::MinerOptionFlagType::Uni)
			})
		),
	new MinerOptionPackage(
		Enums::MinerType::cpuminer_opt,
		new QList<MinerOption*>({
			new MinerOption("Threads", "-t", "--threads=", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("CpuAffinity", "", "--cpu-affinity", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("CpuPriority", "", "--cpu-priority", "-1", Enums::MinerOptionFlagType::MultiParam, ",") // default
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::nheqminer_CPU,
		new QList<MinerOption*>({
			new MinerOption("Threads", "-t", "-t", "-1", Enums::MinerOptionFlagType::SingleParam, " ") // default none
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::nheqminer_CUDA,
		new QList<MinerOption*>({
			new MinerOption("Solver_Version", "-cv", "-cv", "0", Enums::MinerOptionFlagType::SingleParam, " "), // default 0
			new MinerOption("Solver_Block", "-cb", "-cb", "0", Enums::MinerOptionFlagType::MultiParam, " "), // default 0
			new MinerOption("_Solver_Thread", "-ct", "-ct", "0", Enums::MinerOptionFlagType::MultiParam, " ") // default 0
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::eqm_CUDA,
		new QList<MinerOption*>({
			new MinerOption("Solver_Mode", "-cm", "-cm", "0", Enums::MinerOptionFlagType::MultiParam, " ") // default 0
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::ClaymoreZcash,
		new QList<MinerOption*>({
			new MinerOption("ClaymoreZcash_a", "-a", "-a", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_asm", "-asm", "-asm", "1", Enums::MinerOptionFlagType::MultiParam, ","),

			new MinerOption("ClaymoreZcash_i", "-i", "-i", "6", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_wd", "-wd", "-wd", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreZcash_nofee", "-nofee", "-nofee", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreZcash_li", "-li", "-li", "0", Enums::MinerOptionFlagType::MultiParam, ","),

			// MinerOptionFlagType.MultiParam might not work corectly due to ADL indexing so use single param to apply to all
			new MinerOption("ClaymoreZcash_cclock", "-cclock", "-cclock", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_mclock", "-mclock", "-mclock", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_powlim", "-powlim", "-powlim", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_cvddc", "-cvddc", "-cvddc", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_mvddc", "-mvddc", "-mvddc", "0", Enums::MinerOptionFlagType::MultiParam, ",")
			}),
		new QList<MinerOption*>({ // temperature stuff
			// MinerOptionFlagType.MultiParam might not work corectly due to ADL indexing so use single param to apply to all
			new MinerOption("ClaymoreZcash_tt", "-tt", "-tt", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreZcash_ttli", "-ttli", "-ttli", "70", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreZcash_tstop", "-tstop", "-tstop", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreZcash_fanmax", "-fanmax", "-fanmax", "100", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreZcash_fanmin", "-fanmin", "-fanmin", "0", Enums::MinerOptionFlagType::MultiParam, ",")
			})
		),
	new MinerOptionPackage(
		Enums::MinerType::ClaymoreCryptoNight,
		new QList<MinerOption*>({
			new MinerOption("ClaymoreCryptoNight_a", "-a", "-a", "0", Enums::MinerOptionFlagType::MultiParam),
			new MinerOption("ClaymoreCryptoNight_wd", "wd", "-wd", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreCryptoNight_nofee", "-nofee", "-nofee", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreCryptoNight_li", "-li", "-li", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_h", "-h", "-h", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_allpools", "-allpools", "0", Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("ClaymoreCryptoNight_cclock", "-cclock", "-cclock", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_mclock", "-mclock", "-mclock", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_powlim", "-powlim", "-powlim", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_cvddc", "-cvddc", "-cvddc", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_mvddc", "-mvddc", "-mvddc", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_pow7", "-pow7", "-pow7", "0", Enums::MinerOptionFlagType::SingleParam, ",")
			}),
		new QList<MinerOption*>({ // temperature stuff
			// MinerOptionFlagType.MultiParam might not work corectly due to ADL indexing so use single param to apply to all
			new MinerOption("ClaymoreCryptoNight_tt", "-tt", "-tt", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreCryptoNight_tstop", "-tstop", "-tstop", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreCryptoNight_fanmax", "-fanmax", "-fanmax", "100", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreCryptoNight_fanmin", "-fanmin", "-fanmin", "0", Enums::MinerOptionFlagType::MultiParam, ",")
			})
		),
	new MinerOptionPackage(
		Enums::MinerType::OptiminerZcash,
		new QList<MinerOption*>({
			new MinerOption("ForceGenericKernel", "--force-generic-kernel", "--force-generic-kernel", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("ExperimentalKernel", "--experimental-kernel", "--experimental-kernel", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("nodevfee", "--nodevfee", "--nodevfee", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("Intensity", "-i", "--intensity", "0", Enums::MinerOptionFlagType::DuplicateMultiParam),
			new MinerOption("PciMode", "--pci-mode", "--pci-mode", "", Enums::MinerOptionFlagType::SingleParam)
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::excavator,
		new QList<MinerOption*> ({
			// parameters differ according to algorithm
			new MinerOption("parameter1", "-c1", "-c1", "0", Enums::MinerOptionFlagType::MultiParam, " "),
			new MinerOption("parameter2", "-c2", "-c2", "0", Enums::MinerOptionFlagType::MultiParam, " "),
			new MinerOption("debug_param", "-f", "-f", "", Enums::MinerOptionFlagType::SingleParam, " ")
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::ClaymoreDual,
		new QList<MinerOption*>({
			new MinerOption("ClaymoreDual_etha", "-etha", "-etha", "-1", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_ethi", "-ethi", "-ethi", "8", Enums::MinerOptionFlagType::MultiParam, ","),

			new MinerOption("ClaymoreDual_eres", "-eres", "-eres", "2", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_erate", "-erate", "-erate", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_estale", "-estale", "-estale", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_gser", "-gser", "-gser", "0", Enums::MinerOptionFlagType::SingleParam, ","),

			new MinerOption("ClaymoreDual_wd", "-wd", "-wd", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_retrydelay", "-retrydelay", "-retrydelay", "20", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_nofee", "-nofee", "-nofee", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_li", "-li", "-i", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_lidag", "-lidag", "-lidag", "0", Enums::MinerOptionFlagType::MultiParam, ","),

			// MinerOptionFlagType.MultiParam might not work corectly due to ADL indexing so use single param to apply to all
			new MinerOption("ClaymoreDual_cclock", "-cclock", "-cclock", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_mclock", "-mclock", "-mclock", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_powlim", "-powlim", "-powlim", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_cvddc", "-cvddc", "-cvddc", "0", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_mvddc", "-mvddc", "-mvddc", "0", Enums::MinerOptionFlagType::MultiParam, ","),

			// other and dual mining features
			new MinerOption("ClaymoreDual_etht", "-etht", "-etht", "200", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_allcoins", "-allcoins", "-allcoins", "", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_r", "-r", "-r", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_mode", "-mode", "-mode", "0", Enums::MinerOptionFlagType::SingleParam, ","), // this is not well supported in MultiParam

			new MinerOption("ClaymoreDual_ftime", "-ftime", "-ftime", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_eres", "-eres", "-eres", "2", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_dpool", "-dpool", "-dpool", "", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_dwal", "-dwal", "-dwal", "", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_dpsw", "-dpsw", "-dpsw", "", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_dcoin", "-dcoin", "-dcoin", "", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_dcri", "-dcri", "-dcri", "30", Enums::MinerOptionFlagType::MultiParam, ","),

			new MinerOption("ClaymoreDual_dcrt", "-dcrt", "-dcrt", "5", Enums::MinerOptionFlagType::SingleParam, ","),
			// v9.0
			new MinerOption("ClaymoreDual_asm", "-asm", "-asm", "1", Enums::MinerOptionFlagType::MultiParam, ",")
			}),
		new QList<MinerOption*>({ // temperature stuff
			// MinerOptionFlagType.MultiParam might not work corectly due to ADL indexing so use single param to apply to all
			new MinerOption("ClaymoreDual_tt", "-tt", "-tt", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_ttdcr", "-ttdcr", "-ttdcr", "1", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_ttli", "-ttli", "-ttli", "70", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_tstop", "-tstop", "-tstop", "0", Enums::MinerOptionFlagType::SingleParam, ","),
			new MinerOption("ClaymoreDual_fanmax", "-fanmax", "-fanmax", "100", Enums::MinerOptionFlagType::MultiParam, ","),
			new MinerOption("ClaymoreDual_fanmin", "-fanmin", "-fanmin", "0", Enums::MinerOptionFlagType::MultiParam, ",")
			})
		),
	new MinerOptionPackage(
		Enums::MinerType::EWBF,
		new QList<MinerOption*>({ // parameters differ according to algorithm
			new MinerOption("EWBF_fee", "--fee", "--fee", "0", Enums::MinerOptionFlagType::SingleParam, " "),
			new MinerOption("EWBF_templimit", "--templimit", "--templimit", "90", Enums::MinerOptionFlagType::SingleParam, " "),
			new MinerOption("EWBF_tempunits", "--tempunits", "--tempunits", "C", Enums::MinerOptionFlagType::SingleParam, " "),
			new MinerOption("EWBF_eexit", "--eexit", "--eexit", NULL, Enums::MinerOptionFlagType::SingleParam, " "),
			new MinerOption("EWBF_solver", "--solver", "--solver", "0", Enums::MinerOptionFlagType::MultiParam, " "),
			new MinerOption("EWBF_intensity", "--intensity", "--intensity", "64", Enums::MinerOptionFlagType::MultiParam, " "),
			new MinerOption("EWBF_powercalc", "--pec", "--pec", NULL, Enums::MinerOptionFlagType::Uni, " ")
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::Xmrig,
		new QList<MinerOption*>({
			new MinerOption("Xmrig_fee", "--donate-level=", "0", Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Xmrig_threads", "-t", "--threads=", NULL, Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Xmrig_av", "--av=", "0", Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Xmrig_affinity", "--cpu-affinity", NULL, Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Xmrig_priority", "--cpu-priority", NULL, Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Xmrig_nohugepages", "--no-huge-pages", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("Xmrig_nocolor", "--no-color", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("Xmrig_maxusage", "---max-cpu-usage=", "75", Enums::MinerOptionFlagType::SingleParam),
			new MinerOption("Xmrig_safe", "--safe", NULL, Enums::MinerOptionFlagType::Uni)
			}),
		new QList<MinerOption*>()
		),
	new MinerOptionPackage(
		Enums::MinerType::dtsm,
		new QList<MinerOption*>({
			new MinerOption("dtsm_time", "--time", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("dtsm_noreconnect", "--noreconnect", NULL, Enums::MinerOptionFlagType::Uni),
			new MinerOption("dtsm_temp-target", "--temp-target", NULL, Enums::MinerOptionFlagType::SingleParam)
			}),
		new QList<MinerOption*>()
		)
	};

QList<MinerOptionPackage*>* ExtraLaunchParameters::MinerOptionPackages=new QList<MinerOptionPackage*>;


void ExtraLaunchParameters::InitializePackages()
{
	foreach (MinerOptionPackage* pack, Defaults) {
		QString packageName="MinerOptionPackage_"+pack->Name;
		MinerOptionPackageFile* packageFile=new MinerOptionPackageFile(packageName);
		MinerOptionPackage* readPack=packageFile->ReadFile();
		if (readPack==nullptr) { // read has failed
			Helpers::ConsolePrint("ExtraLaunchParameter", "Creating internal params config "+packageName);
			MinerOptionPackages->append(pack);
			// create defaults
			packageFile->Commit(pack);
			}
		else {
			Helpers::ConsolePrint("ExtraLaunchParameters", "Loading internal params config "+packageName);
			MinerOptionPackages->append(readPack);
			}
		}
	QList<Enums::MinerType> defaultKeys;
	foreach (MinerOptionPackage* p, Defaults) {
		defaultKeys.append(p->Type);
		}
	// extra check if DEFAULTS is missing a key
	for (int type=(int)(Enums::MinerType::NONE)+1; type<(int)Enums::MinerType::END; ++type) {
		if (!defaultKeys.contains((Enums::MinerType)type)) {
			QString packageName=QString("MinerOptionPackage_")+QMetaEnum::fromType<Enums::MinerType>().valueToKey((int)type);
			MinerOptionPackageFile* packageFile=new MinerOptionPackageFile(packageName);
			MinerOptionPackage* readPack=packageFile->ReadFile();
			if (readPack!=nullptr) { // read has failed
				Helpers::ConsolePrint("ExtraLaunchParameters", "Creating internal params config "+packageName);
				MinerOptionPackages->append(readPack);
				}
			}
		}
}

MinerOptionPackage* ExtraLaunchParameters::GetMinerOptionPackageForMinerType(Enums::MinerType type)
{
	foreach (MinerOptionPackage* index, *MinerOptionPackages) {
		if (index->Type==type) {
			return index;
			}
		}
	// if none found
	return new MinerOptionPackage(Enums::MinerType::NONE, new QList<MinerOption*>, new QList<MinerOption*>);
}
