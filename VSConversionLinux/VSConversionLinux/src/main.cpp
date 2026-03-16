/*!
 * FileName: main.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2025-12-21
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 将现有的VS工程转换为Linux项目类型以加强intellisense
 */

#include <iostream>

#include <QApplication>
#include <QObject>
#include <QCommandLineParser>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include <QResource>

#include "VSConversionLinuxGlobal.h"
#include "VSConversionLinuxWidget.h"
#include "ParseSolutionThread.h"
#include "SignalController.h"


SignalController *signalController = NULL; // 信号控制器实例
RunMode g_runMode = RUN_MODE_GUI;  // 默认图形界面模式
QString slnFilePath;			   // 待转换解决方案文件路径

bool loadResources(const QString &theme)
{
	QString rcc_path = QApplication::applicationDirPath();
	rcc_path.append("/Resources/");
	rcc_path.append(theme);
	return QResource::registerResource(rcc_path);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	QApplication app(argc, argv);
	QStringList argsList;
	QCommandLineParser argParser;
	VSConversionLinuxWidget* mainWidget = NULL;

	app.setQuitOnLastWindowClosed(true);

	for (int i = 0; i < argc; i++)
	{
		argsList.append(QString::fromLocal8Bit(argv[i]));
	}

	// 设置命令行参数
	argParser.setApplicationDescription(QObject::trUtf8("Convert VS project to VS Linux project to improve intellisense ability of Linux code."));
	argParser.addHelpOption();
	argParser.addOptions({
		// -c, --command   命令行运行
		{{"c","command"},QObject::trUtf8("命令行运行")},
	});

	// sln             Visual Studio Solution File.
	argParser.addPositionalArgument("sln", "Visual Studio Solution File.");
	argParser.process(app);


	//解析命令行参数
	if (argParser.isSet("command"))
	{
		// 命令行模式
		g_runMode = RUN_MODE_COMMANDLINE;
	}
	else
	{
		// GUI模式
		g_runMode = RUN_MODE_GUI;
	}

	// 获取解决方案文件路径
	if (argParser.positionalArguments().size() > 0)
	{
		slnFilePath = argParser.positionalArguments().first();
	}


	// 根据运行模式处理
	if (g_runMode == RUN_MODE_COMMANDLINE)
	{
		qDebug() << "命令行模式";
		if (slnFilePath.isEmpty())
		{
			ret = -1;
			std::cerr << "Error: [Missing parameter] No Visual Studio solution file specified.\n";
			goto out;
		}

		QFileInfo fi(slnFilePath);
		if (!fi.exists())
		{
			ret = -1;
			std::cerr << "Error: [Parameter error] Visual Studio solution file does not exist.\n";
			std::cerr << "       File path: " << slnFilePath.toLocal8Bit().data() << "\n";
			goto out;
		}

		if (fi.suffix() != "sln")
		{
			ret = -1;
			std::cerr << "Error: [Parameter error] Please specify a solution file with a .sln extension.\n";
			std::cerr << "       File path: " << slnFilePath.toLocal8Bit().data() << "\n";
			goto out;
		}
		//TODO ...

		ParseSolutionThread *parseThread = new ParseSolutionThread(&app);
		qDebug() << "-------------------------------------------";
		qDebug()  << fi.absoluteFilePath();
		qDebug() << "-------------------------------------------";
		parseThread->start();
		parseThread->wait();
		delete parseThread;

		goto out;
	}
	else if (g_runMode == RUN_MODE_GUI)
	{
		qDebug() << "GUI模式";
		QFileInfo fi(slnFilePath);
		if (fi.exists() && fi.suffix() == "sln")
		{
			slnFilePath = fi.absoluteFilePath();
			qDebug() << "-------------------------------------------";
			qDebug() << slnFilePath;
			qDebug() << "-------------------------------------------";
		}
		else
		{
			slnFilePath.clear();
		}

		// 创建信号控制器
		signalController = SignalController::createSignalController(&app);

		if (!loadResources("DefaultTheme.rcc")) //加载资源文件
		{
			qDebug() << "Load Resources Error!";
			return 1;
		}

		// 显示主窗口
		mainWidget = new VSConversionLinuxWidget();
		mainWidget->show();
	}

	ret = app.exec();
out:
	if (mainWidget != NULL)
	{
		mainWidget->close();
		delete mainWidget;
	}
	if (signalController != NULL)
	{
		SignalController::destroySignalController();
	}
	return ret;
}


