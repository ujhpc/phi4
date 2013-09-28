#pragma once

#include "types.h"

template <int Comp = 1> class MagnetisationMeasurer {
 public:
  template <typename F> void measure(F& field) {

    Float local_mag_[Comp];
    for (int c = 0; c < Comp; c++)
      local_mag_[c] = 0.0;

    Float local_phi2_ = 0.0;

    for (int s = 0; s < field.size(); ++s) {
      for (int c = 0; c < Comp; c++) {
        Float fi = field.get(s, c);
        local_mag_[c] += fi;
        local_phi2_ += fi * fi;
      }
    }

    for (int c = 0; c < Comp; c++) {
      mag_[c] += local_mag_[c];
      xi_ += local_mag_[c] * local_mag_[c];
    }

    phi2_ += local_phi2_;
    amag_ += sqrt(local_phi2_);
    n_meas_++;
  }

  void reset() {
    n_meas_ = 0.0;
    amag_ = 0.0;
    xi_ = 0.0;
    phi2_ = 0.0;
    for (int c = 0; c < Comp; c++)
      mag_[c] = 0.0;
  }

  int n_meas() const { return n_meas_; }
  Float mag(int i = 0) const { return mag_[i] / n_meas_; }
  Float amag() const { return amag_ / n_meas_; }
  Float xi() const { return xi_ / n_meas_; }
  Float phi2() const { return phi2_ / (n_meas_); }

 private:
  int n_meas_;

  Float phi2_;
  Float amag_;
  Float mag_[Comp];
  Float xi_;
};
