#ifndef UTILS_LINKS_H_
#define UTILS_LINKS_H_

#include <QString>


class Links
{
public:
	static const QString VisitUrl/*="https://www.nicehash.com"*/;

	static const QString VisitUrlNew/*="https://github.com/Lopo/NiceHashQtMiner/releases/tag/"*/; // add version

	static const QString CheckStats/*="https://www.nicehash.com/index.jsp?p=miners&addr="*/; // add btc adress as parameter

	// help and faq
	static const QString NHQTM_Help/*="https://github.com/Lopo/NiceHashQtMiner/"*/;
	static const QString NHQTM_NoDevHelp/*="https://github.com/Lopo/NiceHashQtMiner/wiki/Troubleshooting#nosupportdev"*/;

	// faq
	static const QString NhmBtcWalletFaq/*="https://www.nicehash.com/help/how-to-create-the-bitcoin-addresswallet"*/;
	static const QString NhmPayingFaq/*="https://www.nicehash.com/help/when-and-how-do-you-get-paid"*/;

	// API
	static const QString NhmApiStats/*="https://api.nicehash.com/api?method=stats.provider&addr="*/; // btc adress as parameter
	static const QString NhmApiInfo/*="https://api.nicehash.com/api?method=simplemultialgo.info"*/;
	static const QString NhmApiVersion/*="http://nhqtm.losys.eu/version/?method=version&Qt"*/;

	static const QString NhmProfitCheck/*="https://api.nicehash.com/?p=calc&name="*/; // device profits

	static const QString NhmSocketAddress/*="wss://nhmws.nicehash.com/v2/nhm"*/; // SMA Socket
};

#endif
