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
bool f3::FaceDataset::loadFromFile(const QString &sFileName, QString &sMsgError)
{
	sMsgError = "";
	FileStorage oFS(sFileName.toStdString(), FileStorage::READ);
    if(!oFS.isOpened())
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "não foi possível abrir o arquivo [%1] para leitura")).arg(sFileName);
		return false;
    }

    FileNode oNode = oFS["FaceImages"];
    if(oNode.type() != FileNode::SEQ)
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("SampleImages", sFileName);
        return false;
    }

    vector<QString> vImages;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vImages.push_back(QString::fromStdString(*it));

    oNode = oFS["FeaturePoints"];
    if(oNode.type() != FileNode::SEQ)
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturePoints", sFileName);
        return false;
    }

    vector< vector<Point2f> > vPoints;
    int i = 0;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it, i++)
    {
        FileNode oSubNode = (FileNode) *it;

        if(oSubNode.type() != FileNode::SEQ)
        {
			sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturePoints/subnode", sFileName);
            return false;
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
				sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturePoints/points", sFileName);
                return false;
            }
            y = *it2;

            Point2f oPoint(x, y);
            vMarks.push_back(oPoint);

            ++it2;
        }

        vPoints.push_back(vMarks);
    }

    oNode = oFS["FeaturesConnectivity"];
    if(oNode.type() != FileNode::SEQ)
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturesConnectivity", sFileName);
		return false;
    }

    vector<Vec2i> vConnectivity;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vConnectivity.push_back((Vec2i) *it);

    oNode = oFS["FeaturesSymmetry"];
    if(oNode.type() != FileNode::SEQ)
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("FeaturesSymmetry", sFileName);
        return false;
    }

    vector<int> vSymmetry;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vSymmetry.push_back(*it);

    oNode = oFS["EmotionLabels"];
    if(oNode.type() != FileNode::SEQ)
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "há um erro estrutural (campo [%1]) no arquivo [%2]")).arg("EmotionLabels", sFileName);
        return false;
    }

    vector<EmotionLabel> vEmotionLabels;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
		vEmotionLabels.push_back(EmotionLabel::fromValue(*it));

    clear();
    m_vFaceImages = vImages;
    m_vFeaturePoints = vPoints;
    m_vFeatureSymmetry = vSymmetry;
    m_vFeatureConnectivity = vConnectivity;
	m_vEmotionLabels = vEmotionLabels;

	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::saveToFile(const QString &sFileName, QString &sMsgError) const
{
	sMsgError = "";
	FileStorage oFS(sFileName.toStdString(), FileStorage::WRITE);
    if(!oFS.isOpened())
    {
		sMsgError = QString(QApplication::translate("FaceDataset", "não foi possível escrever no arquivo [%1]")).arg(sFileName);
        return false;
    }

    oFS << "FaceImages" << "[";
    for(unsigned int i = 0; i < m_vFaceImages.size(); i++)
        oFS << m_vFaceImages[i].toStdString();
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

    oFS << "FeaturesConnectivity" << "[:";
    for(unsigned int i = 0; i < m_vFeatureConnectivity.size(); i++)
        oFS << m_vFeatureConnectivity[i];
    oFS << "]";

    oFS << "FeaturesSymmetry" << "[:";
    for(unsigned int i = 0; i < m_vFeatureSymmetry.size(); i++)
        oFS << m_vFeatureSymmetry[i];
    oFS << "]";

    oFS << "EmotionLabels" << "[:";
	for(unsigned int i = 0; i < m_vEmotionLabels.size(); i++)
		oFS << m_vEmotionLabels[i].getValue();
    oFS << "]";

	return true;
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
bool f3::FaceDataset::getImage(const int iIndex, QPixmap &oImage) const
{
    if(iIndex < 0 || iIndex >= size())
		return false;

	if(!oImage.load(m_vFaceImages[iIndex]))
		return false;

    return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::getImage(const int iIndex, Mat &oImage) const
{
    if(iIndex < 0 || iIndex >= size())
		return false;

	oImage = imread(m_vFaceImages[iIndex].toStdString());
	return !oImage.empty();
}

// +-----------------------------------------------------------
bool f3::FaceDataset::getImageFileName(const int iIndex, QString &sFileName) const
{
    if(iIndex < 0 || iIndex >= size())
		return false;

	sFileName = m_vFaceImages[iIndex];
	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::addImage(const QString &sFileName)
{
	if(find(m_vFaceImages.begin(), m_vFaceImages.end(), sFileName) != m_vFaceImages.end())
		return false;

	// All samples have 68 landmarks
	vector<Point2f> vDefault;
	vDefault.insert(vDefault.begin(), 68, Point2f(0, 0)); // TODO: Use the face tracker to suggest the initial landmarks
	
	m_vFaceImages.push_back(sFileName);
	m_vFeaturePoints.push_back(vDefault);
	m_vEmotionLabels.push_back(EmotionLabel::UNDEFINED);

	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::removeImage(const int iIndex)
{
	if(iIndex < 0 || iIndex >= (int) m_vFaceImages.size())
		return false;

	m_vFaceImages.erase(m_vFaceImages.begin() + iIndex);
	m_vFeaturePoints.erase(m_vFeaturePoints.begin() + iIndex);
	m_vEmotionLabels.erase(m_vEmotionLabels.begin() + iIndex);

	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::getEmotionLabel(const int iIndex, EmotionLabel &eLabel) const
{
    if(iIndex < 0 || iIndex >= size())
		return false;

	eLabel = m_vEmotionLabels[iIndex];
	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDataset::setEmotionLabel(const int iIndex, const EmotionLabel eLabel)
{
    if(iIndex < 0 || iIndex >= size())
		return false;

	m_vEmotionLabels[iIndex] = eLabel;
	return true;
}

/*
// +-----------------------------------------------------------
void f3::FaceDataset::addSamples(vector<string> vSamples, vector< vector<Point2f> > vFeatures)
{
    m_vFaceImages.insert(m_vFaceImages.end(), vSamples.begin(), vSamples.end());
    m_vFeaturePoints.insert(m_vFeaturePoints.end(), vFeatures.begin(), vFeatures.end());
}
*/