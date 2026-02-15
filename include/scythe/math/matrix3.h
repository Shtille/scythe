#ifndef __SCYTHE_MATRIX3_H__
#define __SCYTHE_MATRIX3_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include "vector3.h"

namespace scythe {

class Quaternion;
class Matrix4;

/**
 * Defines a 3 x 3 floating point matrix representing a 3D rotation.
 *
 * Vectors are treated as columns, resulting in a matrix that is represented as follows,
 * where x, y and z are the translation components of the matrix:
 *
 * 1  0  0
 * 0  1  0
 * 0  0  1
 *
 * This matrix class is directly compatible with OpenGL since its elements are
 * laid out in memory exactly as they are expected by OpenGL.
 * The matrix uses column-major format such that array indices increase down column first.
 * Since matrix multiplication is not commutative, multiplication must be done in the
 * correct order when combining transformations.
 */
class Matrix3
{
public:

	/**
	 * Stores the columns of this 3x3 matrix.
	 * */
	float m[9];

	/**
	 * Constructs a matrix initialized to the identity matrix:
	 *
	 * 1  0  0
	 * 0  1  0
	 * 0  0  1
	 */
	Matrix3();

	/**
	 * Constructs a matrix initialized to the specified value.
	 *
	 * @param m11 The first element of the first row.
	 * @param m12 The second element of the first row.
	 * @param m13 The third element of the first row.
	 * @param m21 The first element of the second row.
	 * @param m22 The second element of the second row.
	 * @param m23 The third element of the second row.
	 * @param m31 The first element of the third row.
	 * @param m32 The second element of the third row.
	 * @param m33 The third element of the third row.
	 */
	Matrix3(float m11, float m12, float m13, float m21, float m22, float m23,
		   float m31, float m32, float m33);

	/**
	 * Creates a matrix initialized to the specified column-major array.
	 *
	 * The passed-in array is in column-major order, so the memory layout of the array is as follows:
	 *
	 *     0   3   6
	 *     1   4   7
	 *     2   5   8
	 *
	 * @param m An array containing 9 elements in column-major order.
	 */
	Matrix3(const float* m);

	/**
	 * Constructs a new matrix by copying the values from the specified matrix.
	 *
	 * @param copy The matrix to copy.
	 */
	Matrix3(const Matrix3& copy);

	/**
	 * Destructor.
	 */
	~Matrix3();

	/**
	 * Returns the identity matrix:
	 *
	 * 1  0  0
	 * 0  1  0
	 * 0  0  1
	 *
	 * @return The identity matrix.
	 */
	static const Matrix3& Identity();

	/**
	 * Returns the matrix with all zeros.
	 *
	 * @return The matrix with all zeros.
	 */
	static const Matrix3& Zero();

	/**
	 * Creates a matrix basis from the axis.
	 *
	 * @param x_axis A X-direction axis. May not have unit length.
	 * @param y_axis A Y-direction axis. Could be not orthonormal to x-axis. May not have unit length.
	 * @param dst A matrix to store the result in.
	 */
	static void CreateBasis(const Vector3& x_axis, const Vector3& y_axis, Matrix3* dst);

	/**
	 * Creates a rotation matrix from the specified quaternion.
	 *
	 * @param quat A quaternion describing a 3D orientation.
	 * @param dst A matrix to store the result in.
	 */
	static void CreateRotation(const Quaternion& quat, Matrix3* dst);

	/**
	 * Creates a rotation matrix from the specified axis and angle.
	 *
	 * @param axis A vector describing the axis to rotate about.
	 * @param angle The angle (in radians).
	 * @param dst A matrix to store the result in.
	 */
	static void CreateRotation(const Vector3& axis, float angle, Matrix3* dst);

	/**
	 * Creates a matrix describing a rotation around the x-axis.
	 *
	 * @param angle The angle of rotation (in radians).
	 * @param dst A matrix to store the result in.
	 */
	static void CreateRotationX(float angle, Matrix3* dst);

	/**
	 * Creates a matrix describing a rotation around the y-axis.
	 *
	 * @param angle The angle of rotation (in radians).
	 * @param dst A matrix to store the result in.
	 */
	static void CreateRotationY(float angle, Matrix3* dst);

	/**
	 * Creates a matrix describing a rotation around the z-axis.
	 *
	 * @param angle The angle of rotation (in radians).
	 * @param dst A matrix to store the result in.
	 */
	static void CreateRotationZ(float angle, Matrix3* dst);

	/**
	* Creates a matrix describing the yaw, pitch and roll rotations
	*
	* @param yaw The yaw angle (in radians)
	* @param pitch The pitch angle (in radians)
	* @param roll The roll angle (in radians)
	* @param dst A matrix to store the result in.
	*/
	static void CreateFromEuler(float yaw, float pitch, float roll, Matrix3* dst);

	/**
	* Creates a normal matrix from the specified modelview matrix.
	*
	* @param modelview The modelview matrix (view * model).
	* @param dst A matrix to store the result in.
	*/
	static void CreateNormal(const Matrix4& modelview, Matrix3* dst);

	/**
	 * Adds a scalar value to each component of this matrix.
	 *
	 * @param scalar The scalar to add.
	 */
	void Add(float scalar);

	/**
	 * Adds a scalar value to each component of this matrix and stores the result in dst.
	 *
	 * @param scalar The scalar value to add.
	 * @param dst A matrix to store the result in.
	 */
	void Add(float scalar, Matrix3* dst);

	/**
	 * Adds the specified matrix to this matrix.
	 *
	 * @param m The matrix to add.
	 */
	void Add(const Matrix3& m);

	/**
	 * Adds the specified matrices and stores the result in dst.
	 *
	 * @param m1 The first matrix.
	 * @param m2 The second matrix.
	 * @param dst The destination matrix to add to.
	 */
	static void Add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

	/**
	 * Decomposes the rotation component of this matrix.
	 *
	 * @param rotation The rotation.
	 */
	bool Decompose(Quaternion* rotation) const;

	/**
	 * Computes the determinant of this matrix.
	 *
	 * @return The determinant.
	 */
	float Determinant() const;

	/**
	 * Gets the rotational component of this matrix in the specified quaternion.
	 *
	 * @param rotation A quaternion to receive the rotation.
	 * 
	 * @return true if the rotation is successfully extracted, false otherwise.
	 */
	bool GetRotation(Quaternion* rotation) const;

	/**
	 * Gets the up vector of this matrix.
	 *
	 * @param dst The destination vector.
	 */
	void GetUpVector(Vector3* dst) const;

	/**
	 * Gets the down vector of this matrix.
	 *
	 * @param dst The destination vector.
	 */
	void GetDownVector(Vector3* dst) const;

	/**
	 * Gets the left vector of this matrix.
	 *
	 * @param dst The destination vector.
	 */
	void GetLeftVector(Vector3* dst) const;

	/**
	 * Gets the right vector of this matrix.
	 *
	 * @param dst The destination vector.
	 */
	void GetRightVector(Vector3* dst) const;

	/**
	 * Gets the forward vector of this matrix.
	 *
	 * @param dst The destination vector.
	 */
	void GetForwardVector(Vector3* dst) const;

	/**
	 * Gets the backward vector of this matrix.
	 *
	 * @param dst The destination vector.
	 */
	void GetBackVector(Vector3* dst) const;

	/**
	 * Inverts this matrix.
	 *
	 * @return true if the the matrix can be inverted, false otherwise.
	 */
	bool Invert();

	/**
	 * Stores the inverse of this matrix in the specified matrix.
	 *
	 * @param dst A matrix to store the invert of this matrix in.
	 * 
	 * @return true if the the matrix can be inverted, false otherwise.
	 */
	bool Invert(Matrix3* dst) const;

	/**
	 * Determines if this matrix is equal to the identity matrix.
	 *
	 * @return true if the matrix is an identity matrix, false otherwise.
	 */
	bool IsIdentity() const;

	/**
	 * Multiplies the components of this matrix by the specified scalar.
	 *
	 * @param scalar The scalar value.
	 */
	void Multiply(float scalar);

	/**
	 * Multiplies the components of this matrix by a scalar and stores the result in dst.
	 *
	 * @param scalar The scalar value.
	 * @param dst A matrix to store the result in.
	 */
	void Multiply(float scalar, Matrix3* dst) const;

	/**
	 * Multiplies the components of the specified matrix by a scalar and stores the result in dst.
	 *
	 * @param m The matrix.
	 * @param scalar The scalar value.
	 * @param dst A matrix to store the result in.
	 */
	static void Multiply(const Matrix3& m, float scalar, Matrix3* dst);

	/**
	 * Multiplies this matrix by the specified one.
	 *
	 * @param m The matrix to multiply.
	 */
	void Multiply(const Matrix3& m);

	/**
	 * Multiplies m1 by m2 and stores the result in dst.
	 *
	 * @param m1 The first matrix to multiply.
	 * @param m2 The second matrix to multiply.
	 * @param dst A matrix to store the result in.
	 */
	static void Multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

	/**
	 * Negates this matrix.
	 */
	void Negate();

	/**
	 * Negates this matrix and stores the result in dst.
	 *
	 * @param dst A matrix to store the result in.
	 */
	void Negate(Matrix3* dst) const;

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified quaternion rotation.
	 *
	 * @param q The quaternion to rotate by.
	 */
	void Rotate(const Quaternion& q);

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified quaternion rotation and stores the result in dst.
	 *
	 * @param q The quaternion to rotate by.
	 * @param dst A matrix to store the result in.
	 */
	void Rotate(const Quaternion& q, Matrix3* dst) const;

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation about the specified axis.
	 *
	 * @param axis The axis to rotate about.
	 * @param angle The angle (in radians).
	 */
	void Rotate(const Vector3& axis, float angle);

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the specified
	 * rotation about the specified axis and stores the result in dst.
	 *
	 * @param axis The axis to rotate about.
	 * @param angle The angle (in radians).
	 * @param dst A matrix to store the result in.
	 */
	void Rotate(const Vector3& axis, float angle, Matrix3* dst) const;

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation around the x-axis.
	 *
	 * @param angle The angle (in radians).
	 */
	void RotateX(float angle);

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation around the x-axis and stores the result in dst.
	 *
	 * @param angle The angle (in radians).
	 * @param dst A matrix to store the result in.
	 */
	void RotateX(float angle, Matrix3* dst) const;

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation around the y-axis.
	 *
	 * @param angle The angle (in radians).
	 */
	void RotateY(float angle);

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation around the y-axis and stores the result in dst.
	 *
	 * @param angle The angle (in radians).
	 * @param dst A matrix to store the result in.
	 */
	void RotateY(float angle, Matrix3* dst) const;

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation around the z-axis.
	 *
	 * @param angle The angle (in radians).
	 */
	void RotateZ(float angle);

	/**
	 * Post-multiplies this matrix by the matrix corresponding to the
	 * specified rotation around the z-axis and stores the result in dst.
	 *
	 * @param angle The angle (in radians).
	 * @param dst A matrix to store the result in.
	 */
	void RotateZ(float angle, Matrix3* dst) const;

	/**
	 * Sets the values of this matrix.
	 *
	 * @param m11 The first element of the first row.
	 * @param m12 The second element of the first row.
	 * @param m13 The third element of the first row.
	 * @param m21 The first element of the second row.
	 * @param m22 The second element of the second row.
	 * @param m23 The third element of the second row.
	 * @param m31 The first element of the third row.
	 * @param m32 The second element of the third row.
	 * @param m33 The third element of the third row.
	 */
	void Set(float m11, float m12, float m13, float m21, float m22, float m23,
			 float m31, float m32, float m33);

	/**
	 * Sets the values of this matrix to those in the specified column-major array.
	 *
	 * @param m An array containing 16 elements in column-major format.
	 */
	void Set(const float* m);

	/**
	 * Sets the values of this matrix to those of the specified matrix.
	 *
	 * @param m The source matrix.
	 */
	void Set(const Matrix3& m);

	/**
	 * Sets this matrix to the identity matrix.
	 */
	void SetIdentity();

	/**
	 * Sets all elements of the current matrix to zero.
	 */
	void SetZero();

	/**
	 * Subtracts the specified matrix from the current matrix.
	 *
	 * @param m The matrix to subtract.
	 */
	void Subtract(const Matrix3& m);

	/**
	 * Subtracts the specified matrix from the current matrix.
	 *
	 * @param m1 The first matrix.
	 * @param m2 The second matrix.
	 * @param dst A matrix to store the result in.
	 */
	static void Subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

	/**
	 * Transforms the specified vector by this matrix.
	 *
	 * The result of the transformation is stored directly into vector.
	 *
	 * @param vector The vector to transform.
	 */
	void TransformVector(Vector3* vector) const;

	/**
	 * Transforms the specified vector by this matrix.
	 *
	 * @param vector The vector to transform.
	 * @param dst A vector to store the transformed point in.
	 */
	void TransformVector(const Vector3& vector, Vector3* dst) const;

	/**
	 * Transposes this matrix.
	 */
	void Transpose();

	/**
	 * Transposes this matrix and stores the result in dst.
	 *
	 * @param dst A matrix to store the result in.
	 */
	void Transpose(Matrix3* dst) const;

	/**
	 * Calculates the sum of this matrix with the given matrix.
	 * 
	 * Note: this does not modify this matrix.
	 * 
	 * @param m The matrix to add.
	 * @return The matrix sum.
	 */
	inline const Matrix3 operator+(const Matrix3& m) const;
	
	/**
	 * Adds the given matrix to this matrix.
	 * 
	 * @param m The matrix to add.
	 * @return This matrix, after the addition occurs.
	 */
	inline Matrix3& operator+=(const Matrix3& m);

	/**
	 * Calculates the difference of this matrix with the given matrix.
	 * 
	 * Note: this does not modify this matrix.
	 * 
	 * @param m The matrix to subtract.
	 * @return The matrix difference.
	 */
	inline const Matrix3 operator-(const Matrix3& m) const;

	/**
	 * Subtracts the given matrix from this matrix.
	 * 
	 * @param m The matrix to subtract.
	 * @return This matrix, after the subtraction occurs.
	 */
	inline Matrix3& operator-=(const Matrix3& m);

	/**
	 * Calculates the negation of this matrix.
	 * 
	 * Note: this does not modify this matrix.
	 * 
	 * @return The negation of this matrix.
	 */
	inline const Matrix3 operator-() const;

	/**
	 * Calculates the matrix product of this matrix with the given matrix.
	 * 
	 * Note: this does not modify this matrix.
	 * 
	 * @param m The matrix to multiply by.
	 * @return The matrix product.
	 */
	inline const Matrix3 operator*(const Matrix3& m) const;

	/**
	 * Right-multiplies this matrix by the given matrix.
	 * 
	 * @param m The matrix to multiply by.
	 * @return This matrix, after the multiplication occurs.
	 */
	inline Matrix3& operator*=(const Matrix3& m);

	/**
	 * Transforms matrix to an array pointer
	 */
	inline operator float *();

	/**
	 * Transforms matrix to an array pointer
	 */
	inline operator const float *() const;

};

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param v The vector to transform.
 * @param m The matrix to transform by.
 * @return This vector, after the transformation occurs.
 */
inline Vector3& operator*=(Vector3& v, const Matrix3& m);

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param m The matrix to transform by.
 * @param v The vector to transform.
 * @return The resulting transformed vector.
 */
inline const Vector3 operator*(const Matrix3& m, const Vector3& v);

} // namespace scythe

#include "matrix3.inl"

#endif