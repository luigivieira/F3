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
#include <QDebug>
#include <QtMath>

// Scale values for zoom in and out steps
const double f3::FaceWidget::ZOOM_IN_STEP = 1.25;
const double f3::FaceWidget::ZOOM_OUT_STEP = 0.80;

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

	m_dScaleFactor = 1.0;
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
double f3::FaceWidget::getScaleFactor() const
{
	return m_dScaleFactor;
}

// +-----------------------------------------------------------
void f3::FaceWidget::setScaleFactor(const double dScaleFactor, const bool bEmitSignal)
{
	if(dScaleFactor == m_dScaleFactor)
		return;

	if(dScaleFactor >= 0.10 && dScaleFactor <= 10.0)
	{
		// First, go back to the base scale (1.0 or 100%)
		double dAdjust = 1.0 / m_dScaleFactor;
		scale(dAdjust, dAdjust);

		// Then, apply the requested factor.
		m_dScaleFactor = dScaleFactor;
		if(m_dScaleFactor != 1.0)
			scale(dScaleFactor, dScaleFactor);

		// Emit the signal that the scale factor has changed
		if(bEmitSignal)
			emit onScaleFactorChanged(m_dScaleFactor);
	}
}

// +-----------------------------------------------------------
void f3::FaceWidget::scaleViewBy(double dFactorBy)
{
	double dFactor = m_dScaleFactor * dFactorBy;
	if(dFactor >= 0.10 && dFactor <= 10.0)
	{
		m_dScaleFactor = dFactor;
	    scale(dFactorBy, dFactorBy);

		// Emit the signal that the scale factor has changed
		emit onScaleFactorChanged(m_dScaleFactor);
	}
}

#ifndef QT_NO_WHEELEVENT
// +-----------------------------------------------------------
void f3::FaceWidget::wheelEvent(QWheelEvent *pEvent)
{
	bool bCtrl = QApplication::keyboardModifiers() & Qt::ControlModifier;
	bool bAlt = QApplication::keyboardModifiers() & Qt::AltModifier;
	bool bShift = QApplication::keyboardModifiers() & Qt::ShiftModifier;
	int iDelta = pEvent->angleDelta().x() + pEvent->angleDelta().y();
	double dBase = iDelta < 0 ? ZOOM_OUT_STEP : ZOOM_IN_STEP;
	int iSteps = abs(iDelta / 120);

	if(!(bCtrl || bAlt || bShift)) // No special key pressed => scroll vertically
		verticalScrollBar()->setValue(verticalScrollBar()->value() - iDelta);
	else if(bShift && !(bCtrl || bAlt)) // Only shift key pressed => scroll horizontally
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - iDelta);
	else if(bCtrl && !(bAlt || bShift)) // Only ctrl key pressed => zoom in and out
		scaleViewBy(qPow(dBase, iSteps));
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
	scaleViewBy(ZOOM_IN_STEP);
}

// +-----------------------------------------------------------
void f3::FaceWidget::zoomOut()
{
	scaleViewBy(ZOOM_OUT_STEP);
}