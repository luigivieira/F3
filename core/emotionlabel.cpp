#include "emotionlabel.h"

#include <QApplication>
#include <QDebug>

/*
 * Setup the constant labels based on the indexes used in the Cohn-Kanade
 * (CK and CK+) database (http://www.pitt.edu/~emotion/ck-spread.htm).
 */
const f3::EmotionLabel f3::EmotionLabel::UNDEFINED = f3::EmotionLabel(0);
const f3::EmotionLabel f3::EmotionLabel::ANGER     = f3::EmotionLabel(1);
const f3::EmotionLabel f3::EmotionLabel::CONTEMPT  = f3::EmotionLabel(2);
const f3::EmotionLabel f3::EmotionLabel::DISGUST   = f3::EmotionLabel(3);
const f3::EmotionLabel f3::EmotionLabel::FEAR      = f3::EmotionLabel(4);
const f3::EmotionLabel f3::EmotionLabel::HAPPINESS = f3::EmotionLabel(5);
const f3::EmotionLabel f3::EmotionLabel::SADNESS   = f3::EmotionLabel(6);
const f3::EmotionLabel f3::EmotionLabel::SURPRISE  = f3::EmotionLabel(7);

// +-----------------------------------------------------------
f3::EmotionLabel::EmotionLabel()
{
    m_iLabel = 0;
}

// +-----------------------------------------------------------
f3::EmotionLabel::EmotionLabel(const int iLabel)
{
    m_iLabel = iLabel;
}

// +-----------------------------------------------------------
f3::EmotionLabel::EmotionLabel(const EmotionLabel &oOther)
{
    m_iLabel = oOther.m_iLabel;
}

// +-----------------------------------------------------------
f3::EmotionLabel& f3::EmotionLabel::operator=(const EmotionLabel &oOther)
{
    m_iLabel = oOther.m_iLabel;
    return *this;
}

// +-----------------------------------------------------------
bool f3::EmotionLabel::operator==(const EmotionLabel &oOther) const
{
    return m_iLabel == oOther.m_iLabel;
}

// +-----------------------------------------------------------
bool f3::EmotionLabel::operator!=(const EmotionLabel &oOther) const
{
    return m_iLabel != oOther.m_iLabel;
}

// +-----------------------------------------------------------
bool f3::EmotionLabel::operator<(const EmotionLabel &oOther) const
{
    return m_iLabel < oOther.m_iLabel;
}

// +-----------------------------------------------------------
bool f3::EmotionLabel::operator>(const EmotionLabel &oOther) const
{
    return m_iLabel > oOther.m_iLabel;
}

// +-----------------------------------------------------------
bool f3::EmotionLabel::operator<=(const EmotionLabel &oOther) const
{
    return m_iLabel <= oOther.m_iLabel;
}

// +-----------------------------------------------------------
bool f3::EmotionLabel::operator>=(const EmotionLabel &oOther) const
{
    return m_iLabel >= oOther.m_iLabel;
}

// +-----------------------------------------------------------
QString f3::EmotionLabel::getName() const
{
    if(*this == ANGER)
        return qApp->translate("EmotionLabel", "Raiva");
    else if(*this == CONTEMPT)
        return qApp->translate("EmotionLabel", "Desprezo");
    else if(*this == DISGUST)
        return qApp->translate("EmotionLabel", "Nojo");
    else if(*this == FEAR)
        return qApp->translate("EmotionLabel", "Medo");
    else if(*this == HAPPINESS)
        return qApp->translate("EmotionLabel", "Felicidade");
    else if(*this == SADNESS)
        return qApp->translate("EmotionLabel", "Tristeza");
    else if(*this == SURPRISE)
        return qApp->translate("EmotionLabel", "Surpresa");
    else
        return qApp->translate("EmotionLabel", "Indefinido");
}

// +-----------------------------------------------------------
int f3::EmotionLabel::getValue() const
{
    return m_iLabel;
}

// +-----------------------------------------------------------
QStringList f3::EmotionLabel::getLabels()
{
    QStringList lRet;

	lRet.append(UNDEFINED.getName());
    lRet.append(ANGER.getName());
    lRet.append(CONTEMPT.getName());
    lRet.append(DISGUST.getName());
    lRet.append(FEAR.getName());
    lRet.append(HAPPINESS.getName());
    lRet.append(SADNESS.getName());
    lRet.append(SURPRISE.getName());

    return lRet;
}

// +-----------------------------------------------------------
f3::EmotionLabel f3::EmotionLabel::fromValue(const int iIndex)
{
    if(iIndex >= ANGER.getValue() && iIndex <= SURPRISE.getValue())
        return f3::EmotionLabel(iIndex);
    else
        return UNDEFINED;
}
