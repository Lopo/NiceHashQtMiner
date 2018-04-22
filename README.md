# NiceHashQtMiner


NiceHash Qt Miner is a try for Linux port of [NiceHash Miner Legacy](https://github.com/nicehash/NiceHashMinerLegacy)

<img src="http://temp.losys.sk/NHQtM001.png">

:warning: This is development version !!! It's not stable enough for "production" usage and requires some knowledge to make it running/usable !!! :warning:

## Dependencies
- CMake
- Qt5
- CUDA
- OpenCL
- Boost
	- filesystem
	- thread


## Build
```
	cd builddir && cmake .. && make
```

## Usage
Compile/copy miners to appropriate dirs, some examples (I'll test and fill the following table later):

dir | miner
------------ | -------------
bin/bin/excavator | [Excavator](https://github.com/nicehash/excavator)
bin/bin/xmrig | [XMRig](https://github.com/xmrig/xmrig)
bin/bin/bin_3rdparty/claymore_dual | [Claymore](https://mega.nz/#F!O4YA2JgD!n2b4iSHQDruEsYUvTQP5_w)
bin/bin/ccminer_x11gost | [ccminer_alexis](https://github.com/nicehash/ccminer-x11gost/tree/windows)

Then just run bin/NiceHashQtminer and use the same way as NHML

Not all miners already works (implementation problems, unfinished/untested code etc.), for some miners just do benchmark manually and write benched values in settings
If miner binary is missing, NHQtM can crash/stop working ... kill/restart it, disable that miner in settings and continue using

## Donations
* BTC: `1QATWNM7AePyHMGuQxBLTuq76avoDoWsFc`
