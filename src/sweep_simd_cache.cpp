#include <cstdlib>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"

template <typename F>
long int make_sweep(F& field, const parameters<Float>& pars, int block_sweeps) {

  long int accepted = 0;

  Updater<F> update(field, pars);
#pragma omp parallel default(none) shared(update, accepted, block_sweeps)
  for (int p = 0; p < BlockType::n_grids(); ++p) {
/* this loop can be parallelised */
#pragma omp for reduction(+ : accepted)
    for (int s = 0; s < BlockType::grid_size(); ++s) {
      accepted +=
          update.update_block(BlockType(BlockType::grid(p, s)), block_sweeps);
    }
  }
  return accepted;
}

template long int make_sweep<Field>(Field&, const parameters<Float>&, int);
