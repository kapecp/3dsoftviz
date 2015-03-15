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

/*
   LuaGraphTreeItem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include <QStringList>

#include "LuaGraph/LuaGraphTreeItem.h"

Lua::LuaGraphTreeItem::LuaGraphTreeItem( const QList<QVariant>& data,LuaGraphTreeItem* parent )
{
	parentItem = parent;
	itemData = data;
}

Lua::LuaGraphTreeItem::~LuaGraphTreeItem()
{
	qDeleteAll( childItems );
}

void Lua::LuaGraphTreeItem::appendChild( LuaGraphTreeItem* item )
{
	childItems.append( item );
}

Lua::LuaGraphTreeItem* Lua::LuaGraphTreeItem::child( int row )
{
	return childItems.value( row );
}

int Lua::LuaGraphTreeItem::childCount() const
{
	return childItems.count();
}

int Lua::LuaGraphTreeItem::columnCount() const
{
	return itemData.count();
}

QVariant Lua::LuaGraphTreeItem::data( int column ) const
{
	return itemData.value( column );
}

Lua::LuaGraphTreeItem* Lua::LuaGraphTreeItem::parent()
{
	return parentItem;
}

int Lua::LuaGraphTreeItem::row() const
{
	if ( parentItem ) {
		return parentItem->childItems.indexOf( const_cast<LuaGraphTreeItem*>( this ) );
	}

	return 0;
}
