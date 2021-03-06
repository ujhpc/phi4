#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef __linux__
#include <time.h>
#endif
#include "cmdline.h"

#include "types.h"
#include "sweep.h"
#include "measurments.h"

const int meas_freq = 25;
const int write_out = 1;
#if DIM >= 3
const int default_n = 32;
const int default_b = 8;
#else
const int default_n = 128;
const int default_b = 16;
#endif

#ifndef NAME
#define FILE_NAME "phi4"
#else
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#define FILE_NAME STRINGIZE_VALUE_OF(NAME)
#endif

#include "ops.h"

int main(int argc, char* argv[]) {

  cmdline::parser cl;
  cl.footer(FILE_NAME " ...");

  cl.add("verbose", 'v', "be verbose");
#ifdef __linux__
  cl.add("time", 0, "output single line benchmark");
#endif
#ifdef _OPENMP
  cl.add<int>("threads", 0, "number of OpenMP threads", false, 0);
#endif

  cl.add<int>("n-x", 0, "x size", false, default_n);
  cl.add<int>("n-y", 0, "y size", false, default_n);
#if DIM >= 3
  cl.add<int>("n-z", 0, "z size", false, default_n);
#endif

#ifdef CACHE
  cl.add<int>("b-x", 0, "block x size", false, default_b);
  cl.add<int>("b-y", 0, "block y size", false, default_b);
#if DIM >= 3
  cl.add<int>("b-z", 0, "block z size", false, default_b);
#endif
  cl.add<int>("block-sweeps", 0, "block z size", false, 8);
#endif

  cl.add<int>("term", 't', "number of termalisation sweeps", false, 0);
  cl.add<int>("sweep", 'n', "number of measurments sweeps", false, 0);
  cl.add<int>("seed", 's', "rng seed", false, 7675643);
  cl.add<Float>("g", 'g', "g value", false, Float(0));
  cl.add<Float>("mass-squared", 'm', "m^2 value", false, Float(0.25));
  cl.add<Float>("i-Lambda", 'L', "inverse Lambda value", false, Float(2));
  cl.add<std::string>("tag", 0, "tag", false, "txt");

  try {
    cl.parse_check(argc, argv);
  }
  catch (std::string error) {
    std::cerr << error << std::endl;
    return 2;
  }
  catch (const char* error) {
    std::cerr << error << std::endl;
    return 2;
  }

  int n_term = cl.get<int>("term");
  int n_prod = cl.get<int>("sweep");
  int seed = cl.get<int>("seed");

  int dim[DIM] = {
    cl.get<int>("n-x"),
    cl.get<int>("n-y"),
#if DIM >= 3
    cl.get<int>("n-z"),
#endif
  };

#ifdef CACHE
  int block_dim[DIM] = {
    cl.get<int>("b-x"),
    cl.get<int>("b-y"),
#if DIM >= 3
    cl.get<int>("b-z"),
#endif
  };

  int block_sweeps = cl.get<int>("block-sweeps");
#else
  const int block_sweeps = 1;
#endif

  struct parameters<Float> pars = {
    cl.get<Float>("g"), cl.get<Float>("mass-squared"),
        cl.get<Float>("i-Lambda"),
  };

#ifdef _OPENMP
  if (cl.exist("threads")) {
    omp_set_num_threads(cl.get<int>("threads"));
  }
  const int n_threads = omp_get_max_threads();
#else
  const int n_threads = 1;
#endif

  if (cl.exist("verbose")) {
    std::cerr << "# Dimensions " << DIM << std::endl;
    std::cerr << "# Components " << Field::n_components << std::endl;
    std::cerr << "# Float " << cmdline::detail::readable_typename<Float>()
              << std::endl;
#ifdef _OPENMP
    std::cerr << "# OpenMP " << n_threads << " threads" << std::endl;
#else
    std::cerr << "# No OpenMP" << std::endl;
#endif
#ifdef SIMD
    std::cerr << "# SIMD " << SIMD << " elements" << std::endl;
#else
    std::cerr << "# No SIMD" << std::endl;
#endif
    // Dump command line parameters
    std::cerr << cl;
  }

  std::string mag_file_name(FILE_NAME ".");
  mag_file_name += cl.get<std::string>("tag");

  Ind::init(dim);
#ifdef CACHE
  BlockType::init(block_dim);
#endif

  Field phi_field(Ind::n_sites());
  srand48(seed);

  rand_array_t::init(n_threads, seed);

  for (int i = 0; i < Ind::n_sites(); i++) {
    phi_field.set(i, Float(2.0 * drand48() - 1.0));
  }

  long int accepted = 0;
  Partition partition;

#ifdef __linux__
  struct timespec start, stop;
  clock_gettime(CLOCK_REALTIME, &start);
#endif

  // Termalisation loop ///////////////////////////////////////////////////////

  for (int sweep = 0; sweep < n_term; sweep++) {
#ifdef CACHE
    accepted += make_sweep(phi_field, pars, block_sweeps);
#else
    accepted += make_sweep(phi_field, pars, partition);
#endif
  }

#ifdef __linux__
  clock_gettime(CLOCK_REALTIME, &stop);
  double ns =
      1e9 * (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);
  double sec = ns / 1e9;
  long long sweeps = block_sweeps * Ind::n_sites() * n_term;
  double up_ns = ns / (double)sweeps / (double)N_HIT;
  double gops = ((double)sweeps * (double)OPS) / ns;
  double gops_no_log = ((double)sweeps * (double)NO_LOG_OPS) / ns;
  double logs = ((double)sweeps * (double)LOGS) / ns;
  if (cl.exist("verbose")) {
    std::cerr << "sites " << Ind::n_sites() << std::endl;
    std::cerr << "ops per sweep " << OPS << std::endl;
    std::cerr << "total ops " << sweeps* OPS << std::endl;
    std::cerr << "termalisation took " << sec << " s" << std::endl;
    std::cerr << "that makes " << up_ns << " ns per update" << std::endl;
    std::cerr << "Gops " << gops << " per s" << std::endl;
    std::cerr << "Gops (no log) " << gops_no_log << " per s" << std::endl;
    std::cerr << "logs " << logs << " per s" << std::endl;
  }
  if (cl.exist("time")) {
#ifdef __AVX2__
    std::cout << "AVX2\t";
#elif __AVX__
    std::cout << "AVX\t";
#elif __SSE4_2__
    std::cout << "SSE4.2\t";
#elif __SSE4_1__
    std::cout << "SSE4.1\t";
#elif __SSE3__
    std::cout << "SSE3\t";
#elif __SSE2__
    std::cout << "SSE2\t";
#endif
    std::cout << (ns / 1.0e9) << "\t";
    std::cerr << up_ns << "\t";
    std::cerr << gops;
#ifdef LEGACY
    std::cout << "\t(old)";
#endif
    std::cout << std::endl;
  }
#endif

  if (cl.exist("verbose") && n_term > 0)
    std::cerr << "acceptance "
              << (double)accepted /
                     (block_sweeps * N_HIT * (double)Ind::n_sites() * n_term)
              << std::endl;

  MagnetisationMeasurer<Field::n_components> magnetisation;
  accepted = 0;

  FILE* fmag = NULL;
  if (n_prod) {
    if ((fmag = fopen(mag_file_name.c_str(), "w")) == NULL) {
      throw("cannot open file `" + mag_file_name + "' for  writing");
    }
  }

  // Main simulation loop /////////////////////////////////////////////////////

  int n_meas = 0;
  for (int sweep = 0; sweep < n_prod; sweep++) {
#ifdef CACHE
    accepted += make_sweep(phi_field, pars, block_sweeps);
#else
    accepted += make_sweep(phi_field, pars, partition);
#endif
    if ((sweep + 1) % meas_freq == 0) {
      magnetisation.measure(phi_field);
      if ((magnetisation.n_meas()) % write_out == 0) {
        fprintf(fmag,
                "%.12g  %.16g %.16g %.16g\n",
                magnetisation.phi2() / Ind::n_sites(),
                magnetisation.mag(),
                magnetisation.xi(),
                magnetisation.amag());
        magnetisation.reset();
      }
      n_meas++;
    }
    if (n_meas > 0 && n_meas % 100 == 0)
      fflush(fmag);
  }

  if (cl.exist("verbose") && n_prod > 0)
    std::cerr << "acceptance "
              << (double)accepted /
                     (block_sweeps * N_HIT * (double)Ind::n_sites() * n_prod)
              << std::endl;
#ifndef CACHE
  Ind::clean();
#endif
}
