#pragma once
#include <cmath>
#include <iostream>

class Vec3 {
public:
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(double t)       const { return {x * t, y * t, z * t}; }
    Vec3 operator*(const Vec3& v)  const { return {x * v.x, y * v.y, z * v.z}; }
    Vec3 operator/(double t)       const { return {x / t, y / t, z / t}; }
    Vec3 operator-()               const { return {-x, -y, -z}; }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator*=(double t)      { x *= t;   y *= t;   z *= t;   return *this; }

    double length()        const { return std::sqrt(lengthSquared()); }
    double lengthSquared() const { return x*x + y*y + z*z; }

    Vec3 normalized() const {
        double len = length();
        return len > 0 ? *this / len : Vec3(0,0,0);
    }

    bool nearZero() const {
        const double s = 1e-8;
        return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
    }

    static double dot(const Vec3& a, const Vec3& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y*b.z - a.z*b.y,
            a.z*b.x - a.x*b.z,
            a.x*b.y - a.y*b.x
        };
    }

    static Vec3 reflect(const Vec3& v, const Vec3& n) {
        return v - n * (2 * dot(v, n));
    }

    static Vec3 refract(const Vec3& uv, const Vec3& n, double etaiOverEtat) {
        double cosTheta = std::fmin(dot(-uv, n), 1.0);
        Vec3 rOutPerp = (uv + n * cosTheta) * etaiOverEtat;
        Vec3 rOutParallel = n * (-std::sqrt(std::fabs(1.0 - rOutPerp.lengthSquared())));
        return rOutPerp + rOutParallel;
    }

    static Vec3 randomInUnitSphere();
    static Vec3 randomUnitVector();
};

inline Vec3 operator*(double t, const Vec3& v) { return v * t; }

// Color alias
using Color = Vec3;
using Point3 = Vec3;
