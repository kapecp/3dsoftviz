/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*
treemodel.cpp

Provides a simple tree model to show how to create and use hierarchical
models.
*/

#include "QOSG/TreeModel.h"
#include "QOSG/TreeItem.h"

#include "Data/Type.h"
#include "Data/Graph.h"
#include "Manager/Manager.h"

#include <QtGui>

namespace QOSG {

TreeModel::TreeModel( const QStringList& data, QObject* parent )
	: QAbstractItemModel( parent )
{
	QList<QVariant> rootData;
	rootData << "root" ;
	rootItem = new TreeItem( rootData );
	setupModelData( data, rootItem );
}

TreeModel::~TreeModel()
{
	delete rootItem;
}

int TreeModel::columnCount( const QModelIndex& parent ) const
{
	if ( parent.isValid() ) {
		return static_cast<TreeItem*>( parent.internalPointer() )->columnCount();
	}
	else {
		return rootItem->columnCount();
	}
}

QVariant TreeModel::data( const QModelIndex& index, int role ) const
{
	if ( !index.isValid() ) {
		return QVariant();
	}

	if ( role != Qt::DisplayRole ) {
		return QVariant();
	}

	TreeItem* item = static_cast<TreeItem*>( index.internalPointer() );

	return item->data( index.column() );
}

Qt::ItemFlags TreeModel::flags( const QModelIndex& index ) const
{
	if ( !index.isValid() ) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData( int section, Qt::Orientation orientation,
								int role ) const
{
	if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
		return rootItem->data( section );
	}

	return QVariant();
}

QModelIndex TreeModel::index( int row, int column, const QModelIndex& parent )
const
{
	if ( !hasIndex( row, column, parent ) ) {
		return QModelIndex();
	}

	TreeItem* parentItem;

	if ( !parent.isValid() ) {
		parentItem = rootItem;
	}
	else {
		parentItem = static_cast<TreeItem*>( parent.internalPointer() );
	}

	TreeItem* childItem = parentItem->child( row );
	if ( childItem ) {
		return createIndex( row, column, childItem );
	}
	else {
		return QModelIndex();
	}
}

QModelIndex TreeModel::parent( const QModelIndex& index ) const
{
	if ( !index.isValid() ) {
		return QModelIndex();
	}

	TreeItem* childItem = static_cast<TreeItem*>( index.internalPointer() );
	TreeItem* parentItem = childItem->parent();

	if ( parentItem == rootItem ) {
		return QModelIndex();
	}

	return createIndex( parentItem->row(), 0, parentItem );
}

int TreeModel::rowCount( const QModelIndex& parent ) const
{
	TreeItem* parentItem;
	if ( parent.column() > 0 ) {
		return 0;
	}

	if ( !parent.isValid() ) {
		parentItem = rootItem;
	}
	else {
		parentItem = static_cast<TreeItem*>( parent.internalPointer() );
	}

	return parentItem->childCount();
}

void TreeModel::setupModelData( const QStringList& lines, TreeItem* parent )
{
	QList<TreeItem*> parents;
	parents << parent;
	int i;
	QStringList lastPath;
	QStringList path;
	QStringList line;
	TreeItem* index;
	//postupne sa prechadzaju vsetky polozky konfiguraku
	for ( i=0; i<lines.length(); i++ ) {
		//vytiahnutie mena a hodnoty premennej spolu s cestou k nim
		line = lines[i].split( "\t" );
		path = line[0].split( "." );

		//vytvorenie dat v tvare "meno_premmenej,hodnota"
		QString data = QString( "%1,%2" ).arg( path.last() ).arg( line.last() );
		path.pop_back();

		if ( !i ) {
			//ak je to prva hodnota z konfiguraku  tak sa vytvori nova polozka podla cesty
			lastPath = line[0].split( "." );
			lastPath.pop_back();
			createStruct( path,parents.last(),data );
		}
		else {
			int j;
			//vezmem posledny pridany
			index = parents.last();
			for ( j=0; j< path.length(); j++ ) {
				//ak sa posledna pouzita cesta rovna aktualnej tak sem patria tieto data
				if ( path[j] == lastPath[j] ) {
					index = index->child( index->childCount()-1 );
					if ( j == path.length()-1 ) {
						index->addColumnData( data );
					}
				}
				else {
					//ak nie tak sa vytvori nova polozka s novou cestou
					for ( int k=j; k<path.length(); k++ )
						if ( k > lastPath.length()-1 ) {
							lastPath << path[k];
						}
						else {
							lastPath[k] = path[k];
						}

					for ( int l=0; l<j; l++ ) {
						path.pop_front();
					}

					createStruct( path,index,data );
					break;
				}
			}
		}
	}
	if ( Manager::GraphManager::getInstance()->getActiveGraph() != NULL ) {
		addNodesToList( &parents );
	}
}

void TreeModel::createStruct( QStringList path, TreeItem* index, QString data )
{


	int j;
	//postupne sa posklada cesta k novo vytvorenej polozke
	for ( j=0; j<path.length(); j++ ) {
		QList<QVariant>* columnData;
		columnData = new QList<QVariant>();
		columnData->append( path[j] );
		index->appendChild( new TreeItem( *columnData,index ) );
		index = index->child( index->childCount()-1 );
	}
	index->addColumnData( data );

}

TreeItem* TreeModel::getRootItem()
{
	return rootItem;
}

void TreeModel::addNodesToList( QList<TreeItem*>* parents )
{
	//vytvori sa nova korenova polozka pre typy uzlov
	QList<QVariant>* columnData = new QList<QVariant>();
	columnData->append( "Node Types" );
	TreeItem* nodeRoot = new TreeItem( *columnData,parents->last() );
	parents->last()->appendChild( nodeRoot );

	//nacita sa zoznam typov uzlov
	Manager::GraphManager* manager = Manager::GraphManager::getInstance();
	QMap<qlonglong, Data::Type*>* nodes = manager->getActiveGraph()->getTypes();

	QMap<qlonglong, Data::Type*>::iterator iterator;

	//typy uzlyovsa postupne prechadzaju
	for ( iterator = nodes->begin(); iterator != nodes->end(); ++iterator ) {
		//vytvori sa nova polozka a vlozi sa nazov polozky
		QString name = iterator.value()->getName();
		columnData = new QList<QVariant>();
		columnData->append( name );

		//nacitaju sa data typu uzla
		Data::Type* node = iterator.value();
		QMap<QString, QString>* settings = node->getSettings();
		QMap<QString, QString>::iterator settingsIterator;

		QString data;
		//postupne sa data zlozia
		for ( settingsIterator = settings->begin(); settingsIterator != settings->end(); ++settingsIterator ) {
			if ( settingsIterator != settings->begin() ) {
				data.append( ";" );
			}
			data.append( QString( "%1,%2" ).arg( settingsIterator.key() ).arg( settingsIterator.value() ) );
		}

		//nakoniec sa data prilozia danemu typu uzla
		columnData->append( data );
		columnData->append( iterator.key() );
		nodeRoot->appendChild( new TreeItem( *columnData,nodeRoot ) );
	}
}

} // namespace QOSG
