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
bool f3::FaceImage::isEmpty() const
{
	return m_sFileName.isEmpty();
}

// +-----------------------------------------------------------
bool f3::FaceImage::loadFromXML(const QDomElement &oElement, QString &sMsgError)
{
	// Check the element name
	if(oElement.tagName() != "Sample")
	{
		sMsgError = QString(QApplication::translate("FaceImage", "nome de nó inválido [%1] - era esperado o nome de nó '%2'").arg(oElement.tagName(), "Sample"));
		return false;
	}

	QString sFile = oElement.attribute("fileName");
	if(sFile == "")
	{
		sMsgError = QString(QApplication::translate("FaceImage", "o atributo '%1' não existe ou tem valor inválido").arg("fileName"));
		return false;
	}

	int iEmotion = oElement.attribute("emotionLabel", "-1").toInt();
	if(iEmotion < EmotionLabel::UNDEFINED.getValue() || iEmotion > EmotionLabel::SURPRISE.getValue())
	{
		sMsgError = QString(QApplication::translate("FaceImage", "o atributo '%1' não existe ou tem valor inválido").arg("emotionLabel"));
		return false;
	}

	m_sFileName = sFile;
	m_eEmotionLabel = EmotionLabel::fromValue(iEmotion);

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
	foreach(FaceFeature oFeat, m_vFeatures)
		oFeat.saveToXML(oFeatures);
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
