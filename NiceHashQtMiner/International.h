#ifndef INTERNATIONAL_H_
#define INTERNATIONAL_H_

#include "Enums.h"
#include <QMap>


class International
{
private:
	class Language {
	public:
		QString Name;
		Enums::LanguageType ID;
		QMap<QString, QString> Entries;

		static Language* fromJson(QByteArray json);
		};

	static Language* _selectedLanguage;
	static Language* _english;

	static QList<Language*>* GetLanguages();

public:
	static void Initialize(Enums::LanguageType lid);
	static QMap<Enums::LanguageType, QString>* GetAvailableLanguages();
	static QString GetText(QString token);
};

#endif
