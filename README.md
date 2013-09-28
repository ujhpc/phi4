phi4
====

See `doc/` folder for description.

Build
-----

Supports `GCC 4.6`, `ICC 13.0` and `Clang 3.0` or higher.

Requires `Intel SVML` library coming with `ICC` for SIMD builds using non-ICC
compilers.

To build executable with default configuration use:

	make

### Building other configurations

Following build options may be combined. Changing build options changes final
executable suffix indicating used options.

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

6. Enabling index optimization: (implies only `2^n` lattice sizes)

		make FAST_INDEXER=1

7. Enable SVML fix for *ICC*<13 when using non-ICC compilers and SIMD

		make SVML_FIX=1

8. Use old *legacy* code (used in publications) max AVX

		make LEGACY=1

Notes about source code
-----------------------

Source code in `src/` is split into several logical modules and versions
described by prefixes.

### Modules

* `types.h` core configuration and type dispatch header

* `swep_*` files implementing single sweep

* `random_*` fused *Tausworthe*/*LCG* random number generator

* `partition_*` partitioning (satisfying Markov Chain constraints)

* `indexer`, `*_index` memory indexers implementation

* `legacy/*` legacy versions of modules above (used in publications)

### Prefixes

* `_simd` *SIMD* version of module

* `_test` Gtest files (deprecated)

Operations
----------

This is rough estimation of number operations in the algorithm.

**NOTE:** It skips lattice index calculation operations, memory operations,
register transfers and variable initialization. So it estimates minimum number
of operations, not average.

1. Corona gather (plus est. for `Comp=1 Dim=3`)

		Corona = Comp * ( Dim * (4 + 2*(Dim-1)) - Dim + 6 )
			  ~=	1 * (	3 * (4 + 2*	  2	  ) -	3 + 6 )
			   = 27

2. Fused Tausworthe + LCG random number generator

		Rng	   = Taus * 3 + LCG + 3
			   = 6 * 3 + 6
			   = 24

3. Initial action before updates (plus est. for `Comp=1`)

		Init   = Comp * 4 + 2
			  ~= 6

4. Update ops for given `Hits`	(plus est. for `Comp=1 Dim=3`)

		Update = Hits * ( (Comp+1) * Rng + (Comp-1) * 6 + Log + 10 )
			  ~= Hits * (		2  * Rng +				  Log + 10 )
			   = Hits * ( 58 + Log								   )

Gives altogether for `Comp=1 Dim=3`:

	Total  = 33 + Hits * ( 58 + Log )

GPU Implementation
------------------

GPU implementation is available as separate project
[phi4_cuda](/ujhpc/phi4_cuda).

License
-------

> Copyright (C) 2012-2013 Piotr Bialas, Adam Strzelecki
>
> This file is part of phi4.
>
> This program is free software; you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation; either version 3 of the License, or
> (at your option) any later version.
>
> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
> GNU General Public License for more details.
>
> You should have received a copy of the GNU General Public License
> along with this program; if not, write to the Free Software Foundation,
> Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301	USA
