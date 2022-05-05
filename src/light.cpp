#include "light.hpp"

#include "fwd.hpp"
#include "interaction.hpp"

SV_NAMESPACE_BEGIN

// interface for infinite light
Vector3f Light::sampleLe() const {
  return Vector3f::Zero();
}

Float Light::pdfLe() const {
  return 0.0;
}

AreaLight::AreaLight(const std::shared_ptr<Shape> &shape, const Vector3f &Le)
    : m_shape(shape), m_Le(Le) {}

Vector3f AreaLight::sampleLi(const Interaction &ref, const Vector2f &u,
                             Vector3f &wi, Float &pdf,
                             Interaction &sample) const {
  sample = m_shape->sample(u, pdf);
  if (pdf == 0) return Vector3f::Zero();
  wi = (sample.m_p - ref.m_p).normalized();
  return L(sample, -wi);
}

Float AreaLight::pdfLi(const Interaction &isect) const {
  return m_shape->pdf(isect);
}

Vector3f AreaLight::L(const Interaction &isect, const Vector3f &w) const {
  // isect.wo must be initialized
  return isect.m_ng.dot(w) > 0 ? m_Le : Vector3f::Zero();
}

SV_NAMESPACE_END
