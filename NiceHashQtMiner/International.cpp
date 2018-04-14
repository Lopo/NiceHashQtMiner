#include "International.h"
#include "Utils/Helpers.h"
#include <QDir>
#include <QMap>
#include <QException>
#include <QJsonDocument>
#include <QJsonObject>

International::Language* International::_selectedLanguage=nullptr;
International::Language* International::_english=nullptr;


QList<International::Language*>* International::GetLanguages()
{
	QList<Language*>* langs=new QList<Language*>;

	try {
		QDir di("langs");
		QFileInfoList files=di.entryInfoList({"*.lang"});

		foreach (QFileInfo fi, files) {
			try {
				QFile f(fi.filePath());
				f.open(QIODevice::ReadOnly|QIODevice::Text);
				Language* l=Language::fromJson(f.readAll());
				f.close();
				langs->append(l);
				}
			catch (QException ex) {
				Helpers::ConsolePrint("NICEHASH", QString("Lang error: ")+ex.what());
				}
			}
		}
	catch (QException ex) {
		Helpers::ConsolePrint("NICEHASH", QString("Lang error: ")+ex.what());
		}

	return langs;
}

void International::Initialize(Enums::LanguageType lid)
{
	QList<Language*>* langs=GetLanguages();

	foreach (Language* lang, *langs) {
		if (lang->ID==lid) {
			Helpers::ConsolePrint("NICEHASH", "Selected language: "+lang->Name);
			_selectedLanguage=lang;
			}
		if (lang->ID==Enums::LanguageType::En) {
			_english=lang;
			}
		}

	Helpers::ConsolePrint("NICEHASH", "Critical error: missing language");
}

QMap<Enums::LanguageType, QString>* International::GetAvailableLanguages()
{
	QList<Language*>* langs=GetLanguages();
	QMap<Enums::LanguageType, QString>* retdict=new QMap<Enums::LanguageType, QString>;

	foreach (Language* lang, *langs) {
		Helpers::ConsolePrint("NICEHASH", QString("Found language: ")+lang->Name);
		retdict->insert(lang->ID, lang->Name);
		}

	return retdict;
}

QString International::GetText(QString token)
{
	if (_selectedLanguage==nullptr) {
		return "";
		}
	if (!_selectedLanguage->Entries.contains(token)) {
		if (_english->Entries.contains(token)) {
			// Default to English if no token for selected lang
			return _english->Entries.value(token);
			}
		return "";
		}

	return _selectedLanguage->Entries.value(token);
}

International::Language* International::Language::fromJson(QByteArray json)
{
	Language* ret=new Language;
	QJsonObject o=QJsonDocument::fromJson(json).object();
	ret->Name=o.value("Name").toString();
	ret->ID=(Enums::LanguageType)o.value("ID").toInt();
	QJsonObject Entries=o.value("Entries").toObject();
	foreach (QString key, Entries.keys()) {
		ret->Entries.insert(key, Entries.value(key).toString());
		}

	return ret;
}
