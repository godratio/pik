// Copyright 2019 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef PIK_NOISE_DISTRIBUTIONS_H_
#define PIK_NOISE_DISTRIBUTIONS_H_

// Noise distributions for testing partial_derivatives and robust_statistics.

#include <stddef.h>
#include <stdint.h>
#include <random>  // distributions
#include <string>

#include "pik/image.h"

namespace pik {

// Unmodified input
struct NoiseNone {
  std::string Name() const { return "None"; }

  template <class Random>
  float operator()(const float in, Random* rng) const {
    return in;
  }
};

// Salt+pepper
class NoiseImpulse {
 public:
  NoiseImpulse(const uint32_t threshold) : threshold_(threshold) {}
  std::string Name() const { return "Impulse" + std::to_string(threshold_); }

  // Sets pixels to 0 if rand < threshold or 1 if rand > ~threshold.
  template <class Random>
  float operator()(const float in, Random* rng) const {
    const uint32_t rand = (*rng)();
    float out = 0.0f;
    if (rand > ~threshold_) {
      out = 1.0f;
    }
    if (rand > threshold_) {
      out = in;
    }
    return out;
  }

 private:
  const uint32_t threshold_;
};

class NoiseUniform {
 public:
  NoiseUniform(const float min, const float max_exclusive)
      : dist_(min, max_exclusive) {}
  std::string Name() const { return "Uniform" + std::to_string(dist_.b()); }

  template <class Random>
  float operator()(const float in, Random* rng) const {
    return in + dist_(*rng);
  }

 private:
  mutable std::uniform_real_distribution<float> dist_;
};

// Additive, zero-mean Gaussian.
class NoiseGaussian {
 public:
  NoiseGaussian(const float stddev) : dist_(0.0f, stddev) {}
  std::string Name() const {
    return "Gaussian" + std::to_string(dist_.stddev());
  }

  template <class Random>
  float operator()(const float in, Random* rng) const {
    return in + dist_(*rng);
  }

 private:
  mutable std::normal_distribution<float> dist_;
};

// Integer noise is scaled by 1E-3.
class NoisePoisson {
 public:
  NoisePoisson(const double mean) : dist_(mean) {}
  std::string Name() const { return "Poisson" + std::to_string(dist_.mean()); }

  template <class Random>
  float operator()(const float in, Random* rng) const {
    return in + dist_(*rng) * 1E-3f;
  }

 private:
  mutable std::poisson_distribution<int> dist_;
};

// Returns the result of applying the randomized "noise" function to each pixel.
template <class NoiseType, class Random>
ImageF AddNoise(const ImageF& in, const NoiseType& noise, Random* rng) {
  const size_t xsize = in.xsize();
  const size_t ysize = in.ysize();
  ImageF out(xsize, ysize);
  for (size_t y = 0; y < ysize; ++y) {
    const float* PIK_RESTRICT in_row = in.ConstRow(y);
    float* PIK_RESTRICT out_row = out.Row(y);
    for (size_t x = 0; x < xsize; ++x) {
      out_row[x] = noise(in_row[x], rng);
    }
  }
  return out;
}

template <class NoiseType, class Random>
Image3F AddNoise(const Image3F& in, const NoiseType& noise, Random* rng) {
  const size_t xsize = in.xsize();
  const size_t ysize = in.ysize();
  Image3F out(xsize, ysize);
  // noise_estimator_test requires this loop order.
  for (size_t c = 0; c < 3; ++c) {
    for (size_t y = 0; y < ysize; ++y) {
      const float* PIK_RESTRICT in_row = in.ConstPlaneRow(c, y);
      float* PIK_RESTRICT out_row = out.PlaneRow(c, y);

      for (size_t x = 0; x < xsize; ++x) {
        out_row[x] = noise(in_row[x], rng);
      }
    }
  }
  return out;
}

}  // namespace pik

#endif  // PIK_NOISE_DISTRIBUTIONS_H_
