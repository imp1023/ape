#ifndef _POLYUTIL_H_
#define _POLYUTIL_H_
#pragma once

#ifndef PI
const double PI					= 3.141592653589793;
#endif




class Point
{
public:
	Point(){ x_ = 0; y_ = 0;};
    Point(int x, int y) {x_ = x; y_ = y;}
	~Point(){};

    int x() const { return x_; }
    void setX(int val) { x_ = val; }
    int y() const { return y_; }
    void setY(int val) { y_ = val; }

private:
	int x_;
	int y_; 
};

class Rect
{
public:
	Rect(){m_nLeft = 0; m_nTop = 0; m_nRight = 0; m_nBottom = 0;}
	Rect(int nLeft, int nTop, int nRight, int nBottom);
	~Rect(){};

	int		Left() const { return m_nLeft; }
	void	Left(int val) { m_nLeft = val; }

	int		Top() const { return m_nTop; }
	void	Top(int val) { m_nTop = val; }

	int		Right() const { return m_nRight; }
	void	Right(int val) { m_nRight = val; }

	int		Bottom() const { return m_nBottom; }
	void	Bottom(int val) { m_nBottom = val; }

	int		Width()
	{
		return m_nRight - m_nLeft;
	}
	int		Height()
	{
		return m_nBottom - m_nTop;
	}


	bool	PtInRectangle(Point pt);
	static bool IntersectRect(Rect *pRect1, Rect *pRect2);

protected:
	int		m_nLeft;
	int		m_nTop;
	int		m_nRight;
	int		m_nBottom;
};

#endif
