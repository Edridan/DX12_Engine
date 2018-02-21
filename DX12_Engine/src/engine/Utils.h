#pragma once

#include <string>

const float Zero = 0.f;
const float One = 1.f;

const float DegToRad = 0.01745329238f; // retreived from google
const float RadToDeg = 57.29578f;
const float Pi = 3.14159265358f;	// taken from http://www.geom.uiuc.edu/~huberty/math5337/groupe/digits.html

// struct
struct IntVec2
{
	int x, y;
	IntVec2() :x(0), y(0) {};
	IntVec2(int i_X, int i_Y) :x(i_X), y(i_Y) {};

	// operators
	IntVec2 operator+(const IntVec2 & i_Other)
	{
		return IntVec2(x + i_Other.x, y + i_Other.y);
	}

	IntVec2 operator-(const IntVec2 & i_Other)
	{
		return IntVec2(x - i_Other.x, y - i_Other.y);
	}
};

struct Color
{
	float r, g, b;
	Color() :Color(0.f, 0.f, 0.f) {};
	Color(float i_R, float i_G, float i_B)
		:r(i_R)
		, g(i_G)
		, b(i_B)
	{}
	Color(const Color & i_Other)
		:r(i_Other.r)
		, g(i_Other.g)
		, b(i_Other.b)
	{}

	// operators
	Color & operator=(const Color & i_Other)
	{
		r = i_Other.r;
		g = i_Other.g;
		b = i_Other.b;
		return *this;
	}

	Color & operator=(const float * i_Array)
	{
		r = i_Array[0];
		g = i_Array[1];
		b = i_Array[2];
		return *this;
	}
};

namespace color
{
	const Color White(1.f, 1.f, 1.f);
	const Color Black(0.f, 0.f, 0.f);
	const Color Red(0.f, 0.f, 0.f);
	const Color Blue(0.f, 0.f, 1.f);
	const Color Green(0.f, 0.f, 0.f);
	const Color Pink(1.f, 0.13f, 0.41f);
}

// rect definition
struct Rect
{
	float Top, Right, Bottom, Left;

	// create unitary rect
	Rect()
		:Top(0.f)
		,Right(0.f)
		,Bottom(1.f)
		,Left(1.f)
	{}

	Rect(float i_Left, float i_Right, float i_Top, float i_Bottom)
		:Top(i_Top)
		,Right(i_Right)
		,Bottom(i_Bottom)
		,Left(i_Left)
	{}

	Rect & operator=(const Rect & i_Other)
	{
		Top = i_Other.Top;
		Right = i_Other.Right;
		Bottom = i_Other.Bottom;
		Left = i_Other.Left;

		return *this;
	}

	inline float Width() const	{ return Right - Left; };
	inline float Height() const { return Bottom - Top; };
};



///////////////////////////////////////////////
// Math
namespace Math
{

}

///////////////////////////////////////////////
// Files
namespace Files
{
	void	FileToWStr(std::wstring & o_Out, const char * i_Filename);
}
