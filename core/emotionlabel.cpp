#include "emotionlabel.h"
#include <QApplication>
#include <QDebug>

using namespace std;

/*
 * Setup the constant labels based on the indexes used in the Cohn-Kanade
 * (CK and CK+) database (http://www.pitt.edu/~emotion/ck-spread.htm).
 */
const f3::EmotionLabel f3::EmotionLabel::UNDEFINED = f3::EmotionLabel(-1);
const f3::EmotionLabel f3::EmotionLabel::ANGER     = f3::EmotionLabel(1);
const f3::EmotionLabel f3::EmotionLabel::CONTEMPT  = f3::EmotionLabel(2);
const f3::EmotionLabel f3::EmotionLabel::DISGUST   = f3::EmotionLabel(3);
const f3::EmotionLabel f3::EmotionLabel::FEAR      = f3::EmotionLabel(4);
const f3::EmotionLabel f3::EmotionLabel::HAPPINESS = f3::EmotionLabel(5);
const f3::EmotionLabel f3::EmotionLabel::SADNESS   = f3::EmotionLabel(6);
const f3::EmotionLabel f3::EmotionLabel::SURPRISE  = f3::EmotionLabel(7);

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
        return qApp->translate("EmotionLabel", "raiva");
    else if(*this == CONTEMPT)
        return qApp->translate("EmotionLabel", "desprezo");
    else if(*this == DISGUST)
        return qApp->translate("EmotionLabel", "nojo");
    else if(*this == FEAR)
        return qApp->translate("EmotionLabel", "medo");
    else if(*this == HAPPINESS)
        return qApp->translate("EmotionLabel", "felicidade");
    else if(*this == SADNESS)
        return qApp->translate("EmotionLabel", "tristeza");
    else if(*this == SURPRISE)
        return qApp->translate("EmotionLabel", "surpresa");
    else
        return qApp->translate("EmotionLabel", "indefinido");
}

// +-----------------------------------------------------------
int f3::EmotionLabel::getValue() const
{
    return m_iLabel;
}

// +-----------------------------------------------------------
vector<f3::EmotionLabel> f3::EmotionLabel::getLabels()
{
    vector<f3::EmotionLabel> vRet;

    vRet.push_back(ANGER);
    vRet.push_back(CONTEMPT);
    vRet.push_back(DISGUST);
    vRet.push_back(FEAR);
    vRet.push_back(HAPPINESS);
    vRet.push_back(SADNESS);
    vRet.push_back(SURPRISE);

    return vRet;
}

// +-----------------------------------------------------------
f3::EmotionLabel f3::EmotionLabel::fromValue(const int iIndex)
{
    if(iIndex >= ANGER.getValue() && iIndex <= SURPRISE.getValue())
        return f3::EmotionLabel(iIndex);
    else
        return UNDEFINED;
}
