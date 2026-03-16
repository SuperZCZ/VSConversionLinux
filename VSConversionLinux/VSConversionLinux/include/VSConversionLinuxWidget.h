/*!
 * FileName: VSConversionLinuxWidget.h
 *
 * Author:   ZhangChaoZe
 * Date:     2025-12-21
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: VSConversionLinux主窗体
 */

#ifndef _VS_CONVERSION_LINUX_WIDGET_H_
#define _VS_CONVERSION_LINUX_WIDGET_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QDropEvent>
#include <QDragEnterEvent>


#include "VSConversionLinuxGlobal.h"
#include "ParseSolutionThread.h"
#include "SlnConfigWidget.h"
#include "PainterWidget.h"

class VSConversionLinuxWidget :public PainterWidget
{
	Q_OBJECT
public:
	VSConversionLinuxWidget(QWidget *parent = nullptr);
	virtual ~VSConversionLinuxWidget();

private:
	QVBoxLayout *vAllLay;
	QLineEdit *slnFileLineEdit;
	QPushButton *browseSlnFileBtn;
	ParseSolutionThread *parseSlnFileThread;
	QStackedWidget *slnInfoStackWidget;

	//解析状态显示控件
	QLabel *parseStatusImageLabel;
	QLabel *parseStatusTextLabel;
	QTextBrowser *parseStatusDetailBrowser;
	QPushButton *parseStatusBtn;
	QMovie *parseStatusWaitMovie;
	QWidget *parseInfoWidget;
	QWidget *promptInfoWidgte;
	int parseInfoWidgetIndex;
	int promptInfoWidgteIndex;

	//解决方案解析结果显示控件
	SlnConfigWidget *slnConfigWidget;
	QScrollArea *slnConfigScrollArea;
	int slnConfigWidgetIndex;

	VsSolution currentVsSln;

	QHBoxLayout *bottomButtHLay;
	QPushButton *confirmButt;
	QPushButton *cancelButt;


public:
	void resizeMoveToCenter(int w, int h);
	void autoResizeMoveToDesktopCenter(double w_percent = 0.417, double h_percent = 0.484);

protected:
	virtual void dropEvent(QDropEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);

public slots:
	void handleSlnInfoParseResult(VsSolution *vsSln, int retCode, QString msg);
	void handleStatusBtnClicked();
	void newSlnFilePath(const QString &slnFilePath);
	void browseSlnFileBtnClicked();
	void slnFileParsingBegin();
	void slnFileParsingEnd(VsSolution *vsSln, int retCode, QString msg);
	void switchToSlnConfigWidget();
	void switchToPromptInfoWidgte();
	void handleConfirmButtClicked();
	void handleCanclelButtClicked();
	void slnFileSaveAndParsingBegin();
};

#endif
