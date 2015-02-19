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

#include "utils.h"

#include <QRegExp>
#include <QStringList>
#include <QMultiMap>
#include <QDir>

// +-----------------------------------------------------------
f3::Utils::Utils()
{
}

// +-----------------------------------------------------------
QString f3::Utils::shortenPath(const QString &sPath, int iMaxLen)
{
	// If the string is not long enough, simply return it
	if(sPath.length() <= iMaxLen)
		return QDir::toNativeSeparators(sPath);

	QFileInfo oFile = QFileInfo(sPath);
	QString sPathOnly = oFile.path();
	QString sFileName = QDir::separator() + oFile.fileName();
	QString sDriveLetter = ""; // In case it is running on a Windows OS

    // Firstly, split the path (only) into parts (for the drive letter and/or each subfolder)
    QRegExp oRegex("([\\\\\\/][\\w -\\.]*)");
    QStringList lsParts;
	QMultiMap<int, int> mpSortedParts;
    QString sPart;

    bool bFirst = true;
    int iPos = 0;
    while((iPos = oRegex.indexIn(sPathOnly, iPos)) != -1)
    {
        if(bFirst)
        {
            sDriveLetter = sPathOnly.left(iPos);
            bFirst = false;
        }
        sPart = oRegex.cap(1);
        lsParts.push_back(sPart);
		mpSortedParts.insert(sPart.length(), lsParts.count() - 1);
        iPos += oRegex.matchedLength();
    }

	// Then, iteratively remove the larger parts while the path is bigger than
	// the maximum number of characters desired
	QString sNewPath;
	do
	{
		sNewPath = "";

		// Rebuild the path replacing the so far larger part for "..."
		QMapIterator<int, int> oSorted(mpSortedParts);
		oSorted.toBack();
		if(oSorted.hasPrevious())
		{
			int iLength = oSorted.peekPrevious().key();
			int iIndex = oSorted.peekPrevious().value();
			mpSortedParts.remove(iLength, iIndex);

			lsParts.replace(iIndex, QDir::separator() + QString("..."));
		
			for(QStringList::iterator it = lsParts.begin(); it != lsParts.end(); ++it)
				sNewPath += *it;
		}
	} while(sNewPath.length() > 0 && QString(sDriveLetter + sNewPath + sFileName).length() > iMaxLen);

	if(sNewPath.length() == 0)
		sNewPath = QDir::separator() + QString("...");

	return QDir::toNativeSeparators(sDriveLetter + sNewPath + sFileName);
}
