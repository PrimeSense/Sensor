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






#ifndef _XNV_MATRIX3X3_H_
#define _XNV_MATRIX3X3_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVMathCommon.h>
#include <XnEE/XnV3DVector.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XN_EE_FW_API XnVMatrix3X3
{
public:
	inline XnVMatrix3X3();
	inline XnVMatrix3X3(const XnVMatrix3X3& m33Other);
	inline XnVMatrix3X3(const XnV3DVector& v3Col0, const XnV3DVector& v3Col1, const XnV3DVector& v3Col2);
	inline XnVMatrix3X3(XnFloat f00, XnFloat f01, XnFloat f02,
						XnFloat f10, XnFloat f11, XnFloat f12, 
						XnFloat f20, XnFloat f21, XnFloat f22);
	inline explicit XnVMatrix3X3(XnFloat* pf);

	inline XnVMatrix3X3& operator=(const XnVMatrix3X3& m33Other);

	inline XnFloat operator()(XnUInt32 i, XnUInt32 j) const;
	inline XnFloat& operator()(XnUInt32 i, XnUInt32 j);
	inline XnFloat operator[](XnUInt32 i) const;
	inline XnFloat& operator[](XnUInt32 i);

	inline XnVMatrix3X3& SetZero();
	inline XnVMatrix3X3& Set(const XnVMatrix3X3& m33Other);
	inline XnVMatrix3X3& Set(const XnV3DVector& v3Col0, const XnV3DVector& v3Col1, const XnV3DVector& v3Col2);
	inline XnVMatrix3X3& Set(XnFloat f00, XnFloat f01, XnFloat f02,
							 XnFloat f10, XnFloat f11, XnFloat f12, 
							 XnFloat f20, XnFloat f21, XnFloat f22);
	inline XnVMatrix3X3& Set(XnFloat* pf);

	inline XnVMatrix3X3& SetRow(XnUInt32 nRow, const XnV3DVector& v3Row);
	inline XnVMatrix3X3& SetColumn(XnUInt32 nColumn, const XnV3DVector& v3Column);
	inline void GetRow(XnUInt32 nRow, XnV3DVector& v3Row) const;
	inline void GetColumn(XnUInt32 nColumn, XnV3DVector& v3Column) const;
	
	// Comparison
	inline XnBool operator==(const XnVMatrix3X3& m33Rhs) const;
	inline XnBool operator!=(const XnVMatrix3X3& m33Rhs) const;

	inline XnBool Equal(const XnVMatrix3X3& m33Other, XnFloat fTolerance = 1e-5f) const;

	inline XnVMatrix3X3& Negate(const XnVMatrix3X3& m33Other);
	inline XnVMatrix3X3& Negate();
	inline XnVMatrix3X3 operator-() const;

	inline XnVMatrix3X3 operator+(XnFloat f) const;
	inline XnVMatrix3X3& operator+=(XnFloat f);
	inline XnVMatrix3X3& Add(const XnVMatrix3X3& m33Mat, XnFloat f);

	inline XnVMatrix3X3 operator+(const XnVMatrix3X3& m33Other) const;
	inline XnVMatrix3X3& operator+=(const XnVMatrix3X3& m33Other);
	inline XnVMatrix3X3& Add(const XnVMatrix3X3& m33Mat1, const XnVMatrix3X3& m33Mat2);

	inline XnVMatrix3X3 operator-(XnFloat f) const;
	inline XnVMatrix3X3& operator-=(XnFloat f);
	inline XnVMatrix3X3& Subtract(const XnVMatrix3X3& m33Mat, XnFloat f);

	inline XnVMatrix3X3 operator-(const XnVMatrix3X3& m33Other) const;
	inline XnVMatrix3X3& operator-=(const XnVMatrix3X3& m33Other);
	inline XnVMatrix3X3& Subtract(const XnVMatrix3X3& m33Mat1, const XnVMatrix3X3& m33Mat2);

	inline XnVMatrix3X3 operator/(XnFloat f) const;
	inline XnVMatrix3X3& operator/=(XnFloat f);
	inline XnVMatrix3X3& Divide(const XnVMatrix3X3& m33Mat, XnFloat f);

	inline XnVMatrix3X3 operator*(XnFloat f) const;
	inline XnVMatrix3X3& operator*=(XnFloat f);
	inline XnVMatrix3X3& Multiply(const XnVMatrix3X3& m33Mat, XnFloat f);

	friend inline XnVMatrix3X3 operator*(XnFloat f, const XnVMatrix3X3& m33Mat);
	inline XnVMatrix3X3& Multiply(XnFloat f, const XnVMatrix3X3& m33Mat);

	inline XnVMatrix3X3 operator*(const XnVMatrix3X3& m33Other) const;
	inline XnVMatrix3X3& operator*=(const XnVMatrix3X3& m33Other);
	inline XnVMatrix3X3& Multiply(const XnVMatrix3X3& m33Mat1, const XnVMatrix3X3& m33Mat2);

	inline XnV3DVector operator*(const XnV3DVector& v3Vec) const;
	inline void Multiply(const XnV3DVector& v3Src, XnV3DVector& v3Dst) const;

	inline XnFloat InnerProduct(const XnV3DVector& v3Vec1, const XnV3DVector& v3Vec2) const;

	inline XnVMatrix3X3& SetOuterProduct(const XnV3DVector& v3Vec);
	inline XnVMatrix3X3& SetOuterProduct(const XnV3DVector& v3Vec1, const XnV3DVector& v3Vec2);

	inline XnFloat Determinant() const;
	inline XnFloat Trace() const;
	
	inline void SetDiagonal(const XnV3DVector& v3Diag);
	inline void GetDiagonal(XnV3DVector& v3Diag) const;

	inline XnVMatrix3X3 operator~() const;
	inline XnVMatrix3X3& Transpose();
	inline XnVMatrix3X3& Transpose(const XnVMatrix3X3& m33Other);

	inline void CofactorMatrix(XnVMatrix3X3& m33CoMat) const;
	inline XnStatus LargestColumnNormalized(XnV3DVector& v3Col) const;

	inline XnVMatrix3X3& Inverse(const XnVMatrix3X3& m33Other);

	inline XnVMatrix3X3& SetIdentity();

	// Radians
	inline XnVMatrix3X3& SetXRotation(XnFloat fXAngle);
	inline XnVMatrix3X3& SetYRotation(XnFloat fYAngle);
	inline XnVMatrix3X3& SetZRotation(XnFloat fZAngle);

	inline XnStatus GetSymmetricEigenValues(XnV3DVector& v3Values) const;
	inline XnStatus GetSymmetricEigenVectors(const XnV3DVector &v3Lambda,
											XnVMatrix3X3& m3Vectors, XnFloat fTolerance=1e-8f) const;
	inline XnStatus SolveSymmetricEigenProblem(XnV3DVector& v3EigenValues, XnVMatrix3X3& m33EigenVectors, XnFloat fTolerance=1e-4f) const;

	inline XnBool IsSymmetric(XnFloat fTolerance = 1e-5f) const;

protected:
	XnFloat m_fElements[9];
};

XnVMatrix3X3::XnVMatrix3X3()
{
	SetZero();
}
XnVMatrix3X3::XnVMatrix3X3(const XnVMatrix3X3& m33Other)
{
	Set(m33Other);
}
XnVMatrix3X3::XnVMatrix3X3(const XnV3DVector& v3Col0, const XnV3DVector& v3Col1, const XnV3DVector& v3Col2)
{
	Set(v3Col0, v3Col1, v3Col2);
}
XnVMatrix3X3::XnVMatrix3X3(XnFloat f00, XnFloat f01, XnFloat f02, 
						   XnFloat f10, XnFloat f11, XnFloat f12,
						   XnFloat f20, XnFloat f21, XnFloat f22)
{
	Set(f00, f01, f02,
		f10, f11, f12,
		f20, f21, f22);
}

XnVMatrix3X3::XnVMatrix3X3(XnFloat* pf)
{
	Set(pf);
}

XnVMatrix3X3& XnVMatrix3X3::operator=(const XnVMatrix3X3& m33Other)
{
	return Set(m33Other);
}

XnFloat XnVMatrix3X3::operator()(XnUInt32 i, XnUInt32 j) const
{
	return m_fElements[i * 3 + j];
}
XnFloat& XnVMatrix3X3::operator()(XnUInt32 i, XnUInt32 j)
{
	return m_fElements[i * 3 + j];
}

XnFloat XnVMatrix3X3::operator[](XnUInt32 i) const
{
	return m_fElements[i];
}
XnFloat& XnVMatrix3X3::operator[](XnUInt32 i)
{
	return m_fElements[i];
}

XnVMatrix3X3& XnVMatrix3X3::SetZero()
{
	return Set (0, 0, 0,
				0, 0, 0,
				0, 0, 0);
}

XnVMatrix3X3& XnVMatrix3X3::Set(const XnVMatrix3X3& m33Other)
{
	return Set (m33Other(0, 0), m33Other(0, 1), m33Other(0, 2),
				m33Other(1, 0), m33Other(1, 1), m33Other(1, 2),
				m33Other(2, 0), m33Other(2, 1), m33Other(2, 2));
}
XnVMatrix3X3& XnVMatrix3X3::Set(const XnV3DVector& v3Col0, const XnV3DVector& v3Col1, const XnV3DVector& v3Col2)
{
	return Set (v3Col0[0], v3Col1[0], v3Col2[0],
				v3Col0[1], v3Col1[1], v3Col2[1],
				v3Col0[2], v3Col1[2], v3Col2[2]);
}

XnVMatrix3X3& XnVMatrix3X3::Set(XnFloat f00, XnFloat f01, XnFloat f02, 
								XnFloat f10, XnFloat f11, XnFloat f12, 
								XnFloat f20, XnFloat f21, XnFloat f22)
{
	m_fElements[0] = f00; m_fElements[1] = f01; m_fElements[2] = f02;
	m_fElements[3] = f10; m_fElements[4] = f11; m_fElements[5] = f12;
	m_fElements[6] = f20; m_fElements[7] = f21; m_fElements[8] = f22;

	return *this;
}
XnVMatrix3X3& XnVMatrix3X3::Set(XnFloat* pf)
{
	xnOSMemCopy(m_fElements, pf, 9*sizeof(XnFloat));

	return *this;
}

XnVMatrix3X3& XnVMatrix3X3::SetRow(XnUInt32 nRow, const XnV3DVector& v3Row)
{
	m_fElements[nRow * 3    ] = v3Row[0];
	m_fElements[nRow * 3 + 1] = v3Row[1];
	m_fElements[nRow * 3 + 2] = v3Row[2];

	return *this;
}
XnVMatrix3X3& XnVMatrix3X3::SetColumn(XnUInt32 nColumn, const XnV3DVector& v3Column)
{
	m_fElements[        nColumn] = v3Column[0];
	m_fElements[1 * 3 + nColumn] = v3Column[1];
	m_fElements[2 * 3 + nColumn] = v3Column[2];

	return *this;
}
void XnVMatrix3X3::GetRow(XnUInt32 nRow, XnV3DVector& v3Row) const
{
	v3Row.Set(m_fElements[nRow*3], m_fElements[nRow*3+1], m_fElements[nRow*3+2]);
}
void XnVMatrix3X3::GetColumn(XnUInt32 nColumn, XnV3DVector& v3Column) const
{
	v3Column.Set(m_fElements[0*3+nColumn], m_fElements[1*3+nColumn], m_fElements[2*3+nColumn]);
}

XnBool XnVMatrix3X3::IsSymmetric(XnFloat fTolerance) const
{
	if (fabs((*this)(0,1) - (*this)(1,0)) > fTolerance)
		return false;
	if (fabs((*this)(0,2) - (*this)(2,0)) > fTolerance)
		return false;
	if (fabs((*this)(1,2) - (*this)(2,1)) > fTolerance)
		return false;

	return true;
}

// Comparison
inline XnBool XnVMatrix3X3::operator==(const XnVMatrix3X3& m33Rhs) const
{
	return Equal(m33Rhs, 1e-5f);
}
inline XnBool XnVMatrix3X3::operator!=(const XnVMatrix3X3& m33Rhs) const
{
	return !Equal(m33Rhs, 1e-5f);
}

inline XnBool XnVMatrix3X3::Equal(const XnVMatrix3X3& m33Other, XnFloat fTolerance) const
{
	for (XnUInt32 x = 0; x < 3; x++)
		for (XnUInt32 y = 0; y < 3; y++)
			if (fabs((*this)(x, y)-m33Other(x, y)) > fTolerance)
				return false;
	return true;
}

XnVMatrix3X3& XnVMatrix3X3::Negate(const XnVMatrix3X3& m33Other)
{
	return Set (-m33Other(0, 0), -m33Other(0, 1), -m33Other(0, 2),
				-m33Other(1, 0), -m33Other(1, 1), -m33Other(1, 2),
				-m33Other(2, 0), -m33Other(2, 1), -m33Other(2, 2));
}
XnVMatrix3X3& XnVMatrix3X3::Negate()
{
	return *this *= -1;
}
XnVMatrix3X3 XnVMatrix3X3::operator-() const
{
	return *this * -1;
}

XnVMatrix3X3 XnVMatrix3X3::operator+(XnFloat f) const
{
	XnVMatrix3X3 m33Result;
	m33Result.Add(*this, f);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::operator+=(XnFloat f)
{
	return Add(*this, f);

}
XnVMatrix3X3& XnVMatrix3X3::Add(const XnVMatrix3X3& m33Mat, XnFloat f)
{
	return Set( m33Mat(0, 0) + f, m33Mat(0, 1) + f, m33Mat(0, 2) + f,
				m33Mat(1, 0) + f, m33Mat(1, 1) + f, m33Mat(1, 2) + f,
				m33Mat(2, 0) + f, m33Mat(2, 1) + f, m33Mat(2, 2) + f);
}

XnVMatrix3X3 XnVMatrix3X3::operator+(const XnVMatrix3X3& m33Other) const
{
	XnVMatrix3X3 m33Result;
	m33Result.Add(*this, m33Other);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::operator+=(const XnVMatrix3X3& m33Other)
{
	return Add(*this, m33Other);
}

XnVMatrix3X3& XnVMatrix3X3::Add(const XnVMatrix3X3& m33Mat1, const XnVMatrix3X3& m33Mat2)
{
	return Set( m33Mat1.m_fElements[0] + m33Mat2.m_fElements[0],
				m33Mat1.m_fElements[1] + m33Mat2.m_fElements[1],
				m33Mat1.m_fElements[2] + m33Mat2.m_fElements[2],
				m33Mat1.m_fElements[3] + m33Mat2.m_fElements[3],
				m33Mat1.m_fElements[4] + m33Mat2.m_fElements[4],
				m33Mat1.m_fElements[5] + m33Mat2.m_fElements[5],
				m33Mat1.m_fElements[6] + m33Mat2.m_fElements[6],
				m33Mat1.m_fElements[7] + m33Mat2.m_fElements[7],
				m33Mat1.m_fElements[8] + m33Mat2.m_fElements[8]);
}

XnVMatrix3X3 XnVMatrix3X3::operator-(XnFloat f) const
{
	XnVMatrix3X3 m33Result;
	m33Result.Subtract(*this, f);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::operator-=(XnFloat f)
{
	return Subtract(*this, f);
}
XnVMatrix3X3& XnVMatrix3X3::Subtract(const XnVMatrix3X3& m33Mat, XnFloat f)
{
	return Set(	m33Mat.m_fElements[0] - f, m33Mat.m_fElements[1] - f, m33Mat.m_fElements[2] - f,
				m33Mat.m_fElements[3] - f, m33Mat.m_fElements[4] - f, m33Mat.m_fElements[5] - f,
				m33Mat.m_fElements[6] - f, m33Mat.m_fElements[7] - f, m33Mat.m_fElements[8] - f);
}

XnVMatrix3X3 XnVMatrix3X3::operator-(const XnVMatrix3X3& m33Other) const
{
	XnVMatrix3X3 m33Result;
	m33Result.Subtract(*this, m33Other);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::operator-=(const XnVMatrix3X3& m33Other)
{
	return Subtract(*this, m33Other);
}
XnVMatrix3X3& XnVMatrix3X3::Subtract(const XnVMatrix3X3& m33Mat1, const XnVMatrix3X3& m33Mat2)
{
	return Set(	m33Mat1.m_fElements[0] - m33Mat2.m_fElements[0],
				m33Mat1.m_fElements[1] - m33Mat2.m_fElements[1],
				m33Mat1.m_fElements[2] - m33Mat2.m_fElements[2],
				m33Mat1.m_fElements[3] - m33Mat2.m_fElements[3],
				m33Mat1.m_fElements[4] - m33Mat2.m_fElements[4],
				m33Mat1.m_fElements[5] - m33Mat2.m_fElements[5],
				m33Mat1.m_fElements[6] - m33Mat2.m_fElements[6],
				m33Mat1.m_fElements[7] - m33Mat2.m_fElements[7],
				m33Mat1.m_fElements[8] - m33Mat2.m_fElements[8]);
}

XnVMatrix3X3 XnVMatrix3X3::operator/(XnFloat f) const
{
	XnVMatrix3X3 m33Result;
	m33Result.Divide(*this, f);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::operator/=(XnFloat f)
{
	return Divide(*this, f);
}
XnVMatrix3X3& XnVMatrix3X3::Divide(const XnVMatrix3X3& m33Mat, XnFloat f)
{
	return Set(	m33Mat.m_fElements[0] / f, m33Mat.m_fElements[1] / f, m33Mat.m_fElements[2] / f,
				m33Mat.m_fElements[3] / f, m33Mat.m_fElements[4] / f, m33Mat.m_fElements[5] / f,
				m33Mat.m_fElements[6] / f, m33Mat.m_fElements[7] / f, m33Mat.m_fElements[8] / f);
}

XnVMatrix3X3 XnVMatrix3X3::operator*(XnFloat f) const
{
	XnVMatrix3X3 m33Result;
	m33Result.Multiply(*this, f);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::operator*=(XnFloat f)
{
	return Multiply(*this, f);
}

XnVMatrix3X3& XnVMatrix3X3::Multiply(const XnVMatrix3X3& m33Mat, XnFloat f)
{
	return Set(	m33Mat.m_fElements[0] * f, m33Mat.m_fElements[1] * f, m33Mat.m_fElements[2] * f,
				m33Mat.m_fElements[3] * f, m33Mat.m_fElements[4] * f, m33Mat.m_fElements[5] * f,
				m33Mat.m_fElements[6] * f, m33Mat.m_fElements[7] * f, m33Mat.m_fElements[8] * f);
}

XnVMatrix3X3 operator*(XnFloat f, const XnVMatrix3X3& m33Mat)
{
	XnVMatrix3X3 m33Result;
	m33Result.Multiply(f, m33Mat);
	return m33Result;
}
XnVMatrix3X3& XnVMatrix3X3::Multiply(XnFloat f, const XnVMatrix3X3& m33Mat)
{
	return Set(	f * m33Mat.m_fElements[0], f * m33Mat.m_fElements[1], f * m33Mat.m_fElements[2],
				f * m33Mat.m_fElements[3], f * m33Mat.m_fElements[4], f * m33Mat.m_fElements[5],
				f * m33Mat.m_fElements[6], f * m33Mat.m_fElements[7], f * m33Mat.m_fElements[8]);
}

XnVMatrix3X3 XnVMatrix3X3::operator*(const XnVMatrix3X3& m33Other) const
{
		XnVMatrix3X3 m33Result;
		m33Result.Multiply(*this, m33Other);
		return m33Result;

}
XnVMatrix3X3& XnVMatrix3X3::operator*=(const XnVMatrix3X3& m33Other)
{
	return Multiply(*this, m33Other);
}
XnVMatrix3X3& XnVMatrix3X3::Multiply(const XnVMatrix3X3& m33Mat1, const XnVMatrix3X3& m33Mat2)
{
	XnFloat fNewVals[9] = {0};

	for(XnUInt32 i = 0; i < 3; i++)
		for(XnUInt32 j = 0; j < 3; j++)
			for(XnUInt32 k = 0; k < 3; k++)
				fNewVals[i * 3 + j] += m33Mat1(i, k) * m33Mat2(k, j);

	return Set(fNewVals);
}

XnV3DVector XnVMatrix3X3::operator*(const XnV3DVector& v3Vec) const
{
	XnV3DVector m33Result;
	Multiply(v3Vec, m33Result);
	return m33Result;
}
void XnVMatrix3X3::Multiply(const XnV3DVector& v3Src, XnV3DVector& v3Dst) const
{
	v3Dst.Set(	m_fElements[0] * v3Src.X() + m_fElements[1] * v3Src.Y() + m_fElements[2] * v3Src.Z(),
				m_fElements[3] * v3Src.X() + m_fElements[4] * v3Src.Y() + m_fElements[5] * v3Src.Z(),
				m_fElements[6] * v3Src.X() + m_fElements[7] * v3Src.Y() + m_fElements[8] * v3Src.Z());
}


// Relevant for symmetric matrices only - make sure!
XnFloat XnVMatrix3X3::InnerProduct(const XnV3DVector& v3Vec1, const XnV3DVector& v3Vec2) const
{
	return (v3Vec1.X() * ((*this)(0,0) * v3Vec2.X() + (*this)(0,1) * v3Vec2.Y() + (*this)(0,2) * v3Vec2.Z()) +
			v3Vec1.Y() * ((*this)(1,0) * v3Vec2.X() + (*this)(1,1) * v3Vec2.Y() + (*this)(1,2) * v3Vec2.Z()) +
			v3Vec1.Z() * ((*this)(2,0) * v3Vec2.X() + (*this)(2,1) * v3Vec2.Y() + (*this)(2,2) * v3Vec2.Z()));
			
}

XnVMatrix3X3& XnVMatrix3X3::SetOuterProduct(const XnV3DVector& v3Vec1, const XnV3DVector& v3Vec2)
{
	return Set(	v3Vec1.X() * v3Vec2.X(), v3Vec1.X() * v3Vec2.Y(), v3Vec1.X() * v3Vec2.Z(),
				v3Vec1.Y() * v3Vec2.X(), v3Vec1.Y() * v3Vec2.Y(), v3Vec1.Y() * v3Vec2.Z(),
				v3Vec1.Z() * v3Vec2.X(), v3Vec1.Z() * v3Vec2.Y(), v3Vec1.Z() * v3Vec2.Z());
}

XnVMatrix3X3& XnVMatrix3X3::SetOuterProduct(const XnV3DVector& v3Vec)
{
	return SetOuterProduct(v3Vec, v3Vec);
}

XnFloat XnVMatrix3X3::Determinant() const
{
	return	m_fElements[0] * m_fElements[4] * m_fElements[8] +
			m_fElements[1] * m_fElements[5] * m_fElements[6] +
			m_fElements[2] * m_fElements[3] * m_fElements[7] -
			m_fElements[2] * m_fElements[4] * m_fElements[6] -
			m_fElements[1] * m_fElements[3] * m_fElements[8] -
			m_fElements[0] * m_fElements[5] * m_fElements[7];
}
XnFloat XnVMatrix3X3::Trace() const
{
	return m_fElements[0] + m_fElements[4] + m_fElements[8];
}
	
void XnVMatrix3X3::SetDiagonal(const XnV3DVector& v3Diag)
{
	Set(v3Diag.X(), m_fElements[1], m_fElements[2],
		m_fElements[3], v3Diag.Y(), m_fElements[5],
		m_fElements[6], m_fElements[7], v3Diag.Z());
}
void XnVMatrix3X3::GetDiagonal(XnV3DVector& v3Diag) const
{
	v3Diag.Set(m_fElements[0], m_fElements[4], m_fElements[8]);
}

XnVMatrix3X3 XnVMatrix3X3::operator~() const // Transpose?
{
	XnVMatrix3X3 m33Result;
	m33Result.Transpose(*this);
	return m33Result;
}

XnVMatrix3X3& XnVMatrix3X3::Transpose()
{
	return Transpose(*this);
}

XnVMatrix3X3& XnVMatrix3X3::Transpose(const XnVMatrix3X3& m33Other)
{
	return Set(	m33Other.m_fElements[0], m33Other.m_fElements[3], m33Other.m_fElements[6],
				m33Other.m_fElements[1], m33Other.m_fElements[4], m33Other.m_fElements[7],
				m33Other.m_fElements[2], m33Other.m_fElements[5], m33Other.m_fElements[8]);
}
XnVMatrix3X3& XnVMatrix3X3::Inverse(const XnVMatrix3X3& m33Other)
{
	XnVMatrix3X3 cm;
	(~m33Other).CofactorMatrix(cm);
	Divide(cm, m33Other.Determinant());

	return *this;
}


XnVMatrix3X3& XnVMatrix3X3::SetIdentity()
{
	return Set(	1, 0, 0,
				0, 1, 0,
				0, 0, 1);
}

XnVMatrix3X3& XnVMatrix3X3::SetXRotation(XnFloat fXAngle)
{
	XnFloat s = sin(fXAngle), c = cos(fXAngle);
	return Set(	1, 0, 0,
				0, c, -s,
				0, s, c);
}
XnVMatrix3X3& XnVMatrix3X3::SetYRotation(XnFloat fYAngle)
{
	XnFloat s = sin(fYAngle), c = cos(fYAngle);
	return Set(	c, 0, s,
				0, 1, 0,
				-s, 0, c);
}
XnVMatrix3X3& XnVMatrix3X3::SetZRotation(XnFloat fZAngle)
{
	XnFloat s = sin(fZAngle), c = cos(fZAngle);
	return Set(	c, -s, 0,
				s, c, 0,
				0, 0, 1);
}


void XnVMatrix3X3::CofactorMatrix(XnVMatrix3X3& m33CoMat) const
{
	m33CoMat.Set(m_fElements[4]*m_fElements[8] - m_fElements[5]*m_fElements[7],
				 m_fElements[5]*m_fElements[6] - m_fElements[3]*m_fElements[8],
				 m_fElements[3]*m_fElements[7] - m_fElements[4]*m_fElements[6],

				 m_fElements[2]*m_fElements[7] - m_fElements[1]*m_fElements[8],
				 m_fElements[0]*m_fElements[8] - m_fElements[2]*m_fElements[6],
				 m_fElements[1]*m_fElements[6] - m_fElements[0]*m_fElements[7],

				 m_fElements[1]*m_fElements[5] - m_fElements[2]*m_fElements[4],
				 m_fElements[2]*m_fElements[3] - m_fElements[0]*m_fElements[5],
				 m_fElements[0]*m_fElements[4] - m_fElements[1]*m_fElements[3]);
}


XnStatus XnVMatrix3X3::LargestColumnNormalized(XnV3DVector& v3Col) const
{
	XnFloat scales[3] = {
		XnVMathCommon::Sqr((*this)(0,0)) + XnVMathCommon::Sqr((*this)(1,0)) + XnVMathCommon::Sqr((*this)(2,0)),
		XnVMathCommon::Sqr((*this)(0,1)) + XnVMathCommon::Sqr((*this)(1,1)) + XnVMathCommon::Sqr((*this)(2,1)),
		XnVMathCommon::Sqr((*this)(0,2)) + XnVMathCommon::Sqr((*this)(1,2)) + XnVMathCommon::Sqr((*this)(2,2))
	};
	XnUInt16 i = XnVMathCommon::ArgMax(scales[0], scales[1], scales[2]);
	if(scales[i] == 0)
	{
		return XN_STATUS_EE_ERROR;
	}

	switch (i)
	{
	case 0:
		v3Col.Set((*this)(0,0), (*this)(1,0), (*this)(2,0));
		break;
	case 1:
		v3Col.Set((*this)(0,1), (*this)(1,1), (*this)(2,1));
		break;
	case 2:
		v3Col.Set((*this)(0,2), (*this)(1,2), (*this)(2,2));
		break;
	default:
		return false;
	}

	v3Col /= sqrt(scales[i]);
	return XN_STATUS_OK;

}

inline XnStatus XnVMatrix3X3::GetSymmetricEigenValues(XnV3DVector& v3Values) const
{
	if (!IsSymmetric())
	{
		return XN_STATUS_EE_MATRIX_NOT_SYMMETRIC;
	}

    XnFloat m = Trace() / 3.0f;
    XnFloat a00 = (*this)(0,0)-m, a11 = (*this)(1,1)-m, a22 = (*this)(2,2)-m;
	XnFloat a01_sqr = XnVMathCommon::Sqr((*this)(0,1));
	XnFloat a02_sqr = XnVMathCommon::Sqr((*this)(0,2));
	XnFloat a12_sqr = XnVMathCommon::Sqr((*this)(1,2));
    XnFloat p = (a00*a00+a11*a11+a22*a22 + 2*(a01_sqr+a02_sqr+a12_sqr)) / 6.0f;
    XnFloat q = .5f*(a00*(a11*a22-a12_sqr)-a11*a02_sqr-a22*a01_sqr) + (*this)(0,1)*(*this)(0,2)*(*this)(1,2);
    XnFloat sqrt_p = sqrt(p), disc = p*p*p-q*q;
    XnFloat phi = atan2(sqrt(XnVMathCommon::Max(0.0f,disc)),q) / 3.0f;
	XnFloat c=cos(phi) , s=sin(phi);
    XnFloat sqrt_p_cos = sqrt_p*c, root_three_sqrt_p_sin = sqrt(3.0f)*sqrt_p*s;

	v3Values.Set(m+2*sqrt_p_cos, m-sqrt_p_cos-root_three_sqrt_p_sin, m-sqrt_p_cos+root_three_sqrt_p_sin);

	XnVMathCommon::ExchangeSort(v3Values.Z(), v3Values.Y(), v3Values.X());

	return XN_STATUS_OK;
}
XnStatus XnVMatrix3X3::GetSymmetricEigenVectors(const XnV3DVector &v3Lambda,
											 XnVMatrix3X3& m33Vectors,
											 XnFloat tolerance) const
{
	XnVMatrix3X3 id;
	id.SetIdentity();

	if (!IsSymmetric())
	{
		return XN_STATUS_EE_MATRIX_NOT_SYMMETRIC;
	}

	XnFloat fTiny = tolerance * XnVMathCommon::MaxAbs(v3Lambda.X(), v3Lambda.Z());
	XnVMatrix3X3 m33CoMat;
	if (v3Lambda.X() - v3Lambda.Y() <= fTiny)
	{
		if (v3Lambda.Y() - v3Lambda.Z() <= fTiny)
		{
			m33Vectors.SetIdentity();
		}
		else
		{
			XnV3DVector v3Vec2, v3Vec1, v3Cross;
			((*this)-id*v3Lambda.Z()).CofactorMatrix(m33CoMat);
			XnStatus rc = m33CoMat.LargestColumnNormalized(v3Vec2);
			if (rc != XN_STATUS_OK)
			{
				return rc;
			}
			v3Vec1.UnitOrthogonalVector(v3Vec2);
			v3Cross.CrossProduct(v3Vec1, v3Vec2);
			m33Vectors.Set(v3Cross, v3Vec1, v3Vec2);
		}
	}
    else if(v3Lambda.Y() - v3Lambda.Z()<=fTiny)
	{
        XnV3DVector v3Vec0, v3Vec1, v3Cross;
		((*this)-id*v3Lambda.X()).CofactorMatrix(m33CoMat);
		XnStatus rc = m33CoMat.LargestColumnNormalized(v3Vec0);
		if (rc != XN_STATUS_OK)
		{
			return rc;
		}
		v3Vec1.UnitOrthogonalVector(v3Vec0);
		v3Cross.CrossProduct(v3Vec0, v3Vec1);
		m33Vectors.Set(v3Vec0, v3Vec1, v3Cross);
	}
    else
	{
        XnV3DVector v3Vec0, v3Cross, v3Vec2;
		((*this)-id*v3Lambda.X()).CofactorMatrix(m33CoMat);
		XnStatus rc = m33CoMat.LargestColumnNormalized(v3Vec0);
		if (rc != XN_STATUS_OK)
		{
			return rc;
		}
		((*this)-id*v3Lambda.Z()).CofactorMatrix(m33CoMat);
        rc = m33CoMat.LargestColumnNormalized(v3Vec2);
		if (rc != XN_STATUS_OK)
		{
			return rc;
		}
		v3Cross.CrossProduct(v3Vec2, v3Vec0);

		m33Vectors.Set(v3Vec0, v3Cross, v3Vec2);
	}

	return XN_STATUS_OK;
}

XnStatus XnVMatrix3X3::SolveSymmetricEigenProblem(XnV3DVector& v3EigenValues, XnVMatrix3X3& m33EigenVectors, XnFloat fTolerance) const
{
	if (!IsSymmetric())
	{
		return XN_STATUS_EE_MATRIX_NOT_SYMMETRIC;
	}

	GetSymmetricEigenValues(v3EigenValues);
	GetSymmetricEigenVectors(v3EigenValues, m33EigenVectors, fTolerance);

	return XN_STATUS_OK;
}


#endif //_XNV_MATRIX3X3_H_
