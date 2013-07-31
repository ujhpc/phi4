#include <cstdlib>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"

template <typename F, typename P>
long int make_sweep(F& field,
                    const parameters<Float>& pars,
                    const P& partition) {

  long int accepted = 0;

  Updater<F> update(field, pars);
#pragma omp parallel default(none) shared(partition, update, accepted)
  for (int p = 0; p < partition.n_partitions(); ++p) {
    int indices[SIMD] __attribute__((aligned(16)));
/* this loop can be parallelised */
#pragma omp for reduction(+ : accepted)
    for (int s = 0; s < partition.partition_size(); s += SIMD) {
      for (int si = 0; si < SIMD; ++si)
        indices[si] = partition.partition(p, s + si);
      accepted += update(indices);
    }
  }
  return accepted;
}

template long int make_sweep<Field, Partition>(Field&,
                                               const parameters<Float>&,
                                               const Partition& partition);
