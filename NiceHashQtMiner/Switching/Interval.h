#ifndef SWITCHING_INTERVAL_H_
#define SWITCHING_INTERVAL_H_


class Interval
{
public:
	Interval();
	Interval(int lower, int upper);

	void FixRange();
	int RandomInt();

	int Lower; // Inclusive lower end of the interval
	int Upper; // Inclusive upper end of the interval

	inline int UpperExclusive() const { return Upper+1; }; // Upper end as an exclusive interval
};

#endif
