/*!
 * FileName: CombineFileDialog.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2026-3-7
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 混合文件选择对话框，允许用户选择一个或多个文件或目录
 */

#include <QDir>
#include <QFileDialog>
#include <QListView>
#include <QTreeView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDebug>

#include "CombineFileDialog.h"

CombineFileDialog::CombineFileDialog(QWidget *parent /*= nullptr*/, bool _bothFileAndDirSelectEnable /*= false*/, const QString &caption /*= QString()*/, const QString &directory /*= QString()*/, const QString &filter /*= QString()*/) :QFileDialog(parent, caption, directory, filter)
{
	setOption(QFileDialog::DontUseNativeDialog);

	//修改open按钮的动作
	QDialogButtonBox *btnbox = this->findChild<QDialogButtonBox *>("buttonBox");
	if (btnbox != NULL)
	{
		//因为当任何目录和文件都不选择时 默认是选中caption目录 所以要强制 disable一下 choose按钮 这样在什么都不选择时就不能accept
		if (this->acceptMode() == QFileDialog::AcceptOpen)
		{
			if (QPushButton *button = btnbox->button(QDialogButtonBox::Open))
			{
				button->setDisabled(true);
			}
		}
		else
		{
			if (QPushButton *button = btnbox->button(QDialogButtonBox::Save))
			{
				button->setDisabled(true);
			}
		}
	}

	
	bothFileAndDirSelectEnable = _bothFileAndDirSelectEnable;
	if (bothFileAndDirSelectEnable)
	{
		//设置文件选择模式为既可以选择文件又可以选择目录
		setFileMode(QFileDialog::Directory);
		setOption(QFileDialog::ShowDirsOnly, false); // 允许显示文件和目录
		//修改候选文件名的改变信号 以允许选中
		QLineEdit *edit = findChild<QLineEdit *>("fileNameEdit");
		if (NULL != edit)
		{
			connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(filesEditTextChanged(const QString &)), Qt::QueuedConnection);
		}

		QLabel *fileLabel = findChild<QLabel *>("fileNameLabel");
		if (fileLabel != NULL)
		{
			fileLabel->setText("File/Dir Name:");
		}
		if (btnbox != NULL)
		{
			// 断开原有的accepted信号与QFileDialog::accept槽函数的连接 改函数会校验FileMode和当前选中的文件是否匹配 但现在我们允许既可以选择文件又可以选择目录 所以要断开这个连接
			disconnect(btnbox, SIGNAL(accepted()), this, SLOT(accept())); 
			// 直接将accepted信号连接到基类QDialog::accept槽函数 不会校验FileMode和当前选中的文件是否匹配 这样就允许既可以选择文件又可以选择目录了
			connect(btnbox, SIGNAL(accepted()), this, SLOT(fileOrDirOpen())); 
		}
	}
}

void CombineFileDialog::filesEditTextChanged(const QString &text)
{
	QDialogButtonBox *btnbox = this->findChild<QDialogButtonBox *>("buttonBox");

	//qDebug() << "text changed " << text;
	if (btnbox != NULL)
	{
		QLineEdit *edit = findChild<QLineEdit *>("fileNameEdit");
		if (NULL != edit)
		{
			if (!edit->text().isEmpty())
			{
				if (this->acceptMode() == QFileDialog::AcceptOpen)
				{
					if (QPushButton *button = btnbox->button(QDialogButtonBox::Open))
					{
						button->setEnabled(true);
					}
				}
				else
				{
					if (QPushButton *button = btnbox->button(QDialogButtonBox::Save))
					{
						button->setEnabled(true);
					}
				}
			}
		}
	}
}

void CombineFileDialog::fileOrDirOpen()
{
	QDialog::accept();
}

void CombineFileDialog::setMultipleSelectEnable(bool multiple)
{
	//设置QFileDialog内部的表现方式
	QListView *lv = this->findChild<QListView *>("listView");
	if (lv != NULL)
	{
		lv->setSelectionMode(multiple ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
	}

	QTreeView *tv = this->findChild<QTreeView *>();
	if (tv != NULL)
	{
		tv->setSelectionMode(multiple ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
	}
}



