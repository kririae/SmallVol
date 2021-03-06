#include "primitive.hpp"

#include <filesystem>
#include <memory>

#include "aabb.hpp"
#include "accel.hpp"
#include "interaction.hpp"
#include "light.hpp"
#include "plymesh.hpp"
#include "ray.hpp"
#include "shape.hpp"

SV_NAMESPACE_BEGIN

ShapePrimitive::ShapePrimitive(const std::shared_ptr<Shape>     &shape,
                               const std::shared_ptr<Material>  &material,
                               const std::shared_ptr<AreaLight> &areaLight)
    : m_shape(shape), m_material(material), m_areaLight(areaLight) {
  // assert(m_areaLight->m_shape.get() == m_shape.get());
}

AABB ShapePrimitive::getBound() const {
  return m_shape->getBound();
}

bool ShapePrimitive::intersect(const Ray &ray, SInteraction &isect) const {
  Float tHit;
  if (!m_shape->intersect(ray, tHit, isect)) return false;
  ray.m_tMax        = tHit;
  isect.m_primitive = this;
  return true;
}

// if the areaLight actually exists
AreaLight *ShapePrimitive::getAreaLight() const {
  return m_areaLight.get();
}

Material *ShapePrimitive::getMaterial() const {
  return m_material.get();
}

MeshPrimitive::MeshPrimitive(const std::string                &path,
                             const std::shared_ptr<Material>  &material,
                             const std::shared_ptr<AreaLight> &areaLight)
    : m_material(material), m_areaLight(areaLight) {
  if (!std::filesystem::exists(path))
    SErr("mesh %s is not found", path.c_str());

  m_mesh = make_TriangleMesh(path);
  assert(m_mesh->nInd % 3 == 0);
  for (int i = 0; i < m_mesh->nInd / 3; ++i)
    m_triangles.push_back(std::make_shared<Triangle>(m_mesh, i));

  // AND, construct the BVH under the mesh
  // Convert triangles to primitive
  std::vector<std::shared_ptr<Primitive>> p_triangles;
  for (size_t i = 0; i < m_triangles.size(); ++i)
    p_triangles.push_back(
        std::make_shared<ShapePrimitive>(ShapePrimitive(m_triangles[i])));
  m_accel = std::shared_ptr<NaiveBVHAccel>(new NaiveBVHAccel(p_triangles));
}

AABB MeshPrimitive::getBound() const {
  return m_accel->getBound();
}

bool MeshPrimitive::intersect(const Ray &ray, SInteraction &isect) const {
  // Accel implements the interface exactly the same as primitive, so no more
  // operation is needed
  if (!m_accel->intersect(ray, isect)) return false;
  // except this
  isect.m_primitive = this;
  return true;
}

AreaLight *MeshPrimitive::getAreaLight() const {
  return m_areaLight.get();
}

Material *MeshPrimitive::getMaterial() const {
  return m_material.get();
}

SV_NAMESPACE_END
