#include <qt/QtCore/qlist.h>
#include <qt/QtCore/qjsonarray.h>
#include <qt/QtCore/qjsondocument.h>

#include "Miners/XmrStak/Configs/XmrStakConfigGpu.h"
#include "Utils/Helpers.h"


XmrStakGpuItem::XmrStakGpuItem(int index, bool affineToCpu)
{
	this->index=index;
	affine_to_cpu=affineToCpu;
}

template<class I>
void XmrStakConfigGpu<I>::SetupThreads(QList<int> indices)
{
	QList<int> indexList=indices;
	// Remove threads without an activated index
	foreach (I* x, gpu_threads_conf) {
		if (!indexList.contains(((XmrStakGpuItem*)x)->index)) {
			gpu_threads_conf.removeAll(x);
			}
		}
	foreach (int i, indexList) {
		bool a=true;
		foreach (XmrStakGpuItem* x, gpu_threads_conf) {
			if (x->index==i) {
				a=false;
				break;
				}
			}
		if (a) {
			Helpers::ConsolePrint("Xmr-Stak-Config", QString("GPU entry for index %1 not found, setting with default. Performance will likely not be optimal.").arg(i));
			gpu_threads_conf.append(GetGpuItem(i, false));
			}
		}
}

XmrStakAmdItem::XmrStakAmdItem(int index, bool affine)
	: XmrStakGpuItem(index, affine)
{ }

XmrStakAmdItem* XmrStakConfigAmd::GetGpuItem(int index, bool affine)
{
	return new XmrStakAmdItem(index, affine);
}

XmrStakNvidiaItem::XmrStakNvidiaItem(int index, bool affine)
	: XmrStakGpuItem(index, affine)
{ }

XmrStakNvidiaItem* XmrStakConfigNvidia::GetGpuItem(int index, bool affine)
{
	return new XmrStakNvidiaItem(index, affine);
}

void XmrStakConfigNvidia::OverrideBVals()
{
	foreach (XmrStakNvidiaItem* thread, gpu_threads_conf) {
		thread->bfactor=8;
		thread->bsleep=100;
		}
}

QJsonObject XmrStakGpuItem::asJsonObject()
{
	QJsonObject o;
	o.insert("index", index);
	o.insert("affine_to_cpu", affine_to_cpu);
	return o;
}

void XmrStakGpuItem::loadJsonObject(QJsonObject obj)
{
	index=obj.value("index").toInt();
	affine_to_cpu=obj.value("affine_to_cpu").toBool();
}

template<class I>
QJsonObject XmrStakConfigGpu<I>::asJsonObject()
{
	QJsonObject o;
	QJsonArray gtcArr;
	foreach (IasJsonObject* gtc, gpu_threads_conf) {
		gtcArr.append(gtc->asJsonObject());
		}
	o.insert("gpu_threads_conf", gtcArr);
	return o;
}

template<class I>
void XmrStakConfigGpu<I>::loadJsonObject(QJsonObject obj)
{
	gpu_threads_conf.clear();
	foreach (QJsonValue gtcV, obj.value("gpu_threads_conf").toArray()) {
		I* gtc=I::fromJsonObject(gtcV.toObject());
		if (gtc!=nullptr) {
			gpu_threads_conf.append(gtc);
			}
		else {
			Helpers::ConsolePrint("Xmr-Stak-Config GPU", "Error parsing cfg");
			}
		}
}

QJsonObject XmrStakAmdItem::asJsonObject()
{
	QJsonObject o=XmrStakGpuItem::asJsonObject();
	o.insert("intensity", intensity);
	o.insert("worksize", worksize);
	o.insert("strided_index", strided_index);
	o.insert("mem_chunk", mem_chunk);
	o.insert("comp_mode", comp_mode);
	return o;
}

XmrStakAmdItem* XmrStakAmdItem::fromJsonObject(QJsonObject obj)
{
	XmrStakAmdItem* ret=new XmrStakAmdItem(0, 0);
	ret->loadJsonObject(obj);
	ret->intensity=obj.value("intensity").toInt();
	ret->worksize=obj.value("worksize").toInt();
	ret->strided_index=obj.value("strided_index").toBool();
	ret->mem_chunk=obj.value("mem_chunk").toInt();
	ret->comp_mode=obj.value("comp_mode").toBool();
	return ret;
}

QJsonObject XmrStakConfigAmd::asJsonObject()
{
	QJsonObject o=XmrStakConfigGpu<XmrStakAmdItem>::asJsonObject();
	o.insert("platform_index", platform_index);
	return o;
}

XmrStakConfigAmd* XmrStakConfigAmd::fromJson(QString json) // @todo finish
{
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();
	XmrStakConfigAmd* ret=new XmrStakConfigAmd;
	ret->loadJsonObject(o);
	ret->platform_index=o.value("platform_index").toInt();
	return ret;
}

QJsonObject XmrStakNvidiaItem::asJsonObject()
{
	QJsonObject o=XmrStakGpuItem::asJsonObject();
	o.insert("threads", threads);
	o.insert("block", block);
	o.insert("bfactor", bfactor);
	o.insert("bsleep", bsleep);
	o.insert("sync_mode", sync_mode);
	return o;
}

XmrStakNvidiaItem* XmrStakNvidiaItem::fromJsonObject(QJsonObject obj)
{
	XmrStakNvidiaItem* ret=new XmrStakNvidiaItem(0, 0);
	ret->loadJsonObject(obj);
	ret->threads=obj.value("threads").toInt();
	ret->block=obj.value("block").toInt();
	ret->bfactor=obj.value("bfactor").toInt();
	ret->bsleep=obj.value("bsleep").toInt();
	ret->sync_mode=obj.value("sync_mode").toInt();
	return ret;
}

XmrStakConfigNvidia* XmrStakConfigNvidia::fromJson(QString json)
{
	XmrStakConfigNvidia* ret=new XmrStakConfigNvidia;
	ret->loadJsonObject(QJsonDocument::fromJson(json.toLatin1()).object());
	return ret;
}


template class XmrStakConfigGpu<XmrStakAmdItem>;
template class XmrStakConfigGpu<XmrStakNvidiaItem>;
