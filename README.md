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
