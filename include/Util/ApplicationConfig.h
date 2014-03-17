/**
*  ApplicationConfig.h
*  Projekt 3DVisual
*/
#ifndef UTIL_HASHMAPSINGLETON_DEF
#define UTIL_HASHMAPSINGLETON_DEF 1

#ifdef __APPLE__
#include <qstring.h>
#include <qstringlist.h>
#else
#include <Qt/qstring.h>
#include <Qt/qstringlist.h>
#endif

#include <QMap>
#include <OpenThreads/Mutex>

#include <memory>

using namespace std;

typedef QMap<QString, QString> MapSS;

/*!
 * \brief
 * Pomocne triedy aplikacie
 *
 */
namespace Util
{
/*!
	 * \brief
	 * Trieda udrzujuca konfiguracne nastavenia aplikacie
	 *
	 * \author
	 * Adam Pazitnaj, Michal Paprcka
	 *
	 * \version
	 * 1.1
	 *
	 * \date
	 * 13.12.2009
	 *
	 * Multi-Thread Safe Singleton udrzujuci hashmapu, ktora
	 * obsahuje konfiguracne nastavenia aplikacie.
	 */
class ApplicationConfig
{
public:

	/*!
			 * \brief
			 * Vrati instanciu objektu ApplicationConfig.
			 *
			 * \returns
			 * Instancia objektu ApplicationConfig.
			 *
			 */
	static ApplicationConfig* get();

	/*!
			 * \brief
			 * Metoda na vlozenie hodnoty spolu s jej klucom
			 * do hashmapy
			 *
			 * \param key
			 * Kluc.
			 *
			 * \param value
			 * Hodnota priradena ku klucu.
			 *
			 */
	void add(QString key, QString value);

	/*!
			 * \brief
			 * Vrati hodnotu priradenu k zadanemu klucu.
			 *
			 * \param key
			 * Kluc k hodnote.
			 *
			 * \returns
			 * Hodnota priradena ku klucu.
			 *
			 */
	QString getValue(QString key);

	/**
			 * \brief Gets numeric value from settings.
			 * \param[in] key Key to the value.
			 * \param[in] minValue Min. value (NULL if no minumum).
			 * \param[in] maxValue Max. value (NULL if no minimum).
			 * \param[in] defaultValue Default value (if the value does not exist or is not valid).
			 * \return Numeric value for the specified key (defaultValue if the key does not exist or the value is not a valid number).
			 */
	long getNumericValue (
			QString key,
			std::auto_ptr<long> minValue,
			std::auto_ptr<long> maxValue,
			const long defaultValue
			);

	/**
			 * \brief Gets boolean value from settings.
			 * \param[in] key Key to the value.
			 * \param[in] defaultValue Default value (if the value does not exist or is not valid).
			 * \return boolean value for the specified key (defaultValue if the key does not exist or the value is not a valid boolean value).
			 */
	bool getBoolValue (
			QString key,
			const bool defaultValue
			);

	/**
			*  \fn public  getList
			*  \brief
			*  \return QStringList
			*/
	QStringList getList();


	/**
			*  \fn public  saveConfig
			*  \brief
			*/
	void saveConfig();

protected:

private:

	/**
			*  \fn private constructor  ApplicationConfig
			*  \brief
			*/
	ApplicationConfig(void);

	/**
			*  \fn private destructor  ~ApplicationConfig
			*  \brief
			*/
	~ApplicationConfig(void);


	/**
			*  OpenThreads::Mutex _mutex
			*  \brief
			*/
	static OpenThreads::Mutex _mutex;

	/**
			*  Util::ApplicationConfig * _instance
			*  \brief
			*/
	static ApplicationConfig * _instance;

	/**
			*  MapSS _map
			*  \brief
			*/
	MapSS _map;
};
}
#endif
