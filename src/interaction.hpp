#ifndef __INTERACTION_HPP__
#define __INTERACTION_HPP__

#include "fwd.hpp"
#include "ray.hpp"

SV_NAMESPACE_BEGIN

class Primitive;

class Interaction {
public:
  Interaction() = default;
  Interaction(const Vector3f &p, const Vector3f &ns, const Vector3f &ng,
              const Vector3f &wo)
      : m_p(p), m_ns(ns), m_ng(ng), m_wo(wo) {}
  virtual ~Interaction() = default;
  virtual void reset() { m_p = m_ns = m_ng = m_wo = Vector3f::Zero(); }
  virtual Ray  SpawnRay(const Vector3f &d) {
    return {m_p + m_ng * SHADOW_EPS, d.normalized()};
  }
  virtual Ray SpawnRayTo(const Vector3f &p) {
    Float norm = (p - m_p).norm();
    return {m_p + m_ng * SHADOW_EPS, (p - m_p) / norm, norm - SHADOW_EPS};
  }
  virtual Ray SpawnRayTo(const Interaction &it) { return SpawnRayTo(it.m_p); }

  // shading normal, geometry normal
  Vector3f m_p, m_ns, m_ng, m_wo;

private:
};

class SInteraction : public Interaction {
public:
  SInteraction() = default;
  SInteraction(const Vector3f &p, const Vector3f &ns, const Vector3f &ng,
               const Vector3f &wo)
      : Interaction(p, ns, ng, wo) {}
  virtual ~SInteraction() = default;
  virtual Vector3f Le(const Vector3f &w) const;

  const Primitive *m_primitive;
};

class VInteraction : public Interaction {
public:
  VInteraction() = default;
  VInteraction(const Vector3f &p, const Vector3f &wo, Float g)
      : Interaction(p, Vector3f::Zero(), Vector3f::Zero(), wo), m_g(g) {}
  virtual ~VInteraction() = default;

  Float m_g;
};

SV_NAMESPACE_END

#endif
