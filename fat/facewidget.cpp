/*
 * Copyright (C) 2015 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
 *
 * This file is part of Fun from Faces (f3).
 *
 * f3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * f3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "facewidget.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsEffect>
#include <QScrollBar>

// +-----------------------------------------------------------
f3::FaceWidget::FaceWidget(QWidget *pParent) : QGraphicsView(pParent)
{
	m_pScene = new QGraphicsScene(this);
	m_pScene->setItemIndexMethod(QGraphicsScene::NoIndex);
	setScene(m_pScene);

	setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    scale(qreal(1.0), qreal(1.0));

	setBackgroundBrush(QApplication::palette().dark());

	QPixmap oPixmap(":/images/noface");
	m_pPixmapItem = m_pScene->addPixmap(oPixmap);
	m_pPixmapItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
	m_pScene->setSceneRect(0, 0, oPixmap.width(), oPixmap.height());
}

// +-----------------------------------------------------------
f3::FaceWidget::~FaceWidget()
{
	delete m_pScene;
}

// +-----------------------------------------------------------
void f3::FaceWidget::setPixmap(const QPixmap &oPixmap)
{
	m_pPixmapItem->setPixmap(oPixmap);
	m_pScene->setSceneRect(0, 0, oPixmap.width(), oPixmap.height());
}

// +-----------------------------------------------------------
void f3::FaceWidget::scaleView(float fScaleFactor)
{
	float fFactor = transform().scale(fScaleFactor, fScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(fFactor < 0.07 || fFactor > 100)
        return;

    scale(fScaleFactor, fScaleFactor);
}

#ifndef QT_NO_WHEELEVENT
// +-----------------------------------------------------------
void f3::FaceWidget::wheelEvent(QWheelEvent *pEvent)
{
	bool bCtrl = QApplication::keyboardModifiers() & Qt::ControlModifier;
	bool bAlt = QApplication::keyboardModifiers() & Qt::AltModifier;
	bool bShift = QApplication::keyboardModifiers() & Qt::ShiftModifier;
	int iDelta = pEvent->angleDelta().x() + pEvent->angleDelta().y();

	if(!(bCtrl || bAlt || bShift)) // No special key pressed => scroll vertically
		verticalScrollBar()->setValue(verticalScrollBar()->value() - iDelta);
	else if(bShift && !(bCtrl || bAlt)) // Only shift key pressed => scroll horizontally
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - iDelta);
	else if(bCtrl && !(bAlt || bShift)) // Only ctrl key pressed => zoom in and out
		scaleView(pow(2.0, iDelta / 240.0));
}
#endif

// +-----------------------------------------------------------
void f3::FaceWidget::keyPressEvent(QKeyEvent *pEvent)
{
    switch(pEvent->key())
	{
		case Qt::Key_Plus:
			zoomIn();
			break;

		case Qt::Key_Minus:
			zoomOut();
			break;

		default:
			QGraphicsView::keyPressEvent(pEvent);
    }
}

// +-----------------------------------------------------------
void f3::FaceWidget::zoomIn()
{
    scaleView(1.2f);
}

// +-----------------------------------------------------------
void f3::FaceWidget::zoomOut()
{
	scaleView(1.0f / 1.2f);
}