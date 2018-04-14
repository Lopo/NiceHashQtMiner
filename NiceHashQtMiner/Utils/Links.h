#ifndef UTILS_LINKS_H_
#define UTILS_LINKS_H_

#include <QString>


class Links
{
public:
	const static QString VisitUrl;

	const static QString VisitUrlNew; // add version

	const static QString CheckStats; // add btc adress as parameter

	// help and faq
	const static QString NHQTM_Help;
	const static QString NHQTM_NoDevHelp;

	// faq
	const static QString NhmBtcWalletFaq;
	const static QString NhmPayingFaq;

	// API
	const static QString NhmApiStats; // btc adress as parameter
	const static QString NhmApiInfo;
	const static QString NhmApiVersion;

	const static QString NhmProfitCheck; // device profits

	const static QString NhmSocketAddress; // SMA Socket
};

#endif
