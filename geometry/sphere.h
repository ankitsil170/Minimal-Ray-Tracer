#pragma once
#include "hittable.h"
#include <cmath>

class Sphere : public Hittable {
public:
    Point3 center;
    double radius;
    std::shared_ptr<Material> material;

    Sphere(Point3 cen, double r, std::shared_ptr<Material> mat)
        : center(cen), radius(r), material(mat) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        Vec3 oc = r.origin - center;
        double a = r.direction.lengthSquared();
        double halfB = Vec3::dot(oc, r.direction);
        double c = oc.lengthSquared() - radius * radius;
        double discriminant = halfB * halfB - a * c;

        if (discriminant < 0) return false;

        double sqrtD = std::sqrt(discriminant);
        double root  = (-halfB - sqrtD) / a;

        if (root < tMin || tMax < root) {
            root = (-halfB + sqrtD) / a;
            if (root < tMin || tMax < root) return false;
        }

        rec.t     = root;
        rec.point = r.at(rec.t);
        Vec3 outwardNormal = (rec.point - center) / radius;
        rec.setFaceNormal(r, outwardNormal);
        rec.material = material;
        return true;
    }
};
