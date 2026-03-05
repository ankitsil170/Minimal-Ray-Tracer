#pragma once
#include "../geometry/hittable.h"
#include "../core/camera.h"
#include "../math/vec3.h"
#include <functional>
#include <vector>
#include <random>
#include <cmath>
#include <atomic>

class Renderer {
public:
    int  imageWidth    = 800;
    int  imageHeight   = 400;
    int  samplesPerPixel = 50;
    int  maxDepth      = 10;

    // Progress callback: called with (scanline, totalScanlines)
    using ProgressCb = std::function<void(int, int)>;
    // Pixel callback: called per pixel (x, y, r, g, b 0-255)
    using PixelCb = std::function<void(int, int, uint8_t, uint8_t, uint8_t)>;

    std::atomic<bool> cancelled{false};

    std::vector<uint8_t> render(
        const HittableList& world,
        const Camera& camera,
        ProgressCb progressCb = nullptr,
        PixelCb    pixelCb    = nullptr
        ) {
        std::vector<uint8_t> image(imageWidth * imageHeight * 3);
        cancelled = false;

        for (int j = imageHeight - 1; j >= 0 && !cancelled; --j) {
            if (progressCb) progressCb(imageHeight - 1 - j, imageHeight);

            for (int i = 0; i < imageWidth && !cancelled; ++i) {
                Color pixelColor(0, 0, 0);
                for (int s = 0; s < samplesPerPixel; ++s) {
                    double u = (i + randomDouble()) / (imageWidth  - 1);
                    double v = (j + randomDouble()) / (imageHeight - 1);
                    Ray r = camera.getRay(u, v);
                    pixelColor += rayColor(r, world, maxDepth);
                }
                auto [r, g, b] = toRGB(pixelColor, samplesPerPixel);
                int idx = ((imageHeight - 1 - j) * imageWidth + i) * 3;
                image[idx]   = r;
                image[idx+1] = g;
                image[idx+2] = b;
                if (pixelCb) pixelCb(i, imageHeight - 1 - j, r, g, b);
            }
        }
        return image;
    }

private:
    static double randomDouble() {
        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::uniform_real_distribution<double> d(0.0, 1.0);
        return d(rng);
    }

    Color rayColor(const Ray& r, const Hittable& world, int depth) {
        if (depth <= 0) return Color(0, 0, 0);

        HitRecord rec;
        if (world.hit(r, 0.001, 1e9, rec)) {
            Ray   scattered;
            Color attenuation;
            if (rec.material->scatter(r, rec, attenuation, scattered))
                return attenuation * rayColor(scattered, world, depth - 1);
            return Color(0, 0, 0);
        }

        // Sky gradient
        Vec3 unit = r.direction.normalized();
        double t  = 0.5 * (unit.y + 1.0);
        return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.4, 0.6, 1.0) * t;
    }

    static std::tuple<uint8_t,uint8_t,uint8_t> toRGB(Color c, int samples) {
        double scale = 1.0 / samples;
        double r = std::sqrt(c.x * scale);
        double g = std::sqrt(c.y * scale);
        double b = std::sqrt(c.z * scale);
        auto clamp = [](double v) -> uint8_t {
            return static_cast<uint8_t>(256 * std::min(std::max(v, 0.0), 0.999));
        };
        return {clamp(r), clamp(g), clamp(b)};
    }
};
