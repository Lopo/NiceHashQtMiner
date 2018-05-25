#ifndef INTERFACES_IBENCHMARKCOMUNICATOR_H_
#define INTERFACES_IBENCHMARKCOMUNICATOR_H_


#include <QString>

class IBenchmarkComunicator
{
public:
	virtual void OnBenchmarkComplete(bool success, QString status)=0;
};

#endif
