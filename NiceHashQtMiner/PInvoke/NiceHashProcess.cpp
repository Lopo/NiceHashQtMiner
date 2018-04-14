#include "NiceHashProcess.h"
#include "Qt/LException.h"
//#include "Qt/GUILauncher.h"

//#include <QThread>
//#include <QFuture>
//#include <QtConcurrent/QtConcurrent>


bool NiceHashProcess::Start()
{
//	std::string p=program().toStdString();
//	std::string a=arguments().join("|").toStdString();
//	std::string e=environment().join("|").toStdString();
//	std::string pe=processEnvironment().toStringList().join("|").toStdString();
//	std::string wd=workingDirectory().toStdString();

	QString workDir;
	if (workingDirectory().length()>0) {
		workDir=workingDirectory();
		}
	start();
	waitForStarted();
	if (state()==QProcess::NotRunning) {
		throw LException(QString("Failed to start process, err="+errorString()).toLatin1());
		}

//	pHandle=processId();

	Id=processId();

	if (ExitEvent!=nullptr) {
		bRunning=true;
//		tHandle=new QThread;
//		QFuture<void> f=QtConcurrent::run(this, &NiceHashProcess::cThread);
//		QMetaObject::invokeMethod(this, "cThread");
//		tHandle->start();
		}

	return true;

//	GUILauncher gl;
//	gl.moveToThread(this->thread());
//	QCoreApplication::postEvent(&gl, new QEvent(QEvent::User));
//	int r=system(("xterm -e "+program()+" "+(arguments().join(" "))).toStdString().c_str());
	start();
//	QStringList args=arguments();
//	args.removeAll("");
//	startDetached(program(), args);
	waitForStarted(1000);
//	QProcess::ProcessState s=state();
	return state()==QProcess::ProcessState::Starting || state()==QProcess::ProcessState::Running;
//	return !!r;
}

void NiceHashProcess::Kill()
{
	if (tHandle!=nullptr) {
		bRunning=false;
//		tHandle->moveToThread(thread());
		}
	kill();
}

void NiceHashProcess::Close()
{
	if (tHandle!=nullptr) {
		bRunning=false;
//		tHandle->moveToThread(thread());
		}
	close();
}

void NiceHashProcess::cThread()
{
	while (bRunning) {
//		if ()
		}
}
