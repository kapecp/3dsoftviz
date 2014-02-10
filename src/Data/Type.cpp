/*!
 * Type.cpp
 * Projekt 3DVisual
 */
#include "Data/Type.h"

#include "Viewer/DataHelper.h"
#include "Util/ApplicationConfig.h"

#include <QTextStream>

Data::Type::Type(qlonglong id, QString name,Data::Graph* graph, QMap<QString, QString> * settings)
{
	//konstruktor
	this->id = id;
	this->name = name;
	this->inDB = false;
	this->graph = graph;
	this->meta = false;

	Util::ApplicationConfig * appConf = Util::ApplicationConfig::get();

	if (settings == NULL)
	{
		this->settings = new QMap<QString, QString>;

		this->settings->insert("scale", appConf->getValue("Viewer.Textures.DefaultNodeScale"));
		this->settings->insert("textureFile", appConf->getValue("Viewer.Textures.Node"));
		this->settings->insert("color.R", "1");
		this->settings->insert("color.G", "1");
		this->settings->insert("color.B", "1");
		this->settings->insert("color.A", "1");
	}
	else
		this->settings = settings;

	loadConfig();
}

void Data::Type::loadConfig()
{
	//nacitame nastavenia zo suboru
	typeTexture = Vwr::DataHelper::readTextureFromFile(settings->value("textureFile"));
	scale = settings->value("scale").toFloat();
}

Data::Type::~Type(void)
{
	if(settings!=NULL) {
		settings->clear();
	}

	delete settings;
	settings = NULL;
}

QString Data::Type::toString() const {
	QString str;
	QTextStream(&str) << "type id:" << id << " name:" << name << " meta:" << this->isMeta();
	return str;
}

