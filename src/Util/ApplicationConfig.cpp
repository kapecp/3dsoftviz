#include "Util/ApplicationConfig.h"

#include "OpenThreads/Mutex"

#include <QDebug>
#include <QFile>
#include <QDebug>

Util::ApplicationConfig* Util::ApplicationConfig::_instance;
OpenThreads::Mutex Util::ApplicationConfig::_mutex;

Util::ApplicationConfig::ApplicationConfig( void )
{
	//otvorenie suboru na nacitanie dat
	QFile file( "../share/3dsoftviz/config/config" );
	file.open( QIODevice::ReadOnly );
	//rozdelenie dat podla riadkov
	QList<QString> data = QString( file.readAll() ).split( "\n" );
	file.close();

	//sparsovanie dat a ich ulozenie do mapy
	for ( int i=0; i<data.length(); i++ ) {
		if ( data[i].simplified().length()>0 ) {
			QList<QString> row = data[i].split( "=" );
			add( row[0].simplified(),row[row.length()-1].simplified() );
		}
	}

}

Util::ApplicationConfig::~ApplicationConfig( void )
{
}


void Util::ApplicationConfig::add( QString key, QString value )
{
	_map.insert( key,value );
}

QString Util::ApplicationConfig::getValue( QString key )
{
	return _map.value( key );
}


int Util::ApplicationConfig::getNumericValue(
	QString key,
	std::shared_ptr<int> minValue,
	std::shared_ptr<int> maxValue,
	const int defaultValue
)
{
	if ( !_map.contains( key ) ) {
		return defaultValue;
	}

	QString value = getValue( key );
	bool ok = true;
	int result = value.toInt( &ok );

	if ( !ok ) {
		return defaultValue;
	}

	if ( ( minValue.get() != NULL ) && ( result < ( *minValue ) ) ) {
		result = *minValue;
	}

	if ( ( maxValue.get() != NULL ) && ( result > ( *maxValue ) ) ) {
		result = *maxValue;
	}

	return result;
}

bool Util::ApplicationConfig::getBoolValue(
	QString key,
	const bool defaultValue
)
{
	if ( !_map.contains( key ) ) {
		return defaultValue;
	}

	QString value = getValue( key );

	if ( value == "1" ) {
		return true;
	}
	else if ( value == "0" ) {
		return false;
	}
	else {
		return defaultValue;
	}
}

osg::Vec4f Util::ApplicationConfig::getColorValue( QString key )
{
	if ( !_map.contains( key ) ) {
		qDebug() << "Error: key '" + key + "' is not present in application config. Caused by UtilApplicationConfig::getColorValue(key)";
	}

	QString value = this->getValue( key );
	QStringList parts = value.split( "," );

	return osg::Vec4f(
			   parts[0].toFloat() / 255,
			   parts[1].toFloat() / 255,
			   parts[2].toFloat() / 255,
			   parts[3].toFloat() / 255
		   );
}

Util::ApplicationConfig* Util::ApplicationConfig::get()
{
	_mutex.lock();

	if ( _instance == NULL ) {
		_instance = new ApplicationConfig();
	}
	_mutex.unlock();

	return _instance;
}

QStringList Util::ApplicationConfig::getList()
{
	MapSS::iterator i;
	QStringList list;
	QString item;

	//iteruje postupne cez vsetky polozky a uklada ich do zoznamu retazov
	for ( i = _map.begin(); i != _map.end(); ++i ) {
		item = QString( "%1\t%2" ).arg( i.key() ).arg( i.value() );
		list.append( item );
	}
	list.sort();
	return list;
}

void Util::ApplicationConfig::saveConfig()
{
	MapSS::iterator i;
	QStringList list;
	QString item;
	QFile file( "config/config" );
	QByteArray ba;
	file.open( QIODevice::WriteOnly );

	//postupne ukladanie do suboru
	for ( i = _map.begin(); i != _map.end(); ++i ) {
		item = QString( "%1=%2\n" ).arg( i.key() ).arg( i.value() );
		ba = item.toLatin1();
		file.write( ba.data() );
	}
	file.close();

	//GraphMetrics::computeGraphMetrics(Manager::GraphManager::getInstance()->getActiveGraph());
}
