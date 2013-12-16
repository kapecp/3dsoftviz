/****************************************************************************
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
**
****************************************************************************/

#ifndef QTCOLORPICKER_H
#define QTCOLORPICKER_H
#include <QtGui/QPushButton>
#include <QtCore/QString>
#include <QtGui/QColor>

#include <QtGui/QLabel>
#include <QtCore/QEvent>
#include <QtGui/QFocusEvent>

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QColorDialog>
#include <QtCore/QMap>
#include <QtGui/QLayout>
#include <QtGui/QStyle>
#include <QtGui/QLabel>
#include <QtGui/QToolTip>
#include <QtGui/QPixmap>
#include <QtGui/QFocusEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QHideEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QMouseEvent>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTCOLORPICKER_EXPORT) && !defined(QT_QTCOLORPICKER_IMPORT)
#    define QT_QTCOLORPICKER_EXPORT
#  elif defined(QT_QTCOLORPICKER_IMPORT)
#    if defined(QT_QTCOLORPICKER_EXPORT)
#      undef QT_QTCOLORPICKER_EXPORT
#    endif
#    define QT_QTCOLORPICKER_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOLORPICKER_EXPORT)
#    undef QT_QTCOLORPICKER_EXPORT
#    define QT_QTCOLORPICKER_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOLORPICKER_EXPORT
#endif

class ColorPickerPopup;


class ColorPickerButton : public QFrame
{
	Q_OBJECT

public:
	ColorPickerButton(QWidget *parent);

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void paintEvent(QPaintEvent *e);
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);
};

/*
	This class represents each "color" or item in the color grid.
*/
class ColorPickerItem : public QFrame
{
	Q_OBJECT

public:
	ColorPickerItem(const QColor &color = Qt::white, const QString &text = QString::null,
					QWidget *parent = 0);
	~ColorPickerItem();

	QColor color() const;
	QString text() const;

	void setSelected(bool);
	bool isSelected() const;
signals:
	void clicked();
	void selected();

public slots:
	void setColor(const QColor &color, const QString &text = QString());

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void paintEvent(QPaintEvent *e);

private:
	QColor c;
	QString t;
	bool sel;
};

/*

*/
class ColorPickerPopup : public QFrame
{
	Q_OBJECT

public:
	ColorPickerPopup(int width, bool withColorDialog,
					 QWidget *parent = 0);
	~ColorPickerPopup();

	void insertColor(const QColor &col, const QString &text, int index);
	void exec();

	void setExecFlag();

	QColor lastSelected() const;

	ColorPickerItem *find(const QColor &col) const;
	QColor color(int index) const;

signals:
	void selected(const QColor &);
	void hid();

public slots:
	void getColorFromDialog();

protected slots:
	void updateSelected();

protected:
	void keyPressEvent(QKeyEvent *e);
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

	void regenerateGrid();

private:
	QMap<int, QMap<int, QWidget *> > widgetAt;
	QList<ColorPickerItem *> items;
	QGridLayout *grid;
	ColorPickerButton *moreButton;
	QEventLoop *eventLoop;

	int lastPos;
	int cols;
	QColor lastSel;
};


class QT_QTCOLORPICKER_EXPORT QtColorPicker : public QPushButton
{
	Q_OBJECT

	Q_PROPERTY(bool colorDialog READ colorDialogEnabled WRITE setColorDialogEnabled)

public:
	QtColorPicker(QWidget *parent = 0,
				  int columns = -1, bool enableColorDialog = true);

	~QtColorPicker();

	void insertColor(const QColor &color, const QString &text = QString::null, int index = -1);

	QColor currentColor() const;

	QColor color(int index) const;

	void setColorDialogEnabled(bool enabled);
	bool colorDialogEnabled() const;

	void setStandardColors();

	static QColor getColor(const QPoint &pos, bool allowCustomColors = true);

public Q_SLOTS:
	void setCurrentColor(const QColor &col);

Q_SIGNALS:
	void colorChanged(const QColor &);

protected:
	void paintEvent(QPaintEvent *e);

private Q_SLOTS:
	void buttonPressed(bool toggled);
	void popupClosed();

private:
	ColorPickerPopup *popup;
	QColor col;
	bool withColorDialog;
	bool dirty;
	bool firstInserted;
};

#endif
