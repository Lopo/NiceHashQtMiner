#include <QProcess>
#include <iostream>


int main(int argc, char* argv[])
{
	QByteArray stdOut, stdErr;
	QString args, smiPath="nvidia-smi";

	try {
		QProcess* P=new QProcess;
		P->setProgram(smiPath);
		P->setArguments({"--list-gpus"});
		P->start();
		P->waitForFinished(-1);
		
		stdOut=P->readAllStandardOutput();
		stdErr=P->readAllStandardOutput();
		delete P;
		}
	catch (std::exception ex) {
		std::cout << "[ChangeP0State] Exception: " << ex.what() << std::endl;
		return 1;
		}

	if (stdOut.length()<10) {
		std::cout << "[ChangeP0State] NVSMI: Error! Output too short. (" << stdOut.toStdString() << ")" << std::endl;
		return 2;
		}

	int numGPUs=stdOut.split('\n').count()-1;
	std::cout << "[ChangeP0State] Num GPUs: " << numGPUs << std::endl;

	for (int i=0; i<numGPUs; i++) {
		QString mem, clk;
		mem= clk= "";

		try {
			args="-i "+QString::number(i)+" -q -d SUPPORTED_CLOCKS";
			std::cout << "[ChangeP0State] GetClocks Start Process: " << args.toStdString() << std::endl;
			QProcess* GetClocks=new QProcess;
			GetClocks->setProgram(smiPath);
			GetClocks->setArguments({args});
			GetClocks->start();
			GetClocks->waitForFinished();

			foreach (QByteArray outdata, GetClocks->readAllStandardOutput().split('\n')) {
				if (outdata=="") {
					continue;
					}
				if (outdata.contains("Memory")) {
					mem=outdata.split(':').at(1).trimmed();
					mem=mem.mid(0, mem.length()-4);
					}
				else if (outdata.contains("Graphics")) {
					clk=outdata.split(':').at(1).trimmed();
					clk=clk.mid(0, clk.length()-4);
					break;
					}
				}

			GetClocks->close();
			delete GetClocks;
			}
		catch (std::exception ex) {
			std::cout << "[ChangeP0State] Exception: " << ex.what() << std::endl;
			}

		if (mem.length()>1 && clk.length()>1) {
			try {
				args="-i "+QString::number(i)+" -ac "+mem+","+clk;
				std::cout << "[ChangeP0State] SetClock Start Process: " << args.toStdString() << std::endl;
				QProcess* SetClock=new QProcess;
				SetClock->setProgram(smiPath);
				SetClock->setArguments({args});
				SetClock->start();

				QByteArray outdata=SetClock->readAllStandardOutput();
				std::cout << "[ChangeP0State] SetClock: " << outdata.toStdString() << std::endl;
				if (outdata.contains("Applications clocks set to")) {
					std::cout << "[ChangeP0State] SetClock: Successfully set." << std::endl;
					}
				else if (outdata.contains("is not supported")) {
					std::cout << "[ChangeP0State] SetClock: Setting applications clocks is not supported." << std::endl;
					}
				else if (outdata.contains("does not have permission")) {
					std::cout << "[ChangeP0State] SetClock: The current user does not have permission to change clocks." << std::endl;
					}
				delete SetClock;
				}
			catch (std::exception ex) {
				std::cout << "[ChangeP0State] Exception: " << ex.what() << std::endl;
				}
			}
		}

	return 0;
}
