phi4
====

Operations
----------

	corona = 2*3 + 2*3*3 + 6 = 30
	random = 6*3 + 5 = 23
	(11+23) * 8 = 272

GPU Times
---------

	32^3  = 1.262 ns
	64^3  = 0.165 ns
	128^3 = 0.125 ns
	256^3 = 0.113 ns

CPU Times
---------

I cheat here a little, best stock i7 CPUs are clocked at **3.2 GHz** mine is **2.5 GHz** so I scale results back to 3.2 GHz.

### Different size ns per update

**3D** `phi4-1nc-3d-float-vec8-omp-gcc`

	 4.526 s * 1000000000 / 4000 /  32^3 / 8 = 4.316 ns
	10.187 s * 1000000000 / 1000 /  64^3 / 8 = 4.857 ns
	 9.134 s * 1000000000 /  100 / 128^3 / 8 = 5.444 ns
	 9.529 s * 1000000000 /   10 / 256^3 / 8 = 7.099 ns

**2D** `phi4-1nc-2d-float-vec8-omp-gcc`

	 9.964 * 1000000000 /   400 / 1024^2 / 8 = 2.969 ns
	 5.903 * 1000000000 /  1000 /  512^2 / 8 = 2.815 ns
	 4.605 * 1000000000 / 10000 /  128^2 / 8 = 3.513 ns
	 4.774 * 1000000000 / 20000 /   32^2 / 8 = 29.138 ns

### CPU vs GPU

	32^3  = 4.316 / 1.262 = 3.419 x
	64^3  = 4.857 / 0.165 = 29.436 x
	128^3 = 5.444 / 0.125 = 43.552 x
	256^3 = 7.099 / 0.113 = 62.823 x

Scratchpad
----------

### Different implementations timing

	$ time ./phi4-1nc-2d-float-vec4-gcc --n-x 512 --n-y 512 -t 200
	acceptance 0.455913
	real	0m3.661s
	user	0m3.653s
	sys	0m0.005s

	$ time ./phi4-1nc-2d-float-vec8-gcc --n-x 512 --n-y 512 -t 200
	acceptance 0.455958
	real	0m3.441s
	user	0m3.398s
	sys	0m0.041s

	$ time ./phi4-c++-2d-float-vec4-gcc --n-x 512 --n-y 512 -t 200
	acceptance 0.456172

	real	0m2.067s
	user	0m2.063s
	sys	0m0.003s

	$ time ./phi4-c++-2d-float-vec8-gcc --n-x 512 --n-y 512 -t 200
	acceptance 0.456092

	real	0m1.845s
	user	0m1.833s
	sys	0m0.006s

	$ time ./phi4-c-2d-float-vec4-gcc --n-x 512 --n-y 512 -t 200
	acceptance 0.456169

	real	0m2.013s
	user	0m2.009s
	sys	0m0.003s

	$ time ./phi4-c-2d-float-vec8-gcc --n-x 512 --n-y 512 -t 200
	n_sites 262144
	0.250000 0.000000 2.000000
	200 0 7675643
	n threads = 1
	acceptance 0.456146

	real	0m1.712s
	user	0m1.700s
	sys	0m0.010s
