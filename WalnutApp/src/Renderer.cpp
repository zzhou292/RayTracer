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

void Renderer::Render() {
  // render every pixel
  for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
    for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
      glm::vec2 coord = {(float)x / (float)m_FinalImage->GetWidth(),
                         (float)y / (float)m_FinalImage->GetHeight()};
      coord = coord * 2.0f - 1.0f;
      glm::vec4 color = PerPixel(coord);
      color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
      m_ImageData[x + y * m_FinalImage->GetWidth()] =
          Utils::ConvertToRGBA(color);
    }
  }

  m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord) {
  uint8_t r = (uint8_t)(coord.x * 255.0f);
  uint8_t g = (uint8_t)(coord.y * 255.0f);

  glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
  glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
  float radius = 0.5f;
  // rayDirection = glm::normalize(rayDirection);

  // (bx^2 + by^2)*t^2 + (2(axbx+ayby))t + (ax^2 + ay^2 - r^2) = 0
  // where a = rayOrigin, b = rayDirection, r = radius, t=hit distance

  float a = glm::dot(rayDirection, rayDirection);
  float b = 2.0f * glm::dot(rayOrigin, rayDirection);
  float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

  float discriminant = b * b - 4.0f * a * c;
  if (discriminant < 0) {
    return glm::vec4(0, 0, 0, 1);
  }

  float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
  float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

  glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
  glm::vec3 normal = glm::normalize(hitPoint);

  glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

  float d = glm::max(glm::dot(normal, -lightDir), 0.0f);

  auto now = std::chrono::high_resolution_clock::now();
  // Convert time to seconds
  auto timeInMiliSeconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          now.time_since_epoch())
          .count();

  // Calculate sine and cosine based on time
  float sineValue = std::sin(timeInMiliSeconds / 1000.0f);
  float cosineValue = std::cos(timeInMiliSeconds / 1000.0f);

  glm::vec3 sphereColor(sineValue, cosineValue, 0.8);
  sphereColor *= d;

  return glm::vec4(sphereColor, 1);
}