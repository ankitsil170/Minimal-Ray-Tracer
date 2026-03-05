#pragma once
#include "../math/vec3.h"
#include "../core/ray.h"
#include <cmath>
#include <random>

struct HitRecord;  // forward declaration

class Material {
public:
    virtual ~Material() = default;
    virtual bool scatter(
        const Ray& rIn,
        const HitRecord& rec,
        Color& attenuation,
        Ray& scattered
        ) const = 0;
};

struct HitRecord {
    Point3 point;
    Vec3   normal;
    std::shared_ptr<Material> material;
    double t    = 0.0;
    bool frontFace = true;

    void setFaceNormal(const Ray& r, const Vec3& outwardNormal) {
        frontFace = Vec3::dot(r.direction, outwardNormal) < 0;
        normal    = frontFace ? outwardNormal : -outwardNormal;
    }
};

// ──────────────────────────────────────────────
// Lambertian (matte / diffuse)
// ──────────────────────────────────────────────
class Lambertian : public Material {
public:
    Color albedo;
    explicit Lambertian(const Color& a) : albedo(a) {}

    bool scatter(const Ray&, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override {
        Vec3 scatterDir = rec.normal + Vec3::randomUnitVector();
        if (scatterDir.nearZero()) scatterDir = rec.normal;
        scattered   = Ray(rec.point, scatterDir);
        attenuation = albedo;
        return true;
    }
};

// ──────────────────────────────────────────────
// Metal (reflective)
// ──────────────────────────────────────────────
class Metal : public Material {
public:
    Color  albedo;
    double fuzz;

    Metal(const Color& a, double f) : albedo(a), fuzz(std::min(f, 1.0)) {}

    bool scatter(const Ray& rIn, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override {
        Vec3 reflected = Vec3::reflect(rIn.direction.normalized(), rec.normal);
        scattered   = Ray(rec.point, reflected + Vec3::randomInUnitSphere() * fuzz);
        attenuation = albedo;
        return Vec3::dot(scattered.direction, rec.normal) > 0;
    }
};

// ──────────────────────────────────────────────
// Dielectric (glass / refractive)
// ──────────────────────────────────────────────
class Dielectric : public Material {
public:
    double ir;  // index of refraction
    explicit Dielectric(double indexOfRefraction) : ir(indexOfRefraction) {}

    bool scatter(const Ray& rIn, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        double refractionRatio = rec.frontFace ? (1.0 / ir) : ir;

        Vec3 unitDir = rIn.direction.normalized();
        double cosTheta = std::fmin(Vec3::dot(-unitDir, rec.normal), 1.0);
        double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        Vec3 direction;

        if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble())
            direction = Vec3::reflect(unitDir, rec.normal);
        else
            direction = Vec3::refract(unitDir, rec.normal, refractionRatio);

        scattered = Ray(rec.point, direction);
        return true;
    }

private:
    static double randomDouble() {
        static std::mt19937 rng(42);
        static std::uniform_real_distribution<double> d(0.0, 1.0);
        return d(rng);
    }
    static double reflectance(double cosine, double refIdx) {
        double r0 = (1 - refIdx) / (1 + refIdx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
};
