#pragma once
#include "../geometry/hittable.h"
#include "../geometry/sphere.h"
#include "../geometry/material.h"
#include <memory>
#include <random>

class SceneBuilder {
public:
    // Classic Cornell-style scene: ground + 3 spheres
    static HittableList buildDefault() {
        HittableList world;

        auto groundMat = std::make_shared<Lambertian>(Color(0.48, 0.76, 0.46));
        auto centerMat = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.3));
        auto leftMat   = std::make_shared<Dielectric>(1.5);
        auto rightMat  = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

        world.add(std::make_shared<Sphere>(Point3( 0.0, -100.5, -1.0), 100.0, groundMat));
        world.add(std::make_shared<Sphere>(Point3( 0.0,    0.0, -1.0),   0.5, centerMat));
        world.add(std::make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, leftMat));
        world.add(std::make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),  -0.45, leftMat)); // hollow
        world.add(std::make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, rightMat));

        return world;
    }

    // Random scene (many spheres)
    static HittableList buildRandom(int seed = 42) {
        HittableList world;
        std::mt19937 rng(seed);
        auto rnd = [&](double mn=0.0, double mx=1.0){
            return mn + (mx-mn) * std::uniform_real_distribution<double>(0,1)(rng);
        };

        auto groundMat = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
        world.add(std::make_shared<Sphere>(Point3(0,-1000,0), 1000, groundMat));

        for (int a = -6; a < 6; a++) {
            for (int b = -6; b < 6; b++) {
                double chooseMat = rnd();
                Point3 center(a + 0.9*rnd(), 0.2, b + 0.9*rnd());

                if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                    std::shared_ptr<Material> mat;
                    if (chooseMat < 0.7) {
                        Color albedo(rnd()*rnd(), rnd()*rnd(), rnd()*rnd());
                        mat = std::make_shared<Lambertian>(albedo);
                    } else if (chooseMat < 0.9) {
                        Color albedo(rnd(0.5,1), rnd(0.5,1), rnd(0.5,1));
                        mat = std::make_shared<Metal>(albedo, rnd(0, 0.5));
                    } else {
                        mat = std::make_shared<Dielectric>(1.5);
                    }
                    world.add(std::make_shared<Sphere>(center, 0.2, mat));
                }
            }
        }

        world.add(std::make_shared<Sphere>(Point3( 0,1,0), 1.0, std::make_shared<Dielectric>(1.5)));
        world.add(std::make_shared<Sphere>(Point3(-4,1,0), 1.0, std::make_shared<Lambertian>(Color(0.4,0.2,0.1))));
        world.add(std::make_shared<Sphere>(Point3( 4,1,0), 1.0, std::make_shared<Metal>(Color(0.7,0.6,0.5), 0.0)));

        return world;
    }
};
