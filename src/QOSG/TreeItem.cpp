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
	treeitem.cpp

	A container for items of data supplied by the simple tree model.
*/

#include <QStringList>
#include "QOSG/TreeItem.h"

using namespace QOSG;

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}

TreeItem::~TreeItem()
{
	qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
	childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
	return childItems.value(row);
}

int TreeItem::childCount() const
{
	return childItems.count();
}

int TreeItem::columnCount() const
{
	return itemData.count();
}

QVariant TreeItem::data(int column) const
{
	return itemData.value(column);
}

TreeItem *TreeItem::parent()
{
	return parentItem;
}

int TreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

void TreeItem::addColumnData(QString data)
{
	// v pripade ze polozka ma len nazov pridaju sa jej tieto data
	if (itemData.length() == 1 )
	{
		itemData.append(data);
	}
	else
	{
		// ak ma aj ine data tak sa nahradia novymi
		QString str = itemData[1].toString();
		itemData.pop_back();
		str.append(";");
		str.append(data);
		itemData.append(str);
	}
}

void TreeItem::replaceDataColumn(QString data)
{
	//ak ma polozka len nazov pridaju sa jej data
	if (itemData.length() ==1)
	{
		itemData.append(data);
	}
	else if (itemData.length() == 3)
	{
		//ak je to typ uzla tak sa jej nahradia data
		QVariant tmp = itemData.takeLast();
		itemData.pop_back();
		itemData.append(data);
		itemData.append(tmp);
	}
	else
	{
		//ak ma aj vlastne data tieto data sa nahradia
		itemData.pop_back();
		itemData.append(data);
	}
}
