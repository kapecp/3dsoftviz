///////////////////////////////////////////////////////////
//  CheckBoxList.h
//  Header of the Class CheckBoxList
//  Created on:      02-Jun-2008 6:53:56 PM
//  Original author: Nasser M. Al-Ansari (Da-Crystal)
///////////////////////////////////////////////////////////
//	Modification History:
//
//	Who(SNo)			Date			Description
//
///////////////////////////////////////////////////////////

#ifndef CHECKBOXLIST_H
#define CHECKBOXLIST_H 1

#include <QtGui>
#include <QComboBox>

namespace QOSG {
class CheckBoxList: public QComboBox
{

	Q_OBJECT

public:
	explicit CheckBoxList( QWidget* widget = 0 );
	virtual ~CheckBoxList();
	bool eventFilter( QObject* object, QEvent* event );
	virtual void paintEvent( QPaintEvent* );
	void SetDisplayText( QString text );
	QString GetDisplayText() const;
	void paint( QPainter* painter, const QStyleOptionViewItem& option,
				const QModelIndex& index ) const;
	QWidget* createEditor( QWidget* parent,const QStyleOptionViewItem /*& option */,
						   const QModelIndex /*& index*/ ) const;
	void setEditorData( QWidget* editor,
						const QModelIndex& index ) const;
	void setModelData( QWidget* editor, QAbstractItemModel* model,
					   const QModelIndex& index ) const;
	void updateEditorGeometry( QWidget* editor,
							   const QStyleOptionViewItem& option, const QModelIndex /*&index*/ ) const;

private:
	QString m_DisplayText;
};
}

#endif // CHECKBOXLIST_H
