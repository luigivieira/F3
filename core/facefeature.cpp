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

#include "facefeature.h"

#include <QApplication>

// +-----------------------------------------------------------
bool f3::FaceFeature::loadFromXML(const QDomElement &oElement, QString &sMsgError)
{
// Check the element name
	if(oElement.tagName() != "Feature")
	{
		sMsgError = QString(QApplication::translate("FaceFeature", "nome de nó inválido [%1] - era esperado o nome de nó '%2'").arg(oElement.tagName(), "Feature"));
		return false;
	}

	QString sValueX = oElement.attribute("x");
	if(sValueX == "")
	{
		sMsgError = QString(QApplication::translate("FaceFeature", "o atributo '%1' não existe ou tem valor inválido").arg("x"));
		return false;
	}

	QString sValueY = oElement.attribute("y");
	if(sValueY == "")
	{
		sMsgError = QString(QApplication::translate("FaceFeature", "o atributo '%1' não existe ou tem valor inválido").arg("y"));
		return false;
	}

	x = sValueX.toFloat();
	y = sValueY.toFloat();

	return true;
}

// +-----------------------------------------------------------
void f3::FaceFeature::saveToXML(QDomElement &oParent) const
{
	QDomElement oFeature = oParent.ownerDocument().createElement("Feature");
	oParent.appendChild(oFeature);

	oFeature.setAttribute("x", x);
	oFeature.setAttribute("y", y);
}
