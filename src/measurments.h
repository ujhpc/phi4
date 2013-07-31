#pragma once

#include "typedefs.h"

template <int N_COMP = 1> class MagnetisationMeasurer {
 public:
  template <typename F> void measure(F& field) {
    typedef typename F::indexer_t indexer_t;

    Float local_mag_[N_COMP];
    for (int k = 0; k < N_COMP; k++)
      local_mag_[k] = 0.0;

    Float local_phi2_ = 0.0;

    for (int site = 0; site < indexer_t::n_sites(); ++site) {

      for (int k = 0; k < N_COMP; k++) {
        Float fi = field.get(site, k);

        local_mag_[k] += fi;
        local_phi2_ += fi * fi;
      }
    }

    for (int k = 0; k < N_COMP; k++) {
      mag_[k] += local_mag_[k];
      xi_ += local_mag_[k] * local_mag_[k];
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

    for (int k = 0; k < N_COMP; k++)
      mag_[k] = 0.0;
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
  Float mag_[N_COMP];
  Float xi_;
};
