execute_process(
	COMMAND chown root ${CMAKE_INSTALL_PREFIX}/bin/NiceHashQtMiner
	COMMAND chmod u+s  ${CMAKE_INSTALL_PREFIX}/bin/NiceHashQtMiner
	)
