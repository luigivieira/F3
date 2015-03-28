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

#ifndef FACEFEATURENODE_H
#define FACEFEATURENODE_H

#include <QGraphicsItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

namespace f3
{
	class FaceWidget;
	class FaceFeatureEdge;

	/**
	 * Implements the visual manipulator of facial feature nodes to be used together with the FaceWidget.
	 */
	class FaceFeatureNode: public QGraphicsItem
	{
	public:
		/**
		 * Class constructor.
		 * @param pFaceWidget Instance of the FaceWidget to be the parent of the face feature.
		 */
		FaceFeatureNode(FaceWidget *pFaceWidget);

		/**
		 * Adds a new edge to the face feature node.
		 * @param pEdge Instance of the FaceFeatureEdge to be added to the face feature.
		 */
		void addEdge(FaceFeatureEdge *pEdge);

		/**
		 * Queries the list of face feature edges connected to this node.
		 * @return Q QList<FaceFeatureEdge*> with the list of edges connected to this node.
		 */
		QList<FaceFeatureEdge *> edges() const;

		enum { Type = UserType + 1 };
		int type() const Q_DECL_OVERRIDE { return Type; }

		QRectF boundingRect() const Q_DECL_OVERRIDE;

		QPainterPath shape() const Q_DECL_OVERRIDE;

		/** Constant with the radius of the node drawn, in pixels. */
		const static int RADIUS;

	protected:

		/**
		 * Paint method for the face feature node.
		 * @param pPainter Instance of a QPainter to allow drawing the node.
		 * @param pOption Instance of a QStyleOptionGraphicsItem with information on the style and state of the node.
		 * @param pWidget Instance of a QWidget with the widget that the node is being painted on. Optional, and might be 0.
		 */
		void paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget) Q_DECL_OVERRIDE;

		/**
		 * Captures the event of changes in the node.
		 * @param eChange GraphicsItemChange enumeration value indicating what is the change.
		 * @param oValue QVariant const reference with the value that changed.
		 */
		QVariant itemChange(GraphicsItemChange eChange, const QVariant &oValue) Q_DECL_OVERRIDE;

		void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

	private:

		/** Reference to the parent face widget. */
		FaceWidget *m_pFaceWidget;

		/** List of edges added to the node. */
		QList<FaceFeatureEdge *> m_pEdges;
	};
};

#endif // FACEFEATURENODE_H
