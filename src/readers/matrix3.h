/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
/** @author Xoppa */
#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_MATRIX3_H
#define FBXCONV_READERS_MATRIX3_H

namespace fbxconv {
namespace readers {
	// Quick and dirty matrix implementation, might not work as expected...
	template<class T> struct Matrix3 {
		T m[9];
		T &x1;
		T &x2;
		T &x3;
		T &y1;
		T &y2;
		T &y3;
		T &z1;
		T &z2;
		T &z3;

		Matrix3() : x1(m[0]), x2(m[1]), x3(m[2]), y1(m[3]), y2(m[4]), y3(m[5]), z1(m[6]), z2(m[7]), z3(m[8]) {
			idt();
		}

		Matrix3(const Matrix3 &copyFrom) : x1(m[0]), x2(m[1]), x3(m[2]), y1(m[3]), y2(m[4]), y3(m[5]), z1(m[6]), z2(m[7]), z3(m[8]) {
			set(copyFrom);
		}

		inline Matrix3 &operator=(const Matrix3 &rhs) {
			return set(rhs);
		}

		inline Matrix3 &operator*=(const Matrix3 &rhs) {
			return mul(rhs);
		}

		inline Matrix3 &operator+=(const Matrix3 &rhs) {
			return add(rhs);
		}

		Matrix3<T> &idt() {
			return set(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
		}

		Matrix3<T> &set(const Matrix3<T> &rhs) {
			return set(rhs.x1, rhs.x2, rhs.x3, rhs.y1, rhs.y2, rhs.y3, rhs.z1, rhs.z2, rhs.z3);
		}

		Matrix3<T> &set(const T &_x1, const T &_x2, const T &_x3, const T &_y1, const T &_y2, const T &_y3, const T &_z1, const T &_z2, const T &_z3) {
			x1 = _x1; x2 = _x2; x3 = _x3;
			y1 = _y1; y2 = _y2; y3 = _y3;
			z1 = _z1; z2 = _z2; z3 = _z3;
			return *this;
		}

		inline Matrix3<T> &scale(const T &x = (T)1., const T &y = (T)1., const T &z = (T)1.) {
			return mulr(x, y, z);
		}

		Matrix3<T> &rotate(const T &radians) {
			const T c = cos(radians);
			const T s = sin(radians);
			return mul(c, -s, s, c);
		}

		Matrix3<T> &translate(const T &x  = (T)0., const T &y = (T)0.) {
			return set(	x1, x2, x1 * x + x2 * y + x3,
						y1, y2, y1 * x + y2 * y + y3, 
						z1, z2, z1 * x + z2 * y + z3);
		}

		Matrix3<T> &trn(const T &x = (T)0., const T &y = (T)0.) {
			x3 += x;
			y3 += y;
			return *this;
		}

		Matrix3<T> &mul(const Matrix3<T> &rhs) {
			return mul(rhs.x1, rhs.x2, rhs.x3, rhs.y1, rhs.y2, rhs.y3, rhs.z1, rhs.z2, rhs.z3);
		}

		Matrix3<T> &mul(const T &_x1, const T &_x2, const T &_x3, const T &_y1, const T &_y2, const T &_y3, const T &_z1, const T &_z2, const T &_z3) {
			return set(	x1 * _x1 + x2 * _y1 + x3 * _z1, x1 * _x2 + x2 * _y2 + x3 * _z2, x1 * _x3 + x2 * _y3 + x3 * _z3,
						y1 * _x1 + y2 * _y1 + y3 * _z1, x2 * _x2 + y2 * _y2 + y3 * _z2, y1 * _x3 + y2 * _y3 + y3 * _z3, 
						z1 * _x1 + z2 * _y1 + z3 * _z1, x3 * _x2 + z2 * _y2 + z3 * _z2, z1 * _x3 + z2 * _y3 + z3 * _z3);
		}

		Matrix3<T> &mulr(const T &x = (T)1., const T &y = (T)1., const T &z = (T)1.) {
			return set(	x1 * x, z2 * y, x3 * z,
						y1 * x, y2 * y, y3 * z, 
						z1 * x, z2 * y, z3 * z);
		}

		Matrix3<T> &mulc(const T &x = (T)1., const T &y = (T)1., const T &z = (T)1.) {
			return set(	x1 * x, x2 * x, z3 * x,
						y1 * y, y2 * y, y3 * y, 
						z1 * z, z2 * z, z3 * z);
		}

		Matrix3<T> &mul(const T &_x1, const T &_x2, const T &_y1, const T &_y2) {
			return set(	x1 * _x1 + x2 * _y1, x1 * _x2 + x2 * _y2, x3,
						y1 * _x1 + y2 * _y1, y1 * _x2 + y2 * _y2, y3, 
						z1 * _x1 + z2 * _y1, z1 * _x2 + z2 * _y2, z3);
		}

		Matrix3<T> &add(const Matrix3<T> &rhs) {
			return add(rhs.x1, rhs.x2, rhs.x3, rhs.y1, rhs.y2, rhs.y3, rhs.z1, rhs.z2, rhs.z3);
		}

		Matrix3<T> &add(const T &_x1, const T &_x2, const T &_x3, const T &_y1, const T &_y2, const T &_y3, const T &_z1, const T &_z2, const T &_z3) {
			return set(x1 + _x1, x2 + _x2, x3 + _x3, y1 + _y1, y2 + _y2, y3 + _y3, z1 + _z1, z2 + _z2, z3 * _z3);
		}

		void transform(float &x, float &y) const {
			const float _x = x, _y = y;
			x = x1 * _x + x2 * _y + x3;
			y = y1 * _x + y2 * _y + y3;
		}

		void transform(float &x, float &y, float &z) const {
			const float _x = x, _y = y, _z = z;
			x = x1 * _x + x2 * _y + x3 * _z;
			y = y1 * _x + y2 * _y + y3 * _z;
			z = z1 * _x + z2 * _y + z3 * _z;
		}
	};
} }

#endif //FBXCONV_READERS_MATRIX3_H