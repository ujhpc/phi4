phi4 
====

Operations
----------

	corona = 2*3 + 2*3*3 + 6 = 30
	random = 6*3 + 5 = 23
	(11+23) * 8 = 272

CPU Times
---------

I cheat here a little, best stock i7 CPUs are clocked at **3.2 GHz** mine is **2.5 GHz** so I scale results back to 3.2 GHz.

### CPU vs GPU

	 3.0 * 1000000000 /  200 / 512 / 512 /       8 * 2.5 / 3.2 / 0.12 = 46.56612873077392578125
	 4.6 * 1000000000 / 4000 /  32 /  32 /  32 / 8 * 2.5 / 3.2 / 0.12 = 28.56055895487467447916
	3.76 * 1000000000 /   40 / 128 / 128 / 128 / 8 * 2.5 / 3.2 / 0.12 = 36.47680083910624186191

### FLOPs

	4000 * 32 * 32 * 32 * 300 / 3.76 = 10457872340.42553191489361702127

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
