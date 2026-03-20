/*!
 * FileName: SlnConfigWidget.h
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-5
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 解决方案配置窗体
 */
#ifndef _SLN_CONFIG_WIDGET_H_
#define _SLN_CONFIG_WIDGET_H_

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include "VSConversionLinuxGlobal.h"
#include "PainterWidget.h"
#include "SignalController.h"
#include "ListLineViewEditWidget.h"

class PlatformConfigWidget :public PainterWidget
{
	Q_OBJECT
public:
	PlatformConfigWidget(ProjectConfigPlatform *_projConfigPlatform, QWidget *parent = nullptr);
	virtual ~PlatformConfigWidget();

	bool conversionEnabled();
private:
	ProjectConfigPlatform *projectConfigPlatform;
	QVBoxLayout *vALlLay;
	QLabel *platformNameLabel;	// 项目名称标签
	QButtonGroup *platformConfigRadioGroup; // 项目配置单选按钮组
	QRadioButton *convertPlatformConfigRadioBtn; // 转换项目默认配置单选
	QRadioButton *deletePlatformConfigRadioBtn; // 删除项目单选
	QRadioButton *copyPlatformtConfigConvertRadioBtn; // 复制项目并转换单选
	QPushButton *configureProjectBtn; // 配置项目按钮
	QVBoxLayout *configureProjectVLay; // 配置项目布局
	PainterWidget *configureProjectWidget; // 配置项目窗体
	ListLineViewEditWidget *includeDirListLineEditWidget; // 包含目录编辑控件
	QLineEdit *includeDirLineEdit; // 包含目录输入框
	QPushButton *expandIncludeDirBtn; // 展开包含目录按钮

public slots:
	void handlePlatformConfigRadioGroupBntClicked(QAbstractButton *btn);
	void handleConfigureProjectBtnToggled(bool checked);
	void updateConfigPlatform();
	void savePlatformConfig();
};

class ProjectConfigGroupBox :public QGroupBox
{
	Q_OBJECT
public:
	ProjectConfigGroupBox(VsProject *_vsProject, QWidget *parent = nullptr);
	~ProjectConfigGroupBox();
private:
	QVBoxLayout *vAllLay;
	QVector<PlatformConfigWidget *> projectConfigWidgetVec;
	VsProject *vsProject;
	QHBoxLayout *clangModeHLay;
	QCheckBox *clangModeCheckBox;
	QCheckBox *enableAllWaringCheckBox;
	QComboBox *cLanguageStandardComboBox;
	QComboBox *cppLanguageStandardComboBox;

public slots:
	void saveProjectConfig();
	void clangModeToggled(bool checked);
};



class SlnConfigWidget :public PainterWidget
{
	Q_OBJECT
public:
	SlnConfigWidget(QWidget *parent = nullptr);
	virtual ~SlnConfigWidget();
private:
	QVBoxLayout *vAllLay;
	QVector<ProjectConfigGroupBox *> projectConfigGroupBoxVec;
	VsSolution *vsSln;
public:
	void updateSlnConfig(VsSolution *sln);

public slots:
	void saveSlnConfig();
};

#endif