#pragma once
#include "../math/vec3.h"
#include "ray.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Camera {
public:
    Camera(
        Point3 lookFrom,
        Point3 lookAt,
        Vec3   vup,
        double vfov,       // vertical field of view in degrees
        double aspectRatio,
        double aperture,
        double focusDist
        ) {
        double theta = vfov * M_PI / 180.0;
        double h = std::tan(theta / 2.0);
        double viewportHeight = 2.0 * h;
        double viewportWidth  = aspectRatio * viewportHeight;

        w = (lookFrom - lookAt).normalized();
        u = Vec3::cross(vup, w).normalized();
        v = Vec3::cross(w, u);

        origin     = lookFrom;
        horizontal = u * (viewportWidth  * focusDist);
        vertical   = v * (viewportHeight * focusDist);
        lowerLeftCorner = origin
                          - horizontal / 2.0
                          - vertical   / 2.0
                          - w * focusDist;

        lensRadius = aperture / 2.0;
    }

    Ray getRay(double s, double t) const {
        Vec3 rd     = randomInUnitDisk() * lensRadius;
        Vec3 offset = u * rd.x + v * rd.y;

        return Ray(
            origin + offset,
            lowerLeftCorner
                + horizontal * s
                + vertical   * t
                - origin - offset
            );
    }

private:
    Point3 origin;
    Point3 lowerLeftCorner;
    Vec3   horizontal, vertical;
    Vec3   u, v, w;
    double lensRadius;

    static Vec3 randomInUnitDisk() {
        while (true) {
            Vec3 p(
                ((double)rand()/RAND_MAX)*2.0 - 1.0,
                ((double)rand()/RAND_MAX)*2.0 - 1.0,
                0
                );
            if (p.lengthSquared() < 1.0) return p;
        }
    }
};
