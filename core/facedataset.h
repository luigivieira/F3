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

#include <opencv2/core/core.hpp>

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
		int count() const;

		/**
		 * Loads (unserializes) the instance from the given text file in the YAML format
		 * (YAML Ain't Markup Language - http://en.wikipedia.org/wiki/YAML).
		 * This method throws exceptions indicating different errors:
		 *   - invalid_argument: if the given file does not exist or if it can not be opened or read
		 *   - runtime_error: if there is an error in the expected file format
		 * @param sFileName Char pointer with the name of the file to read the data from.
		 */
		void loadFromFile(const char* sFileName);

        /**
         * Saves (serializes) the instance to the given file in the YAML format
		 * (YAML Ain't Markup Language - http://en.wikipedia.org/wiki/YAML).
		 * This method throws exceptions indicating different errors:
		 *   - invalid_argument: if the given file can not be created or opened to write
         * @param sFileName Char pointer with the name of the file to write the data to.
         */
        void saveToFile(const char* sFileName) const;

        /**
         * @brief Clear the training data.
         */
        void clear();

		/**
		 * Gets the face image data (OpenCV Matrix) for the given index. The index must be in
		 * the range [0, count - 1], where count is the number of face images in the dataset.
		 * If the given index is out of this range, the method throws the invalid_argument exception.
		 * @param iIndex Integer with the index of the image file to load.
		 * @return OpenCV Matrix (cv::Mat) with the image data loaded from the file at the given index.
		 */
		cv::Mat getImage(const int iIndex) const;

        /**
         * Adds the given samples to training set.
         * @param vSamples Vector of strings with the paths of the sample files.
         * @param vFeatures Vector of cv::Point2f vectors with the feature points for each sample file.
         */
        void addSamples(std::vector<std::string> vSamples, std::vector< std::vector<cv::Point2f> > vFeatures);

        /**
         * Gets the name and landmarks' coordinates from the given sample index.
         * @param iIndex Integer with the index (0-based) of the sample to get.
         * @param sName Reference to a string to receive the sample file name.
         * @param vLandmarks Reference to a vector of OpenCV Point2f to receive the sample landmarks.
         * @return True if data was correctly obtained, false in case of error.
         */
        bool getSample(const int iIndex, std::string &sName, std::vector<cv::Point2f> &vLandmarks);

	private:
		/** Names of the face image files. */
		std::vector<std::string> m_vFaceImages;

        /** Information about the symmetry of facial features. */
        std::vector<int> m_vFeatureSymmetry;

        /** Information about the connectivity between the facial features. */
        std::vector<cv::Vec2i> m_vFeatureConnectivity;

        /** 2D coordinates of the facial features in each image sample. */
        std::vector< std::vector<cv::Point2f> > m_vFeaturePoints;
	};

}

#endif // FACEDATASET_H
