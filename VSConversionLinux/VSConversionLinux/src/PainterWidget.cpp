/*!
 * FileName: PainterWidget.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-6
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 背景窗体
 */

#include <QApplication>
#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include "PainterWidget.h"


void PainterWidget::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	QWidget::paintEvent(event);
}

void PainterWidget::loadStyleSheet(const QString &res_url)
{
	QFile qss(res_url);
	qss.open(QFile::ReadOnly);
	QString qssStr = qss.readAll();
	qss.close();
	setStyleSheet(qssStr);
	setStyle(QApplication::style());
}