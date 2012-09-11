static inline uvec_t taus_step(uvec_t z, uvec_t S1, uvec_t S2, uvec_t S3, uvec_t M) {
  uvec_t b = (((z<<S1)^z)>>S2);
  return (((z&M)<<S3)^b);
}

static inline uvec_t LCG_step(uvec_t z, uvec_t A, uvec_t C) {
  return (A*z+C);
}

uvec_t utausv(uvec_t *seed) {
  seed[0] = taus_step(seed[0], uvec_t(13), uvec_t(19), uvec_t(12), uvec_t(4294967294u));
  seed[1] = taus_step(seed[1], uvec_t( 2), uvec_t(25), uvec_t( 4), uvec_t(4294967288u));
  seed[2] = taus_step(seed[2], uvec_t( 3), uvec_t(11), uvec_t(17), uvec_t(4294967280u));
  seed[3] = LCG_step( seed[3], uvec_t(1664525u), uvec_t(1013904223u));
  return seed[0]^seed[1]^seed[2]^seed[3];
}

// this is symetric, add 0.5 to get 0-1 range
fvec_t sftausvsym(uvec_t *restrict seed) {
  return fvec_t(2.328306436538696e-10) * utausv(seed);
}
