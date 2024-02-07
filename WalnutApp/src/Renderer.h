#pragma once

#include "Ray.h"
#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include <memory>

#include "Camera.h"

class Renderer {
public:
  Renderer() = default;

  void OnResize(uint32_t width, uint32_t height);
  void Render(const Camera &camera);

  std::shared_ptr<Walnut::Image> GetFinalImage() { return m_FinalImage; }

private:
  glm::vec4 TraceRay(const Ray &ray);

  std::shared_ptr<Walnut::Image> m_FinalImage;
  uint32_t *m_ImageData = nullptr;
};