#include "BitcoinAddress.h"
#include "Qt/LException.h"
#include <QRegExp>
#include <QCryptographicHash>
#include <string>
#include <vector>

typedef unsigned char byte;


bool BitcoinAddress::ValidateBitcoinAddress(const QString address)
{
	try {
		if (address.length()<26 || address.length()>35) {
			return false;
			}
		QByteArray decoded=DecodeBase58(address);
		if (Hash(Hash(SubArray(decoded, 0, 21))).left(4)!=decoded.mid(21, 4)) {
			return false;
			}
		return true;
		}
	catch (...) {
		return false;
		}
}

bool BitcoinAddress::ValidateWorkerName(const QString workername)
{
	return workername.length()<=15 && isAlphaNumeric(workername) && !workername.contains(' ');
}

bool BitcoinAddress::isAlphaNumeric(const QString strToCheck)
{
	QRegExp rg("[a-zA-Z0-9\\s,]*");
	if (rg.exactMatch(strToCheck)) {
		return true;
		}
	return false;
}

QByteArray BitcoinAddress::DecodeBase58(const QString input)
{
	std::string stdInput=input.toStdString();
	const char* psz=stdInput.c_str();
	const char* pszBase58="123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
	// Skip and count leading '1's.
	int zeroes=0, length=0;
	while (*psz=='1') {
		zeroes++;
		psz++;
		}
	// Allocate enough space in big-endian base256 representation
	int size=strlen(psz)*733/1000+1; // log(58) / log(256), rounded up
	std::vector<byte> b256(size);
	// Process the characters
	while (*psz && !isspace(*psz)) {
		// Decode base58 character
		const char* ch=strchr(pszBase58, *psz);
		if (ch==NULL) {
			throw LException("Invalid character found.");
			}
		// Apply "b256 = b256 * 58 + ch".
		int carry=ch-pszBase58;
		int i=0;
		for (std::vector<byte>::reverse_iterator it=b256.rbegin(); (carry!=0 || i<length) && (it!=b256.rend()); ++it, ++i) {
			carry+=58*(*it);
			*it=carry%256;
			carry/=256;
			}
		Q_ASSERT(carry==0);
		if (carry) {
			throw LException("Address too long.");
			}
		length=i;
		psz++;
		}
	// Skip trailing spaces
	while (isspace(*psz)) {
		psz++;
		}
	if (*psz!=0) {
		throw LException("Address too long.");
		}
	// Skip leading zeroes in b256.
	std::vector<byte>::iterator it=b256.begin() + (size-length);
	while (it!=b256.end() && *it==0) {
		it++;
		}
	QByteArray output;
	output.fill(0x00, zeroes);
	while (it!=b256.end()) {
		output.append(*(it++));
		}
	return output;
}

QByteArray BitcoinAddress::Hash(const QByteArray bytes)
{
	return QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);
}

QByteArray BitcoinAddress::SubArray(const QByteArray data, const int index, const int length)
{
	return data.mid(index, length);
}
