# nu

> **Nu** (Yiddish: נו): A multipurpose interjection analogous to "well?", "so?", or "so what?"
> 
> [Wikipedia](https://en.wikipedia.org/wiki/List_of_English_words_of_Yiddish_origin#N)

(rhymes with "new")

A simple tool for computing allocation intensity (that is, time spent
in `malloc` and `free`) and basic allocation statistics (for now, just
the raw total count of `malloc`s and `free`s).

By [Emery Berger](https://emeryberger.com)

_NOTE: this is currently MacOS only._

## Usage

Build with `make`, then:

```
DYLD_INSERT_LIBRARIES=./libnu.dylib program_to_profile
```

## Example output

Nu reports statistics to `stderr` every second, sampling at a 100Hz rate.

```
malloc ratio = 0.843333 (506 / 600); mallocs=10670891, frees=9575472
malloc ratio = 0.745714 (522 / 700); mallocs=11988059, frees=10576502
malloc ratio = 0.693750 (555 / 800); mallocs=15280774, frees=13049209
malloc ratio = 0.668889 (602 / 900); mallocs=20520815, frees=17759256
malloc ratio = 0.650000 (650 / 1000); mallocs=25480426, frees=22268773
malloc ratio = 0.628182 (691 / 1100); mallocs=28986884, frees=24738074
malloc ratio = 0.613333 (736 / 1200); mallocs=33192965, frees=28253947
malloc ratio = 0.611538 (795 / 1300); mallocs=38776636, frees=33334842
malloc ratio = 0.605000 (847 / 1400); mallocs=43169626, frees=37102203
```

* `malloc ratio` is the proportion of time that the program was allocating or freeing
* the numerator is the number of samples when in `malloc` or `free`
* the denominator is the total number of samples taken (100 per second)
* `mallocs` = total calls to `malloc`
* `frees` = total calls to `free`
