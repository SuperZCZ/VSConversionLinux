/*!
 * FileName: ListLineViewEditWidget.h
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-29
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 列表行视图编辑控件
 */

#ifndef _LIST_LINE_VIEW_EDIT_WIDGET_H_
#define _LIST_LINE_VIEW_EDIT_WIDGET_H_

#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVariant>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include "PainterWidget.h"
#include "SignalController.h"

enum ListEditType
{
	ListEditType_Text, //文本编辑
	ListEditType_Dir,  //目录编辑
	ListEditType_File, //文件编辑
	ListEditType_Path, //路径编辑 (目录或文件)
};

class ListEditViewDelegate;

class ListEditModel :public QStandardItemModel
{
	Q_OBJECT
public:
	ListEditModel(QObject *parent = nullptr) :QStandardItemModel(parent) {}
	virtual ~ListEditModel() {}
	virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild);
};

class ListEditView :public QListView
{
	Q_OBJECT
public:
	ListEditView(QWidget *parent = nullptr, ListEditType _listEditType = ListEditType_Text);
	virtual ~ListEditView() {}

	void setPwd(const QString &path);

public slots:
	void addNewItem(const QString &text = QString());
	void removeSlectedItem();
	void editSelectedItem();
	void moveItemUp();
	void moveItemDown();
	void updateToolState();
	void rowsCountChanged();
	void modelDataChanged();

signals:
	void updateToolState(bool addEnabled, bool removeEnabled, bool editEnabled, bool upEnabled, bool downEnabled);
	void listDataChanged(const QList<QVariant> &);

protected:
	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	virtual QSize sizeHint() const;
private:
	QString pwd;  // 当前路径
	ListEditModel *listEditModel;
	ListEditViewDelegate *listEditDelegate;
	ListEditType listEditType;
};


class ListEditViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ListEditViewDelegate(ListEditType _listEditType = ListEditType_Text, QObject *parent = nullptr);
	virtual ~ListEditViewDelegate() {}

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
	
	void setPwd(const QString &path) { pwd = path; }

public slots:
	void commitAndCloseEditor();
private:
	void paintTextItem(QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
	QString pwd;  // 当前路径
	ListEditType listEditType;
	QLabel *listItemDisplayStyleLabel;
};

class PathItemEditWidget : public PainterWidget
{
	Q_OBJECT
public:
	PathItemEditWidget(ListEditType _listEditType, QWidget *parent = nullptr);
	virtual ~PathItemEditWidget();

	QString getEditPath() const;
	void setPwd(const QString &path)
	{ 
		pwd = path; 
	}

public slots:
	void setEditPath(const QString &path);
	void browsePath();
	void updateRelativeBox();
	void isRelativeCheckBoxClicked(bool checked);
signals:
	void editFinished();
private:
	QString pwd;  // 当前路径
	ListEditType listEditType;
	QLineEdit *pathLineEdit;
	QPushButton *browseBtn;
	QCheckBox *isRelativeCheckBox;
};


class ListLineViewEditWidget : public PainterWidget
{
	Q_OBJECT
public:
	ListLineViewEditWidget(const QString &_name, const QString _separator, ListEditType _listEditType = ListEditType_Text, QWidget *parent = nullptr);
	virtual ~ListLineViewEditWidget();

	void setPwd(const QString &path) { pwd = path; listEditView->setPwd(path); }
	QString getLineEditText();

private:
	QString pwd;  // 当前路径
	QString name;
	QString separator;
	ListEditType listEditType;

	QHBoxLayout *hAllLay;
	QVBoxLayout *leftVLay;
	QVBoxLayout *centerVLay;
	QVBoxLayout *rightVLay;
	
	QLabel *nameLabel;
	QLineEdit *lineEdit;
	QPushButton *expandBtn;
	
	ListEditView *listEditView;
	QWidget *listEditBtnWidget;
	QPushButton *addItemBtn;
	QPushButton *removeItemBtn;
	QPushButton *editItemBtn;
	QPushButton *upItemBtn;
	QPushButton *downItemBtn;

public slots:
	void onExpandBtnToggled(bool checked);
	void updateToolState(bool addEnabled, bool removeEnabled, bool editEnabled, bool upEnabled, bool downEnabled);
	void updateListText(const QString &listText);
	void updateListText(const QStringList &textList);
	void listDataChanged(const QList<QVariant> &listData);
	void lineEditTextEdited(const QString &text);
};


#endif