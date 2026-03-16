/*!
 * FileName: SlnConfigWidget.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-5
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 解决方案配置窗体
 */

#include <QDebug>
#include <QLabel>
#include <QCheckBox>
#include <QButtonGroup>
#include <QSpacerItem>
#include <QFrame>
#include <QFileInfo>
#include <QDir>


#include "SlnConfigWidget.h"
#include "common/EasyDomOpt.h"


ProjectConfigGroupBox::ProjectConfigGroupBox(VsProject *_vsProject, QWidget *parent /*= nullptr*/) :QGroupBox(parent)
{
	vsProject = _vsProject;
	vAllLay = new QVBoxLayout(this);
	vAllLay->setAlignment(Qt::AlignTop);
	vAllLay->setSpacing(5);
	vAllLay->setContentsMargins(10, 10, 10, 10);
	for (int i = 0; i < vsProject->configPlatforms.size(); i++)
	{
		PlatformConfigWidget *platformConfigWidget = new PlatformConfigWidget(vsProject->configPlatforms[i]);
		vAllLay->addWidget(platformConfigWidget);
		projectConfigWidgetVec.append(platformConfigWidget);
	}
	setObjectName("ProjectConfigGroupBox");
}

ProjectConfigGroupBox::~ProjectConfigGroupBox()
{

}

void ProjectConfigGroupBox::saveProjectConfig()
{
	bool hasPlatformConversion = false;

	//依次保存每个平台配置 并检查是否有平台配置启用了转换
	for (PlatformConfigWidget *pcw : projectConfigWidgetVec)
	{
		pcw->savePlatformConfig();
		if (pcw->conversionEnabled())
		{
			hasPlatformConversion = true;
		}
	}
	QDomDocument projectDomDoc = *(vsProject->projectDomDocument);
	QDomElement globalsPropertyGroupEle = comm::getDomElement(projectDomDoc, "Project/PropertyGroup", "Label", "Globals");
	if (hasPlatformConversion && !globalsPropertyGroupEle.isNull())
	{
		QDomElement applicationTypeEle = globalsPropertyGroupEle.firstChildElement("ApplicationType");
		if (applicationTypeEle.isNull())
		{
			applicationTypeEle = vsProject->projectDomDocument->createElement("ApplicationType");
			comm::setDomElement(globalsPropertyGroupEle, "PropertyGroup/ApplicationType", applicationTypeEle);
		}
		comm::setDomElementText(globalsPropertyGroupEle, "PropertyGroup/ApplicationType", "Linux");
	}

	vsProject->saveFlag = true; //更新保存标识
	//comm::saveDomDocument(*(vsProject->projectDomDocument), vsProject->path, true);
	//qDebug() << "Save ------ " << vsProject->path;
}

PlatformConfigWidget::PlatformConfigWidget(ProjectConfigPlatform *_projConfigPlatform, QWidget *parent /*= nullptr*/):PainterWidget(parent)
{
	projectConfigPlatform = _projConfigPlatform;
	vALlLay = new QVBoxLayout(this);
	QHBoxLayout *platformNameHLay = new QHBoxLayout();
	platformNameLabel = new QLabel(projectConfigPlatform->config + " = " + projectConfigPlatform->platform);
	platformConfigRadioGroup = new QButtonGroup(this);
	convertPlatformConfigRadioBtn = new QRadioButton(trUtf8("转换"));
	deletePlatformConfigRadioBtn = new QRadioButton(trUtf8("删除"));
	copyPlatformtConfigConvertRadioBtn = new QRadioButton(trUtf8("复制并转换"));
	platformConfigRadioGroup->addButton(convertPlatformConfigRadioBtn);
	platformConfigRadioGroup->addButton(deletePlatformConfigRadioBtn);
	platformConfigRadioGroup->addButton(copyPlatformtConfigConvertRadioBtn);
	platformConfigRadioGroup->setExclusive(false); // 取消按钮组的互斥性，允许不选
	configureProjectBtn = new QPushButton;
	configureProjectBtn->setCheckable(true);
	configureProjectBtn->setObjectName("ConfigureProjectBtn");
	platformNameHLay->addWidget(platformNameLabel);
	platformNameHLay->addStretch(1);
	platformNameHLay->addWidget(convertPlatformConfigRadioBtn);
	platformNameHLay->addWidget(deletePlatformConfigRadioBtn);
	platformNameHLay->addWidget(copyPlatformtConfigConvertRadioBtn);
	platformNameHLay->addSpacing(5);
	platformNameHLay->addWidget(configureProjectBtn);
	vALlLay->addLayout(platformNameHLay);

	// 配置项目区域
	configureProjectWidget = new PainterWidget;
	configureProjectVLay = new QVBoxLayout(configureProjectWidget);
	configureProjectWidget->setObjectName("ConfigureProjectWidget");
	configureProjectWidget->setVisible(false);
	configureProjectVLay->setContentsMargins(5, 0, 5, 5);
	configureProjectVLay->setSpacing(5);

	includeDirListLineEditWidget = new ListLineViewEditWidget(trUtf8("包含目录"), ";", ListEditType_Path);
	includeDirListLineEditWidget->setPwd(projectConfigPlatform->vsProject->projectDir);
	configureProjectVLay->addWidget(includeDirListLineEditWidget);


	vALlLay->addWidget(configureProjectWidget);
	vALlLay->setSpacing(5);
	vALlLay->setContentsMargins(5, 0, 5, 0);
	setObjectName("PlatformConfigWidget");

	ConnectInfo connectInfo[] = {
		{platformConfigRadioGroup,SIGNAL(buttonClicked(QAbstractButton *)),this,SLOT(handlePlatformConfigRadioGroupBntClicked(QAbstractButton *)),Qt::AutoConnection}, // 实现单选并支持取消选择
		{configureProjectBtn,SIGNAL(toggled(bool)),this,SLOT(handleConfigureProjectBtnToggled(bool))}
	};

	SignalController::setConnectInfo(connectInfo, sizeof(connectInfo) / sizeof(ConnectInfo));
	updateConfigPlatform();
}

PlatformConfigWidget::~PlatformConfigWidget()
{

}

bool PlatformConfigWidget::conversionEnabled()
{
	return convertPlatformConfigRadioBtn->isChecked() || copyPlatformtConfigConvertRadioBtn->isChecked();
}

void PlatformConfigWidget::handlePlatformConfigRadioGroupBntClicked(QAbstractButton *btn)
{
	if (btn == NULL)
	{
		return;
	}
	if (btn == convertPlatformConfigRadioBtn || btn == deletePlatformConfigRadioBtn || btn == copyPlatformtConfigConvertRadioBtn)
	{
		// 只允许单选
		QList<QAbstractButton *> btnList = platformConfigRadioGroup->buttons();
		for (int i = 0; i < btnList.size(); i++)
		{
			if (btnList[i] != btn)
			{
				btnList[i]->setChecked(false);
			}
		}
	}
}

void PlatformConfigWidget::handleConfigureProjectBtnToggled(bool checked)
{
	configureProjectWidget->setVisible(checked);
}

void PlatformConfigWidget::updateConfigPlatform()
{
	QDomElement projectEle = comm::getDomElement(*(projectConfigPlatform->vsProject->projectDomDocument), "Project");
	QDomNodeList propertyGroups = projectEle.elementsByTagName("PropertyGroup");
	QString propertyGroupCondition = trUtf8("'$(Configuration)|$(Platform)'=='%1'").arg(projectConfigPlatform->platform);
	for (int i = 0; i < propertyGroups.count(); i++)
	{
		QDomElement propertyGroupEle = propertyGroups.at(i).toElement();
		if (!propertyGroupEle.isNull() &&
			propertyGroupEle.attribute("Condition") == propertyGroupCondition &&
			!propertyGroupEle.elementsByTagName("NMakeOutput").isEmpty())
		{
			includeDirListLineEditWidget->updateListText(QDir::fromNativeSeparators(comm::getDomElementText(propertyGroupEle, "PropertyGroup/NMakeIncludeSearchPath")));
			break;
		}
	}
}

void PlatformConfigWidget::savePlatformConfig()
{
	QDomDocument projectDomDoc= *(projectConfigPlatform->vsProject->projectDomDocument);
	QDomElement projectEle = comm::getDomElement(projectDomDoc, "Project");
	QDomElement projectConfigurationsItemGroup = comm::getDomElement(projectDomDoc, "Project/ItemGroup", "Label", "ProjectConfigurations");
	QDomNodeList propertyGroups = projectEle.elementsByTagName("PropertyGroup");
	QString propertyGroupCondition = trUtf8("'$(Configuration)|$(Platform)'=='%1'").arg(projectConfigPlatform->platform);
	QDomElement propertyGroupEle;                 //<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='XXXX|XXXX'">
	QDomElement propertyGroupConfigurationEle;    //<PropertyGroup Label="Configuration" Condition="'$(Configuration)|$(Platform)'=='XXXX|XXXX'">
	QDomElement projectConfigurationItemEle = comm::getDomElement(projectConfigurationsItemGroup, "ItemGroup/ProjectConfiguration", "Include", projectConfigPlatform->platform); //<ProjectConfiguration Include="XXXX|XXXX">

	for (int i = 0; i < propertyGroups.count(); i++)
	{
		QDomElement pe = propertyGroups.at(i).toElement();
		if (!pe.isNull() &&
			pe.attribute("Condition") == propertyGroupCondition)
		{
			if (pe.attribute("Label") == "Configuration")
			{
				propertyGroupConfigurationEle = pe;
			}
			else if (!pe.elementsByTagName("NMakeOutput").isEmpty())
			{
				propertyGroupEle = pe;
			}
			if (!propertyGroupConfigurationEle.isNull() && !propertyGroupEle.isNull())
			{
				break;
			}
		}
	}
	if (propertyGroupEle.isNull() || propertyGroupConfigurationEle.isNull())
	{
		return;
	}

	if (conversionEnabled()) //转换平台配置
	{
		QString newPlatform = projectConfigPlatform->platform;
		newPlatform.replace(QRegExp("\\|[[a-zA-Z0-9]+"), "|x64");
		projectConfigPlatform->conversionType = ProjectConfigPlatform::ConversionType_replace;
		if (copyPlatformtConfigConvertRadioBtn->isChecked())
		{
			projectConfigPlatform->conversionType = ProjectConfigPlatform::ConversionType_copyReplace;
		}

		//复制或转换前先查找是不是已经有了 有的话直接在目标配置上进行修改
		QDomElement newPropertyGroupConfigurationEle, newPropertyGroupEle, newProjectConfigurationItemEle;
		for (int i = 0; i < propertyGroups.count(); i++)
		{
			QDomElement pe = propertyGroups.at(i).toElement();
			if (!pe.isNull() &&
				pe.attribute("Condition") == trUtf8("'$(Configuration)|$(Platform)'=='%1'").arg(newPlatform))
			{
				if (pe.attribute("Label") == "Configuration")
				{
					newPropertyGroupConfigurationEle = pe;
				}
				else if (!pe.elementsByTagName("NMakeOutput").isEmpty())
				{
					newPropertyGroupEle = pe;
				}
				if (!newPropertyGroupConfigurationEle.isNull() && !newPropertyGroupEle.isNull())
				{
					break;
				}
			}
		}

		newProjectConfigurationItemEle = comm::getDomElement(projectConfigurationsItemGroup, "ItemGroup/ProjectConfiguration", "Include", newPlatform);
		if (newProjectConfigurationItemEle.isNull())
		{
			//如果没有新平台配置的节点则复制并转换平台配置节点并插入旧节点后面
			if (projectConfigPlatform->conversionType == ProjectConfigPlatform::ConversionType_copyReplace)
			{
				QDomNode copyNode = projectConfigurationsItemGroup.insertAfter(projectConfigurationItemEle.cloneNode(true), projectConfigurationItemEle);
				projectConfigurationItemEle = copyNode.toElement();
			}
		}
		else
		{
			//如果有的话直接使用已有节点
			projectConfigurationItemEle = newProjectConfigurationItemEle;
		}

		if (newPropertyGroupConfigurationEle.isNull())
		{
			//如果没有新平台配置的节点则复制并转换平台配置节点并插入旧节点后面
			if (projectConfigPlatform->conversionType == ProjectConfigPlatform::ConversionType_copyReplace)
			{
				QDomNode copyNode = projectEle.insertAfter(propertyGroupConfigurationEle.cloneNode(true), propertyGroupConfigurationEle);
				propertyGroupConfigurationEle = copyNode.toElement();
			}
		}
		else
		{
			//如果有的话直接使用已有节点
			propertyGroupConfigurationEle = newPropertyGroupConfigurationEle;
		}

		if (newPropertyGroupEle.isNull())
		{
			//如果没有新平台配置的节点则复制并转换平台配置节点并插入旧节点后面
			if (projectConfigPlatform->conversionType == ProjectConfigPlatform::ConversionType_copyReplace)
			{
				QDomNode copyNode = projectEle.insertAfter(propertyGroupEle.cloneNode(true), propertyGroupEle);
				propertyGroupEle = copyNode.toElement();
			}
		}
		else
		{
			//如果有的话直接使用已有节点
			propertyGroupEle = newPropertyGroupEle;
		}

		//转换平台配置为x64 这样才能作为linux项目进行解析
		propertyGroupEle.setAttribute("Condition", trUtf8("'$(Configuration)|$(Platform)'=='%1'").arg(newPlatform));
		propertyGroupConfigurationEle.setAttribute("Condition", trUtf8("'$(Configuration)|$(Platform)'=='%1'").arg(newPlatform));
		projectConfigurationItemEle.setAttribute("Include", newPlatform);
		comm::setDomElementText(projectConfigurationItemEle, "ProjectConfiguration/Platform", "x64");

		//被转换的项目需要加 IncludePath  ExternalIncludePath 两个节点
		QDomElement IncludePathEle = comm::getDomElement(propertyGroupEle, "PropertyGroup/IncludePath");
		if (IncludePathEle.isNull())
		{
			IncludePathEle = projectDomDoc.createElement("IncludePath");
			comm::setDomElement(propertyGroupEle, "PropertyGroup/IncludePath", IncludePathEle);
		}

		QDomElement ExternalIncludePathEle = comm::getDomElement(propertyGroupEle, "PropertyGroup/ExternalIncludePath");
		if (ExternalIncludePathEle.isNull())
		{
			ExternalIncludePathEle = projectDomDoc.createElement("ExternalIncludePath");
			comm::setDomElement(propertyGroupEle, "PropertyGroup/ExternalIncludePath", ExternalIncludePathEle);
		}
		
	}

	if (deletePlatformConfigRadioBtn->isChecked())//删除平台配置
	{
		projectConfigPlatform->conversionType = ProjectConfigPlatform::ConversionType_remove;
		propertyGroupEle.parentNode().removeChild(propertyGroupEle);
		propertyGroupConfigurationEle.parentNode().removeChild(propertyGroupConfigurationEle);
		projectConfigurationItemEle.parentNode().removeChild(projectConfigurationItemEle);
	}
	else
	{
		// 不删除则更新平台配置选项
		QDomElement NMakeIncludeSearchPathEle = comm::getDomElement(propertyGroupEle, "PropertyGroup/NMakeIncludeSearchPath");
		if (NMakeIncludeSearchPathEle.isNull())
		{
			NMakeIncludeSearchPathEle = projectDomDoc.createElement("NMakeIncludeSearchPath");
			comm::setDomElement(propertyGroupEle, "PropertyGroup/NMakeIncludeSearchPath", NMakeIncludeSearchPathEle);
		}
		comm::setDomElementText(propertyGroupEle, "PropertyGroup/NMakeIncludeSearchPath", QDir::toNativeSeparators(includeDirListLineEditWidget->getLineEditText()));

		/*qDebug() << "----------------------------------";
		qDebug() << projectConfigPlatform->vsProject->name << "   " << projectConfigPlatform->platform;
		qDebug() << comm::domElementToString(propertyGroupEle, "PropertyGroup");
		qDebug() << "----------------------------------";*/
	}
}

SlnConfigWidget::SlnConfigWidget(QWidget *parent /*= nullptr*/) :PainterWidget(parent)
{
	vsSln = NULL;
	vAllLay = new QVBoxLayout(this);
	vAllLay->setContentsMargins(5, 5, 5, 5);
	vAllLay->setSpacing(5);
	setObjectName("SlnConfigWidget");

	loadStyleSheet(":/qss/SlnConfigWidget.qss");
}

SlnConfigWidget::~SlnConfigWidget()
{

}

void SlnConfigWidget::updateSlnConfig(VsSolution *sln)
{
	vsSln = sln;
	int i = 0;
	for (i = 0; i < projectConfigGroupBoxVec.size(); i++)
	{
		vAllLay->removeWidget(projectConfigGroupBoxVec[i]);
		delete projectConfigGroupBoxVec[i];
	}
	projectConfigGroupBoxVec.clear();

	for (i = 0; i < vsSln->projects.size(); i++)
	{
		//qDebug() << "SlnConfigWidget::updateSlnConfig:" << vsSln->projectInfos[i]->name;
		ProjectConfigGroupBox *pcgb = new ProjectConfigGroupBox(vsSln->projects[i], this);
		pcgb->setTitle(vsSln->projects[i]->name);
		pcgb->setCheckable(true);
		pcgb->setChecked(false);
		vAllLay->addWidget(pcgb);
		projectConfigGroupBoxVec.append(pcgb);
	}
}

void SlnConfigWidget::saveSlnConfig()
{
	for (ProjectConfigGroupBox *pcgb : projectConfigGroupBoxVec)
	{
		if (pcgb->isChecked())
		{
			pcgb->saveProjectConfig();
		}
	}

}

