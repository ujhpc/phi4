#ifndef _MEASURMENTS_H__
#define _MEASURMENTS_H__



class MagnetisationMeasurer {
public:
  template<typename F> 
  void measure(F &field) {
    typedef typename F::indexer_t indexer_t;

    local_mag_ = 0.0;
    local_phi2_= 0.0;
    for(int site=0;site<indexer_t::n_sites();++site) { 
      double fi=field[site];
      local_mag_+=fi;
      local_phi2_+= fi*fi;
    }

    amag_ += fabs(local_mag_);
    mag_  += local_mag_;
    xi_   += local_mag_*local_mag_;
    phi2_+=local_phi2_;
    n_meas_++;
  }

  void reset() {
    n_meas_= 0.0;
    amag_  = 0.0;
    mag_   = 0.0;
    xi_    = 0.0;
    phi2_  = 0.0;

  }

  int    n_meas() const {return n_meas_;}
  double mag() const { return mag_/n_meas_;}
  double amag() const { return amag_/n_meas_;}
  double xi() const { return xi_/n_meas_;}
  double phi2() const { return phi2_/(n_meas_);}

private:
  int n_meas_;

  double local_mag_;
  double local_phi2_;

  double phi2_;
  double amag_;
  double mag_;
  double xi_;

};


#endif
