#include "Utils/Extensions.h"
#include <vector>


std::tuple<double, double, double> Quartiles(std::list<double> source)
{
	source.sort();
	std::vector<double> afVal;
	std::copy(source.begin(), source.end(), std::back_inserter(afVal));
	size_t iSize=afVal.size();
	int iMid=iSize/2; //this is the mid from a zero based index, eg mid of 7 = 3;

	double fQ1=0;
	double fQ2=0;
	double fQ3=0;

	if (iSize%2==0) {
		// ================ EVEN NUMBER OF POINTS: =====================
		// even between low and high point
		fQ2=(afVal[iMid-1]+afVal[iMid])/2;
	
		int iMidMid=iMid/2;
	
		// easy split
		if (iMid%2==0) {
			fQ1=(afVal[iMidMid-1]+afVal[iMidMid])/2;
			fQ3=(afVal[iMid+iMidMid+1]+afVal[iMid+iMidMid])/2;
			}
		else {
			fQ1=afVal[iMidMid];
			fQ3=afVal[iMidMid+iMid];
			}
		}
	else {
		// odd number so the median is just the midpoint in the array.
		fQ2=afVal[iMid];

		if ((iSize-1)%4==0) {
			// ======================(4n-1) POINTS =========================
			int n=(iSize-1)/4;
			fQ1=(afVal[n-1]*0.25)+(afVal[n]*0.25);
			fQ3=(afVal[3*n]*0.75)+(afVal[3*n+1]*0.25);
			}
		else if ((iSize-3)%4==0) {
			// ======================(4n-3) POINTS =========================
			int n=(iSize-3)/4;
			fQ1=(afVal[n]*0.75)+(afVal[n+1]*0.25);
			fQ3=(afVal[3*n+1]*0.25)+(afVal[3*n+2]*0.75);
			}
		}
	return std::tuple<double, double, double>(fQ1, fQ2, fQ3);
}
