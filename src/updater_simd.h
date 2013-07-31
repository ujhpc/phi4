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

    FVec corona[F::n_components];
    FVec phi[F::n_components];
    FVec phi2((Float)0.0);
    FVec action((Float)0.0);

    for (int k = 0; k < F::n_components; ++k) {
      FVec small_corona((Float)0.0);
      // NOTE: big_corona_01 comes from exactly same values as small_corona,
      // so there is no sense to count it again
      FVec& big_corona_01 = small_corona;
      FVec big_corona_02((Float)0.0);
      FVec big_corona_11((Float)0.0);

      // NOTE: This code doesn't look nice as it could, however due lack of
      // gatter on SSE/AVX,
      // loading values into SSE/AVX registers then performing addition produces
      // many more
      // instructions than first adding them then putting (gathering) them into
      // register
      for (int mu = 0; mu < indexer_t::D; mu++) {
#define index_up(n, i) indexer_t::up(i[n], mu)
        int up[]
            __attribute__((aligned(16))) = { REP(SIMD, index_up, indices) };
#define index_dn(n, i) indexer_t::dn(i[n], mu)
        int dn[]
            __attribute__((aligned(16))) = { REP(SIMD, index_dn, indices) };

#define field_up_dn(n, p) p.get(up[n], k) + p.get(dn[n], k)
        small_corona += FVec(REP(SIMD, field_up_dn, field));
#define field_up_dn_2(n, p) \
  p.get(indexer_t::up(up[n], mu), k) + p.get(indexer_t::dn(dn[n], mu), k)
        big_corona_02 += FVec(REP(SIMD, field_up_dn_2, field));

        for (int nu = 0; nu < mu; nu++) {
#define index_nu(n, p)                                                      \
  p.get(indexer_t::up(up[n], nu), k) + p.get(indexer_t::up(dn[n], nu), k) + \
      p.get(indexer_t::dn(up[n], nu), k) + p.get(indexer_t::dn(dn[n], nu), k)
          big_corona_11 += FVec(REP(SIMD, index_nu, field));
        }
      }

      FVec big_corona = FVec(-pars_.i_Lambda) *
                        (big_corona_02 -
                         FVec((Float)4.0) * FVec(indexer_t::D) * big_corona_01 +
                         FVec((Float)2.0) * big_corona_11);
      corona[k] = small_corona + big_corona;

#define indexed_field(n, p, i) p.get(i[n], k)
      phi[k] = FVec(REP(SIMD, indexed_field, field, indices));
      phi2 += phi[k] * phi[k];
      action += corona[k] * phi[k];
    }

    action -= (FVec(quadratic_coef) + FVec(gr) * phi2) * phi2;

    FVec randsym[N_HIT][F::n_components];
    FVec randlog[N_HIT];

// NOTE: Why keep accepted as float vector, well simple
// there are 8 component no integer operations on AVX,
// but there are 8 component float operations.
#if SIMD <= 1
    int accepted = 0;
#else
    FVec accepted((Float)0.0);
#endif

    for (int h = 0; h < N_HIT; h++)
      for (int k = 0; k < F::n_components; k++)
        randsym[h][k] = RAND_SYM(tid);
    for (int h = 0; h < N_HIT; h++)
      randlog[h] = RAND(tid);
    for (int h = 0; h < N_HIT; h++)
      randlog[h] = log(randlog[h]);

    for (int h = 0; h < N_HIT; h++) {
      FVec new_phi[F::n_components];
      new_phi[0] = phi[0] + randsym[h][0];

      FVec new_phi2 = new_phi[0] * new_phi[0];
      FVec new_action = corona[0] * new_phi[0];

      for (int k = 1; k < F::n_components; k++) {
        new_phi[k] = phi[k] + randsym[h][k];
        new_phi2 += new_phi[k] * new_phi[k];
        new_action += corona[k] * new_phi[k];
      }

      new_action -=
          (FVec(quadratic_coef) + FVec(gr) * FVec(new_phi2)) * new_phi2;
      FVec delta_action = new_action - action;

#if SIMD <= 1
      if (delta_action >= (Float)0.0 || delta_action >= randlog[h]) {
        for (int k = 0; k < F::n_components; k++)
          phi[k] = new_phi[k];
        action = new_action;
        accepted++;
      }
#else
      IVec action_cond = delta_action >= min((Float)0.0, randlog[h]);

      for (int k = 0; k < F::n_components; k++)
        phi[k] = action_cond(new_phi[k], phi[k]);

      action = action_cond(new_action, action);
      accepted += action_cond((Float)1.0);
#endif
    }

// Scatter into field
#if SIMD <= 1
    for (int k = 0; k < F::n_components; k++)
      field.set(*indices, k, phi[k]);
#else
    for (int si = 0; si < SIMD; si++)
      for (int k = 0; k < F::n_components; k++)
        field.set(indices[si], k, phi[k][si]);
#endif

// Reduce acceptance
#if SIMD <= 1
    return accepted;
#else
    int ret = 0;
    for (int si = 0; si < SIMD; si++)
      ret += accepted[si];
    return ret;
#endif
  }

  Float epsilon_;
  F& field;
  const parameters<Float> pars_;

  const Float quadratic_coef_1;
  const Float quadratic_coef_2;
  const Float quadratic_coef;

  Float gr;
};
