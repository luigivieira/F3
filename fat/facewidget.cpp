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
#include <QtMath>

// Scale values for zoom in and out steps
const double f3::FaceWidget::ZOOM_IN_STEP = 1.25;
const double f3::FaceWidget::ZOOM_OUT_STEP = 0.80;

// Number of face features edited by the widget
const int f3::FaceWidget::NUM_FACE_FEATURES = 68;

// +-----------------------------------------------------------
f3::FaceWidget::FaceWidget(QWidget *pParent) : QGraphicsView(pParent)
{
	setDragMode(RubberBandDrag);

	m_pScene = new QGraphicsScene(this);
	m_pScene->setItemIndexMethod(QGraphicsScene::NoIndex);
	setScene(m_pScene);

	setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    scale(1.0, 1.0);
	m_dScaleFactor = 1.0;

	setBackgroundBrush(QApplication::palette().dark());

	// Add the image item
	QPixmap oPixmap(":/images/noface");
	m_pPixmapItem = m_pScene->addPixmap(oPixmap);
	m_pPixmapItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
	m_pScene->setSceneRect(0, 0, oPixmap.width(), oPixmap.height());

	// Setup the face features editor
	m_bDisplayFaceFeatureNodes = true;
	m_bDisplayFaceFeatureEdges = true;
	m_bDisplayIDs = false;

	createFaceFeatures();
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
	return qFloor(m_dScaleFactor * 100000.0) / 100000.0; // Returns it with precision of 5 decimals
}

// +-----------------------------------------------------------
void f3::FaceWidget::setScaleFactor(const double dScaleFactor)
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
		emit onScaleFactorChanged(getScaleFactor());
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
void f3::FaceWidget::zoomIn()
{
	scaleViewBy(ZOOM_IN_STEP);
}

// +-----------------------------------------------------------
void f3::FaceWidget::zoomOut()
{
	scaleViewBy(ZOOM_OUT_STEP);
}

// +-----------------------------------------------------------
void f3::FaceWidget::createFaceFeatures()
{
	// These are the coordinates for the face features of a sample face model
	double aFaceModel[NUM_FACE_FEATURES][2] =
	{
		{217.79878, 217.08728},
		{216.89078, 254.86311},
		{223.67206, 292.07462},
		{238.23885, 326.79248},
		{254.94789, 360.61587},
		{279.53139, 388.90414},
		{310.45043, 410.42450},
		{345.37818, 425.29204},
		{382.36266, 428.51270},
		{418.06689, 424.13911},
		{449.99561, 408.56366},
		{477.42884, 385.63278},
		{499.83194, 357.72590},
		{509.49470, 323.37268},
		{516.13174, 287.86876},
		{518.15853, 252.06194},
		{518.69634, 216.55158},
		{247.62010, 180.34162},
		{268.70792, 165.39026},
		{293.56778, 158.93545},
		{319.16937, 160.78699},
		{343.48894, 169.15112},
		{397.25624, 164.28431},
		{420.26527, 156.48688},
		{443.57508, 150.79923},
		{467.19522, 154.84270},
		{488.87233, 165.42594},
		{372.77664, 199.50671},
		{373.99586, 223.36618},
		{375.57578, 246.90136},
		{378.10595, 270.34028},
		{345.61168, 293.24677},
		{361.62731, 300.03883},
		{378.63337, 301.76439},
		{393.55789, 298.57543},
		{406.85187, 291.64665},
		{280.10863, 206.43671},
		{300.22526, 197.40182},
		{321.95062, 195.23294},
		{339.75602, 206.97070},
		{319.74604, 209.54150},
		{299.69791, 210.59487},
		{405.25688, 203.23706},
		{420.24655, 189.68711},
		{440.47753, 188.93026},
		{459.57889, 196.31247},
		{442.35281, 203.82019},
		{423.65388, 206.10059},
		{329.76923, 340.09600},
		{344.93415, 328.45414},
		{359.76855, 316.54668},
		{377.56618, 319.87236},
		{395.18127, 314.47085},
		{411.21730, 325.09745},
		{427.09559, 335.85024},
		{415.75540, 349.79882},
		{398.78054, 356.02983},
		{380.76123, 358.75053},
		{361.44337, 358.76781},
		{343.08983, 353.52445},
		{341.63086, 335.00864},
		{353.49249, 334.72611},
		{377.56554, 336.91333},
		{402.31187, 331.58481},
		{414.70373, 331.84948},
		{401.78216, 332.11416},
		{377.03251, 337.44653},
		{353.00775, 335.29117},
	};

	// Add the face feature nodes
	FaceFeatureNode *pPrevNode = NULL;
	FaceFeatureNode *pCurNode = NULL;
	for(int i = 0; i < NUM_FACE_FEATURES; i++)
	{
		pCurNode = addFaceFeatureNode(QPointF(aFaceModel[i][0], aFaceModel[i][1]));
		if(!pPrevNode)
			pPrevNode = pCurNode;
		else
		{
			addFaceFeatureEdge(pPrevNode, pCurNode);
			pPrevNode = pCurNode;
		}
	}
}

// +-----------------------------------------------------------
const QList<f3::FaceFeatureNode*>& f3::FaceWidget::getFaceFeatureNodes() const
{
	return m_lFaceFeatureNodes;
}

// +-----------------------------------------------------------
f3::FaceFeatureNode* f3::FaceWidget::addFaceFeatureNode(const QPointF &oPos)
{
	FaceFeatureNode *pNode = new FaceFeatureNode(this);
	m_pScene->addItem(pNode);
	m_lFaceFeatureNodes.append(pNode);
	pNode->setPos(oPos);
	return pNode;
}

// +-----------------------------------------------------------
void f3::FaceWidget::removeFaceFeatureNode(FaceFeatureNode* pNode)
{
	// First, remove all edges connected to the node
	QList<FaceFeatureEdge*> lEdges = pNode->edges();
	for(int i = 0; i < lEdges.size(); i++)
		removeFaceFeatureEdge(lEdges[i]);

	// Then, remove the node
	m_lFaceFeatureNodes.removeOne(pNode);
	m_pScene->removeItem(pNode);
	delete pNode;
}

// +-----------------------------------------------------------
f3::FaceFeatureEdge* f3::FaceWidget::addFaceFeatureEdge(FaceFeatureNode* pSource, FaceFeatureNode* pTarget)
{
	FaceFeatureEdge* pEdge = new FaceFeatureEdge(pSource, pTarget);
	m_pScene->addItem(pEdge);
	m_lFaceFeatureEdges.append(pEdge);
	return pEdge;
}

// +-----------------------------------------------------------
void f3::FaceWidget::removeFaceFeatureEdge(FaceFeatureEdge* pEdge)
{
	pEdge->sourceNode()->removeEdge(pEdge);
	pEdge->targetNode()->removeEdge(pEdge);
	m_lFaceFeatureEdges.removeOne(pEdge);
	m_pScene->removeItem(pEdge);
	delete pEdge;
}

// +-----------------------------------------------------------
void f3::FaceWidget::faceFeatureMoved(FaceFeatureNode *pNode)
{
	Q_UNUSED(pNode);
	emit onFaceFeaturesChanged();
}

// +-----------------------------------------------------------
bool f3::FaceWidget::displayFaceFeatureNodes() const
{
	return m_bDisplayFaceFeatureNodes;
}
// +-----------------------------------------------------------
void f3::FaceWidget::setDisplayFaceFeatureNodes(const bool bValue)
{
	if(bValue == m_bDisplayFaceFeatureNodes)
		return;

	m_bDisplayFaceFeatureNodes = bValue;
	foreach(FaceFeatureNode *pNode, m_lFaceFeatureNodes)
		pNode->setVisible(bValue);
	update();
}

// +-----------------------------------------------------------
bool f3::FaceWidget::displayFaceFeatureEdges() const
{
	return m_bDisplayFaceFeatureEdges;
}

// +-----------------------------------------------------------
void f3::FaceWidget::setDisplayFaceFeatureEdges(const bool bValue)
{
	if(bValue == m_bDisplayFaceFeatureEdges)
		return;

	m_bDisplayFaceFeatureEdges = bValue;
	foreach(FaceFeatureEdge *pEdge, m_lFaceFeatureEdges)
		pEdge->setVisible(bValue);
	update();
}

// +-----------------------------------------------------------
bool f3::FaceWidget::displayIDs() const
{
	return m_bDisplayIDs;
}

// +-----------------------------------------------------------
void f3::FaceWidget::setDisplayIDs(const bool bValue)
{
	m_bDisplayIDs = bValue;
	foreach(FaceFeatureNode *pNode, m_lFaceFeatureNodes)
		pNode->update();
	update();
}