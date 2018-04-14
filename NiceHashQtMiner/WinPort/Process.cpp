#include "WinPort/Process.h"
#include <QDir>
#include <QRegularExpression>
#include <csignal>


QList<pid_t>* Process::GetProcessesByName(QString procName)
{
	QList<pid_t>* ret=new QList<pid_t>;

	// Open the /proc directory
	QDir dp("/proc");
	foreach (QFileInfo dirp, dp.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
		if (dirp.fileName().contains(QRegularExpression("[^\\d]"))) {
			continue;
			}
//std::string fn=dirp.fileName().toStdString();
		QFile cmdPath("/proc/"+dirp.fileName()+"/cmdline");
		cmdPath.open(QIODevice::ReadOnly);
		QString cmdLine=cmdPath.readLine();
//std::string cl=cmdLine.toStdString();
		if (cmdLine!="") {
			auto pos=cmdLine.lastIndexOf('/');
			QString pName=cmdLine.mid(pos+1);
//std::string pn=pName.toStdString();
			if (pName==procName) {
				ret->append(dirp.fileName().toInt());
				}
			}
		}
/*
	DIR *dp=opendir("/proc");
	if (dp!=NULL) {
		// Enumerate all entries in directory until process found
		struct dirent *dirp;
		while (pid<0&&(dirp=readdir(dp))) {
			// Skip non-numeric entries
			int id=atoi(dirp->d_name);
			if (id>0) {
				// Read contents of virtual /proc/{pid}/cmdline file
				string cmdPath=string("/proc/")+dirp->d_name+"/cmdline";
				ifstream cmdFile(cmdPath.c_str());
				string cmdLine;
				getline(cmdFile, cmdLine);
				if (!cmdLine.empty()) {
					// Keep first cmdline item which contains the program path
					size_t pos=cmdLine.find('\0');
					if (pos!=string::npos)
						cmdLine=cmdLine.substr(0, pos);
					// Keep program name only, removing the path
					pos=cmdLine.rfind('/');
					if (pos!=string::npos)
						cmdLine=cmdLine.substr(pos+1);
					// Compare against requested process name
					if (procName==cmdLine)
						pid=id;
				}
			}
		}
	}

	closedir(dp);
*/
	return ret;
}

bool Process::CheckProcessById(int processId)
{
	return !::kill(processId, 0);
}
