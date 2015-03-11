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

#ifndef EMOTIONLABEL_H
#define EMOTIONLABEL_H

#include "core_global.h"

#include <vector>

namespace f3
{
	/**
	 * Represents the different prototypical emotional labels.
	 */
	class CORE_EXPORT EmotionLabel
	{
	protected:
		/**
		 * Base constructor. Protected to avoid being used outside the escope of this class.
		 * @param iLabel Integer value with the emotion label.
		 */
		EmotionLabel(const int iLabel);

	public:

		/**
		 * Default constructor. Initializes the label as UNDEFINED.
		 */
		EmotionLabel();

		/**
		 * Copy constructor.
		 * @param oOther Reference to the other EmotionLabel from which to copy the data.
		 */
		EmotionLabel(const EmotionLabel &oOther);

		/**
		 * Assign operator.
		 * @param oOther Reference to the other EmotionLabel from which to copy the data.
		 * @return Reference to this object after copying the data fro the other object.
		 */
		EmotionLabel& operator=(const EmotionLabel &oOther);

		/**
		 * Equal-comparison operator.
		 * @param oOther Reference to the other EmotionLabel to be copared with.
		 * @return True if the objects are equal, and false otherwise.
		 */
		inline bool operator==(const EmotionLabel &oOther) const;

		/**
		 * Difference-comparison operator.
		 * @param oOther Reference to the other EmotionLabel to be copared with.
		 * @return True if the objects are different, and false otherwise.
		 */
		inline bool operator!=(const EmotionLabel &oOther) const;

		/**
		 * Less-then-comparison operator.
		 * @param oOther Reference to the other EmotionLabel to be copared with.
		 * @return True if this object is less than the other, and false otherwise.
		 */
		inline bool operator<(const EmotionLabel &oOther) const;

		/**
		 * Greater-then-comparison operator.
		 * @param oOther Reference to the other EmotionLabel to be copared with.
		 * @return True if this object is greater than the other, and false otherwise.
		 */
		inline bool operator>(const EmotionLabel &oOther) const;

		/**
		 * Less-or-equal-then-comparison operator.
		 * @param oOther Reference to the other EmotionLabel to be copared with.
		 * @return True if this object is less or equal than the other, and false otherwise.
		 */
		inline bool operator<=(const EmotionLabel &oOther) const;

		/**
		 * Greater-or-equal-then-comparison operator.
		 * @param oOther Reference to the other EmotionLabel to be copared with.
		 * @return True if this object is greater or equal than the other, and false otherwise.
		 */
		inline bool operator>=(const EmotionLabel &oOther) const;

		/**
		 * Gets a string representation of the emotion label in the currently set idiom.
		 * @return String representation of the emotion label.
		 */
		QString getName() const;

		/**
		 * Gets an integer representation of the emotion label.
		 * @return Integer representation of the emotion label.
		 */
		int getValue() const;

		/**
		 * Queries the existing emotion labels.
		 * @return Vector with the existing emotion labels.
		 */
		static std::vector<EmotionLabel> getLabels();

		/**
		 * Creates a emotion label object from the given index value.
		 * @param iIndex Integer with the index representing the emotion.
		 * @return Object of EmotionLabel representing the emotion.
		 */
		static EmotionLabel fromValue(const int iIndex);

	public:
		/** Indicates an undefined emotion label. */
		const static EmotionLabel UNDEFINED;

		/** Indicates the expression of anger. */
		const static EmotionLabel ANGER;

		/** Indicates the expression of contempt. */
		const static EmotionLabel CONTEMPT;

		/** Indicates the expression of disgust. */
		const static EmotionLabel DISGUST;

		/** Indicates the expression of fear. */
		const static EmotionLabel FEAR;

		/** Indicates the expression of happiness. */
		const static EmotionLabel HAPPINESS;

		/** Indicates the expression of sadness. */
		const static EmotionLabel SADNESS;

		/** Indicates the expression of surprise. */
		const static EmotionLabel SURPRISE;

	private:
		/**
		 * Integer index representing the emotional label.
		 * The indexes are defined to follow the labels used in the Cohn-Kanade
		 * (CK and CK+) database (http://www.pitt.edu/~emotion/ck-spread.htm).
		 */
		int m_iLabel;
	};
};

#endif //EMOTIONLABEL_H