/*!
 * FileName: PainterWidget.h
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-6
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 背景窗体
 */

#ifndef _PAINTER_WIDGET_H_
#define _PAINTER_WIDGET_H_
#include <QWidget>

class PainterWidget :public QWidget
{
	Q_OBJECT
public:
	PainterWidget(QWidget *parent = nullptr) :QWidget(parent) {}
	virtual ~PainterWidget() {}
protected:

	/***********************************************************************************/
	/* Qt 要求从QWidget继承的子类必须重写paintEvent以提供派生类对qss的支持                 */
	/* assistant中写到:                                                                  */
	/* Supports only the background, background-clip and background-origin properties.  */
	/* If you subclass from QWidget, you need to provide a paintEvent for your custom   */
	/* QWidget as below:                                                                */
	/************************************************************************************/
	virtual void paintEvent(QPaintEvent *event);
public:
	void loadStyleSheet(const QString &res_url);
	
};
#endif
