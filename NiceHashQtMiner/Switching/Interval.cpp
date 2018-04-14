#include "Switching/Interval.h"
#include <qglobal.h>


Interval::Interval()
{ }

Interval::Interval(int lower, int upper)
{
	Lower=lower;
	Upper=upper;
}

void Interval::FixRange()
{
	if (Lower<0) {
		Lower=0;
		}
	if (Upper<=Lower) {
		Upper=Lower+1;
		}
}

int Interval::RandomInt()
{
	return qrand()%((UpperExclusive()+1)-Lower)+Lower;
}
