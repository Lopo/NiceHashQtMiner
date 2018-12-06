#include <qt/QtCore/qjsonarray.h>
#include <qt/QtCore/qjsondocument.h>

#include "Miners/XmrStak/Configs/XmrStakConfigCpu.h"


XmrStakConfigCpu::XmrStakConfigCpu(int numberOfthreads)
{
	cpu_thread_num=numberOfthreads;
}

void XmrStakConfigCpu::Inti_cpu_threads_conf(bool low_power_mode, bool no_prefetch, bool affine_to_cpu, bool isHyperThreading)
{
	cpu_threads_conf.clear();
	if (isHyperThreading) {
		for (int i_cpu=0; i_cpu<cpu_thread_num; ++i_cpu) {
			QJsonObject o;
			o.insert("low_power_mode", low_power_mode);
			o.insert("no_prefetch", no_prefetch);
			o.insert("asm", "auto");
			o.insert("affine_to_cpu", (i_cpu*2)+1);
			cpu_threads_conf.append(o);
			}
		}
	else {
		for (int i_cpu=0; i_cpu<cpu_thread_num; ++i_cpu) {
			QJsonObject o;
			o.insert("low_power_mode", low_power_mode);
			o.insert("no_prefetch", no_prefetch);
			o.insert("asm", "auto");
			o.insert("affine_to_cpu", affine_to_cpu? i_cpu : false);
			cpu_threads_conf.append(o);
			}
		}
}

QJsonObject XmrStakConfigCpu::asJsonObject()
{
	QJsonObject o;
	QJsonArray ctcArr;
	foreach (QJsonObject ctc, cpu_threads_conf) {
		ctcArr.append(ctc);
		}
	o.insert("cpu_threads_conf", ctcArr);
	o.insert("cpu_thread_num", cpu_thread_num);
	return o;
}

XmrStakConfigCpu* XmrStakConfigCpu::fromJson(QString json)
{
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();
	XmrStakConfigCpu* ret=new XmrStakConfigCpu(o.value("cpu_thread_num").toInt());
	ret->cpu_threads_conf.clear();
	foreach (QJsonValue ctcV, o.value("cpu_threads_conf").toArray()) {
		ret->cpu_threads_conf.append(ctcV.toObject());
		}
	return ret;
}
