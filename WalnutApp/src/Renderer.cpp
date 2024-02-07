#include "Renderer.h"
#include <chrono>
#include <cmath>
#include <iostream>

namespace Utils {
static uint32_t ConvertToRGBA(const glm::vec4 &color) {
  uint8_t r = (uint8_t)(color.r * 255.0f);
  uint8_t g = (uint8_t)(color.g * 255.0f);
  uint8_t b = (uint8_t)(color.b * 255.0f);
  uint8_t a = (uint8_t)(color.a * 255.0f);

  uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
  return result;
}
} // namespace Utils

void Renderer::OnResize(uint32_t width, uint32_t height) {

  if (m_FinalImage) {
    // no resize necessary
    if (m_FinalImage->GetWidth() == width &&
        m_FinalImage->GetHeight() == height)
      return;

    m_FinalImage->Resize(width, height);
  } else {
    m_FinalImage = std::make_shared<Walnut::Image>(width, height,
                                                   Walnut::ImageFormat::RGBA);
  }

  delete[] m_ImageData;
  m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene &scene, const Camera &camera) {

  Ray ray;
  ray.origin = camera.GetPosition();

  // render every pixel
  for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
    for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
      ray.direction =
          camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

      glm::vec4 color = TraceRay(scene, ray);
      color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
      m_ImageData[x + y * m_FinalImage->GetWidth()] =
          Utils::ConvertToRGBA(color);
    }
  }

  m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene &scene, const Ray &ray) {

  if (scene.Spheres.size() == 0)
    return glm::vec4(0, 0, 0, 1);

  const Sphere *closestSphere = nullptr;
  float hitDistance = FLT_MAX;
  for (const Sphere &sphere : scene.Spheres) {

    glm::vec3 origin = ray.origin - sphere.Position;

    // rayDirection = glm::normalize(rayDirection);

    // (bx^2 + by^2)*t^2 + (2(axbx+ayby))t + (ax^2 + ay^2 - r^2) = 0
    // where a = rayOrigin, b = rayDirection, r = radius, t=hit distance

    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(origin, ray.direction);
    float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
      continue;
    }

    float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

    if (closestT < hitDistance) {
      hitDistance = closestT;
      closestSphere = &sphere;
    }
  }

  if (closestSphere == nullptr)
    return glm::vec4(0, 0, 0, 1);

  glm::vec3 origin = ray.origin - closestSphere->Position;
  glm::vec3 hitPoint = origin + ray.direction * hitDistance;
  glm::vec3 normal = glm::normalize(hitPoint);

  glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

  float d = glm::max(glm::dot(normal, -lightDir), 0.0f);

  glm::vec3 sphereColor = closestSphere->Albedo;
  sphereColor *= d;

  return glm::vec4(sphereColor, 1.0f);
}