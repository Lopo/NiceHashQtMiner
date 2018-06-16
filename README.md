# NiceHashQtMiner


NiceHash Qt Miner is a try for Linux port of [NiceHash Miner Legacy](https://github.com/nicehash/NiceHashMinerLegacy)

<img src="http://temp.losys.sk/Screenshot_20180616_142252.png">

:warning: This is development version !!! It's not stable enough for "production" usage and requires some knowledge to make it running/usable !!! :warning:

## Dependencies
- CMake
- Qt5
- Log4Qt
- CUDA
- OpenCL
- ADL
- Boost
	- filesystem
	- thread
- hwinfo
- libcpuid


## Build
```
	cd builddir && cmake .. && make
```

## Usage
Compile/copy miners to appropriate dirs, some examples (I'll test and fill the following table later):

dir | miner | notes
----------- | ----------- | -----------
bin/xmr-stak | [XMR-Stak](https://github.com/nicehash/xmr-stak) | fDevDonationLevel set to 0
bin/xmrig | [XMRig](https://github.com/xmrig/xmrig) | kDefaultDonateLevel & kMinimumDonateLevel set to 0
bin/xmrig | [XMRigAmd](https://github.com/xmrig/xmrig-amd) | kDefaultDonateLevel & kMinimumDonateLevel set to 0
bin/xmrig | [XMRigNvidia](https://github.com/xmrig/xmrig-nvidia) | kDefaultDonateLevel & kMinimumDonateLevel set to 0
bin/ccminer_cryptonight | [ccminer](https://github.com/KlausT/ccminer-cryptonight) | untested
bin/ccminer_decred | | unknown
bin/excavator | [Excavator](https://github.com/nicehash/excavator/releases/tag/v1.5.4a) | 
bin_3rdparty/claymore_cryptonight | [Claymore](https://mega.co.nz/#F!e4JVEAIJ!l1iF4z10fMyJzY5-LnyC2A) | 
bin_3rdparty/claymore_dual | [Claymore](https://mega.nz/#F!O4YA2JgD!n2b4iSHQDruEsYUvTQP5_w) | 
bin_3rdparty/claymore_zcash | [Claymore](https://mega.nz/#F!P0ZjFDjC!Cmb-ZRqlbAnNrajRtp4zvA) | 
bin_3rdparty/dtsm | [dstm](https://bitcointalk.org/index.php?topic=2021765.0) | 
bin_3rdparty/ewbf | [EWBF](https://bitcointalk.org/index.php?topic=1707546) | 
bin_3rdparty/optiminer_zcash | [OptiminerZcash](https://github.com/Optiminer/OptiminerZcash) | 
bin_3rdparty/prospector | [Prospector](https://github.com/semtexzv/Prospector) | 
bin/ccminer_x11gost | [ccminer_alexis](https://github.com/nicehash/ccminer-x11gost/tree/windows) | 

Then just run bin/NiceHashQtminer and use the same way as NHML

Not all miners already works (implementation problems, unfinished/untested code etc.), for some miners just do benchmark manually and write benched values in settings
If miner binary is missing, NHQtM can crash/stop working ... kill/restart it, disable that miner in settings and continue using

## Donations
* BTC: `1QATWNM7AePyHMGuQxBLTuq76avoDoWsFc`
