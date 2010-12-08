/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.0 Alpha                                               *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Common.                                   *
*                                                                            *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  PrimeSense Sensor is distributed in the hope that it will be useful,      *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>. *
*                                                                            *
*****************************************************************************/






#ifndef _XNV_3DVECTOR_H_
#define _XNV_3DVECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVPoint.h>
#include "XnVMathCommon.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XN_EE_FW_API XnV3DVector : public XnVPoint
{
public:
	// Constructors
	inline XnV3DVector();
	inline XnV3DVector(XnFloat fX, XnFloat fY, XnFloat fZ);
	inline XnV3DVector(const XnVPoint& ptOther);
	inline XnV3DVector(const XnV3DVector& v3Other);
	inline XnV3DVector(XnFloat f);
	inline XnV3DVector(XnFloat* pf);

	// Assignment
	inline XnV3DVector& operator=(const XnV3DVector& v3Rhs);
	inline XnV3DVector& operator=(const XnVPoint& ptRhs);

	// Access
	inline XnFloat operator[](XnUInt32 index) const;
	inline XnFloat& operator[](XnUInt32 index);

	// Set
	inline XnV3DVector& Set(const XnV3DVector& v3Other);
	inline XnV3DVector& Set(const XnVPoint& ptOther);
	inline XnV3DVector& Set(XnFloat fX, XnFloat fY, XnFloat fZ);
	inline XnV3DVector& Set(XnFloat f);

	// Zero
	inline XnV3DVector& SetZero();
	inline XnBool IsZero() const;

	// Comparison
	inline XnBool operator==(const XnV3DVector& v3Rhs) const;
	inline XnBool operator!=(const XnV3DVector& v3Rhs) const;

	// Negation
	inline XnV3DVector operator-() const;
	inline XnV3DVector& Negate(const XnV3DVector& v3Other);
	inline XnV3DVector& Negate();

	// Multiplication
	inline XnV3DVector operator*(const XnV3DVector& v3Rhs) const;
	inline XnV3DVector& operator*=(const XnV3DVector& v3Rhs);
	inline XnV3DVector& Multiply(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs);

	friend inline XnV3DVector operator*(XnFloat f, const XnV3DVector& v3Rhs);
	inline XnV3DVector& Multiply(XnFloat f, const XnV3DVector& v3Rhs);

	inline XnV3DVector operator*(XnFloat f) const;
	inline XnV3DVector& operator*=(XnFloat f);
	inline XnV3DVector& Multiply(const XnV3DVector& v3Lhs, XnFloat f);

	// Division
	inline XnV3DVector operator/(XnFloat f) const;
	inline XnV3DVector& operator/=(XnFloat f);
	inline XnV3DVector& Divide(const XnV3DVector& v3Lhs, XnFloat f);

	// Addition
	inline XnV3DVector operator+(const XnV3DVector& v3Rhs) const;
	inline XnV3DVector& operator+=(const XnV3DVector& v3Rhs);
	inline XnV3DVector& Add(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs);

	inline XnV3DVector operator+(XnFloat f) const;
	inline XnV3DVector& operator+=(XnFloat f);
	inline XnV3DVector& Add(const XnV3DVector& v3Lhs, XnFloat f);

	// Subtraction
	inline XnV3DVector operator-(const XnV3DVector& v3Rhs) const;
	inline XnV3DVector& operator-=(const XnV3DVector& v3Rhs);
	inline XnV3DVector& Subtract(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs);

	inline XnV3DVector operator-(XnFloat f) const;
	inline XnV3DVector& operator-=(XnFloat f);
	inline XnV3DVector& Subtract(const XnV3DVector& v3Lhs, XnFloat f);

	inline XnV3DVector& Sqrt();
	inline XnV3DVector& Sqrt(const XnV3DVector& v3Other);

	// Magnitude
	inline XnFloat Magnitude() const;
	inline XnFloat MagnitudeSquared() const;

	// Distance
	inline XnFloat Distance(const XnV3DVector& v3Other) const;
	inline XnFloat DistanceSquared(const XnV3DVector& v3Other) const;

	// Normalize
	inline XnFloat Normalize();

	// Orthogonal
	inline XnV3DVector& OrthogonalVector(const XnV3DVector& v3Other);
	inline XnV3DVector& UnitOrthogonalVector(const XnV3DVector& v3Other);

	// Cross product
	inline XnV3DVector operator^(const XnV3DVector& v3Rhs) const;
	inline XnV3DVector& CrossProduct(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs);

	// Dot Product
	inline XnFloat operator|(const XnV3DVector& v3Rhs) const;
	friend inline XnFloat DotProduct(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs);

	// Interpolation
	inline XnV3DVector& Interpolate(const XnV3DVector& v3Vec1, const XnV3DVector& v3Vec2, XnFloat fAlpha);

	inline XnBool IsSameDirection(const XnV3DVector& v3Other) const;

	inline XnFloat GetTolerance() const;
	inline void SetTolerance(XnFloat fTolerance);

protected:
	XnFloat m_fTolerance;
};

XnFloat XnV3DVector::GetTolerance() const
{
	return m_fTolerance;
}

void XnV3DVector::SetTolerance(XnFloat fTolerance)
{
	m_fTolerance = fTolerance;
}

XnV3DVector::XnV3DVector() :
	m_fTolerance(1e-5f)
{
	Set(0);
}
XnV3DVector::XnV3DVector(XnFloat fX, XnFloat fY, XnFloat fZ) :
	m_fTolerance(1e-5f)
{
	Set(fX, fY, fZ);
}
XnV3DVector::XnV3DVector(const XnV3DVector& v3Other)
{
	Set(v3Other);
}
XnV3DVector::XnV3DVector(const XnVPoint& ptOther)
{
	Set(ptOther);
}
XnV3DVector::XnV3DVector(XnFloat f) :
	m_fTolerance(1e-5f)
{
	Set(f);
}
XnV3DVector::XnV3DVector(XnFloat* pf) :
	m_fTolerance(1e-5f)
{
	Set(pf[0], pf[1], pf[2]);
}

// Assignment
XnV3DVector& XnV3DVector::operator=(const XnV3DVector& v3Rhs)
{
	m_fTolerance = v3Rhs.m_fTolerance;
	return Set(v3Rhs.X(), v3Rhs.Y(), v3Rhs.Z());
}
XnV3DVector& XnV3DVector::operator=(const XnVPoint& ptRhs)
{
	m_fTolerance = 1e-5f;
	return Set(ptRhs.X(), ptRhs.Y(), ptRhs.Z());
}

// Access
XnFloat XnV3DVector::operator[](XnUInt32 nIndex) const
{
	if (nIndex > 2)
		return elements[3];
	return elements[nIndex];
}
XnFloat& XnV3DVector::operator[](XnUInt32 nIndex)
{
	if (nIndex > 2)
		return elements[3];
	return elements[nIndex];
}

// Set
XnV3DVector& XnV3DVector::Set(const XnV3DVector& v3Other)
{
	return *this = v3Other;
}
XnV3DVector& XnV3DVector::Set(const XnVPoint& ptOther)
{
	return *this = ptOther;
//	return Set(ptOther.X(), ptOther.Y(), ptOther.Z())
}
XnV3DVector& XnV3DVector::Set(XnFloat x, XnFloat y, XnFloat z)
{
	SetPoint(x, y, z);

	return *this;
}
XnV3DVector& XnV3DVector::Set(XnFloat f)
{
	return Set(f, f, f);
}

// Zero
XnV3DVector& XnV3DVector::SetZero()
{
	return Set(0);
}
XnBool XnV3DVector::IsZero() const
{
	return (fabs(X()) < m_fTolerance &&
			fabs(Y()) < m_fTolerance &&
			fabs(Z()) < m_fTolerance);
}

// Comparison
XnBool XnV3DVector::operator==(const XnV3DVector& v3Rhs) const
{
	return (fabs(X() - v3Rhs.X()) < m_fTolerance &&
			fabs(Y() - v3Rhs.Y()) < m_fTolerance &&
			fabs(Z() - v3Rhs.Z()) < m_fTolerance);
}
XnBool XnV3DVector::operator!=(const XnV3DVector& v3Rhs) const
{
	return !this->operator==(v3Rhs);
}

// Negation
XnV3DVector XnV3DVector::operator-() const
{
	return XnV3DVector(-X(), -Y(), -Z());
}
XnV3DVector& XnV3DVector::Negate(const XnV3DVector& v3Other)
{
	return Set(-v3Other.X(), -v3Other.Y(), -v3Other.Z());
}
XnV3DVector& XnV3DVector::Negate()
{
	return Set(-X(), -Y(), -Z());
}

// Multiplication
XnV3DVector XnV3DVector::operator*(const XnV3DVector& v3Rhs) const
{
	return XnV3DVector(X() * v3Rhs.X(), Y() * v3Rhs.Y(), Z()*v3Rhs.Z());
}
XnV3DVector& XnV3DVector::operator*=(const XnV3DVector& v3Rhs)
{
	return Set(X() * v3Rhs.X(), Y() * v3Rhs.Y(), Z() * v3Rhs.Z());
}
XnV3DVector& XnV3DVector::Multiply(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs)
{
	return Set(v3Lhs.X() * v3Rhs.X(), v3Lhs.Y() * v3Rhs.Y(), v3Lhs.Z() * v3Rhs.Z());
}

XnV3DVector operator*(XnFloat f, const XnV3DVector& v3Rhs)
{
	return XnV3DVector(f * v3Rhs.X(), f * v3Rhs.Y(), f * v3Rhs.Z());
}
XnV3DVector& XnV3DVector::Multiply(XnFloat f, const XnV3DVector& v3Rhs)
{
	return Set(f * v3Rhs.X(), f * v3Rhs.Y(), f * v3Rhs.Z());
}

XnV3DVector XnV3DVector::operator*(XnFloat f) const
{
	return XnV3DVector(X() * f, Y() * f, Z() * f);
}
XnV3DVector& XnV3DVector::operator*=(XnFloat f)
{
	return Set(X() * f, Y() * f, Z() * f);
}
XnV3DVector& XnV3DVector::Multiply(const XnV3DVector& v3Lhs, XnFloat f)
{
	return Set(v3Lhs.X() * f, v3Lhs.Y() * f, v3Lhs.Z() * f);
}

// Division
XnV3DVector XnV3DVector::operator/(XnFloat f) const
{
	return XnV3DVector(X() / f, Y() / f, Z() / f);
}
XnV3DVector& XnV3DVector::operator/=(XnFloat f)
{
	return Set(X() / f, Y() / f, Z() / f);
}
XnV3DVector& XnV3DVector::Divide(const XnV3DVector& v3Lhs, XnFloat f)
{
	return Set(v3Lhs.X() / f, v3Lhs.Y() / f, v3Lhs.Z() / f);
}

// Addition
XnV3DVector XnV3DVector::operator+(const XnV3DVector& v3Rhs) const
{
	return XnV3DVector(X() + v3Rhs.X(), Y() + v3Rhs.Y(), Z() + v3Rhs.Z());
}
XnV3DVector& XnV3DVector::operator+=(const XnV3DVector& v3Rhs)
{
	return Set(X() + v3Rhs.X(), Y() + v3Rhs.Y(), Z() + v3Rhs.Z());
}
XnV3DVector& XnV3DVector::Add(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs)
{
	return Set(v3Lhs.X() + v3Rhs.X(), v3Lhs.Y() + v3Rhs.Y(), v3Lhs.Z() + v3Rhs.Z());
}

XnV3DVector XnV3DVector::operator+(XnFloat f) const
{
	return XnV3DVector(X() + f, Y() + f, Z() + f);
}
XnV3DVector& XnV3DVector::operator+=(XnFloat f)
{
	return Set(X() + f, Y() + f, Z() + f);
}
XnV3DVector& XnV3DVector::Add(const XnV3DVector& v3Lhs, XnFloat f)
{
	return Set(v3Lhs.X() + f, v3Lhs.Y() + f, v3Lhs.Z() + f);
}

// Subtraction
XnV3DVector XnV3DVector::operator-(const XnV3DVector& v3Rhs) const
{
	return XnV3DVector(X() - v3Rhs.X(), Y() - v3Rhs.Y(), Z() - v3Rhs.Z());
}
XnV3DVector& XnV3DVector::operator-=(const XnV3DVector& v3Rhs)
{
	return Set(X() - v3Rhs.X(), Y() - v3Rhs.Y(), Z() - v3Rhs.Z());
}
XnV3DVector& XnV3DVector::Subtract(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs)
{
	return Set(v3Lhs.X() - v3Rhs.X(), v3Lhs.Y() - v3Rhs.Y(), v3Lhs.Z() - v3Rhs.Z());
}

XnV3DVector XnV3DVector::operator-(XnFloat f) const
{
	return XnV3DVector(X() - f, Y() - f, Z() - f);
}
XnV3DVector& XnV3DVector::operator-=(XnFloat f)
{
	return Set(X() - f, Y() - f, Z() - f);
}
XnV3DVector& XnV3DVector::Subtract(const XnV3DVector& v3Lhs, XnFloat f)
{
	return Set(v3Lhs.X() - f, v3Lhs.Y() - f, v3Lhs.Z() - f);
}

XnV3DVector& XnV3DVector::Sqrt()
{
	return Set(sqrt(X()), sqrt(Y()), sqrt(Z()));
}

XnV3DVector& XnV3DVector::Sqrt(const XnV3DVector& v3Other)
{
	Set(v3Other);
	return Sqrt();
}

// Magnitude
XnFloat XnV3DVector::Magnitude() const
{
	return sqrt(MagnitudeSquared());
}
XnFloat XnV3DVector::MagnitudeSquared() const
{
	return X() * X() +
		   Y() * Y() +
		   Z() * Z();
}

// Distance
XnFloat XnV3DVector::Distance(const XnV3DVector& v3Other) const
{
	return sqrt(DistanceSquared(v3Other));
}
XnFloat XnV3DVector::DistanceSquared(const XnV3DVector& v3Other) const
{
	return (*this-v3Other).MagnitudeSquared();
}

// Normalize
XnFloat XnV3DVector::Normalize()
{
	XnFloat fLen = Magnitude();
	if (fLen > m_fTolerance)
		*this /= fLen;
	else
		Set(1, 0, 0);

	return fLen;
}

// Orthogonal
XnV3DVector& XnV3DVector::OrthogonalVector(const XnV3DVector& v3Other)
{
		XnFloat abs_x = fabs(v3Other.X()), abs_y = fabs(v3Other.Y()), abs_z = fabs(v3Other.Z());

		if (abs_x < abs_y)
			if (abs_x < abs_z)
				Set(0, v3Other.Z(), -v3Other.Y());
			else
				Set(v3Other.Y(), -v3Other.X(), 0);
		else
			if (abs_y < abs_z)
				Set(-v3Other.Z(), 0, v3Other.X());
			else
				Set(v3Other.Y(), -v3Other.X(), 0);

		return *this;
}
XnV3DVector& XnV3DVector::UnitOrthogonalVector(const XnV3DVector& v3Other)
{
		OrthogonalVector(v3Other);
		Normalize();

		return *this;
}

// Cross product
XnV3DVector XnV3DVector::operator^(const XnV3DVector& v3Rhs) const
{
	XnV3DVector v3Result;
	v3Result.CrossProduct(*this, v3Rhs);

	return v3Result;
}
XnV3DVector& XnV3DVector::CrossProduct(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs)
{
		return Set(v3Lhs.Y() * v3Rhs.Z() - v3Lhs.Z() * v3Rhs.Y(),
				   v3Lhs.Z() * v3Rhs.X() - v3Lhs.X() * v3Rhs.Z(),
				   v3Lhs.X() * v3Rhs.Y() - v3Lhs.Y() * v3Rhs.X());
}

// Dot Product
XnFloat XnV3DVector::operator|(const XnV3DVector& v3Rhs) const
{
	return DotProduct(*this, v3Rhs);
}
XnFloat DotProduct(const XnV3DVector& v3Lhs, const XnV3DVector& v3Rhs)
{
		return v3Lhs.X() * v3Rhs.X() +
			   v3Lhs.Y() * v3Rhs.Y() +
			   v3Lhs.Z() * v3Rhs.Z();
}

// Interpolation
XnV3DVector& XnV3DVector::Interpolate(const XnV3DVector& v3Vec1, const XnV3DVector& v3Vec2, XnFloat fAlpha)
{
		return Set(v3Vec1.X() + fAlpha * (v3Vec2.X() - v3Vec1.X()),
				   v3Vec1.Y() + fAlpha * (v3Vec2.Y() - v3Vec1.Y()),
				   v3Vec1.Z() + fAlpha * (v3Vec2.Z() - v3Vec1.Z()));
}
XnBool XnV3DVector::IsSameDirection(const XnV3DVector& v3Other) const
{
	if (IsZero() || v3Other.IsZero())
	{
		return true;
	}

	XnFloat fRatio = 0;

	if (!XnVMathCommon::IsZero(X(), m_fTolerance) && !XnVMathCommon::IsZero(v3Other.X(), m_fTolerance))
		fRatio = v3Other.X()/X();
	else if (!XnVMathCommon::IsZero(Y(), m_fTolerance) && !XnVMathCommon::IsZero(v3Other.Y(), m_fTolerance))
		fRatio = v3Other.Y()/Y();
	else if (!XnVMathCommon::IsZero(Z(), m_fTolerance) && !XnVMathCommon::IsZero(v3Other.Z(), m_fTolerance))
		fRatio = v3Other.Z()/Z();
	else
	{
		// Combination sof zeros, though not all zeros!
		return false;
	}

	if (v3Other/fRatio == (*this))
		return true;

	return false;
}


#endif
