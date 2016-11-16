/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/


#include <QtGui>

#include <LuaInterface/LuaInterface.h>

#include "LuaGraph/LuaGraphTreeModel.h"
#include "LuaGraph/LuaGraphTreeItem.h"
#include "LuaGraph/LuaNode.h"

#include "LuaTypes/LuaValue.h"
#include "LuaTypes/LuaValueList.h"
#include "LuaTypes/LuaValueMap.h"

Lua::LuaGraphTreeModel::LuaGraphTreeModel( const Lua::LuaNode* node, QObject* parent )
	: QAbstractItemModel( parent )
{
	QList<QVariant> rootData;
	rootData << "Property" << "Value";
	rootItem = new Lua::LuaGraphTreeItem( rootData );
	setupModelData( node, rootItem );
}

Lua::LuaGraphTreeModel::~LuaGraphTreeModel()
{
	delete rootItem;
}

int Lua::LuaGraphTreeModel::columnCount( const QModelIndex& parent ) const
{
	if ( parent.isValid() ) {
		return static_cast<LuaGraphTreeItem*>( parent.internalPointer() )->columnCount();
	}
	else {
		return rootItem->columnCount();
	}
}

QVariant Lua::LuaGraphTreeModel::data( const QModelIndex& index, int role ) const
{
	if ( !index.isValid() ) {
		return QVariant();
	}

	if ( role != Qt::DisplayRole ) {
		return QVariant();
	}

	LuaGraphTreeItem* item = static_cast<LuaGraphTreeItem*>( index.internalPointer() );

	return item->data( index.column() );
}

Qt::ItemFlags Lua::LuaGraphTreeModel::flags( const QModelIndex& index ) const
{
	if ( !index.isValid() ) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant Lua::LuaGraphTreeModel::headerData( int section, Qt::Orientation orientation,
		int role ) const
{
	if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
		return rootItem->data( section );
	}

	return QVariant();
}

QModelIndex Lua::LuaGraphTreeModel::index( int row, int column, const QModelIndex& parent )
const
{
	if ( !hasIndex( row, column, parent ) ) {
		return QModelIndex();
	}

	LuaGraphTreeItem* parentItem;

	if ( !parent.isValid() ) {
		parentItem = rootItem;
	}
	else {
		parentItem = static_cast<LuaGraphTreeItem*>( parent.internalPointer() );
	}

	LuaGraphTreeItem* childItem = parentItem->child( row );
	if ( childItem ) {
		return createIndex( row, column, childItem );
	}
	else {
		return QModelIndex();
	}
}

QModelIndex Lua::LuaGraphTreeModel::parent( const QModelIndex& index ) const
{
	if ( !index.isValid() ) {
		return QModelIndex();
	}

	LuaGraphTreeItem* childItem = static_cast<LuaGraphTreeItem*>( index.internalPointer() );
	LuaGraphTreeItem* parentItem = childItem->parent();

	if ( parentItem == rootItem ) {
		return QModelIndex();
	}

	return createIndex( parentItem->row(), 0, parentItem );
}

int Lua::LuaGraphTreeModel::rowCount( const QModelIndex& parent ) const
{
	LuaGraphTreeItem* parentItem;
	if ( parent.column() > 0 ) {
		return 0;
	}

	if ( !parent.isValid() ) {
		parentItem = rootItem;
	}
	else {
		parentItem = static_cast<LuaGraphTreeItem*>( parent.internalPointer() );
	}

	return parentItem->childCount();
}

void Lua::LuaGraphTreeModel::setupModelData( const Lua::LuaNode* node, LuaGraphTreeItem* parent )
{
	parent->appendChild( new LuaGraphTreeItem( QList<QVariant>() << "id" << node->getId(), parent ) );
	parent->appendChild( new LuaGraphTreeItem( QList<QVariant>() << "label" << node->getLabel(), parent ) );
	Lua::LuaValue luaParams = node->getParams();
	loadLuaModel( "params", luaParams, parent );
}

void Lua::LuaGraphTreeModel::loadLuaModel( QString name, const Lua::LuaValue value, LuaGraphTreeItem* parent )
{
	if ( value.getValue().type() == 5 ) {
		LuaGraphTreeItem* newParent = new LuaGraphTreeItem( QList<QVariant>() << name << "", parent );
		parent->appendChild( newParent );
		Lua::LuaValueMap tableValue = value.getValue().asTable();
		for ( auto i = tableValue.begin(); i != tableValue.end(); ++i ) {
			loadLuaModel( QString::fromStdString( i->first.asString() ), i->second, newParent );
		}
	}
	else {
		Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();
		Lua::LuaValueList param;
		param.push_back( value.getValue() );
		std::string textValue = lua->callFunction( "tostring", param.getValue() )[0].asString();
		QList<QVariant> list = QList<QVariant>() << name << QString::fromStdString( textValue );
		parent->appendChild( new LuaGraphTreeItem( list, parent ) );
	}
}
