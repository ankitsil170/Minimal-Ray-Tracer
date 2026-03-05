#include "vec3.h"
#include <random>

static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<double> dist(0.0, 1.0);

static double randomDouble() { return dist(rng); }
static double randomDouble(double mn, double mx) { return mn + (mx - mn) * randomDouble(); }

Vec3 Vec3::randomInUnitSphere() {
    while (true) {
        Vec3 p(randomDouble(-1,1), randomDouble(-1,1), randomDouble(-1,1));
        if (p.lengthSquared() < 1.0) return p;
    }
}

Vec3 Vec3::randomUnitVector() {
    return randomInUnitSphere().normalized();
}
