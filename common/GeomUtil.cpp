#include "GeomUtil.h"


Rect::Rect(int nLeft, int nTop, int nRight, int nBottom)
{
	m_nLeft = nLeft; 
	m_nTop = nTop; 
	m_nRight = nRight; 
	m_nBottom = nBottom;
}

bool Rect::PtInRectangle(Point pt)
{
	if (pt.x() >= m_nLeft && pt.x() < m_nRight &&
		pt.x() >= m_nTop && pt.x() < m_nBottom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Rect::IntersectRect(Rect *pRect1, Rect *pRect2)
{
	int nMaxLeft = 0;
	int nMaxTop = 0;
	int nMinRight = 0;
	int nMinBottom = 0;

	if (pRect1->m_nLeft >= pRect2->m_nLeft)
	{
		nMaxLeft = pRect1->m_nLeft;
	}
	else
	{
		nMaxLeft = pRect2->m_nLeft;
	}

	if (pRect1->m_nTop >= pRect2->m_nTop)
	{
		nMaxTop = pRect1->m_nTop;
	}
	else
	{
		nMaxTop = pRect2->m_nTop;
	}

	if (pRect1->m_nRight <= pRect2->m_nRight)
	{
		nMinRight = pRect1->m_nRight;
	}
	else
	{
		nMinRight = pRect2->m_nRight;
	}

	if (pRect1->m_nBottom <= pRect2->m_nBottom)
	{
		nMinBottom = pRect1->m_nBottom;
	}
	else
	{
		nMinBottom = pRect2->m_nBottom;
	}

	if (nMaxLeft >= nMinRight || nMaxTop >= nMinBottom)
	{
		return false;
	}
	else
	{
		return true;
	}
}
