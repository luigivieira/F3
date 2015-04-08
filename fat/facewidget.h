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

#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

#include "facefeaturenode.h"
#include "facefeatureedge.h"

namespace Ui {
    class MainWindow;
}

namespace f3
{
    /**
     * Implements the widget used to display face images and to edit facial features by dragging its points.
     */
    class FaceWidget : public QGraphicsView
    {
        Q_OBJECT
	public:

		/** Scale value for the zoom in step. */
		static const double ZOOM_IN_STEP;

		/** Scale value for the zoom out step. */
		static const double ZOOM_OUT_STEP;

		/**
		 * Class constructor.
		 * @param pParent Instance of the parent widget.
		 */
		FaceWidget(QWidget *pParent = 0);

		/**
		 * Class destructor.
		 */
		virtual ~FaceWidget();

		/**
		 * Updates the pixmap displayed at the central area.
		 * @param oPixmap Reference for a QPixmap with the new pixmap to display.
		 */
		void setPixmap(const QPixmap &oPixmap);

		/**
		 * Gets the currently applied scale factor on the image displayed.
		 * @return Double with the currently applied scale factor.
		 */
		double getScaleFactor() const;

		/**
		 * Sets the scale factor of the image displayed.
		 * @param dScaleFactor Double with the new scale factor to be applied. It must be 
		 * in the interval [0.10, 10.0].
		 */
		void setScaleFactor(const double dScaleFactor);

		/**
		 * Performs one step of zoom in.
		 */
	    void zoomIn();

		/**
		 * Performs one step of zoom out.
		 */
		void zoomOut();

		/**
		 * Queries the list of existing face feature nodes.
		 * @param Const reference to the QList of existing nodes.
		 */
		const QList<FaceFeatureNode*>& getFaceFeatures() const;

		/**
		 * Queries the selected face feature nodes.
		 * @return QList with the pointers to the selected face feature nodes.
		 */
		QList<FaceFeatureNode*> getSelectedFeatures() const;

		/**
		 * Queries the selected face feature edges.
		 * @return QList with the pointers to the selected face feature edges.
		 */
		QList<FaceFeatureEdge*> getSelectedConnections() const;

		/**
		 * Adds a new face feature node in the given position.
		 * @param oPos A QPointF with the coordinates for the new node. If not provided, (0, 0) is assumed.
		 * @return Pointer to the instance of the newly added face feature node.
		 */
		FaceFeatureNode* addFaceFeature(const QPointF &oPos = QPointF());

		/**
		 * Removes an existing face feature node.
		 * @param pNode Pointer to the instance of the face feature node to remove.
		 */
		void removeFaceFeature(FaceFeatureNode* pNode);

		/**
		 * Adds a new face feature edge connecting two existing nodes.
		 * @param pSource Pointer to the instance of the first face feature node.
		 * @param pTarget Pointer to the instance of the second face feature node.
		 * @return Pointer to the instance of the newly added face feature edge connecting the two nodes.
		 */
		FaceFeatureEdge* connectFaceFeatures(FaceFeatureNode* pSource, FaceFeatureNode* pTarget);

		/**
		 * Removes an existing face feature edge.
		 * @param pEdge Pointer to the instance of the face feature edge to remove.
		 */
		void removeConnection(FaceFeatureEdge* pEdge);

		/**
		 * Captures the indication that a face feature node has been moved by the user.
		 * @param pNode Instance of the Face Feature Node that has been moved.
		 */
		void faceFeatureMoved(FaceFeatureNode *pNode);

		/**
		 * Indicates if the face feature nodes are on display.
		 * @return Boolean indicating if the face feature nodes are being displayed or not.
		 */
		bool displayFaceFeatures() const;

		/**
		 * Updates the indication on if the face feature nodes shall be displayed or not.
		 * @param bValue Boolean with the new value (true means show, false means hide).
		 */
		void setDisplayFaceFeatures(const bool bValue);

		/**
		 * Indicates if the face feature edges are on display.
		 * @return Boolean indicating if the face feature edges are being displayed or not.
		 */
		bool displayConnections() const;

		/**
		 * Updates the indication on if the face feature edges shall be displayed or not.
		 * @param bValue Boolean with the new value (true means show, false means hide).
		 */
		void setDisplayConnections(const bool bValue);

		/**
		 * Indicates if the identifiers of the face feature nodes are on display.
		 * @return Boolean indicating if the identifiers of the face feature nodes are being displayed or not.
		 */
		bool displayFeatureIDs() const;

		/**
		 * Updates the indication on if the identifiers of the face feature nodes shall be displayed or not.
		 * @param bValue Boolean with the new value (true means show, false means hide).
		 */
		void setDisplayFeatureIDs(const bool bValue);

		/**
		 * Sets the menus to be displayed upon events of context menu on the face features editor.
		 * The actions used in the menus must be controlled by the caller.
		 * @param pEditorMenu Instance of the QMenu to be used for the context of the whole editor.
		 * @param pFeaturesMenu Instance of the QMenu to be used for the context of feature nodes.
		 * @param pConnectionsMenu Instance of the QMenu to be used for the context of feature edges.
		 */
		void setContextMenus(QMenu *pEditorMenu, QMenu *pFeaturesMenu, QMenu *pConnectionsMenu);

		/**
		 * Indicates that the context menu must be displayed for the given face feature node.
		 * @param pEvent Instance with the event data.
		 * @param pNode Instance of the node for which the event happened.
		 */
		void showContextMenu(QGraphicsSceneContextMenuEvent *pEvent, FaceFeatureNode *pNode);

		/**
		 * Indicates that the context menu must be displayed for the given face feature edge.
		 * @param pEvent Instance with the event data.
		 * @param pEdge Instance of the edge for which the event happened.
		 */
		void showContextMenu(QGraphicsSceneContextMenuEvent *pEvent, FaceFeatureEdge *pEdge);

		/**
		 * Indicates that the context menu must be displayed for the face feature editor (the whole scene).
		 * @param pEvent Instance with the event data.
		 */
		void showContextMenu(QGraphicsSceneContextMenuEvent *pEvent);

	public:

		/** Constant with the number of face features. */
		const static int NUM_FACE_FEATURES;

	signals:

		/**
		 * Signal to indicate that the scale of the display image has changed from direct user interaction
		 * with the widget (that is, a call to setScaleFactor does not emit this signal).
		 * @param dScaleFactor Double with the new scale factor.
		 */
		void onScaleFactorChanged(const double dScaleFactor);

		/**
		 * Signal to indicate that the face features changed somehow: a face feature node was added, removed or moved
		 * or a connection between two face feature nodes was created or removed.
		 */
		void onFaceFeaturesChanged();

		/**
		 * Signal to indicate that face features were selected or unselected in the editor.
		 * The selection can be queried through getSelectedFeatures() and getSelectedConnections().
		 */
		void onFaceFeaturesSelectionChanged();

	protected:

	#ifndef QT_NO_WHEELEVENT
		/**
		 * Captures and handles the mouse wheel events to automate user interactions.
		 * @param pEvent Pointer to a QWheelEvent with the mouse event.
		 */
		void wheelEvent(QWheelEvent *pEvent) Q_DECL_OVERRIDE;
	#endif

		/**
		 * Scales the widget view to emulate a zoom behaviour by the given factor.
		 * @param dFactorBy Double with the scale to be added/removed.
		 */
		void scaleViewBy(double dFactorBy);

		/**
		 * Creates the face feature nodes and edges to be used by the face features editor.
		 */
		void createFaceFeatures();

	protected slots:

		/**
		 * Captures the signal of selection changed in the graphics scene.
		 */
		void onSelectionChanged();

	private:
		/** The scene used to render the widget contents. */
		QGraphicsScene *m_pScene;

		/** Pixmap item used to display the face image. */
		QGraphicsPixmapItem *m_pPixmapItem;

		/** The current applied scale factor. */
		double m_dScaleFactor;

		/** List of nodes used to edit the coordinates of facial features. */
		QList<FaceFeatureNode*> m_lFaceFeatures;

		/** List of edges connecting two feature nodes. */
		QList<FaceFeatureEdge*> m_lConnections;

		/** Indicates if the face feature nodes should be displayed or not. */
		bool m_bDisplayFaceFeatures;

		/** Indicates if the face feature edges should be displayed or not. */
		bool m_bDisplayConnections;

		/** Indicates if the IDs of the face feature nodes should be displayed or not. */
		bool m_bDisplayFeatureIDs;

		/** Context menu for face feature editor. */
		QMenu *m_pEditorContextMenu;

		/** Context menu for face feature nodes. */
		QMenu *m_pFeaturesContextMenu;

		/** Context menu for face feature edges. */
		QMenu *m_pConnectionsContextMenu;
	};
};


#endif //FACEWIDGET_H
