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

// Radius of the drawn node, in pixels
const int f3::FaceFeatureNode::RADIUS = 4;

// +-----------------------------------------------------------
f3::FaceFeatureNode::FaceFeatureNode(FaceWidget *pFaceWidget)
{
	m_pFaceWidget = pFaceWidget;

    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::addEdge(FaceFeatureEdge *pEdge)
{
	m_pEdges.append(pEdge);
    pEdge->adjust();
}


// +-----------------------------------------------------------
QList<f3::FaceFeatureEdge *> f3::FaceFeatureNode::edges() const
{
    return m_pEdges;
}

// +-----------------------------------------------------------
QRectF f3::FaceFeatureNode::boundingRect() const
{
    return QRectF(-RADIUS, -RADIUS, 2 * RADIUS, 2 * RADIUS);
}

// +-----------------------------------------------------------
QPainterPath f3::FaceFeatureNode::shape() const
{
    QPainterPath oPath;
	oPath.addEllipse(boundingRect());
    return oPath;
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget)
{
	Q_UNUSED(pWidget);
	QBrush oBrush;
	oBrush.setStyle(Qt::SolidPattern);
    if(pOption->state & QStyle::State_Sunken)
		oBrush.setColor(QColor(Qt::red));
	else
		oBrush.setColor(QColor(Qt::yellow));

    pPainter->setBrush(oBrush);
    pPainter->setPen(QPen(Qt::black, 0));
	pPainter->drawEllipse(boundingRect());
}

// +-----------------------------------------------------------
QVariant f3::FaceFeatureNode::itemChange(GraphicsItemChange eChange, const QVariant &oValue)
{
    switch(eChange)
	{
		case ItemPositionHasChanged:
			foreach(FaceFeatureEdge *pEdge, m_pEdges)
				pEdge->adjust();
			//m_pFaceWidget->itemMoved();
			break;
	
		default:
			break;
    };

    return QGraphicsItem::itemChange(eChange, oValue);
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

// +-----------------------------------------------------------
void f3::FaceFeatureNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
