#pragma once
#include "../core/ray.h"
#include "material.h"
#include <vector>
#include <memory>

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};

class HittableList : public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    void add(std::shared_ptr<Hittable> obj) { objects.push_back(obj); }
    void clear() { objects.clear(); }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        HitRecord tempRec;
        bool hitAnything = false;
        double closestSoFar = tMax;

        for (const auto& obj : objects) {
            if (obj->hit(r, tMin, closestSoFar, tempRec)) {
                hitAnything  = true;
                closestSoFar = tempRec.t;
                rec          = tempRec;
            }
        }
        return hitAnything;
    }
};
