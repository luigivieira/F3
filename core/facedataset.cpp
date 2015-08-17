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
#include "facedataset.h"

#include <QDebug>
#include <QFileInfo>
#include <QApplication>

#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

// +-----------------------------------------------------------
f3::FaceDataset::FaceDataset()
{
	m_iNumFeatures = 0;
}

// +-----------------------------------------------------------
f3::FaceDataset::~FaceDataset()
{
	clear();
}

// +-----------------------------------------------------------
int f3::FaceDataset::size() const
{
	return m_vSamples.size();
}

// +-----------------------------------------------------------
bool f3::FaceDataset::loadFromFile(const QString &sFileName, QString &sMsgError)
{
	/******************************************************
	 * Open and read the file
	 ******************************************************/
	QFile oFile(sFileName);
	if (!oFile.open(QFile::ReadOnly))
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "não foi possível ler o arquivo [%1]")).arg(sFileName);
        return false;
    }

	QTextStream oData(&oFile);
	QString sData = oData.readAll();
	oFile.close();

	/******************************************************
	 * Parse the xml document
	 ******************************************************/
	QDomDocument oDoc;
	QString sError;
	int iLine, iColumn;
	if(!oDoc.setContent(sData, true, &sError, &iLine, &iColumn))
	{
		sMsgError = QString(QApplication::translate("FaceDataset", "erro no conteúdo do arquivo [%1]: problema [%2], linha [%3], coluna [%4]")).arg(sFileName, sError, QString::number(iLine), QString::number(iColumn));
		return false;
	}

	// Root node
	QDomElement oRoot = oDoc.firstChildElement("FaceDataset");

	if(oRoot.isNull())
	{
		sMsgError = QString(QApplication::translate("FaceDataset", "erro no conteúdo do arquivo [%1]: o nó '%2' não existe")).arg(sFileName, "FaceDataset");
		return false;
	}

	int iNumFeats = oRoot.attribute("numberOfFeatures", "-1").toInt();
	if(iNumFeats < 0)
	{
		sMsgError = QString(QApplication::translate("FaceDataset", "erro no conteúdo do arquivo [%1]: o atributo '%2' não existe ou contém um valor inválido")).arg(sFileName, "numberOfFeatures");
		return false;
	}

	// Sample images
	QDomElement oSamples = oRoot.firstChildElement("Samples");
	if(oSamples.isNull())
	{
		sMsgError = QString(QApplication::translate("FaceDataset", "erro no conteúdo do arquivo [%1]: o nó '%2' não existe")).arg(sFileName, "Samples");
		return false;
	}

	vector<FaceImage*> vSamples;
	for(QDomElement oElement = oSamples.firstChildElement(); !oElement.isNull(); oElement = oElement.nextSiblingElement())
	{
		FaceImage *pSample = new FaceImage();
		if(!pSample->loadFromXML(oElement, sError))
		{
			sMsgError = QString(QApplication::translate("FaceDataset", "erro no conteúdo do arquivo [%1]: %2")).arg(sFileName, sError);
			return false;
		}
		vSamples.push_back(pSample);
	}

	clear();
	m_iNumFeatures = iNumFeats;
	m_vSamples = vSamples;

	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::saveToFile(const QString &sFileName, QString &sMsgError) const
{
	/******************************************************
	 * Create the xml document
	 ******************************************************/
	QDomDocument oDoc;

	// Processing instruction
	QDomProcessingInstruction oInstr = oDoc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
	oDoc.appendChild(oInstr);

	// Root node
	QDomElement oRoot = oDoc.createElementNS("http://www.luiz.vieira.nom.br/f3/", "FaceDataset");
	oDoc.appendChild(oRoot);

	oRoot.setAttribute("numberOfFeatures", m_iNumFeatures);

	// Sample images
	QDomElement oSamples = oDoc.createElement("Samples");
	oRoot.appendChild(oSamples);

	foreach(FaceImage *pImage, m_vSamples)
		pImage->saveToXML(oSamples);

	/******************************************************
	 * Save the file
	 ******************************************************/
	QFile oFile(sFileName);
	if (!oFile.open(QFile::WriteOnly | QFile::Truncate))
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "não foi possível escrever no arquivo [%1]")).arg(sFileName);
        return false;
    }

	QTextStream oData(&oFile);
	oDoc.save(oData, 4);
	oFile.close();

	return true;
}

// +-----------------------------------------------------------
void f3::FaceDataset::clear()
{
	foreach(FaceImage *pImage, m_vSamples)
		delete pImage;
	m_vSamples.clear();

	m_iNumFeatures = 0;
}

// +-----------------------------------------------------------
f3::FaceImage* f3::FaceDataset::getImage(const int iIndex) const
{
    if(iIndex < 0 || iIndex >= size())
		return NULL;

	return m_vSamples[iIndex];
}

// +-----------------------------------------------------------
f3::FaceImage* f3::FaceDataset::addImage(const QString &sFileName)
{
	foreach(FaceImage *pImage, m_vSamples)
		if(pImage->fileName() == sFileName)
			return pImage;

	FaceImage *pRet = new FaceImage(sFileName);
	m_vSamples.push_back(pRet);
	return pRet;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::removeImage(const int iIndex)
{
	if(iIndex < 0 || iIndex >= size())
		return false;

	FaceImage *pImage = m_vSamples[iIndex];
	m_vSamples.erase(m_vSamples.begin() + iIndex);
	delete pImage;

	return true;
}

// +-----------------------------------------------------------
int f3::FaceDataset::numFeatures() const
{
	return m_iNumFeatures;
}

// +-----------------------------------------------------------
void f3::FaceDataset::setNumFeatures(int iNumFeats)
{
	m_iNumFeatures = iNumFeats;
}