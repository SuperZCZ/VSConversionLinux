/*!
 * FileName: ListLineViewEditWidget.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-29
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 列表行视图编辑控件
 */

#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QEvent>
#include <QApplication>
#include <QFileDialog>
#include <QDir>

#include "CombineFileDialog.h"
#include "ListLineViewEditWidget.h"



ListLineViewEditWidget::ListLineViewEditWidget(const QString &_name, const QString _separator, ListEditType _listEditType /*= ListEditType_Text*/, QWidget *parent /*= nullptr*/) :PainterWidget(parent)
{
	name = _name;
	separator = _separator;
	listEditType = _listEditType;

	hAllLay = new QHBoxLayout(this);
	leftVLay = new QVBoxLayout;
	centerVLay = new QVBoxLayout;
	rightVLay = new QVBoxLayout;

	hAllLay->setContentsMargins(0, 0, 0, 0);
	leftVLay->setAlignment(Qt::AlignTop);
	centerVLay->setAlignment(Qt::AlignTop);
	rightVLay->setAlignment(Qt::AlignTop);

	hAllLay->addLayout(leftVLay);
	hAllLay->addLayout(centerVLay, 1);
	hAllLay->addLayout(rightVLay);

	nameLabel = new QLabel(name + ":");
	nameLabel->setObjectName("NameLabel");
	leftVLay->addWidget(nameLabel);

	lineEdit = new QLineEdit;
	lineEdit->setObjectName("ListLineEdit");
	listEditView = new ListEditView(this, listEditType);
	centerVLay->addWidget(lineEdit);
	centerVLay->addWidget(listEditView);

	expandBtn = new QPushButton;
	expandBtn->setObjectName("ExpandConfigureBtn");
	expandBtn->setCheckable(true);
	rightVLay->addWidget(expandBtn);
	listEditBtnWidget = new QWidget;
	QVBoxLayout *listEditBtnWidgetVLay = new QVBoxLayout(listEditBtnWidget);
	listEditBtnWidgetVLay->setAlignment(Qt::AlignTop);
	listEditBtnWidgetVLay->setContentsMargins(0, 0, 0, 0);
	addItemBtn = new QPushButton;
	addItemBtn->setObjectName("AddItemBtn");
	addItemBtn->setEnabled(true);
	removeItemBtn = new QPushButton;
	removeItemBtn->setObjectName("RemoveItemBtn");
	removeItemBtn->setEnabled(false);
	editItemBtn = new QPushButton;
	editItemBtn->setObjectName("EditItemBtn");
	editItemBtn->setEnabled(false);
	upItemBtn = new QPushButton;
	upItemBtn->setObjectName("UpItemBtn");
	upItemBtn->setEnabled(false);
	downItemBtn = new QPushButton;
	downItemBtn->setObjectName("DownItemBtn");
	downItemBtn->setEnabled(false);
	listEditBtnWidgetVLay->addWidget(addItemBtn);
	listEditBtnWidgetVLay->addWidget(removeItemBtn);
	listEditBtnWidgetVLay->addWidget(editItemBtn);
	listEditBtnWidgetVLay->addWidget(upItemBtn);
	listEditBtnWidgetVLay->addWidget(downItemBtn);
	rightVLay->addWidget(listEditBtnWidget);

	listEditView->hide();
	listEditBtnWidget->hide();

	setObjectName("ListLineViewEditWidget");

	ConnectInfo connectInfo[] = {
		{lineEdit,SIGNAL(textEdited(const QString &)),this,SLOT(lineEditTextEdited(const QString &)),Qt::AutoConnection},
		{expandBtn,SIGNAL(toggled(bool)),this,SLOT(onExpandBtnToggled(bool)),Qt::AutoConnection},
		{addItemBtn,SIGNAL(clicked()),listEditView,SLOT(addNewItem()),Qt::AutoConnection},
		{removeItemBtn,SIGNAL(clicked()),listEditView,SLOT(removeSlectedItem()),Qt::AutoConnection},
		{editItemBtn,SIGNAL(clicked()),listEditView,SLOT(editSelectedItem()),Qt::AutoConnection},
		{upItemBtn,SIGNAL(clicked()),listEditView,SLOT(moveItemUp()),Qt::AutoConnection},
		{downItemBtn,SIGNAL(clicked()),listEditView,SLOT(moveItemDown()),Qt::AutoConnection},
		{listEditView,SIGNAL(updateToolState(bool,bool,bool,bool,bool)),this,SLOT(updateToolState(bool,bool,bool,bool,bool)),Qt::AutoConnection}
	};

	SignalController::setConnectInfo(connectInfo, sizeof(connectInfo) / sizeof(ConnectInfo));

	loadStyleSheet(":/qss/ListLineViewEditWidget.qss");
}

ListLineViewEditWidget::~ListLineViewEditWidget()
{
}

QString ListLineViewEditWidget::getLineEditText()
{
	return lineEdit->text();
}

void ListLineViewEditWidget::onExpandBtnToggled(bool checked)
{
	lineEdit->setEnabled(!checked);
	listEditView->setVisible(checked);
	listEditBtnWidget->setVisible(checked);
	if (checked)
	{
		QStringList textList;
		if (!lineEdit->text().isEmpty())
		{
			textList = lineEdit->text().split(separator);
		}
		updateListText(textList);
		connect(listEditView, SIGNAL(listDataChanged(const QList<QVariant> &)), this, SLOT(listDataChanged(const QList<QVariant> &)));
	}
	else
	{
		disconnect(listEditView, SIGNAL(listDataChanged(const QList<QVariant> &)), this, SLOT(listDataChanged(const QList<QVariant> &)));
	}
}


void ListLineViewEditWidget::updateToolState(bool addEnabled, bool removeEnabled, bool editEnabled, bool upEnabled, bool downEnabled)
{
	addItemBtn->setEnabled(addEnabled);
	removeItemBtn->setEnabled(removeEnabled);
	editItemBtn->setEnabled(editEnabled);
	upItemBtn->setEnabled(upEnabled);
	downItemBtn->setEnabled(downEnabled);
}

void ListLineViewEditWidget::updateListText(const QString &listText)
{
	lineEdit->setText(listText);	
}

void ListLineViewEditWidget::updateListText(const QStringList &textList)
{
	QStandardItemModel *model = static_cast<QStandardItemModel *>(listEditView->model());
	model->clear();
	for (const QString &text : textList)
	{
		QStandardItem *item = new QStandardItem(text);
		model->appendRow(item);
	}
}

void ListLineViewEditWidget::listDataChanged(const QList<QVariant> &listData)
{
	QString lineEditText;
	bool isfirst = true;
	for (QVariant data : listData)
	{
		if (!isfirst)
		{
			lineEditText.append(separator);
		}
		else
		{
			isfirst = false;
		}
		lineEditText.append(data.toString());
	}
	lineEdit->setText(lineEditText);
}

void ListLineViewEditWidget::lineEditTextEdited(const QString &text)
{
	if (listEditType == ListEditType_Dir || listEditType == ListEditType_File || listEditType == ListEditType_Path)
	{
		lineEdit->setText(QDir::fromNativeSeparators(text));
	}
}

ListEditView::ListEditView(QWidget *parent /*= nullptr*/, ListEditType _listEditType /*= ListEditType_Text*/) :QListView(parent)
{
	listEditType = _listEditType;

	QItemSelectionModel *m = selectionModel();
	listEditModel = new ListEditModel(this);
	setModel(listEditModel);
	delete m;

	listEditDelegate = new ListEditViewDelegate(listEditType, this);
	setItemDelegate(listEditDelegate); // 设置委托以实现不同的编辑器和显示方式

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁止垂直滚动条条
	setSpacing(0); // 设置项之间的间距为0
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);


	setObjectName("ListEditView");

	connect(listEditModel, &QStandardItemModel::rowsInserted, this, &ListEditView::rowsCountChanged);
	connect(listEditModel, &QStandardItemModel::rowsRemoved, this, &ListEditView::rowsCountChanged);
	connect(listEditModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this, SLOT(modelDataChanged()));
	updateToolState();
}

void ListEditView::setPwd(const QString &path)
{
	pwd = path;
	listEditDelegate->setPwd(pwd);
}

void ListEditView::addNewItem(const QString &text /*= QString()*/)
{
	QStandardItem *item = new QStandardItem(text);
	listEditModel->appendRow(item);
	updateToolState();
	modelDataChanged(); // 数据更新
}

void ListEditView::removeSlectedItem()
{
	QModelIndexList now_selected = selectedIndexes();
	QSet<int> selected_rows_set;
	QList<int> selected_rows;
	for (const QModelIndex &index : now_selected)
	{
		selected_rows_set.insert(index.row());
	}
	selected_rows = selected_rows_set.toList();
	qSort(selected_rows.begin(), selected_rows.end(), [](const int &a, const int &b) {return a > b; }); // 从大到小排序，以确保删除时不会影响未删除行的行号
	for (int row : selected_rows)
	{
		listEditModel->removeRow(row);
	}
	if (listEditModel->rowCount() > 0)
	{
		// 删除后选中被删除行的第一个行号所在的行 如果该行号已经超过了当前最大行号 则选中当前最大行号所在的行
		QItemSelection newSelection;
		QModelIndex topLeft = listEditModel->index(std::min(selected_rows.last(), listEditModel->rowCount() - 1), 0);
		QModelIndex bottomRight = listEditModel->index(std::min(selected_rows.last(), listEditModel->rowCount() - 1), listEditModel->columnCount() - 1);
		newSelection.select(topLeft, bottomRight);
		selectionModel()->select(newSelection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
	updateToolState();
	modelDataChanged(); // 数据更新
}

void ListEditView::editSelectedItem()
{
	QModelIndexList now_selected = selectedIndexes();
	if (!now_selected.empty())
	{
		edit(now_selected.first());
	}
}

void ListEditView::moveItemUp()
{
	QModelIndexList now_selected = selectedIndexes();
	QSet<int> selected_rows_set;
	QList<int> selected_rows;
	for (const QModelIndex &index : now_selected)
	{
		selected_rows_set.insert(index.row());
	}
	selected_rows = selected_rows_set.toList();
	qSort(selected_rows);
	QModelIndex item_index = listEditModel->index(selected_rows.first(), 0); // 获取第一个选中行的索引
	listEditModel->moveRows(item_index.parent(), selected_rows.first(), selected_rows.count(), item_index.parent(), selected_rows.first() - 1);
	updateToolState();
	modelDataChanged(); // 数据更新
}

void ListEditView::moveItemDown()
{
	QModelIndexList now_selected = selectedIndexes();
	QSet<int> selected_rows_set;
	QList<int> selected_rows;
	for (const QModelIndex &index : now_selected)
	{
		selected_rows_set.insert(index.row());
	}
	selected_rows = selected_rows_set.toList();
	qSort(selected_rows);
	QModelIndex item_index = listEditModel->index(selected_rows.first(), 0); // 获取第一个选中行的索引
	// 向下移动其实是移动到当前选中行的最后一行的下下一行前 因此 destinationChild 应该是最后一个选中行的行号 + 2
	listEditModel->moveRows(item_index.parent(), selected_rows.first(), selected_rows.count(), item_index.parent(), selected_rows.last() + 2);
	updateToolState();
	modelDataChanged(); // 数据更新
}

void ListEditView::updateToolState()
{
	bool addEnabled = true;
	bool removeEnabled = false;
	bool editEnabled = false;
	bool upEnabled = false;
	bool downEnabled = false;

	QModelIndexList now_selected = selectedIndexes();
	QSet<int> selected_rows_set;
	QList<int> selected_rows;
	for (const QModelIndex &index : now_selected)
	{
		selected_rows_set.insert(index.row());
	}
	selected_rows = selected_rows_set.toList();
	qSort(selected_rows);

	if (!selected_rows.empty())
	{
		removeEnabled = true;
		editEnabled = (selected_rows.size() == 1);

		int min_row = *std::min_element(selected_rows.begin(), selected_rows.end());
		int max_row = *std::max_element(selected_rows.begin(), selected_rows.end());

		//判断选中行号是否连续
		if (max_row - min_row + 1 == selected_rows.count())
		{
			//选中行连续
			upEnabled = (min_row > 0);  // 只有当最小选中行不是第一行时，才允许上移
			downEnabled = (max_row < listEditModel->rowCount() - 1); // 只有当最大选中行不是最后一行时，才允许下移
		}
	}
	updateToolState(addEnabled, removeEnabled, editEnabled, upEnabled, downEnabled);
}

void ListEditView::rowsCountChanged()
{
	updateGeometry();
}

void ListEditView::modelDataChanged()
{
	QList<QVariant> listData;

	for (int i = 0; i < listEditModel->rowCount(); i++)
	{
		listData.append(listEditModel->data(listEditModel->index(i, 0)));
	}
	emit listDataChanged(listData);
}

void ListEditView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	updateToolState(); // 更新工具状态（如移动按钮的可用性）
	return QAbstractItemView::selectionChanged(selected, deselected);
}


QSize ListEditView::sizeHint() const
{
	QSize size = QListView::sizeHint();
	int rowCount = model()->rowCount();
	if (rowCount > 0)
	{
		int itemHeight = itemDelegate()->sizeHint(QStyleOptionViewItem(), model()->index(0, 0)).height() + 1;
		size.setHeight(itemHeight * (rowCount + 2)); // 根据行数调整高度，增加边框宽度
	}
	else
	{
		size.setHeight(0); // 没有行时高度为0 QSizePolicy::MinimumExpanding 会允许视图根据内容调整大小，但不会小于这个最小高度
	}
	return size;
}


bool ListEditModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
	//使用takeRow和insertRow实现moveRows功能  移动的语义为将sourceRow行及其后续行（共count行）移动到destinationChild行之前，
	if (count <= 0)
	{
		return false;
	}

	// 校验目标行范围
	if (destinationChild < 0 || destinationChild > rowCount(destinationParent))
	{
		return false;
	}
		
	// 校验源行范围
	if (sourceRow < 0 || destinationChild < 0 || sourceRow + count > rowCount(sourceParent))
	{
		return false;
	}

	// 如果目标父索引不等于源父索引，直接失败（当前只支持同一父项内移动）
	if (sourceParent != destinationParent)
	{
		return false;
	}

	// 目标位置在被移动行的范围内，直接失败（不允许插入到自己内部）
	if (destinationChild > sourceRow && destinationChild <= sourceRow + count - 1)
	{
		return false;
	}
		
	// 以下情况不需要实际移动：目标位置就是源行的起始位置，或者目标位置在被移动行的最后一行后面（即目标位置就是被移动行的最后一行的下一行）
	if (sourceRow == destinationChild || sourceRow + count == destinationChild )
	{
		return true; // 没有实际移动
	}

	QModelIndexList oldIndexes;
	QModelIndexList newIndexes;
	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < columnCount(); j++)
		{
			oldIndexes << index(sourceRow + i, j, sourceParent);
		}
	}

	// 尝试获取视图的 selectionModel，以便在移动后恢复选中状态
	QAbstractItemView *view = static_cast<QAbstractItemView *>(parent());
	QItemSelectionModel *selectionModel = NULL;
	if (view && view->selectionModel())
	{
		selectionModel= view->selectionModel();
	}

	// 通知视图即将发生布局变化，以便视图在移动过程中保持稳定
	layoutAboutToBeChanged();

	// 收集需要移动的行（保持原有顺序）
	QList<QList<QStandardItem *>> rows;
	rows.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		// 每次都从 sourceRow 位置取出一行（因为上一行已被移除，下一行会新的位于 sourceRow）
		QList<QStandardItem *> taken = takeRow(sourceRow);
		rows.append(taken);
	}

	// 调整目标索引：如果目标原本位于被移除行之后，那么在移除后索引要减去已移除的行数
	if (destinationChild > sourceRow)
	{
		destinationChild -= count;
	}

	// 限制目标范围
	if (destinationChild < 0)
	{
		destinationChild = 0;
	}
	if (destinationChild > rowCount(sourceParent))
	{
		destinationChild = rowCount(sourceParent);
	}

	// 插入已保存的行（保持原顺序）
	for (int i = 0; i < rows.size(); ++i)
	{
		insertRow(destinationChild + i, rows.at(i));
	}

	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < columnCount(); j++)
		{
			newIndexes << index(destinationChild + i, j, destinationParent);
		}
	}

	// 更新持久化索引列表，以便视图能够正确跟踪被移动的项
	changePersistentIndexList(oldIndexes, newIndexes);

	// 通知视图布局已发生变化
	layoutChanged();

	// 恢复 UI 选中状态
	if (view && selectionModel)
	{
		QItemSelection newSelection;
		QModelIndex topLeft = index(destinationChild, 0);
		QModelIndex bottomRight = index(destinationChild + count - 1, columnCount() - 1);
		//qDebug() << "Restoring selection to new range: " << topLeft.row() << "-" << bottomRight.row();
		newSelection.select(topLeft, bottomRight);
		// ClearAndSelect 确保旧的残留选中被清除
		selectionModel->select(newSelection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
	
	return true;
}


ListEditViewDelegate::ListEditViewDelegate(ListEditType _listEditType /*= ListEditType_Text*/, QObject *parent /*= nullptr*/)
{

	listItemDisplayStyleLabel = new QLabel(static_cast<QWidget *>(parent));
	listItemDisplayStyleLabel->setHidden(true);
	listItemDisplayStyleLabel->setObjectName("ListItemDisplayStyleLabel"); // 通过这个隐藏的 QLabel 来获取 QSS 中设置的单行高度

	listEditType = _listEditType;


}

QSize ListEditViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QStyledItemDelegate::sizeHint(option, index);
	size.setHeight(listItemDisplayStyleLabel->height());   // 从Qss中获取单行高度（通过一个隐藏的 QLabel 来获取，因为 QListView 的 itemDelegate 的 sizeHint 可能不受 QSS 中设置的 item 高度影响）
	return size;
}

void ListEditViewDelegate::paintTextItem(QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();
	option.textElideMode = Qt::ElideMiddle; //某些QT版本好像有点bug 设置了但不生效(Qt5.6.0可以)
	const QWidget *widget = option.widget;
	QStyle *style = widget ? widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &option, painter, widget);  //使用qss中设置的样式再次绘制
	painter->restore();
}

void ListEditViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.isValid())
	{
		QStyleOptionViewItem custom_opt(option);
		initStyleOption(&custom_opt, index);

		//清除具有焦点时的边框
		if (custom_opt.state & QStyle::State_HasFocus)
		{
			custom_opt.state ^= QStyle::State_HasFocus;
		}

		paintTextItem(painter, custom_opt, index); // 绘制文本内容
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
}

QWidget *ListEditViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QWidget *editor = nullptr;
	switch (listEditType)
	{
		case ListEditType_Dir:
		case ListEditType_File:
		case ListEditType_Path:
		{
			editor = new PathItemEditWidget(listEditType, parent);
			PathItemEditWidget *pathEditor = qobject_cast<PathItemEditWidget *>(editor);
			pathEditor->setPwd(pwd); // 将当前路径传递给编辑器，以便在浏览文件时能够正确设置初始路径
			editor->setObjectName("PathItemEditWidget");
			connect(editor, SIGNAL(editFinished()), this, SLOT(commitAndCloseEditor()));
			break;
		}
		default:
		{
			editor = new QLineEdit(parent);
			editor->setObjectName("TextItemLineEditor");
			break;
		}
	}
	return editor;
}


void ListEditViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	switch (listEditType)
	{
		case ListEditType_Dir:
		case ListEditType_File:
		case ListEditType_Path:
		{
			PathItemEditWidget *pathEditor = qobject_cast<PathItemEditWidget *>(editor);
			if (pathEditor)
			{
				QString text = index.model()->data(index, Qt::EditRole).toString();
				pathEditor->setEditPath(text);
			}
			break;
		}
		default:
		{
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
			if (lineEdit)
			{
				QString text = index.model()->data(index, Qt::EditRole).toString();
				lineEdit->setText(text);
				lineEdit->selectAll(); // 选中全部文本，方便用户直接输入覆盖
			}
			break;
		}
	}
}

void ListEditViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	switch (listEditType)
	{
		case ListEditType_Dir:
		case ListEditType_File:
		case ListEditType_Path:
		{
			PathItemEditWidget *pathEditor = qobject_cast<PathItemEditWidget *>(editor);
			if (pathEditor)
			{
				model->setData(index, pathEditor->getEditPath(), Qt::EditRole);
			}
			break;
		}
		default:
		{
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
			if (lineEdit)
			{
				model->setData(index, lineEdit->text(), Qt::EditRole);
			}
			break;
		}
	}
}



void ListEditViewDelegate::commitAndCloseEditor()
{
	QWidget *editor = qobject_cast<QWidget *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

PathItemEditWidget::PathItemEditWidget(ListEditType _listEditType, QWidget *parent /*= nullptr*/) :PainterWidget(parent)
{
	listEditType = _listEditType;
	QHBoxLayout *layout = new QHBoxLayout(this);

	pathLineEdit = new QLineEdit;
	pathLineEdit->setObjectName("PathItemLineEditor");

	QPushButton *browseBtn = new QPushButton;
	browseBtn->setObjectName("PathItemBrowseBtn");

	isRelativeCheckBox = new QCheckBox("Relative");
	isRelativeCheckBox->setObjectName("PathItemRelativeCheckBox");

	
	// 示例代码，使用QFrame创建红色水平分割线
	QFrame *vframe = new QFrame(this);
	vframe->setFrameShape(QFrame::VLine);
	vframe->setObjectName("PathItemEditWidgetLineFrame");

	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);
	layout->addWidget(pathLineEdit);
	layout->addWidget(vframe);
	layout->addWidget(browseBtn);
	layout->addWidget(isRelativeCheckBox);

	browseBtn->setFocusPolicy(Qt::NoFocus); // 设置浏览按钮不接受焦点，以避免在浏览文件时令lineEdit失去焦点
	isRelativeCheckBox->setFocusPolicy(Qt::NoFocus); // 设置相对路径复选框不接受焦点，以避免在操作复选框时令lineEdit失去焦点

	setFocusProxy(pathLineEdit); // 将焦点代理设置为 QLineEdit，以便用户点击编辑器时LineEdit直接获得焦点

	connect(browseBtn, &QPushButton::clicked, this, &PathItemEditWidget::browsePath);
	// 连接 editingFinished 信号到外部的 editFinished 信号，以便在编辑完成时能够提交数据和关闭编辑器
	connect(pathLineEdit, SIGNAL(editingFinished()), this, SIGNAL(editFinished()));
	connect(pathLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateRelativeBox()));
	connect(isRelativeCheckBox, SIGNAL(clicked(bool)), this, SLOT(isRelativeCheckBoxClicked(bool)));
}

PathItemEditWidget::~PathItemEditWidget()
{
	//qDebug() << "PathItemEditWidget destroyed";
}

QString PathItemEditWidget::getEditPath() const
{
	return QDir::fromNativeSeparators(pathLineEdit->text());
}

void PathItemEditWidget::setEditPath(const QString &path)
{
	pathLineEdit->setText(QDir::fromNativeSeparators(path));
	pathLineEdit->selectAll(); // 选中全部文本，方便用户直接输入覆盖
}

void PathItemEditWidget::browsePath()
{
	// 在打开文件对话框之前断开 editingFinished 信号与外部的连接，以避免在浏览文件时触发编辑完成逻辑（如提交数据和关闭编辑器）
	disconnect(pathLineEdit, SIGNAL(editingFinished()), this, SIGNAL(editFinished())); 
	QString browsePath;

	QString caption, homeDir;
	homeDir = pwd;
	switch (listEditType)
	{
	case ListEditType_Dir:
		caption = trUtf8("选择目录");
		break;
	case ListEditType_File:
		caption = trUtf8("选择文件");
		break;
	case ListEditType_Path:
		caption = trUtf8("选择文件/目录");
		break;
	default:
		caption = trUtf8("选择");
		break;
	}

	QFileInfo fi(pathLineEdit->text());
	if (fi.exists())
	{
		if (fi.isDir())
		{
			homeDir = fi.absoluteFilePath();
		}
		else
		{
			homeDir = fi.absolutePath();
		}
	}

	CombineFileDialog combineFileDialog(this, listEditType == ListEditType_Path, caption, homeDir);
	if (listEditType == ListEditType_Dir)
	{
		combineFileDialog.setFileMode(QFileDialog::Directory);
	}
	else if (listEditType == ListEditType_File)
	{
		combineFileDialog.setFileMode(QFileDialog::ExistingFile);
	}

	if (combineFileDialog.exec() == QFileDialog::Accepted)
	{
		if (!combineFileDialog.selectedFiles().empty())
		{
			browsePath = combineFileDialog.selectedFiles().first();
		}

		if (!browsePath.isEmpty())
		{
			setEditPath(browsePath);
		}
	}
	// 在浏览文件结束后重新连接 editingFinished 信号与外部的连接，以便后续编辑完成时能够正常提交数据和关闭编辑器
	connect(pathLineEdit, SIGNAL(editingFinished()), this, SIGNAL(editFinished()));
}

void PathItemEditWidget::updateRelativeBox()
{
	int cursorPos = pathLineEdit->cursorPosition();
	pathLineEdit->setText(QDir::fromNativeSeparators(pathLineEdit->text()));
	pathLineEdit->setCursorPosition(cursorPos);
	QString pathText = pathLineEdit->text();
	isRelativeCheckBox->setEnabled(true);
	isRelativeCheckBox->setChecked(!QFileInfo(pathText).isAbsolute());
	if (QFileInfo(pathText).isAbsolute() && !pathText.startsWith(pwd)) //只有当路径为绝对路径切不能转换为相对路径时禁用
	{
		isRelativeCheckBox->setDisabled(true);
	}
}

void PathItemEditWidget::isRelativeCheckBoxClicked(bool checked)
{
	QString pathText = QDir::fromNativeSeparators(pathLineEdit->text());
	QFileInfo pathInfo(pathText);
	if (checked && pathInfo.isAbsolute() && pathText.startsWith(pwd)) //当前是绝对路径将其变为相对路径
	{
		pathText = pathText.mid(pwd.length());
		if (pathText.startsWith("/"))
		{
			pathText = pathText.mid(1);
		}
	}
	else if (!checked && !pathInfo.isAbsolute()) //当前是相对路径将其给为绝对路径
	{

		pathText = pwd + (pathText.startsWith("/")? "" : "/") + pathText;
	}

	pathLineEdit->setText(QDir::fromNativeSeparators(pathText));
}
