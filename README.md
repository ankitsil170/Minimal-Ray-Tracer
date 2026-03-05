# Minimal-Ray-Tracer

A minimal **CPU-based path tracer built from scratch in C++17** with a Qt-powered live preview interface.

This project demonstrates the fundamentals of **ray tracing and physically-based rendering**, including ray-object intersection, material scattering, and Monte Carlo sampling. The goal of this project is to keep the rendering engine **small, readable, and educational** while still producing visually correct results.

---

## Features

- CPU-based **Monte Carlo path tracer**
- **Lambertian (diffuse) materials**
- **Metallic reflection with fuzz control**
- **Dielectric (glass) refraction**
- **Anti-aliasing via multi-sampling**
- **Gamma correction**
- **Procedural scene generation**
- **Qt GUI with live rendering preview**
- **Progress tracking during rendering**
- **Image export (PNG / JPEG / PPM)**

---

## How It Works

Ray tracing simulates the physical behavior of light. For every pixel in the image, the renderer shoots a ray from the camera into the scene.

The ray travels through the scene until it intersects with an object. At the hit point, the surface material determines how the ray behaves:

- **Diffuse materials** scatter the ray in random directions.
- **Metal materials** reflect rays based on the reflection equation.
- **Dielectric materials** refract light using Snell's law.

Multiple samples are taken per pixel and averaged together to reduce noise and produce a realistic image.

---

## Project Structure

```
RayTracer/
│
├── CMakeLists.txt
├── main.cpp
│
├── math/
│  ├── vec3.h
│  └── vec3.cpp
│  
├── core/
│   ├── ray.h
│   └── camera.h
│
├── geometry/
│   ├── hittable.h
│   ├── sphere.h
│   └── material.h
│ 
├── rendering/
│   ├── renderer.h
│   └── scene_builder.h
│
├── mainwindow.h
└── mainwindow.cpp
```

The renderer core is separated into modules for **math utilities, scene geometry, rendering logic, and UI** so the engine can easily be extended.

---
