template <typename F> class Updater {
 public:
  typedef typename F::indexer_t indexer_t;

  Updater(F& f, const parameters<Float>& pars)
      : field(f),
        pars_(pars),
        quadratic_coef_1(indexer_t::D + pars.m_2 / 2.0),
        quadratic_coef_2(indexer_t::D * (1.0 + 2.0 * indexer_t::D) *
                         pars.i_Lambda),
        quadratic_coef(quadratic_coef_1 + quadratic_coef_2),
        gr(pars.g / 24.0) {

    set_epsilon(1.0);
  }

  void set_epsilon(Float epsilon) {
    epsilon_ = epsilon / std::sqrt(F::n_components);
  }

  int operator()(int* indices) {
#ifdef _OPENMP
    const int tid = omp_get_thread_num();
#else
    const int tid = 0;
#endif

    IVec i(indices);
    FVec corona[F::n_components];
    FVec phi[F::n_components];
    FVec phi2(Float(0));
    FVec action(Float(0));

    for (int k = 0; k < F::n_components; ++k) {
      FVec small_corona(Float(0));
      // NOTE: big_corona_01 comes from exactly same values as small_corona,
      // so there is no sense to count it again
      FVec& big_corona_01 = small_corona;
      FVec big_corona_02(Float(0));
      FVec big_corona_11(Float(0));

      for (int mu = 0; mu < indexer_t::D; mu++) {
        IVec up = indexer_t::up(i, mu);
        IVec dn = indexer_t::dn(i, mu);
        big_corona_02 += field.template get<FVec>(indexer_t::up(up, mu), k);
        big_corona_02 += field.template get<FVec>(indexer_t::dn(dn, mu), k);
        small_corona += field.template get<FVec>(up, k);
        small_corona += field.template get<FVec>(dn, k);

        for (int nu = 0; nu < mu; nu++) {
          big_corona_11 += field.template get<FVec>(indexer_t::up(up, nu), k);
          big_corona_11 += field.template get<FVec>(indexer_t::dn(up, nu), k);
          big_corona_11 += field.template get<FVec>(indexer_t::dn(dn, nu), k);
          big_corona_11 += field.template get<FVec>(indexer_t::up(dn, nu), k);
        }
      }

      FVec big_corona =
          FVec(-pars_.i_Lambda) *
          (big_corona_02 -
           FVec(Float(4.0)) * FVec((Float)indexer_t::D) * big_corona_01 +
           FVec(Float(2.0)) * big_corona_11);
      corona[k] = small_corona + big_corona;

      phi[k] = field.template get<FVec>(i, k);
      phi2 += phi[k] * phi[k];
      action += corona[k] * phi[k];
    }

    action -= (FVec(quadratic_coef) + FVec(gr) * phi2) * phi2;

    FVec randsym[N_HIT][F::n_components];
    FVec randlog[N_HIT];

    IVec accepted(0);
    for (int h = 0; h < N_HIT; h++)
      for (int k = 0; k < F::n_components; k++)
        randsym[h][k] = RAND_SYM(tid);

    for (int h = 0; h < N_HIT; h++)
      randlog[h] = RAND(tid);
    for (int h = 0; h < N_HIT; h++)
      randlog[h] = log(randlog[h]);

    for (int h = 0; h < N_HIT; h++) {
      FVec new_phi[F::n_components];
      new_phi[0] = phi[0] + FVec(epsilon_) * randsym[h][0];

      FVec new_phi2 = new_phi[0] * new_phi[0];
      FVec new_action = corona[0] * new_phi[0];

      for (int k = 1; k < F::n_components; k++) {
        new_phi[k] = phi[k] + FVec(epsilon_) * randsym[h][k];
        new_phi2 += new_phi[k] * new_phi[k];
        new_action += corona[k] * new_phi[k];
      }

      new_action -=
          (FVec(quadratic_coef) + FVec(gr) * FVec(new_phi2)) * new_phi2;
      FVec delta_action = new_action - action;

      IVec action_cond = delta_action >= min(Float(0), randlog[h]);

      for (int k = 0; k < F::n_components; k++)
        phi[k] = action_cond(new_phi[k], phi[k]);

      action = action_cond(new_action, action);
      accepted += action_cond(Float(1));
    }

    // Scatter into field
    for (int si = 0; si < SIMD; si++)
      for (int k = 0; k < F::n_components; k++)
        field.set(indices[si], k, phi[k][si]);

    // Reduce acceptance
    int ret = 0;
    for (int si = 0; si < SIMD; si++)
      ret += accepted[si];
    return ret;
  }

#ifdef CACHE
  int update_block(const BlockType& block, int n = 1) {
    int indices[SIMD] __attribute__((aligned(16)));
    long int accepted = 0;
    for (int i = 0; i < n; ++i) {
      for (int p = 0; p < block.n_partitions(); ++p) {
        for (int s = 0; s < block.partition_size(); s += SIMD) {
          for (int si = 0; si < SIMD; ++si) {
            indices[si] = block.corner_offset() + block.partition(p, s + si);
          }
          accepted += operator()(indices);
        }
      }
    }
    return accepted;
  }
#endif

  Float epsilon_;
  F& field;
  const parameters<Float> pars_;

  const Float quadratic_coef_1;
  const Float quadratic_coef_2;
  const Float quadratic_coef;

  Float gr;
};
