phi4
====

Build
-----

Supports `GCC 4.6`, `ICC 13.0` and `Clang 3.0` or higher.

To build executables with default config use:

	make

### Building other configurations

Following build options may be combined. Changing build options changes final
executable suffix.

1. SIMD version via `SIMD=<size>`, i.e.:

		make SIMD=4

2. Enabling OpenMP:

		make OPENMP=1

3. Different float type via `FLOAT=<type>`, i.e.:

		make FLOAT=float

4. Different compiler via `CC=<compiler>`, i.e.:

		make CC=icc

5. Enabling cache optimization:

		make CACHE=1

6. Enabling index optimization:

		make FIDX=1

Operations
----------

This is rough estimation of number operations in the algorithm:

1. Corona gather (plus est. for `Comp=1 Dim=3`)

		Corona = Comp * ( Dim * (4 + 2*(Dim-1)) - Dim + 6 )
		      ~=    1 * (   3 * (4 + 2*   2   ) -   3 + 6 )
		       = 27

2. Fused Tausworthe + LCG random number generator

		Rng    = Taus * 3 + LCG + 3
	           = 6 * 3 + 5
		       = 23

3. Initial action before updates (plus est. for `Comp=1`)

		Init   = Comp * 4 + 2
		      ~= 6

4. Update ops for given `Hits`  (plus est. for `Comp=1 Dim=3`)

		Update = Hits * ( (Comp+1) * Rng + (Comp-1) * 6 + Log + 10 )
		      ~= Hits * (       2  * Rng +                Log + 10 )
		       = Hits * ( 56 + Log                                 )

Gives altogether for `Comp=1 Dim=3`:

	Total  = 33 + Hits * ( 56 + Log )

GPU Implementation
------------------

GPU implementation is available as separate project
[phi4_cuda](/ujhpc/phi4_cuda).
