#ifndef VECTOR3_H_
#define VECTOR3_H_

namespace gameplay
{

class Matrix;
class Quaternion;

/**
 * Defines a 3-element floating point vector.
 *
 * When using a vector to represent a surface normal,
 * the vector should typically be normalized.
 * Other uses of directional vectors may wish to leave
 * the magnitude of the vector intact. When used as a point,
 * the elements of the vector represent a position in 3D space.
 */
class Vector3
{
public:

    /**
     * The x-coordinate.
     */
    float x;

    /**
     * The y-coordinate.
     */
    float y;

    /**
     * The z-coordinate.
     */
    float z;

    /**
     * Constructs a new vector initialized to all zeros.
     */
    Vector3();

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param x The x coordinate.
     * @param y The y coordinate.
     * @param z The z coordinate.
     */
    Vector3(float x, float y, float z);

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    Vector3(float* array);

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    Vector3(const Vector3& p1, const Vector3& p2);

    /**
     * Constructs a new vector that is a copy of the specified vector.
     *
     * @param copy The vector to copy.
     */
    Vector3(const Vector3& copy);

    /**
     * Creates a new vector from an integer interpreted as an RGB value.
     * E.g. 0xff0000 represents red or the vector (1, 0, 0).
     *
     * @param color The integer to interpret as an RGB value.
     *
     * @return A vector corresponding to the interpreted RGB color.
     */
    static Vector3 fromColor(unsigned int color);

    /**
     * Destructor.
     */
    ~Vector3();

    /**
     * Returns the zero vector.
     *
     * @return The 3-element vector of 0s.
     */
    static const Vector3& zero();

    /**
     * Returns the one vector.
     *
     * @return The 3-element vector of 1s.
     */
    static const Vector3& one();

    /**
     * Returns the unit x vector.
     *
     * @return The 3-element unit vector along the x axis.
     */
    static const Vector3& unitX();

    /**
     * Returns the unit y vector.
     *
     * @return The 3-element unit vector along the y axis.
     */
    static const Vector3& unitY();

    /**
     * Returns the unit z vector.
     *
     * @return The 3-element unit vector along the z axis.
     */
    static const Vector3& unitZ();

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    bool isZero() const;

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    bool isOne() const;

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The angle between the two vectors (in radians).
     */
    static float angle(const Vector3& v1, const Vector3& v2);


    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    void add(const Vector3& v);

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vector3& v1, const Vector3& v2, Vector3* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vector3& min, const Vector3& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vector3& v, const Vector3& min, const Vector3& max, Vector3* dst);

    /**
     * Sets this vector to the cross product between itself and the specified vector.
     *
     * @param v The vector to compute the cross product with.
     */
    void cross(const Vector3& v);

    /**
     * Computes the cross product of the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void cross(const Vector3& v1, const Vector3& v2, Vector3* dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     * 
     * @return The distance between this vector and v.
     * 
     * @see distanceSquared
     */
    float distance(const Vector3& v) const;

    /**
     * Returns the squared distance between this vector and v.
     *
     * When it is not necessary to get the exact distance between
     * two vectors (for example, when simply comparing the
     * distance between different vectors), it is advised to use
     * this method instead of distance.
     *
     * @param v The other vector.
     * 
     * @return The squared distance between this vector and v.
     * 
     * @see distance
     */
    float distanceSquared(const Vector3& v) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     * 
     * @return The dot product.
     */
    float dot(const Vector3& v);

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The dot product between the vectors.
     */
    static float dot(const Vector3& v1, const Vector3& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     * 
     * @see lengthSquared
     */
    float length() const;

    /**
     * Returns the squared length of this vector.
     *
     * When it is not necessary to get the exact length of a
     * vector (for example, when simply comparing the lengths of
     * different vectors), it is advised to use this method
     * instead of length.
     *
     * @return The squared length of the vector.
     * 
     * @see length
     */
    float lengthSquared() const;

    /**
     * Negates this vector.
     */
    void negate();

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vector3 so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     */
    void normalize();

    /**
     * Normalizes this vector and stores the result in dst.
     *
     * If the vector already has unit length or if the length
     * of the vector is zero, this method simply copies the
     * current vector into dst.
     *
     * @param dst The destination vector.
     */
    void normalize(Vector3* dst) const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    void scale(float scalar);

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param x The new x coordinate.
     * @param y The new y coordinate.
     * @param z The new z coordinate.
     */
    void set(float x, float y, float z);

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    void set(float* array);

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    void set(const Vector3& v);

    /**
     * Sets this vector to the directional vector between the specified points.
     */
    void set(const Vector3& p1, const Vector3& p2);

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    void subtract(const Vector3& v);

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vector3& v1, const Vector3& v2, Vector3* dst);

    /**
     * Calculates the sum of this vector with the given vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline Vector3 operator+(const Vector3& v);

    /**
     * Adds the given vector to this vector.
     * 
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    inline Vector3& operator+=(const Vector3& v);

    /**
     * Calculates the sum of this vector with the given vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline Vector3 operator-(const Vector3& v);

    /**
     * Subtracts the given vector from this vector.
     * 
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    inline Vector3& operator-=(const Vector3& v);

    /**
     * Calculates the negation of this vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @return The negation of this vector.
     */
    inline Vector3 operator-();

    /**
     * Calculates the scalar product of this vector with the given value.
     * 
     * Note: this does not modify this vector.
     * 
     * @param x The value to scale by.
     * @return The scaled vector.
     */
    inline Vector3 operator*(float x);

    /**
     * Scales this vector by the given value.
     * 
     * @param x The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline Vector3& operator*=(float x);

    /**
     * Determines if this vector is less than the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is less than the given vector, false otherwise.
     */
    inline bool operator<(const Vector3& v) const;

    /**
     * Determines if this vector is equal to the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    inline bool operator==(const Vector3& v) const;
};

/**
 * Calculates the scalar product of the given vector with the given value.
 * 
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline Vector3 operator*(float x, const Vector3& v);

}

#include "Vector3.inl"

#endif
