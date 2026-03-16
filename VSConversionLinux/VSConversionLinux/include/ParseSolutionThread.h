/*!
 * FileName: ParseSolutionThread.h
 *
 * Author:   ZhangChaoZe
 * Date:     2025-12-23
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 解析解决方案线程
 */

#ifndef _PARSE_SOLUTION_THREAD_H_
#define _PARSE_SOLUTION_THREAD_H_

#include <QThread>
#include <QString>

#include "VSConversionLinuxGlobal.h"

class ParseSolutionWorker : public QObject
{
	Q_OBJECT
public:
	ParseSolutionWorker(QObject *parent = nullptr) :QObject(parent) {}
	virtual ~ParseSolutionWorker();



	//************************************
	// Method:    parseSolutionFile
	// FullName:  ParseSolutionWorker::parseSolutionFile
	// Access:    public 
	// Returns:   int
	// Qualifier: 同步方式解析解决方案文件
	// Parameter: const QString & slnFilePath
	// Parameter: VsSolution * vsSln
	// Parameter: QString & msg 解析状态或错误信息 仅当返回值不为0时有效
	//************************************
	int parseSolutionFile(const QString &slnFilePath, VsSolution *vsSln, QString &msg);

	//************************************
	// Method:    parseProjectFile
	// FullName:  ParseSolutionWorker::parseProjectFile
	// Access:    public 
	// Returns:   int
	// Qualifier: 同步方式解析项目文件*.vcxproj
	// Parameter: const QString & projectPath
	// Parameter: VsProject * vsProject
	// Parameter: QString & msg
	//************************************
	int parseProjectFile(const QString &projectPath, VsProject *vsProject, QString &msg);

	int saveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln, QString &msg);

public slots:
	//************************************
	// Method:    parseSolutionFile
	// FullName:  ParseSolutionWorker::parseSolutionFile
	// Access:    public 
	// Returns:   void
	// Qualifier: 解析解决方案文件槽函数 完成后发出parsingFinished信号并传递解析结果
	// Parameter: const QString & slnFilePath
	// Parameter: VsSolution * vsSln
	//************************************
	void parseSolutionFile(const QString &slnFilePath, VsSolution *vsSln);

	void saveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln);
signals:
	void parsingFinished(VsSolution *vsSln, int retCode, QString msg);

private:
	QString readFileByText(const QString &slnFilePath);
	bool saveFileByText(const QString &slnFilePath,const QString &Text);
};


class ParseSolutionThread : public QThread
{
	Q_OBJECT
public:
	ParseSolutionThread(QObject *parent = nullptr);
	virtual ~ParseSolutionThread();
public slots:
	//************************************
	// Method:    asyncParseSolutionFile
	// FullName:  ParseSolutionThread::asyncParseSolutionFile
	// Access:    public 
	// Returns:   void
	// Qualifier: 异步方式解析解决方案文件 通过信号槽机制调用异步线程中工作对象的解析函数
	//            解析完成后发出parsingFinished信号并传递解析结果
	// Parameter: const QString & slnFilePath
	// Parameter: VsSolution * vsSln
	//************************************
	void asyncParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln);

	void asyncSaveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln);
signals:
	void parsingFinished(VsSolution *vsSln, int retCode, QString msg);

signals:
	void SIG_parseSolutionFile(const QString &slnFilePath, VsSolution *vsSln);
	void SIG_saveAndParseSolutionFile(const QString &slnFilePath, VsSolution *vsSln);
};


#endif //_PARSE_SOLUTION_THREAD_H_