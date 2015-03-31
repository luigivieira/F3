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

#include "facefeatureedge.h"
#include "facefeaturenode.h"

#include <math.h>

#include <QPainter>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

// +-----------------------------------------------------------
f3::FaceFeatureEdge::FaceFeatureEdge(FaceFeatureNode *pSourceNode, FaceFeatureNode *pTargetNode)
{
    setAcceptedMouseButtons(0);

    m_pSourceNode = pSourceNode;
    m_pTargetNode = pTargetNode;

    m_pTargetNode->addEdge(this);
    m_pSourceNode->addEdge(this);

    adjust();
}

// +-----------------------------------------------------------
f3::FaceFeatureNode* f3::FaceFeatureEdge::sourceNode() const
{
    return m_pSourceNode;
}

// +-----------------------------------------------------------
f3::FaceFeatureNode* f3::FaceFeatureEdge::destNode() const
{
    return m_pTargetNode;
}

// +-----------------------------------------------------------
void f3::FaceFeatureEdge::adjust()
{
    if (!m_pSourceNode || !m_pTargetNode)
        return;

    QLineF line(mapFromItem(m_pSourceNode, 0, 0), mapFromItem(m_pTargetNode, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    QPointF edgeOffset((line.dx() * FaceFeatureNode::RADIUS) / length, (line.dy() * FaceFeatureNode::RADIUS) / length);
	m_oSourcePoint = line.p1() + edgeOffset;
    m_oTargetPoint = line.p2() - edgeOffset;
}

// +-----------------------------------------------------------
QRectF f3::FaceFeatureEdge::boundingRect() const
{
    if (!m_pSourceNode || !m_pTargetNode)
        return QRectF();

    QRectF oRet = QRectF(m_oSourcePoint, QSizeF(m_oTargetPoint.x() - m_oSourcePoint.x(), m_oTargetPoint.y() - m_oSourcePoint.y()));
	return oRet.normalized();
}

// +-----------------------------------------------------------
void f3::FaceFeatureEdge::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget)
{
	Q_UNUSED(pOption);
	Q_UNUSED(pWidget);
    if (!m_pSourceNode || !m_pTargetNode)
        return;

    QLineF oLine(m_oSourcePoint, m_oTargetPoint);
    /*if(qFuzzyCompare(oLine.length(), 0.0))
        return;*/

    pPainter->setPen(QPen(Qt::yellow, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    pPainter->drawLine(oLine);
}