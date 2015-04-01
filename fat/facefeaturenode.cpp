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

#include "facefeaturenode.h"
#include "facewidget.h"
#include "facefeatureedge.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>

// Radius of the drawn node, in pixels
const int f3::FaceFeatureNode::RADIUS = 4;

// Next available identifier for a new face feature node
int f3::FaceFeatureNode::m_siNextID = 1;

// +-----------------------------------------------------------
f3::FaceFeatureNode::FaceFeatureNode(FaceWidget *pFaceWidget)
{
	m_pFaceWidget = pFaceWidget;

    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsSelectable);
	setFlag(ItemIsFocusable);
    setCacheMode(DeviceCoordinateCache);

	setSelected(false);

	m_iID = m_siNextID++;
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::addEdge(FaceFeatureEdge *pEdge)
{
	m_pEdges.append(pEdge);
    pEdge->adjust();
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::removeEdge(FaceFeatureEdge *pEdge)
{
	m_pEdges.removeOne(pEdge);
}

// +-----------------------------------------------------------
QList<f3::FaceFeatureEdge *> f3::FaceFeatureNode::edges() const
{
    return m_pEdges;
}

// +-----------------------------------------------------------
QRectF f3::FaceFeatureNode::boundingRect() const
{
	if(m_pFaceWidget->displayIDs())
	{
		QString sID = QString::number(m_iID);
		int iHeight = m_pFaceWidget->fontMetrics().height();
		int iWidth = m_pFaceWidget->fontMetrics().width(sID);
		return QRectF(-(iWidth + RADIUS), -(iHeight + RADIUS), 2 * RADIUS + iWidth, 2 * RADIUS + iHeight);
	}
	else
		return QRectF(-RADIUS, -RADIUS, 2 * RADIUS, 2 * RADIUS);
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget)
{
	Q_UNUSED(pOption);
	Q_UNUSED(pWidget);

	QBrush oBrush;
	oBrush.setStyle(Qt::SolidPattern);
	if(isSelected())
	{
	    pPainter->setPen(QPen(Qt::red, 0));
		oBrush.setColor(QColor(Qt::red));
	}
	else
	{
	    pPainter->setPen(QPen(Qt::yellow, 0));
		oBrush.setColor(QColor(Qt::yellow));
	}

	QRectF oBounds;
	if(m_pFaceWidget->displayIDs())
	{
		QString sID = QString::number(m_iID);
		int iHeight = m_pFaceWidget->fontMetrics().height();
		int iWidth = m_pFaceWidget->fontMetrics().width(sID);
		oBounds = QRectF(-(iWidth + RADIUS), -(iHeight + RADIUS), iWidth, iHeight);
		pPainter->drawText(oBounds, sID);

		oBounds = QRectF(-RADIUS, -RADIUS, 2 * RADIUS, 2 * RADIUS);
	}
	else
		oBounds = boundingRect();

    pPainter->setBrush(oBrush);
	pPainter->drawEllipse(oBounds);
}

// +-----------------------------------------------------------
QVariant f3::FaceFeatureNode::itemChange(GraphicsItemChange eChange, const QVariant &oValue)
{
	QString sTooltip;
    switch(eChange)
	{
		case ItemPositionHasChanged:
			sTooltip = QString("#%1: (%2, %3)").arg(m_iID).arg(QString::number(pos().x(), 'f', 2)).arg(QString::number(pos().y(), 'f', 2));
			setToolTip(sTooltip);

			foreach(FaceFeatureEdge *pEdge, m_pEdges)
				pEdge->adjust();

			m_pFaceWidget->faceFeatureMoved(this);
			break;
	
		default:
			break;
    };

    return QGraphicsItem::itemChange(eChange, oValue);
}