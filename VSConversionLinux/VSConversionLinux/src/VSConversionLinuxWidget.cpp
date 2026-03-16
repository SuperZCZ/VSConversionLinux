/*!
 * FileName: VSConversionLinuxWidget.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2025-12-21
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: VSConversionLinux主窗体
 */


#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QMovie>
#include <QMimeData>

#include "VSConversionLinuxGlobal.h"
#include "VSConversionLinuxWidget.h"
#include "ParseSolutionThread.h"

#include "CombineFileDialog.h"


VSConversionLinuxWidget::VSConversionLinuxWidget(QWidget* parent /*= nullptr*/):PainterWidget(parent)
{
	this->setWindowTitle(QObject::trUtf8("VS to Linux Project Converter"));
	vAllLay = new QVBoxLayout(this);
	this->setLayout(vAllLay);
	setWindowIcon(QIcon(":/images/logo.png"));
	setAcceptDrops(true);

	QHBoxLayout *slnFileHLay = new QHBoxLayout();
	QLabel *slnFileLabel = new QLabel(trUtf8("解决方案："));
	slnFileLineEdit = new QLineEdit(this);
	slnFileLineEdit->setPlaceholderText(trUtf8("请选择一个 Visual Studio 解决方案文件(*.sln)"));
	
	browseSlnFileBtn = new QPushButton(trUtf8("浏览"));
	slnFileHLay->addWidget(slnFileLabel);
	slnFileHLay->addWidget(slnFileLineEdit);
	slnFileHLay->addWidget(browseSlnFileBtn);

	slnInfoStackWidget = new QStackedWidget;
	parseInfoWidget = new QWidget;
	promptInfoWidgte = new QWidget;
	QVBoxLayout *promptInfoWidgteVLay = new QVBoxLayout(promptInfoWidgte);
	QTextBrowser *promptInfoTextBrowser = new QTextBrowser(promptInfoWidgte);
	promptInfoWidgteVLay->addWidget(promptInfoTextBrowser);
	promptInfoWidgteVLay->setContentsMargins(50, 50, 50, 50);
	promptInfoTextBrowser->setText("<h1 style=\"text-align:center;\">转换前请先备份</h1><p>&nbsp;</p><h4 style=\"text-align:center;\">现有的VisualGDB工程转换为Linux项目类型< / h4><h4>加强intellisense支持而不改变VisualGDB项目属性< / h4><p>&nbsp; < / p><p>&nbsp; < / p><p>&nbsp; < / p>");
	promptInfoTextBrowser->setObjectName("PromptInfoTextBrowser");
	promptInfoTextBrowser->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	
	QVBoxLayout *parseInfoVLay = new QVBoxLayout(parseInfoWidget);
	QHBoxLayout *parseStatusImageHLay = new QHBoxLayout;
	parseStatusImageLabel = new QLabel;
	parseStatusImageLabel->setObjectName("ParseStatusImageLabel");
	parseStatusWaitMovie = new QMovie(":/images/loading.gif");
	parseStatusImageLabel->setAlignment(Qt::AlignCenter);
	parseStatusImageHLay->addWidget(parseStatusImageLabel);
	parseStatusImageHLay->setAlignment(Qt::AlignHCenter);
	parseStatusWaitMovie->start();
	parseStatusTextLabel = new QLabel;
	parseStatusTextLabel->setAlignment(Qt::AlignCenter);
	parseStatusTextLabel->setObjectName("ParseStatusTextLabel");
	parseStatusDetailBrowser = new QTextBrowser;
	parseStatusDetailBrowser->setAlignment(Qt::AlignCenter);
	parseStatusDetailBrowser->setObjectName("ParseStatusDetailBrowser");
	parseStatusBtn = new QPushButton(trUtf8("确定"));
	parseStatusBtn->setFixedSize(100, 25);
	QHBoxLayout *parseStatusBtnHLay = new QHBoxLayout;
	parseStatusBtnHLay->addWidget(this->parseStatusBtn);
	parseStatusBtnHLay->setAlignment(Qt::AlignCenter);
	parseInfoVLay->setAlignment(Qt::AlignCenter);
	parseInfoVLay->setSpacing(10);
	parseInfoVLay->addStretch(1);
	parseInfoVLay->addLayout(parseStatusImageHLay);
	parseInfoVLay->addWidget(parseStatusTextLabel);
	parseInfoVLay->addWidget(parseStatusDetailBrowser);
	parseInfoVLay->addLayout(parseStatusBtnHLay);
	parseStatusBtn->hide();
	parseInfoVLay->addStretch(1);
	slnConfigWidget = new SlnConfigWidget;
	slnConfigScrollArea = new QScrollArea;
	slnConfigScrollArea->setWidget(slnConfigWidget);
	slnConfigScrollArea->setObjectName("SlnConfigScrollArea");
	slnConfigScrollArea->setWidgetResizable(true);
	parseInfoWidgetIndex = slnInfoStackWidget->addWidget(parseInfoWidget);
	promptInfoWidgteIndex = slnInfoStackWidget->addWidget(promptInfoWidgte);
	slnConfigWidgetIndex = slnInfoStackWidget->addWidget(slnConfigScrollArea);
	slnInfoStackWidget->setObjectName("SlnInfoStackWidget");
	slnInfoStackWidget->setCurrentIndex(promptInfoWidgteIndex);

	bottomButtHLay = new QHBoxLayout();
	confirmButt = new QPushButton(trUtf8("确认"));
	cancelButt = new QPushButton(trUtf8("取消"));
	confirmButt->setDisabled(true);
	bottomButtHLay->addStretch(1);
	bottomButtHLay->addWidget(confirmButt);
	bottomButtHLay->addWidget(cancelButt);


	vAllLay->setContentsMargins(10, 10, 10, 10);
	vAllLay->setAlignment(Qt::AlignTop);
	vAllLay->addLayout(slnFileHLay);
	vAllLay->addWidget(slnInfoStackWidget, 1);
	vAllLay->addLayout(bottomButtHLay);
	
	resizeMoveToCenter(850, 530);

	parseSlnFileThread = new ParseSolutionThread(this);
	connect(parseSlnFileThread, SIGNAL(finished()), parseSlnFileThread, SLOT(deleteLater()));
	connect(parseSlnFileThread, SIGNAL(parsingFinished(VsSolution *, int, QString)), this, SLOT(handleSlnInfoParseResult(VsSolution *, int, QString)));
	connect(slnFileLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(newSlnFilePath(const QString &)));
	connect(browseSlnFileBtn, SIGNAL(clicked()), this, SLOT(browseSlnFileBtnClicked()));
	connect(parseStatusBtn, SIGNAL(clicked()), this, SLOT(handleStatusBtnClicked()));
	connect(confirmButt, SIGNAL(clicked()), this, SLOT(handleConfirmButtClicked()));
	connect(cancelButt, SIGNAL(clicked()), this, SLOT(handleCanclelButtClicked()));

	// 启动解析线程
	parseSlnFileThread->start();
	// 如果命令行参数中有指定解决方案文件 则直接加载该文件绝对路径路径
	slnFileLineEdit->setText(QFileInfo(slnFilePath).absoluteFilePath());


	setObjectName("VSConversionLinuxWidget");
	loadStyleSheet(":/qss/VSConversionLinuxWidget.qss");

}


VSConversionLinuxWidget::~VSConversionLinuxWidget()
{
	if (parseSlnFileThread != NULL)
	{
		// 停止解析线程 并回收线程资源
		parseSlnFileThread->quit();
		parseSlnFileThread->wait();
	}
	if (parseStatusWaitMovie != NULL)
	{
		delete parseStatusWaitMovie;
	}
}



void VSConversionLinuxWidget::resizeMoveToCenter(int w, int h)
{
	int resize_width, resize_height;
	QRect rect = QApplication::desktop()->screenGeometry();   //获取默认屏幕的宽高 


	resize_width = w;
	resize_height = h;

	if (w >= rect.width() || w <= 0)
	{
		resize_width = rect.width();
	}

	if (h >= rect.height() || h <= 0)
	{
		resize_height = rect.height();
	}

	resize(resize_width, resize_height);

	move(rect.center() - this->rect().center());   //move 是从默认屏幕的起始点移动  扩展屏按扩展坐标算
}



void VSConversionLinuxWidget::autoResizeMoveToDesktopCenter(double w_percent /*= 0.417*/, double h_percent /*= 0.484*/)
{
	int resize_width, resize_height;
	QRect rect = QApplication::desktop()->screenGeometry();   //获取默认屏幕的宽高 

	if (w_percent == 0.417 && h_percent == 0.484)
	{
		//如果使用默认参数 则进行自动适配
		if ((rect.width() >= 1024 && rect.width() <= 1440) && (rect.height() >= 768 && rect.height() <= 1024))
		{
			//小屏幕的策略
			w_percent = 0.44;
			h_percent = 0.52;
		}
		else if ((rect.width() >= 1680 && rect.width() <= 2048) && (rect.height() >= 1050 && rect.height() <= 1563))
		{
			w_percent = 0.349;
			h_percent = 0.398;
		}
	}
	//否则直接使用设定的值

	resize_width = rect.width() * w_percent;
	resize_height = rect.height() * h_percent;

	//qDebug() << " w_p " << w_percent << " h_p " << h_percent << "   " << rect << " w " << resize_width << " h " << resize_height;

	resize(resize_width, resize_height);

	move(rect.center() - this->rect().center());   //move 是从默认屏幕的起始点移动  扩展屏按扩展坐标算
}


void VSConversionLinuxWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> urlList = event->mimeData()->urls();
		if (urlList.size() > 0)
		{
			QFileInfo fileInfo(urlList.at(0).toLocalFile());
			if (fileInfo.exists() && fileInfo.suffix() == "sln")
			{
				slnFileLineEdit->setText(fileInfo.absoluteFilePath());
			}
			else
			{
				event->ignore();
			}
		}
	}
}


void VSConversionLinuxWidget::dragEnterEvent(QDragEnterEvent *event)
{
	//qDebug() << "dragEnterEvent: " << event->pos() << "  data: " << event->mimeData()->text();
	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> urlList = event->mimeData()->urls();
		if (urlList.size() > 0)
		{
			QFileInfo fileInfo(urlList.at(0).toLocalFile());
			if (fileInfo.exists() && fileInfo.suffix() == "sln")
			{
				event->acceptProposedAction();
			}
			else
			{
				event->ignore();
			}
		}
	}
}

void VSConversionLinuxWidget::handleSlnInfoParseResult(VsSolution *vsSln, int retCode, QString msg)
{
	QFileInfo slnFileInfo(slnFileLineEdit->text());
	
	if (slnFileInfo.exists())
	{
		slnFilePath = slnFileInfo.absoluteFilePath();
	}

	if (!slnFilePath.isEmpty() && vsSln->path.compare(slnFilePath) == 0)
	{
		// 是当前编辑的解决方案文件路径
		slnFileParsingEnd(vsSln, retCode, msg);
		if (retCode == 0)
		{
			// 解析成功且是当前编辑的解决方案文件路径
			qDebug() << "解析解决方案文件成功：" << vsSln->path;
			qDebug() << "包含项目数量：" << vsSln->projects.size();
		}	
	}
}

void VSConversionLinuxWidget::handleStatusBtnClicked()
{
	switchToSlnConfigWidget();
}

void VSConversionLinuxWidget::newSlnFilePath(const QString &slnFilePath)
{
	
	QFileInfo slnFileInfo(slnFilePath);
	if (slnFileInfo.exists() && slnFileInfo.suffix() == "sln")
	{
		if (slnFileInfo.absoluteFilePath() == currentVsSln.path)
		{
			//继续配置旧的项目
			confirmButt->setEnabled(true);
			slnInfoStackWidget->setCurrentIndex(slnConfigWidgetIndex);
		}
		else
		{
			qDebug() << "新解决方案文件路径：" << slnFilePath;
			// 触发异步线程解析解决方案文件
			slnFileParsingBegin();
			parseSlnFileThread->asyncParseSolutionFile(slnFileInfo.absoluteFilePath(), &currentVsSln);
		}
	}
	else
	{
		//输入的解决方案路径不存在或者不是解决方案路径显示提升窗体
		switchToPromptInfoWidgte();
	}
}

void VSConversionLinuxWidget::browseSlnFileBtnClicked()
{
	QString dirStr = slnFileLineEdit->text();
	if (dirStr.isEmpty())
	{
		dirStr = QDir::homePath();
	}
	QString newSlnFilePath = QFileDialog::getOpenFileName(this, trUtf8("选择Visual Studio解决方案文件"), dirStr, trUtf8("Visual Studio Solution Files (*.sln)"));
	if (!newSlnFilePath.isEmpty())
	{
		slnFileLineEdit->setText(newSlnFilePath);
	}
}

void VSConversionLinuxWidget::slnFileParsingBegin()
{
	confirmButt->setDisabled(true);
	slnInfoStackWidget->setCurrentIndex(parseInfoWidgetIndex);
	browseSlnFileBtn->setDisabled(true);
	slnFileLineEdit->setDisabled(true);
	parseStatusImageLabel->clear();
	parseStatusDetailBrowser->clear();
	parseStatusBtn->hide();
	parseStatusImageLabel->setMovie(parseStatusWaitMovie);
	parseStatusTextLabel->setText(trUtf8("正在解析"));
	// 清空当前解决方案信息
	currentVsSln.clear();
}

void VSConversionLinuxWidget::slnFileParsingEnd(VsSolution *vsSln, int retCode, QString msg)
{
	Q_UNUSED(vsSln);
	slnInfoStackWidget->setCurrentIndex(parseInfoWidgetIndex);
	slnFileLineEdit->setEnabled(true);
	browseSlnFileBtn->setEnabled(true);
	parseStatusImageLabel->clear();
	parseStatusDetailBrowser->clear();
	if (retCode == 0)
	{
		parseStatusTextLabel->setText(trUtf8("解析完成"));
		if (msg.isEmpty())
		{
			parseStatusImageLabel->setPixmap(QPixmap(":/images/success.png"));
			switchToSlnConfigWidget();
		}
		else
		{
			parseStatusImageLabel->setPixmap(QPixmap(":/images/warning.png"));
			parseStatusDetailBrowser->setText(msg);
			parseStatusBtn->show();
		}
	}
	else
	{
		parseStatusImageLabel->setPixmap(QPixmap(":/images/error.png"));
		parseStatusTextLabel->setText(trUtf8("解析失败"));
		parseStatusDetailBrowser->setText(msg);
	}
}

void VSConversionLinuxWidget::switchToSlnConfigWidget()
{
	confirmButt->setEnabled(true);
	slnInfoStackWidget->setCurrentIndex(slnConfigWidgetIndex);
	slnConfigWidget->updateSlnConfig(&currentVsSln);
}


void VSConversionLinuxWidget::switchToPromptInfoWidgte()
{
	confirmButt->setDisabled(true);
	slnInfoStackWidget->setCurrentIndex(promptInfoWidgteIndex);
}

void VSConversionLinuxWidget::handleConfirmButtClicked()
{
	slnConfigWidget->saveSlnConfig();
	QFileInfo slnFileInfo(slnFilePath);
	if (slnFileInfo.exists() && slnFileInfo.suffix() == "sln")
	{
		// 触发异步线程解析解决方案文件
		slnFileSaveAndParsingBegin();
		parseSlnFileThread->asyncSaveAndParseSolutionFile(slnFileInfo.absoluteFilePath(), &currentVsSln);
	}
}

void VSConversionLinuxWidget::handleCanclelButtClicked()
{
	close();
}

void VSConversionLinuxWidget::slnFileSaveAndParsingBegin()
{
	confirmButt->setDisabled(true);
	slnInfoStackWidget->setCurrentIndex(parseInfoWidgetIndex);
	browseSlnFileBtn->setDisabled(true);
	slnFileLineEdit->setDisabled(true);
	parseStatusImageLabel->clear();
	parseStatusDetailBrowser->clear();
	parseStatusBtn->hide();
	parseStatusImageLabel->setMovie(parseStatusWaitMovie);
	parseStatusTextLabel->setText(trUtf8("保存并重新加载"));
}
