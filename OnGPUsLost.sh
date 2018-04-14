#!/bin/sh

#:: This script contains two option how you can "repair" lost GPU
#:: Default option (option 1) will restart your entire rig, you can also uncomment "import" to make a screenshot befor reboot (you need to install ImageMagick)
#:: Second option will use rmmod/modprobe to restart display drivers
#:: Option 1 is used by default, if you whant to use Option 2 you need to change line "OPTION=1" to "OPTION=2"

#::Select desire option here
OPTION=1 

if [ $OPTION -eq 2 ]; then
	#:: Option 2: close NHML, restart display drivers, start NHML, nvidiaInspector is required in NHML directory
	echo %DATE% %TIME% Lost GPU >> logs\GPU_Lost.txt
	#import -window root "logs/GPU_Lost_$(date -Iseconds).jpg"
	killall NiceHashQtMiner
	sleep 3
	#NV_Inspector\nvidiaInspector.exe -restartDisplayDriver
	rmmod nvidia
	modprobe nvidia
	#:: It is possible that this timeout should be increased when more GPUs are present
	sleep 12
	#:: Save your custom config for all GPUs under same profile number, if you use default config for your GPUs this step is not required
	#"c:\Program Files (x86)\MSI Afterburner\MSIAfterburner.exe" -Profile2
	echo $(date +"%F %T") Profile 2 set >> logs\GPU_Lost.txt
	sleep 4
	exec NiceHashQtMiner
else
	#:: Option 1: restart RIG
	date +"%F %T" >> logs\GPU_Lost.txt
	#import -window root "logs/GPU_Lost_$(date -Iseconds).jpg"
	shutdown -r 0
	exit
fi
