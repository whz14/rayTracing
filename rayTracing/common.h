#pragma once
#ifndef COMMON_H
#define COMMON_H
#include <math.h>
#include <iostream>
#include <cstring>
//#include <cassert>
#include <climits>
#include <cstdlib>
#include <opencv2/core/matx.hpp>

const double EPS = 1e-4;
const double INF = 1e10;
const double PI = 2 * acos(0);
const int TRACEDEPTH = 6;
inline double rand1(double range) { return rand() * 1.0 / RAND_MAX * range; }

class vec3 {
public:
#define x1 v.x
#define y1 v.y
#define z1 v.z
	vec3() : x(0), y(0), z(0) {}
	vec3(const vec3& from) : x(from.x), y(from.y), z(from.z) {};
	vec3(double a, double b, double c) : x(a), y(b), z(c) {}

	double length() { return sqrt(x*x + y*y + z*z); }
	double sqrLength() { return x*x + y*y + z*z; }
	double dotPro(const vec3& v) { return x * x1 + y * y1 + z * z1; }
	vec3 crossPro(const vec3& v) { return vec3(y*z1 - z * y1, z*x1 - x*z1, x*y1 - y*x1); }

	void operator += (const vec3& v) { x += x1, y += y1, z += z1; }
	void operator -= (const vec3& v) { x -= x1, y -= y1, z -= z1; }
	void operator *= (double d) { x *= d, y *= d, z *= d; }
	void operator /= (double d) {/* assert(d > EPS);*/ x /= d, y /= d, z /= d; }
	vec3 operator - () { return vec3(-x, -y, -z); }
	friend vec3 operator + (const vec3& v, const vec3& v1) { return vec3(x1 + v1.x, y1 + v1.y, z1 + v1.z); }
	friend vec3 operator - (const vec3& v, const vec3& v1) { return vec3(x1 - v1.x, y1 - v1.y, z1 - v1.z); }
	friend vec3 operator * (const vec3& v, const vec3& v1) { return vec3(x1*v1.x, y1*v1.y, z1*v1.z); }	// behaviour same as .* in matlab
	friend vec3 operator * (double d, const vec3& v) { return vec3(x1 * d, y1 * d, z1 * d); }
	friend vec3 operator / (const vec3& v, double d) { return vec3(x1 / d, y1 / d, z1 / d); }
	friend std::ostream& operator << (std::ostream& out, vec3 v) {
		out << (int)(255*v.x) << " " << (int)(255 * v.y) << " " << (int)(255 * v.z);
		return out;
	}
	vec3 normlize() {
		double l = length(); 
		//assert(l >= EPS);
		x /= l; y /= l; z /= l;
		return *this;
	}
	double& operator [](unsigned int i) {
		if(i == 0) {
			return x;
		}
		else if(i = 1) {
			return y;
		}
		else if(i == 2) {
			return z;
		}
	}
	const double& operator[](unsigned int i) const {
		if(i == 0) {
			return x;
		}
		else if(i = 1) {
			return y;
		}
		else if(i == 2) {
			return z;
		}
	}

#undef x1
#undef y1
#undef z1

	union {
		struct { double x, y, z; };
		struct { double r, g, b; };
		struct { double cell[3]; };
	};
};

class plane {
public:
	union{
		struct {
			vec3 nvec;
			double D;
		};
		double cell[4];
	};
	plane() : nvec(0, 0, 0), D(0) {};
	plane(const vec3& v, double d) : nvec(v), D(d) {}
};
typedef vec3 Color;
typedef unsigned int Piaxl;

class aabb {
	vec3 posi, size;
public:
	aabb(const vec3& pos, const vec3& siz) :posi(pos), size(siz) {}
	aabb() {}
	aabb(const aabb& b1) :posi(b1.getPosi()), size(b1.getSize()) {}
	const vec3& getPosi() const {
		return posi;
	}
	const vec3& getSize() const {
		return size;
	}
	vec3& getPosi() {
		return posi;
	}
	vec3& getSize() {
		return size;
	}
	bool intersect(const aabb& box2) {
		vec3 v1 = box2.getPosi(), v2 = box2.getSize() + box2.getPosi();
		#define v3 posi
		vec3 v4 = posi + size;
		return v4.x >= v1.x && v2.x >= v3.x &&
			v4.y >= v1.y && v2.y >= v3.y &&
			v4.z >= v1.z && v2.z >= v3.z;
		#undef v3
	}
	bool contains(const vec3& p) {
		vec3 ru = posi + size;		// right-up
		return p.x <= ru.x && p.x >= posi.x &&
			p.y <= ru.y && p.y >= posi.y &&
			p.z <= ru.z && p.z >= posi.z;
	}
};

#endif