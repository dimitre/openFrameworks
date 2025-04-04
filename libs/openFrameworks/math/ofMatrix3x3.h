/*
 * Credits:
 * Code adopted from Lode Vandevenne http://student.kuleuven.be/~m0216922/CG/
 */

#pragma once

#if !defined(GLM_FORCE_CTOR_INIT)
	#define GLM_FORCE_CTOR_INIT
#endif
#if !defined(GLM_ENABLE_EXPERIMENTAL)
	#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/mat3x3.hpp>

#include <iostream>

/// \brief A 3x3 Matrix
///
/// The layout is like this:
///
///     [ a b c ]
///     [ d e f ]
///     [ g h i ]

class ofMatrix3x3 {
	
	
public:
	
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
	float g;
	float h;
	float i;
	
	
	
	//---------------------
	/// \name Constructor
	/// \{
	
	ofMatrix3x3( float _a=0.0, float _b=0.0, float _c=0.0,
				float _d=0.0, float _e=0.0, float _f=0.0,
				float _g=0.0, float _h=0.0, float _i=0.0 );


	ofMatrix3x3( const glm::mat3 & mat) {
		*this = reinterpret_cast<const ofMatrix3x3&>(mat);
	}

	operator glm::mat3() const{
		return *reinterpret_cast<const glm::mat3*>(this);
	}
	
	/// \}
	
	//---------------------
	/// \name Matrix access
	/// \{
	
	void set( float _a, float _b, float _c,
			 float _d, float _e, float _f,
			 float _g, float _h, float _i );
	
	
	float& operator[]( const int& index );

	
	/// \}
	
	//---------------------
	/// \name Functions
	/// \{
	
	
	/// \brief Transpose the matrix
	///
	/// This changes the matrix.
	///
	///     [ a b c ]T    [ a d g ]
	///     [ d e f ]  =  [ b e h ]
	///     [ g h i ]     [ c f i ]
	void transpose();
		
	/// \brief Transpose without changing the matrix.
	/// Uses the "swap" method with additions and subtractions to swap the elements that aren't on the main diagonal.
	/// \returns transposed matrix.	
	ofMatrix3x3 transpose(const ofMatrix3x3& A);
	
	/// \brief Find the determinant of the matrix
	/// \sa [Wolfram explanation](http://mathworld.wolfram.com/Determinant.html)
	float determinant() const;
	
	float determinant(const ofMatrix3x3& A);
	
	
	
	
	/// \brief Inverse of a 3x3 matrix
	///
	/// the inverse is the adjoint divided through the determinant
	/// find the matrix of minors (minor = determinant of 2x2 matrix of the 2 rows/colums current element is NOT in)
	/// turn them in cofactors (= change some of the signs)
	/// find the adjoint by transposing the matrix of cofactors
	/// divide this through the determinant to get the inverse
	///
	/// \sa invert();	
	ofMatrix3x3 inverse(const ofMatrix3x3& A);
	
	void invert();
	
	
	/// \brief Multiply a matrix by a matrix entry by entry (i.e. a*a, b*b, c*c...)
	///
	/// This is referred to as an entrywise, Hadamard, or Schur product.
	ofMatrix3x3 entrywiseTimes(const ofMatrix3x3& A);

	/// \}
	
	//---------------------
	/// \name Operators
	/// \{
	

	
	/// \brief Add two matrices
	ofMatrix3x3 operator+(const ofMatrix3x3& B);
	
	/// \brief Add matrix to existing matrix
	void operator+=(const ofMatrix3x3& B);
	
	/// \brief Subtract two matrices
	ofMatrix3x3 operator-(const ofMatrix3x3& B);
	
	/// \brief Subtract matrix from existing matrix
	void operator-=(const ofMatrix3x3& B);
	
	/// \brief Multiply a matrix with a scalar		
	ofMatrix3x3 operator*(float scalar);
	
	/// \brief Multiply a matrix by a matrix this = this*B (in that order)	 
	void operator*=(const ofMatrix3x3& B);
	
	/// \brief Multiply a matrix by a scalar (multiples all entries by scalar)	 
	void operator*=(float scalar);
	
	/// \brief Multiply a 3x3 matrix with a 3x3 matrix	 
	ofMatrix3x3 operator*(const ofMatrix3x3& B);
	
	/// \brief Divide a matrix through a scalar		
	ofMatrix3x3 operator/(float scalar);
	
	void operator/=(const ofMatrix3x3& B);
	
	void operator/=(float scalar);
	
	friend std::ostream& operator<<(std::ostream& os, const ofMatrix3x3& M);
	friend std::istream& operator>>(std::istream& is, ofMatrix3x3& M);
	
	/// \}
};

