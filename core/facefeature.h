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

#ifndef FACE_FEATURE_H
#define FACE_FEATURE_H

#include "core_global.h"

#include <QDomDocument>

#include <opencv2/core/core.hpp>

namespace f3
{
	/**
	 * Represents the data of a facial feature in the image dataset.
	 */
	class CORE_EXPORT FaceFeature : public cv::Point2f
    {
	public:

		/**
		 * Class constructor.
		 */
		FaceFeature();

		/**
		 * Class constructor.
		 * @param iID Integer with the feature identifier.
		 * @param x Float with the x coordinate of the face feature.
		 * @param y Float with the y coordinate of the face feature.
		 */
		FaceFeature(int iID, float x, float y);

		/**
		 * Getter of the feature ID.
		 * @return Integer with the ID of the feature.
		 */
		int getID() const;

		/**
		 * Setter of the feature ID.
		 * @param iID Integer with the new ID for the feature.
		 */
		void setID(int iID);

		/**
		 * Loads (unserializes) the face feature data from the given xml element.
		 * @param oElement QDomElement from where to read the feature data (the feature node in the xml).
		 * @param sMsgError QString to receive the error message in case the method fails.
		 * @return Boolean indicating if the loading was successful (true) or if it failed (false).
		 */
		bool loadFromXML(const QDomElement &oElement, QString &sMsgError);

        /**
         * Saves the face feature data into the given xml element.
		 * @param oParent QDomElement to receive the node of the face feature data.
         */
        void saveToXML(QDomElement &oParent) const;

	private:

		/** Identifier of the face feature. */
		int m_iID;

    };
}

#endif // FACE_FEATURE_H