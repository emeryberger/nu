# nu

> **Nu** (Yiddish: נו): A multipurpose interjection analogous to "well?", "so?", or "so what?"
> 
> [Wikipedia](https://en.wikipedia.org/wiki/List_of_English_words_of_Yiddish_origin#N)

(rhymes with "new")

A simple tool for computing allocation intensity (that is, time spent
in `malloc` and `free`) and basic allocation statistics (for now, just
the raw total count of `malloc`s and `free`s), as well as a compact
summary of the number of objects per size.

Works on Linux and Mac platforms.

By [Emery Berger](https://emeryberger.com)


## Usage

Build with `make`, then:

(Mac)
```
DYLD_INSERT_LIBRARIES=./libnu.dylib program_to_profile
```

(Linux)
```
LD_PRELOAD=./libnu.so program_to_profile
```

## Example output

Nu reports statistics to `stderr` every second, sampling at a 100Hz rate.

For example, after 3 seconds:
```
malloc ratio = 0.360000 (36 / 100); mallocs=6184638, frees=895853
tvwvriiiohhggggffffbaaa_________
malloc ratio = 0.365000 (73 / 200); mallocs=11409903, frees=1896888
uxxvrjiiohhhhhhhggfbaaaaa_______
malloc ratio = 0.373333 (112 / 300); mallocs=17551202, frees=2897921
vxxwrjjioiiihiiihggbaaaaa_______
```

* `malloc ratio` is the proportion of time that the program was allocating or freeing
* the numerator is the number of samples when in `malloc` or `free`
* the denominator is the total number of samples taken (100 per second)
* `mallocs` = total calls to `malloc`
* `frees` = total calls to `free`

* The final line compactly summarizes the number of allocations per
  power-of-two size class (8, 16, ...).  Each letter is a + log_2(# of
  allocations). For example, the first letter is v, which is the 22nd
  letter of the alphabet. This means that the program has so far
  allocated at least 2^22 objects of size 1-8. If log_2(# of
  allocations) exceeds 26, nu will switch to using capital letters
  (so, for example, A means that the program has allocated at least
  2^27 objects of the given size).

  You can decode these histograms by running `decode_histogram` as shown below:

```
% echo "vxxwrjjioiiihiiihggbaaaaa_______" | ./decode_histogram
size	8 = 2097152
size	16 = 8388608
size	32 = 8388608
size	64 = 4194304
size	128 = 131072
size	256 = 512
size	512 = 512
size	1024 = 256
size	2048 = 16384
size	4096 = 256
size	8192 = 256
size	16384 = 256
size	32768 = 128
size	65536 = 256
size	131072 = 256
size	262144 = 256
size	524288 = 128
size	1048576 = 64
size	2097152 = 64
size	4194304 = 2
...
```
