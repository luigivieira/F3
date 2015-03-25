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

#ifndef FACEDATASET_H
#define FACEDATASET_H

#include "core_global.h"
#include "emotionlabel.h"

#include <opencv2/core/core.hpp>

#include <QPixmap>

#include <vector>
#include <string>

namespace f3
{
	/**
	 * Represents a dataset of facial images that can be annotated with facial landmarks and
	 * prototypical emotional labels.
	 */
	class CORE_EXPORT FaceDataset
	{
	public:
		/**
		 * Class constructor.
		 */
		FaceDataset();

		/**
		 * Class destructor.
		 */
		virtual ~FaceDataset();

		/**
		 * Returns the number of face samples in the dataset.
		 * @return Integer with the number of face samples in this dataset.
		 */
		int size() const;

		/**
		 * Loads (unserializes) the instance from the given text file in the YAML format
		 * (YAML Ain't Markup Language - http://en.wikipedia.org/wiki/YAML).
		 * @param sFileName QString with the name of the file to read the data from.
		 * @param sMsgError QString to receive the error message in case the method fails.
		 * @return Boolean indicating if the loading was successful (true) of failed (false).
		 */
		bool loadFromFile(const QString &sFileName, QString &sMsgError);

        /**
         * Saves (serializes) the instance to the given file in the YAML format
		 * (YAML Ain't Markup Language - http://en.wikipedia.org/wiki/YAML).
         * @param sFileName QString with the name of the file to write the data to.
		 * @param sMsgError QString to receive the error message in case the method fails.
		 * @return Boolean indicating if the saving was successful (true) of failed (false).
         */
        bool saveToFile(const QString &sFileName, QString &sMsgError) const;

        /**
         * Clear the face annotation dataset.
         */
        void clear();

		/**
		 * Gets the face image for the given index. The index must be in the range [0, count - 1],
		 * where count is the number of face images in the dataset.
		 * @param iIndex Integer with the index of the image file to load.
		 * @param oImage QPixmap reference for receiving the image loaded.
		 * @return Boolean indicating if the image file could be loaded (true) or not (false, in case
		 * the given index is out of range or the image file could not be read).
		 */
		bool getImage(const int iIndex, QPixmap &oImage) const;

		/**
		 * Gets the face image for the given index. The index must be in the range [0, count - 1],
		 * where count is the number of face images in the dataset.
		 * @param iIndex Integer with the index of the image file to load.
		 * @param oImage OpenCV's Mat reference for receiving the image loaded.
		 * @return Boolean indicating if the image file could be loaded (true) or not (false, in case
		 * the given index is out of range or the image file could not be read).
		 */
		bool getImage(const int iIndex, cv::Mat &oImage) const;

		/**
		 * Adds a new image to the face annotation dataset. All other data (landmarks, connections, etc)
		 * are created with default values.
		 * @param sFileName QString with the path and filename of the image file to be added to the dataset.
		 * @return Boolean indicating if the image was added (true) or not (false, in case it already exists in
		 * the dataset).
		 */
		bool addImage(const QString &sFileName);

		/**
		 * Removes an image from the face annotation dataset. All other data (landmarks, connections, etc)
		 * are also removed.
		 * @param iIndex Integer with the index of the image to remove.
		 * @return Boolean indicating if the image was removed (true) or not (false, in case
		 * the given index is out of range).
		 */
		bool removeImage(const int iIndex);

		/**
		 * Gets the name of the file for the image at the given index. The index must be in
		 * the range [0, count - 1], where count is the number of face images in the dataset.
		 * @param iIndex Integer with the index of the image file from which to get the file name.
		 * @param sFileName QString reference for receiving the image file name.
		 * @return Boolean indicating if the image file name could be obtained (true) or not (false, in case
		 * the given index is out of range).
		 */
		bool getImageFileName(const int iIndex, QString &sFileName) const;

		/**
		 * Gets the emotion label of the sample at the given index. The index must be in
		 * the range [0, count - 1], where count is the number of face images in the dataset.
		 * @param iIndex Integer with the index of sample from which to get the emotion label.
		 * @param eLabel EmotionLabel reference to receive the emotion label for the given image.
		 * @return Boolean indicating if the method call was successful (true) or not (false, in case
		 * the given index is out of range).
		 */
		bool getEmotionLabel(const int iIndex, EmotionLabel &eLabel) const;

	private:
		/** Names of the face image files. */
		std::vector<QString> m_vFaceImages;

        /** 2D coordinates of the facial features in each image sample. */
        std::vector< std::vector<cv::Point2f> > m_vFeaturePoints;

		/** Emotion labels for each image sample. */
		std::vector<EmotionLabel> m_vEmotionLabels;

        /** Information about the symmetry of facial features. */
        std::vector<int> m_vFeatureSymmetry;

        /** Information about the connectivity between the facial features. */
        std::vector<cv::Vec2i> m_vFeatureConnectivity;
	};

}

#endif // FACEDATASET_H
