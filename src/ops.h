/// (G)ops estimation for algorithm

#pragma once

#define LOG_OPS 20
#define RNG_OPS 24

#define CORONA_OPS (NCOMP) * ((DIM) * (4 + 2 * ((DIM) - 1)) - (DIM) + 6)
#define INIT_OPS (NCOMP) * 4 + 2
#define UPDATE_OPS \
  (N_HIT) * (((NCOMP) + 1) * (RNG_OPS) + ((NCOMP) - 1) * 6 + (LOG_OPS) + 10)
#define OPS ((CORONA_OPS) + (INIT_OPS) + (UPDATE_OPS))
#define NO_LOG_OPS ((OPS) - (N_HIT) * (LOG_OPS))
#define LOGS (N_HIT)
