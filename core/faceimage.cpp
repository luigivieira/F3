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

#include "faceimage.h"

#include <QApplication>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

// +-----------------------------------------------------------
f3::FaceImage::FaceImage(const QString &sFileName, const EmotionLabel eEmotionLabel)
{
	m_sFileName = sFileName;
	m_eEmotionLabel = eEmotionLabel;
}

// +-----------------------------------------------------------
f3::FaceImage::~FaceImage()
{
	clear();
}

// +-----------------------------------------------------------
void f3::FaceImage::clear()
{
	foreach(FaceFeature *pFeature, m_vFeatures)
		delete pFeature;
	m_vFeatures.clear();
}

// +-----------------------------------------------------------
QString f3::FaceImage::fileName() const
{
	return m_sFileName;
}

// +-----------------------------------------------------------
f3::EmotionLabel f3::FaceImage::emotionLabel() const
{
	return m_eEmotionLabel;
}

// +-----------------------------------------------------------
void f3::FaceImage::setEmotionLabel(const EmotionLabel eEmotionLabel)
{
	m_eEmotionLabel = eEmotionLabel;
}

// +-----------------------------------------------------------
bool f3::FaceImage::loadFromXML(const QDomElement &oElement, QString &sMsgError, int iNumExpectedFeatures)
{
	// Check the element name
	if(oElement.tagName() != "Sample")
	{
		sMsgError = QString(QApplication::translate("FaceImage", "nome de nó inválido [%1] - era esperado o nome de nó '%2'").arg(oElement.tagName(), "Sample"));
		return false;
	}

	// Read the file name
	QString sFile = oElement.attribute("fileName");
	if(sFile == "")
	{
		sMsgError = QString(QApplication::translate("FaceImage", "o atributo '%1' não existe ou tem valor inválido").arg("fileName"));
		return false;
	}

	// Read the emotion label
	int iEmotion = oElement.attribute("emotionLabel", "-1").toInt();
	if(iEmotion < EmotionLabel::UNDEFINED.getValue() || iEmotion > EmotionLabel::SURPRISE.getValue())
	{
		sMsgError = QString(QApplication::translate("FaceImage", "o atributo '%1' não existe ou tem valor inválido").arg("emotionLabel"));
		return false;
	}

	// Read the face features
	// Sample images
	QDomElement oFeatures = oElement.firstChildElement("Features");
	if(oFeatures.isNull() || oFeatures.childNodes().count() != iNumExpectedFeatures)
	{
		sMsgError = QString(QApplication::translate("FaceImage", "o nó '%1' não existe ou não tem o número esperado de nós filhos").arg("Features"));
		return false;
	}

	vector<FaceFeature*> vFeatures;
	for(QDomElement oElement = oFeatures.firstChildElement(); !oElement.isNull(); oElement = oElement.nextSiblingElement())
	{
		FaceFeature *pFeature = new FaceFeature();
		if(!pFeature->loadFromXML(oElement, sMsgError))
		{
			foreach(FaceFeature *pFeat, vFeatures)
				delete(pFeat);
			delete pFeature;

			return false;
		}
		vFeatures.push_back(pFeature);
	}

	clear();
	m_sFileName = sFile;
	m_eEmotionLabel = EmotionLabel::fromValue(iEmotion);
	m_vFeatures = vFeatures;
	return true;
}

// +-----------------------------------------------------------
void f3::FaceImage::saveToXML(QDomElement &oParent) const
{
	// Add the "Sample" node
	QDomElement oSample = oParent.ownerDocument().createElement("Sample");
	oParent.appendChild(oSample);

	// Define it's attributes
	oSample.setAttribute("fileName", m_sFileName);
	oSample.setAttribute("emotionLabel", m_eEmotionLabel.getValue());

	// Add the "Features" subnode
	QDomElement oFeatures = oParent.ownerDocument().createElement("Features");
	oSample.appendChild(oFeatures);

	// Add the nodes for the features
	foreach(FaceFeature *pFeat, m_vFeatures)
		pFeat->saveToXML(oFeatures);
}

// +-----------------------------------------------------------
QPixmap f3::FaceImage::pixMap() const
{
	QPixmap oRet;
	oRet.load(m_sFileName);
	return oRet;
}

// +-----------------------------------------------------------
Mat f3::FaceImage::mat() const
{
	Mat oRet = imread(m_sFileName.toStdString());
	return oRet;
}

// +-----------------------------------------------------------
f3::FaceFeature* f3::FaceImage::addFeature(int iID, float x, float y)
{
	FaceFeature *pFeat = new FaceFeature(iID, x, y);
	m_vFeatures.push_back(pFeat);
	return pFeat;
}

// +-----------------------------------------------------------
f3::FaceFeature* f3::FaceImage::getFeature(const int iIndex) const
{
	if(iIndex < 0 || iIndex >= (int) m_vFeatures.size())
		return NULL;
	return m_vFeatures[iIndex];
}

// +-----------------------------------------------------------
std::vector<f3::FaceFeature*> f3::FaceImage::getFeatures() const
{
	return m_vFeatures;
}

// +-----------------------------------------------------------
bool f3::FaceImage::removeFeature(const int iIndex)
{
	if(iIndex < 0 || iIndex >= (int) m_vFeatures.size())
		return false;

	FaceFeature *pFeat = m_vFeatures[iIndex];
	m_vFeatures.erase(m_vFeatures.begin() + iIndex);
	delete pFeat;

	return true;
}