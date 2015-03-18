#include "leveldelegate.h"

#include <QApplication>

// +-----------------------------------------------------------
f3::LevelDelegate::LevelDelegate(QObject *pParent):
	QItemDelegate(pParent)
{
}

// +-----------------------------------------------------------
f3::LevelDelegate::~LevelDelegate()
{
}

// +-----------------------------------------------------------
QWidget* f3::LevelDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &oOption, const QModelIndex &oIndex) const
{
	Q_UNUSED(oOption);
	QComboBox *pEditor = new QComboBox(pParent);
	pEditor->setFocusPolicy(Qt::StrongFocus);
	pEditor->setAutoFillBackground(true);

	pEditor->addItem(tr("Depuração (DEBUG)"), QtDebugMsg);
	pEditor->addItem(tr("Aviso (WARNING)"), QtWarningMsg);
	pEditor->addItem(tr("Crítico (CRITICAL)"), QtCriticalMsg);
	pEditor->addItem(tr("Fatal (FATAL)"), QtFatalMsg);

	pEditor->setCurrentText(oIndex.data(Qt::DisplayRole).toString());
	connect(pEditor, SIGNAL(currentIndexChanged(int)), this, SLOT(setData(int)));
	return pEditor;
}

// +-----------------------------------------------------------
void f3::LevelDelegate::setEditorData(QWidget *pEditor, const QModelIndex &oIndex) const
{
	((QComboBox*) pEditor)->setCurrentText(oIndex.data(Qt::DisplayRole).toString());
	((QComboBox*) pEditor)->showPopup();
}

// +-----------------------------------------------------------
void f3::LevelDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &oIndex) const
{
	QtMsgType eLevel = (QtMsgType) ((QComboBox*) pEditor)->itemData(((QComboBox*) pEditor)->currentIndex(), Qt::UserRole).toInt();
	pModel->setData(oIndex, QVariant(eLevel), Qt::UserRole);
}

// +-----------------------------------------------------------
void f3::LevelDelegate::setData(int iIndex)
{
	Q_UNUSED(iIndex);
	emit commitData((QWidget*) sender());
	closeEditor((QWidget*) sender());
}