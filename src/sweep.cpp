#include <cstdlib>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"

#ifdef STRIPS
template <typename F, typename P>
long int make_sweep(F& field,
                    const parameters<Float>& pars,
                    const P& partition) {

  long int accepted = 0;

  Updater<F, Partition> update(field, partition, pars);
#if _OPENMP
  const int num_threads = omp_get_max_threads();
#else
  const int num_threads = 1;
#endif

  const int n_sites = F::indexer_t::n_sites();
  const int chunk = n_sites / (2 * num_threads);

#pragma omp parallel default(none) shared(update, accepted)
  {
#pragma omp for reduction(+ : accepted)
    for (int i = 0; i < num_threads; ++i) {
      for (int s = 2 * i * chunk; s < (2 * i + 1) * chunk; ++s)
        accepted += update(s);
    }
#pragma omp for reduction(+ : accepted)
    for (int i = 0; i < num_threads; ++i) {
      for (int s = (2 * i + 1) * chunk; s < (2 * i + 2) * chunk; ++s)
        accepted += update(s);
    }
  }
  return accepted;
}

#else
template <typename F, typename P>
long int make_sweep(F& field,
                    const parameters<Float>& pars,
                    const P& partition) {

  long int accepted = 0;

  Updater<F> update(field, pars);
#pragma omp parallel default(none) shared(partition, update, accepted)
  for (int p = 0; p < partition.n_partitions(); ++p) {

/* this loop can be parallelised */
#pragma omp for reduction(+ : accepted)
    for (int s = 0; s < partition.partition_size(); ++s) {
      int i = partition.partition(p, s);

      accepted += update(i);
    }
  }
  return accepted;
}
#endif

template long int make_sweep<Field, Partition>(Field&,
                                               const parameters<Float>&,
                                               const Partition& partition);
