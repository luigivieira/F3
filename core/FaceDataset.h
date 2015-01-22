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

#include <opencv2\core\core.hpp>

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
		 */
		unsigned int count() const;

		/**
		 * Loads (unserializes) the instance from the given text file in the YAML format
		 * (YAML Ain't Markup Language - http://en.wikipedia.org/wiki/YAML).
		 * This method throws the system_error exception indicating an error code
		 * (http://en.cppreference.com/w/cpp/header/system_error) in case something wrong happens:
		 *   - errc::io_error: if the file can not be opened or read
		 *   - errc::no_protocol_option: if there is an error in the expected file format
		 * @param sFileName Char pointer with the name of the file to read the data from.
		 */
		void loadFromFile(const char* sFileName);

		/**
		 * Gets the face image data (OpenCV Matrix) for the given index. The index must be in
		 * the range [0, count - 1], where count is the number of face images in the dataset.
		 * If the given index is out of this range, the method throws the system_error exception
		 * indicating the error code errc::invalid_argument (http://en.cppreference.com/w/cpp/header/system_error).
		 * @param uiIndex Unsigned integer with the index of the image file to load.
		 * @return OpenCV Matrix (cv::Mat) with the image data loaded from the file at the given index.
		 */
		cv::Mat getImage(const unsigned int uiIndex) const;

	private:

		/**
		 * Stores the name of the face image files.
		 */
		std::vector<std::string> m_vFaceImages;
	};

}

#endif // FACEDATASET_H