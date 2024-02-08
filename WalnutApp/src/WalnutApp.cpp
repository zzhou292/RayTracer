#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
public:
  ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f) {

    Material &pinkSphere = m_Scene.Materials.emplace_back();
    pinkSphere.Albedo = {1.0f, 0.0f, 1.0f};
    pinkSphere.Roughness = 0.0f;

    Material &blueSphere = m_Scene.Materials.emplace_back();
    blueSphere.Albedo = {0.2f, 0.3f, 1.0f};
    blueSphere.Roughness = 0.1f;

    {
      Sphere sphere;
      sphere.Position = {0.0f, 0.0f, 0.0f};
      sphere.Radius = 1.0f;
      sphere.MaterialIndex = 0;
      m_Scene.Spheres.push_back(sphere);
    }

    {
      Sphere sphere;
      sphere.Position = {0.0f, -101.0f, 0.0f};
      sphere.Radius = 100.0f;
      sphere.MaterialIndex = 1;
      m_Scene.Spheres.push_back(sphere);
    }

    {
      Sphere sphere;
      sphere.Position = {0.2f, 0.0f, 4.0f};
      sphere.Radius = 0.8f;
      sphere.MaterialIndex = 0;
      m_Scene.Spheres.push_back(sphere);
    }
  }

  virtual void OnUpdate(float ts) override {

    if (m_Camera.OnUpdate(ts)) {
      m_Renderer.ResetFrameIndex();
    };
  }

  virtual void OnUIRender() override {
    ImGui::Begin("Settings");
    ImGui::Text("Last Render Time: %.2fms", m_LastRenderTime);
    if (ImGui::Button("Render")) {
      Render();
    };

    ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

    ImGui::End();

    ImGui::Begin("Scene");
    for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {

      ImGui::PushID(i);
      ImGui::Text("Sphere %d", i);
      ImGui::DragFloat3("Position %d",
                        glm::value_ptr(m_Scene.Spheres[i].Position), 0.1f);
      ImGui::DragFloat("Radius %d", &m_Scene.Spheres[i].Radius, 0.1f);
      ImGui::DragInt("Material %d", &m_Scene.Spheres[i].MaterialIndex, 1, 0,
                     m_Scene.Materials.size() - 1);

      ImGui::Separator();
      ImGui::PopID();
    }

    for (size_t i = 0; i < m_Scene.Materials.size(); i++) {
      ImGui::PushID(i);
      ImGui::Text("Material %d", i);
      ImGui::ColorEdit3("Albedo %d",
                        glm::value_ptr(m_Scene.Materials[i].Albedo));
      ImGui::DragFloat("Metallic %d", &m_Scene.Materials[i].Metallic, 0.05f,
                       0.0f, 1.0f);
      ImGui::DragFloat("Roughness %d", &m_Scene.Materials[i].Roughness, 0.05f,
                       0.0f, 1.0f);

      ImGui::Separator();
      ImGui::PopID();
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    m_ViewportWidth = ImGui::GetContentRegionAvail().x;
    m_ViewportHeight = ImGui::GetContentRegionAvail().y;

    auto image = m_Renderer.GetFinalImage();
    if (image)
      ImGui::Image(image->GetDescriptorSet(),
                   {(float)image->GetWidth(), (float)image->GetHeight()},
                   ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
    ImGui::PopStyleVar();

    Render();
  }

  void Render() {
    Timer timer;

    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render(m_Scene, m_Camera);

    m_LastRenderTime = timer.ElapsedMillis();
  }

private:
  Renderer m_Renderer;
  Camera m_Camera;
  Scene m_Scene;
  uint32_t m_ViewportWidth = 0;
  uint32_t m_ViewportHeight = 0;

  float m_LastRenderTime = 0.0f;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv) {
  Walnut::ApplicationSpecification spec;
  spec.Name = "Json Ray Tracer";

  Walnut::Application *app = new Walnut::Application(spec);
  app->PushLayer<ExampleLayer>();
  app->SetMenubarCallback([app]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
  });
  return app;
}