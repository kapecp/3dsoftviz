#include "Data/VizNode.h"

Data::VizNode::VizNode( qlonglong id, QString name, Data::Type* type, Data::Graph* graph, float scaling )
	:AbsNode( id,name,type,graph )
{
	this->scale = scaling;
	int pos = 0;
	int cnt = 0;

	labelText = this->name;

	while ( ( pos = labelText.indexOf( QString( " " ), pos + 1 ) ) != -1 ) {
		if ( ++cnt % 3 == 0 ) {
			labelText = labelText.replace( pos, 1, "\n" );
		}
	}

	this->ignore = false;
}



