/*!
 * FileName: VSConversionLinuxGlobal.h
 *
 * Author:   ZhangChaoZe
 * Date:     2025-12-23
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 全局头文件
 */

#ifndef _VS_CONVERSION_LINUX_GLOBAL_H_
#define _VS_CONVERSION_LINUX_GLOBAL_H_

#include <QString>
#include <QVector>
#include <QList>
#include <QFileInfo>
#include <QDomDocument>

enum RunMode
{
	RUN_MODE_INVALID = -1,
	RUN_MODE_GUI = 0,      // 图形界面模式
	RUN_MODE_COMMANDLINE,  // 命令行模式
};

extern RunMode g_runMode;              // 默认图形界面模式
extern QString slnFilePath;			   // 待转换解决方案文件路径

class VsSolution;
class VsProject;
class ProjectConfigPlatform;


// 项目配置与平台结构体
class ProjectConfigPlatform
{
public:
	ProjectConfigPlatform(const QString &_config, const QString &_platform, const VsProject *_vsp) :config(_config), platform(_platform), vsProject(_vsp)
	{
		conversionType = ConversionType_none;
	}

	~ProjectConfigPlatform() {}

	enum ConversionType
	{
		ConversionType_none = 0,              //不转换
		ConversionType_replace = 1,           //转换
		ConversionType_remove = 2,            //删除
		ConversionType_copyReplace = 3,       //复制并转换
	};

public:
	QString config; // 项目配置
	QString platform; // 项目平台
	const VsProject *vsProject; // 平台配置所属的项目指针
	ConversionType conversionType;
private:
	explicit ProjectConfigPlatform(const ProjectConfigPlatform &); //禁止复制
	ProjectConfigPlatform &operator=(const ProjectConfigPlatform &); //禁止赋值
};

class VsProject
{
public:
	VsProject(const QString &_path, const QString &_name, const QString &_guid, const VsSolution *_vs) :path(_path), name(_name), guid(_guid), vsSolution(_vs) 
	{
		QFileInfo fi(path);
		projectDir = fi.absolutePath();
		projectDomDocument = NULL;
		saveFlag = false;
	}
	~VsProject() 
	{
		for (ProjectConfigPlatform *pcp : configPlatforms)
		{
			delete pcp;
		}
		configPlatforms.clear();
		if (projectDomDocument)
		{
			delete projectDomDocument;
			projectDomDocument = NULL;
		}
	}


	QString name;	// 项目名称
	QString path;   // 项目绝对路径
	QString guid;   // 项目GUID
	QString projectDir;    //项目目录
	const VsSolution *vsSolution; // 项目所属的解决方案指针
	QVector<ProjectConfigPlatform *> configPlatforms; // 项目下的平台配置列表
	QDomDocument *projectDomDocument;
	bool saveFlag;    //保存标识

	ProjectConfigPlatform *findConfigPlatform(const QString &config, const QString &platform)
	{
		ProjectConfigPlatform *configPlatform = NULL;
		for (ProjectConfigPlatform *pcp : configPlatforms)
		{
			if (pcp->config == config && pcp->platform == platform)
			{
				configPlatform = pcp;
				break;
			}
		}
		return configPlatform;
	}
private:
	explicit VsProject(const VsProject &); //禁止复制
	VsProject &operator=(const VsProject &); //禁止赋值
};

//解决方案信息结构体
class VsSolution
{
public:
	VsSolution(const QString &sln_path = QString()) 
	{ 
		path = sln_path; 
		QFileInfo fi(path);
		slnDir = fi.absolutePath();
	}
	~VsSolution()
	{
		clear();
	}

	QString path; // 解决方案绝对路径
	QString slnDir; // 解决方案目录
	QVector<VsProject *> projects; // 解决方案中包含的项目列表

	void clear()
	{
		path.clear();
		for (VsProject *vsp : projects)
		{
			delete vsp;
		}
		projects.clear();
	}
private:
	explicit VsSolution(const VsSolution &);  //禁止复制
	VsSolution &operator=(const VsSolution &); //禁止赋值
};



#endif
