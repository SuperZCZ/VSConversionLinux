/*!
 * FileName: ParseSolutionThread.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2025-12-23
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 解析解决方案线程
 */

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QVector>
#include <QFileInfo>
#include <QDir>

#include "ParseSolutionThread.h"
#include "common/EasyDomOpt.h"

ParseSolutionThread::ParseSolutionThread(QObject *parent /*= nullptr*/) :QThread(parent)
{
	ParseSolutionWorker *worker = new ParseSolutionWorker;
	worker->moveToThread(this);
	connect(this, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(this, SIGNAL(SIG_parseSolutionFile(const QString &, VsSolution *)), worker, SLOT(parseSolutionFile(const QString &, VsSolution *)));
	connect(this, SIGNAL(SIG_saveAndParseSolutionFile(const QString &, VsSolution *)), worker, SLOT(saveAndParseSolutionFile(const QString &, VsSolution *)));
	connect(worker, SIGNAL(parsingFinished(VsSolution *, int, QString)), this, SIGNAL(parsingFinished(VsSolution *, int, QString)));
}

ParseSolutionThread::~ParseSolutionThread()
{
	qDebug() << "ParseSolutionThread Destructor called. Thread:" << QThread::currentThread();
}

void ParseSolutionThread::asyncParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln)
{
	//发出信号 触发异步线程中工作对象的解析函数
	emit SIG_parseSolutionFile(slnFilePath, vsSln);
}

void ParseSolutionThread::asyncSaveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln)
{
	emit SIG_saveAndParseSolutionFile(slnFilePath, vsSln);
}

ParseSolutionWorker::~ParseSolutionWorker()
{
	qDebug() << "ParseSolutionWorker Destructor called. Thread:" << QThread::currentThread();
}

QString ParseSolutionWorker::readFileByText(const QString &slnFilePath)
{
	QFile slnFile(slnFilePath);
	slnFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream slnStream(&slnFile);
	slnStream.setGenerateByteOrderMark(true);
	slnStream.setCodec("UTF-8");
	slnStream.setAutoDetectUnicode(true);
	QString slnFileText = slnStream.readAll();
	return slnFileText;
}

bool ParseSolutionWorker::saveFileByText(const QString &slnFilePath, const QString &Text)
{
	bool res = false;
	QFile slnFile(slnFilePath);
	res = slnFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	if (res)
	{
		QTextStream slnStream(&slnFile);
		slnStream.setGenerateByteOrderMark(true);
		slnStream.setCodec("UTF-8");
		slnStream << Text;
	}
	slnFile.close();
	return res;
}

void ParseSolutionWorker::parseSolutionFile(const QString &slnFilePath, VsSolution *vsSln)
{
	QString msg;
	int res = parseSolutionFile(slnFilePath, vsSln, msg);
	emit parsingFinished(vsSln, res, msg);
}

void ParseSolutionWorker::saveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln)
{
	QString msg;
	int res = saveAndParseSolutionFile(slnFilePath, vsSln, msg);
	emit parsingFinished(vsSln, res, msg);
}


int ParseSolutionWorker::saveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln, QString &msg)
{
	int res = 0;
	QFileInfo slnFileInfo(slnFilePath);
	if (!slnFileInfo.exists())
	{
		msg = trUtf8("解决方案文件不存在：%1").arg(slnFilePath);
		return -1; // 文件不存在
	}
	QString slnFileText = readFileByText(slnFilePath);
	QString slnFileTextBackup = slnFileText;
	int pos = 0;

	//保存旧的
	for (VsProject *vsp : vsSln->projects)
	{
		if (vsp->saveFlag)
		{
			for (ProjectConfigPlatform *pcp : vsp->configPlatforms)
			{
				pos = 0;
				QRegExp projectConfigInfoRegExp(QString("([\\t ]+\\{%1\\}\\.%2\\.[a-zA-Z0-9\\|\\.]+ = %3\\n)").arg(vsp->guid).arg(QString(pcp->config).replace("|","\\|")).arg(QString(pcp->platform).replace("|","\\|")));
				switch (pcp->conversionType)
				{
				case ProjectConfigPlatform::ConversionType_replace:
				{
					while ((pos = projectConfigInfoRegExp.indexIn(slnFileText, pos)) != -1)
					{
						QString newConfText = projectConfigInfoRegExp.cap(1);
						int pos1 = newConfText.indexOf("|");
						int pos2 = newConfText.indexOf(".", pos1);
						newConfText.replace(pos1 + 1, pos2 - pos1 - 1, "x64");
						pos1 = newConfText.lastIndexOf("|");
						pos2 = newConfText.lastIndexOf("\n");
						newConfText.replace(pos1 + 1, pos2 - pos1 - 1, "x64");
						slnFileText.replace(pos, projectConfigInfoRegExp.matchedLength(), newConfText);
						pos = pos + newConfText.length();
					}
					break;
				}
				case ProjectConfigPlatform::ConversionType_remove:
				{
					while ((pos = projectConfigInfoRegExp.indexIn(slnFileText, pos)) != -1)
					{
						slnFileText.remove(pos, projectConfigInfoRegExp.matchedLength());
					}
					break;
				}
				case ProjectConfigPlatform::ConversionType_copyReplace:
				{
					while ((pos = projectConfigInfoRegExp.indexIn(slnFileText, pos)) != -1)
					{
						QString newConfText = projectConfigInfoRegExp.cap(1);
						int pos1 = newConfText.indexOf("|");
						int pos2 = newConfText.indexOf(".", pos1);
						newConfText.replace(pos1 + 1, pos2 - pos1 - 1, "x64");
						pos1 = newConfText.lastIndexOf("|");
						pos2 = newConfText.lastIndexOf("\n");
						newConfText.replace(pos1 + 1, pos2 - pos1 - 1, "x64");
						if (slnFileText.indexOf(newConfText) == -1)  //新增加的不存在时才插入
						{
							slnFileText.insert(pos + projectConfigInfoRegExp.matchedLength(), newConfText);
							pos = pos + projectConfigInfoRegExp.matchedLength() + newConfText.length();
						}
						else
						{
							pos = pos + projectConfigInfoRegExp.matchedLength();
						}
					}
					break;
				}
				default:
				{
					break;
				}
				}
			}
			qDebug() << "Save:" << vsp->path;
			comm::saveDomDocument(*(vsp->projectDomDocument), vsp->path, true); //保存vcxproj
		}
	}

	//整理平台配置并更新GlobalSection(SolutionConfigurationPlatforms) = preSolution段落
	if (slnFileText != slnFileTextBackup)
	{
		pos = 0;
		QStringList platformStrList;
		QRegExp projectConfigInfoRegExp(QString("\\{[A-F0-9\\-]+\\}\\.([a-zA-Z0-9\\|]+)\\.[a-zA-Z0-9\\|\\.]+ = ([a-zA-Z0-9\\|]+)\\n"));
		while ((pos = projectConfigInfoRegExp.indexIn(slnFileText, pos)) != -1)
		{
			if (projectConfigInfoRegExp.captureCount() == 2)
			{
				QString platformConfigStr = trUtf8("%1 = %2").arg(projectConfigInfoRegExp.cap(1)).arg(projectConfigInfoRegExp.cap(1));
				platformStrList.append(platformConfigStr);
			}
			pos = pos + projectConfigInfoRegExp.matchedLength();
		}
		platformStrList.removeDuplicates();
		QRegExp globalSectionRegExp("(GlobalSection\\(SolutionConfigurationPlatforms\\).*EndGlobalSection)", Qt::CaseInsensitive);
		globalSectionRegExp.setMinimal(true);
		if ((pos = globalSectionRegExp.indexIn(slnFileText, 0)) != -1)
		{
			QString newSolutionConfigurationPlatformsStr = "GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
			for (QString platformStr : platformStrList)
			{
				newSolutionConfigurationPlatformsStr.append(trUtf8("\t\t%1\n").arg(platformStr));
			}
			newSolutionConfigurationPlatformsStr.append("\tEndGlobalSection");

			slnFileText.replace(pos, globalSectionRegExp.matchedLength(), newSolutionConfigurationPlatformsStr);
		}
		//qDebug() << slnFileText;
		qDebug() << "Save:" << slnFilePath;
		saveFileByText(slnFilePath , slnFileText);
	}

	//解析新的
	res = parseSolutionFile(slnFilePath, vsSln, msg);
	return res;
}



int ParseSolutionWorker::parseSolutionFile(const QString &slnFilePath, VsSolution *vsSln, QString &msg)
{
	msg.clear();
	if (!vsSln)
	{
		msg = trUtf8("内部错误");
		return -3;
	}
	QFileInfo slnFileInfo(slnFilePath);
	if (!slnFileInfo.exists())
	{
		msg = trUtf8("解决方案文件不存在：%1").arg(slnFilePath);
		return -1; // 文件不存在
	}

	//重置解析内容
	vsSln->projects.clear();
	vsSln->path = slnFileInfo.absoluteFilePath();

	// 处理解析解决方案的逻辑
	//qDebug() << "Parsing solution file: " << slnFilePath << "Thread:" << QThread::currentThread();

	QFile slnFile(vsSln->path);
	slnFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream slnStream(&slnFile);
	slnStream.setCodec("UTF-8");
	slnStream.setAutoDetectUnicode(true);
	QString slnFileText = slnStream.readAll();

	QString  slnFileDirPath = QDir::toNativeSeparators(QFileInfo(vsSln->path).absolutePath()); //sln文件中的文件路径都是Windows格式的 \ 分隔符
	if (!slnFileDirPath.endsWith(QDir::separator()))
	{
		slnFileDirPath.append(QDir::separator());
	}

	if (!slnFileText.trimmed().startsWith("Microsoft Visual Studio Solution File, Format Version"))
	{
		// 不是有效的解决方案文件
		qDebug() << "Not a valid Visual Studio Solution file.";
		msg = trUtf8("不是有效的Visual Studio解决方案文件：%1").arg(slnFilePath);
		return -2;
	}


	QRegExp projectRegExp("(Project\\(\"\\{[A-F0-9\\-]+\\}\"\\) = \"[^\"]+\", \"[^\"]+\", \"\\{[A-F0-9\\-]+\\}\")", Qt::CaseInsensitive);
	QRegExp projectInfoRegExp("^Project\\(\"\\{[A-F0-9\\-]+\\}\"\\) = \"([^\"]+)\", \"([^\"]+)\", \"\\{([A-F0-9\\-]+)\\}\"$", Qt::CaseInsensitive);
	QRegExp globalSectionRegExp("(GlobalSection\\(ProjectConfigurationPlatforms\\).*EndGlobalSection)", Qt::CaseInsensitive);
	globalSectionRegExp.setMinimal(true);
	int pos = 0;
	while ((pos = projectRegExp.indexIn(slnFileText, pos)) != -1)
	{
		//qDebug() << "Found project: " << projectRegExp.cap(1);
		QString projectLine = projectRegExp.cap(1);
		// 进一步处理projectLine以提取项目名称、路径等信息
		if (projectInfoRegExp.indexIn(projectLine) != -1 && projectInfoRegExp.captureCount() == 3)
		{
			QString projectName = projectInfoRegExp.cap(1);
			QString projectPath = projectInfoRegExp.cap(2);
			QString projectGUID = projectInfoRegExp.cap(3);

			if (projectPath.endsWith(".vcxproj") || projectPath.endsWith(".csproj") || projectPath.endsWith(".vbproj"))
			{
				// 这是一个有效的项目文件
				//qDebug() << "  Project Name: " << projectName;
				//qDebug() << "  Project Path: " << projectPath;
				//qDebug() << "  Project GUID: " << projectGUID;
				// 记录项目基本信息
				QString projPath;
				QFileInfo projFileInfo(projectPath);
				if (projFileInfo.isAbsolute())
				{
					projPath = QFileInfo(projectPath).absoluteFilePath();
				}
				else
				{
					projPath = QFileInfo(slnFileDirPath + projectPath).absoluteFilePath();
				}
				vsSln->projects.append(new VsProject(projPath, projectName, projectGUID, vsSln));
			}
		}
		pos += projectRegExp.matchedLength();
	}

	// 根据项目基本信息继续获取项目平台配置
	if ((vsSln->projects).size() > 0 && globalSectionRegExp.indexIn(slnFileText, 0) != -1)
	{
		QString globalSectionText = globalSectionRegExp.cap(1);
		//qDebug() << globalSectionText;
		for (int i = 0; i < (vsSln->projects.size()); i++)
		{
			QRegExp projectConfigInfoRegExp(QString("\\{%1\\}\\.([a-zA-Z0-9\\|]+)\\.[a-zA-Z0-9\\|\\.]+ = ([a-zA-Z0-9\\|]+)\\n").arg((vsSln->projects[i])->guid));
			pos = 0;
			while ((pos = projectConfigInfoRegExp.indexIn(globalSectionText, pos)) != -1)
			{
				if (projectConfigInfoRegExp.captureCount() == 2)
				{
					//qDebug() << projectInfos[i].guid << " [" << projectConfigInfoRegExp.cap(1) << "] [" << projectConfigInfoRegExp.cap(2) << "]";
					// 查找已有项目配置与平台信息 避免重复添加
					QString projectConfig = projectConfigInfoRegExp.cap(1);
					QString projectPlatform = projectConfigInfoRegExp.cap(2);
					if (!projectConfig.isEmpty() && 
						!projectPlatform.isEmpty() && 
						!vsSln->projects[i]->findConfigPlatform(projectConfig, projectPlatform))
					{
						vsSln->projects[i]->configPlatforms.append(new ProjectConfigPlatform(projectConfig, projectPlatform, vsSln->projects[i]));
					}
				}
				pos += projectConfigInfoRegExp.matchedLength();
			}
			if (0 != parseProjectFile(vsSln->projects[i]->path, vsSln->projects[i], msg))
			{
				return -4;
			}
		}
	}

	/*for (int i = 0; i < vsSln->projects.size(); i++)
	{
		qDebug() << "\n\n------------------------" << vsSln->projects[i]->name << "-------------------------------";
		qDebug() << "Path:" << vsSln->projects[i]->path;
		qDebug() << "UUID:" << vsSln->projects[i]->guid;
		qDebug() << "ConfigPlatforms:";
		for (int j = 0; j < vsSln->projects[i]->configPlatforms.size(); j++)
		{
			qDebug() << vsSln->projects[i]->configPlatforms[j]->config << " = " << vsSln->projects[i]->configPlatforms[j]->platform;
		}
		qDebug() << "-------------------------------------------------------";
	}*/
	return 0;
}


int ParseSolutionWorker::parseProjectFile(const QString &projectPath, VsProject *vsProject, QString &msg)
{
	QDomDocument *projectDoc = NULL;
	if (vsProject->projectDomDocument == NULL)
	{
		vsProject->projectDomDocument = new QDomDocument;
	}
	projectDoc = vsProject->projectDomDocument;

	if (!comm::loadDomDocument(*projectDoc, projectPath))
	{
		msg = trUtf8("项目文件解析错误 %1").arg(projectPath);
		return -1;
	}
	QDomElement projectEle = projectDoc->documentElement();
	QDomNodeList propertyGroups = projectEle.elementsByTagName("PropertyGroup");
	if (propertyGroups.isEmpty())
	{
		msg = trUtf8("项目文件内容错误");
		return -2;
	}

	QVector<ProjectConfigPlatform *> notFoundPcpVec;
	for (ProjectConfigPlatform *pcp : vsProject->configPlatforms)
	{
		QString propertyGroupCondition = trUtf8("'$(Configuration)|$(Platform)'=='%1'").arg(pcp->platform);
		bool hasProperty = false;
		for (int i = 0; i < propertyGroups.count(); i++)
		{
			QDomElement propertyGroupEle = propertyGroups.at(i).toElement();
			if (!propertyGroupEle.isNull() && 
				propertyGroupEle.attribute("Condition") == propertyGroupCondition &&
				!propertyGroupEle.elementsByTagName("NMakeOutput").isEmpty())
			{
				hasProperty = true;
				break;
			}
		}
		if (!hasProperty)
		{
			//在项目文件中未找到对应的平台配置项
			notFoundPcpVec.append(pcp);
		}
	}

	//删除配置文件中没有的平台配置
	for (ProjectConfigPlatform *pcp : notFoundPcpVec)
	{
		vsProject->configPlatforms.remove(vsProject->configPlatforms.indexOf(pcp));
	}


	return 0;
}
