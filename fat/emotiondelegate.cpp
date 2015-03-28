#include "emotiondelegate.h"
#include "emotionlabel.h"

#include <QApplication>

// +-----------------------------------------------------------
f3::EmotionDelegate::EmotionDelegate(QObject *pParent):
	QItemDelegate(pParent)
{
}

// +-----------------------------------------------------------
f3::EmotionDelegate::~EmotionDelegate()
{
}

// +-----------------------------------------------------------
QWidget* f3::EmotionDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &oOption, const QModelIndex &oIndex) const
{
	Q_UNUSED(oOption);
	QComboBox *pEditor = new QComboBox(pParent);
	pEditor->setFocusPolicy(Qt::StrongFocus);
	pEditor->setAutoFillBackground(true);
	pEditor->addItems(EmotionLabel::getLabels());

	pEditor->setCurrentText(oIndex.data(Qt::DisplayRole).toString());
	connect(pEditor, SIGNAL(currentIndexChanged(int)), this, SLOT(setData(int)));
	return pEditor;
}

// +-----------------------------------------------------------
void f3::EmotionDelegate::setEditorData(QWidget *pEditor, const QModelIndex &oIndex) const
{
	((QComboBox*) pEditor)->setCurrentText(oIndex.data(Qt::DisplayRole).toString());
	((QComboBox*) pEditor)->showPopup();
}

// +-----------------------------------------------------------
void f3::EmotionDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &oIndex) const
{
	int iValue = ((QComboBox*) pEditor)->currentIndex();
	if(pModel->data(oIndex, Qt::UserRole).toInt() != iValue)
		pModel->setData(oIndex, iValue, Qt::UserRole);
}

// +-----------------------------------------------------------
void f3::EmotionDelegate::setData(int iIndex)
{
	Q_UNUSED(iIndex);
	emit commitData((QWidget*) sender());
	closeEditor((QWidget*) sender());
}