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
int f3::FaceDataset::count() const
{
	return m_vFaceImages.size();
}

// +-----------------------------------------------------------
void f3::FaceDataset::loadFromFile(const char* sFileName)
{
    FileStorage oFS(sFileName, FileStorage::READ);
    if(!oFS.isOpened())
    {
        qWarning() << "Error opening file " << sFileName << " for reading";
        throw invalid_argument(string(sFileName));
    }

    FileNode oNode = oFS["FeaturesSymmetry"];
    if(oNode.type() != FileNode::SEQ)
    {
        qWarning() << "Structural error (field FeaturesSymmetry) in the structure of file " << sFileName;
		throw runtime_error(string("FeaturesSymmetry"));
    }

    vector<int> vSymmetry;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vSymmetry.push_back(*it);

    oNode = oFS["FeaturesConnectivity"];
    if(oNode.type() != FileNode::SEQ)
    {
        qWarning() << "Structural error (field FeaturesConnectivity) in the structure of file " << sFileName;
        throw runtime_error(string("FeaturesConnectivity"));
    }

    vector<Vec2i> vConnectivity;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vConnectivity.push_back((Vec2i) *it);

    oNode = oFS["SampleImages"];
    if(oNode.type() != FileNode::SEQ)
    {
        qWarning() << "Structural error (field SampleImages) in the structure of file " << sFileName;
        throw runtime_error(string("SampleImages"));
    }

    vector<string> vImages;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it)
        vImages.push_back(*it);

    oNode = oFS["FeaturePoints"];
    if(oNode.type() != FileNode::SEQ)
    {
        qWarning() << "Structural error (field FeaturePoints) in the structure of file " << sFileName;
        throw runtime_error(string("FeaturePoints"));
    }

    vector< vector<Point2f> > vPoints;
    int i = 0;
    for(FileNodeIterator it = oNode.begin(); it != oNode.end(); ++it, i++)
    {
        FileNode oSubNode = (FileNode) *it;

        if(oSubNode.type() != FileNode::SEQ)
        {
            qWarning() << "Structural error (field FeaturePoints subnode) in the structure of file " << sFileName;
            throw runtime_error(string("FeaturePoints (subnode)"));
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
                qWarning() << "Structural error (field FeaturePoints/number of points) in the structure of file " << sFileName;
                throw runtime_error(string("FeaturePoints (number of points)"));
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
        qWarning() << "Error creating file " << sFileName;
        throw invalid_argument(string(sFileName));
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
    m_vFeatureSymmetry.clear();
    m_vFeatureConnectivity.clear();
    m_vFaceImages.clear();
    m_vFeaturePoints.clear();
}


// +-----------------------------------------------------------
Mat f3::FaceDataset::getImage(const int iIndex) const
{
    if(iIndex < 0 || iIndex >= count())
		throw invalid_argument(string("index out of bounds"));

    return imread(m_vFaceImages[iIndex]);
}

// +-----------------------------------------------------------
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
}