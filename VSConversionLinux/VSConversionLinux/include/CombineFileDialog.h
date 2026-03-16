/*!
 * FileName: CombineFileDialog.h
 *
 * Author:   ZhangChaoZe
 * Date:     2026-3-7
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 混合文件选择对话框，允许用户选择一个或多个文件或目录
 */
#ifndef _COMBINE_FILE_DIALOG_H_
#define _COMBINE_FILE_DIALOG_H_

#include <QDialog>
#include <QFileDialog>
#include <QVector>

class CombineFileDialog :public QFileDialog
{
	Q_OBJECT
public:
	CombineFileDialog(QWidget *parent = nullptr, bool _bothFileAndDirSelectEnable = false, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString());
	virtual ~CombineFileDialog() {}
	void setMultipleSelectEnable(bool multiple);
public slots:
	void filesEditTextChanged(const QString &text);
	void fileOrDirOpen();
private:
	bool bothFileAndDirSelectEnable; // 是否允许同时选择文件和目录
};


#endif // _COMBINE_FILE_DIALOG_H_

