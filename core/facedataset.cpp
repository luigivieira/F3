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

#include <stdexcept>

#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QPixmap>

#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

// +-----------------------------------------------------------
f3::FaceDataset::FaceDataset()
{
}

// +-----------------------------------------------------------
f3::FaceDataset::~FaceDataset()
{
}

// +-----------------------------------------------------------
int f3::FaceDataset::size() const
{
	return m_vFaceImages.size();
}

// +-----------------------------------------------------------
void f3::FaceDataset::loadFromFile(const char* sFileName)
{
    FileStorage oFS(sFileName, FileStorage::READ);
    if(!oFS.isOpened())
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "Não foi possível abrir o arquivo [%1] para leitura")).arg(sFileName);
        qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
    }

    FileNode oNode = oFS["FeaturesSymmetry"];
    if(oNode.type() != FileNode::SEQ)
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "Há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturesSymmetry", sFileName);
        qWarning() << sMsg;
		throw runtime_error(sMsg.toStdString());
    }

    vector<int> vSymmetry;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vSymmetry.push_back(*it);

    oNode = oFS["FeaturesConnectivity"];
    if(oNode.type() != FileNode::SEQ)
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "Há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturesConnectivity", sFileName);
        qWarning() << sMsg;
		throw runtime_error(sMsg.toStdString());
    }

    vector<Vec2i> vConnectivity;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vConnectivity.push_back((Vec2i) *it);

    oNode = oFS["SampleImages"];
    if(oNode.type() != FileNode::SEQ)
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "Há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("SampleImages", sFileName);
        qWarning() << sMsg;
		throw runtime_error(sMsg.toStdString());
    }

    vector<string> vImages;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vImages.push_back(*it);

    oNode = oFS["FeaturePoints"];
    if(oNode.type() != FileNode::SEQ)
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "Há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturePoints", sFileName);
        qWarning() << sMsg;
		throw runtime_error(sMsg.toStdString());
    }

    vector< vector<Point2f> > vPoints;
    int i = 0;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it, i++)
    {
        FileNode oSubNode = (FileNode) *it;

        if(oSubNode.type() != FileNode::SEQ)
        {
			QString sMsg = QString(QApplication::translate("FaceDataset", "Há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturePoints/subnode", sFileName);
            qWarning() << sMsg;
			throw runtime_error(sMsg.toStdString());
        }

        vector<Point2f> vMarks;

        FileNodeIterator it2 = oSubNode.begin();
        while(it2 != oSubNode.end())
        {
            float x, y;
            x = *it2;
            ++it2;
            if(it2 == oSubNode.end())
            {
				QString sMsg = QString(QApplication::translate("FaceDataset", "Há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturePoints/points", sFileName);
                qWarning() << sMsg;
				throw runtime_error(sMsg.toStdString());
            }
            y = *it2;

            Point2f oPoint(x, y);
            vMarks.push_back(oPoint);

            ++it2;
        }

        vPoints.push_back(vMarks);
    }

    clear();
    m_vFeatureSymmetry = vSymmetry;
    m_vFeatureConnectivity = vConnectivity;
    m_vFaceImages = vImages;
    m_vFeaturePoints = vPoints;
}

// +-----------------------------------------------------------
void f3::FaceDataset::saveToFile(const char* sFileName) const
{
    FileStorage oFS(sFileName, FileStorage::WRITE);
    if(!oFS.isOpened())
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "Não foi possível escrever no arquivo [%1]")).arg(sFileName);
        qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
    }

    oFS << "FeaturesSymmetry" << "[:";
    for(unsigned int i = 0; i < m_vFeatureSymmetry.size(); i++)
        oFS << m_vFeatureSymmetry[i];
    oFS << "]";

    oFS << "FeaturesConnectivity" << "[:";
    for(unsigned int i = 0; i < m_vFeatureConnectivity.size(); i++)
        oFS << m_vFeatureConnectivity[i];
    oFS << "]";

    oFS << "SampleImages" << "[";
    for(unsigned int i = 0; i < m_vFaceImages.size(); i++)
        oFS << m_vFaceImages[i];
    oFS << "]";

    oFS << "FeaturePoints" << "[";
    for(unsigned int i = 0; i < m_vFeaturePoints.size(); i++)
    {
        oFS << "[:";
        for(unsigned int j = 0; j < m_vFeaturePoints[i].size(); j++)
        {
            Point2f oPoint = m_vFeaturePoints[i][j];
            oFS << oPoint.x;
            oFS << oPoint.y;
        }
        oFS << "]";
    }
    oFS << "]";
}

// +-----------------------------------------------------------
void f3::FaceDataset::clear()
{
    m_vFaceImages.clear();
    m_vFeaturePoints.clear();
	m_vEmotionLabels.clear();

    m_vFeatureSymmetry.clear();
    m_vFeatureConnectivity.clear();
}


// +-----------------------------------------------------------
Mat f3::FaceDataset::getImageMat(const int iIndex) const
{
    if(iIndex < 0 || iIndex >= size())
	{
		QString sMsg = QString(QApplication::translate("FaceDataset", "O índice [%1] está fora do intervalo de imagens existentes")).arg(iIndex);
		qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
	}

    return imread(m_vFaceImages[iIndex]);
}

// +-----------------------------------------------------------
QImage f3::FaceDataset::getImage(const int iIndex) const
{
    if(iIndex < 0 || iIndex >= size())
	{
		QString sMsg = QString(QApplication::translate("FaceDataset", "O índice [%1] está fora do intervalo de imagens existentes")).arg(iIndex);
		qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
	}

	QImage oImage;
	if(!oImage.load(QString().fromStdString(m_vFaceImages[iIndex])))
	{
		QString sMsg = QString(QApplication::translate("FaceDataset", "Não foi possível abrir a imagem do arquivo [%1]")).arg(QString().fromStdString(m_vFaceImages[iIndex]));
		qWarning() << sMsg;
		throw runtime_error(sMsg.toStdString());
	}

    return oImage;
}

// +-----------------------------------------------------------
QString f3::FaceDataset::getImageFile(const int iIndex) const
{
    if(iIndex < 0 || iIndex >= size())
	{
		QString sMsg = QString(QApplication::translate("FaceDataset", "O índice [%1] está fora do intervalo de imagens existentes")).arg(iIndex);
		qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
	}

	return QString().fromStdString(m_vFaceImages[iIndex]);
}

// +-----------------------------------------------------------
void f3::FaceDataset::addImage(const char *sFileName)
{
	QFileInfo oFile(sFileName);
	if(!oFile.exists())
    {
		QString sMsg = QString(QApplication::translate("FaceDataset", "O arquivo [%1] não pode ser adicionado porque ele não existe")).arg(sFileName);
        qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
    }

	if(find(m_vFaceImages.begin(), m_vFaceImages.end(), string(sFileName)) != m_vFaceImages.end())
	{
		QString sMsg = QString(QApplication::translate("FaceDataset", "O arquivo [%1] já existe no banco de faces anotadas")).arg(sFileName);
        qWarning() << sMsg;
		throw runtime_error(sMsg.toStdString());
	}

	// All samples have 68 landmarks
	vector<Point2f> vDefault;
	vDefault.insert(vDefault.begin(), 68, Point2f(0, 0));
	
	m_vFaceImages.push_back(sFileName);
	m_vFeaturePoints.push_back(vDefault);
	m_vEmotionLabels.push_back(EmotionLabel::UNDEFINED);
}

// +-----------------------------------------------------------
f3::EmotionLabel f3::FaceDataset::getEmotionLabel(const int iIndex) const
{
    if(iIndex < 0 || iIndex >= size())
	{
		QString sMsg = QString(QApplication::translate("FaceDataset", "O índice [%1] está fora do intervalo de imagens existentes")).arg(iIndex);
		qWarning() << sMsg;
		throw invalid_argument(sMsg.toStdString());
	}

	return m_vEmotionLabels[iIndex];
}

/*// +-----------------------------------------------------------
void f3::FaceDataset::addSamples(vector<string> vSamples, vector< vector<Point2f> > vFeatures)
{
    m_vFaceImages.insert(m_vFaceImages.end(), vSamples.begin(), vSamples.end());
    m_vFeaturePoints.insert(m_vFeaturePoints.end(), vFeatures.begin(), vFeatures.end());
}

// +-----------------------------------------------------------
bool f3::FaceDataset::getSample(const int iIndex, string &sName, vector<Point2f> &vLandmarks)
{
    if(iIndex < 0 || iIndex >= (int) m_vFaceImages.size())
        return false;

    sName = m_vFaceImages[iIndex];
    vLandmarks = m_vFeaturePoints[iIndex];

    return true;
}*/