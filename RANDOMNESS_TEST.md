Randomness Test
===============

A continuous stream of null characters were fed through `mrrcrypt` and a
number of randomness tests were applied to the output. See below for details.

Visual Analysis
---------------

Three bitmap bitmap images of varying size were generated from each of
four random keys, for a total of 12 visual analysis tests. In all cases,
the output did not show any striations or noticible patterns of any kind.

No particular bitmap seemed to favor a color, or show any bias towards
light or dark, when compared to bitmaps generated from other keys,
suggesting that different keys produce the similar random results.

Key  | Bitmap Size | Result
-----|-------------|-------
key1 | 320x200     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test01.bmp)
key2 | 320x200     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test02.bmp)
key3 | 320x200     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test03.bmp)
key4 | 320x200     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test04.bmp)
key1 | 640x400     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test05.bmp)
key2 | 640x400     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test06.bmp)
key3 | 640x400     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test07.bmp)
key4 | 640x400     | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test08.bmp)
key1 | 1280x800    | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test09.bmp)
key2 | 1280x800    | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test10.bmp)
key3 | 1280x800    | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test11.bmp)
key4 | 1280x800    | [LINK](http://brianbarto.info/extern/images/mrrcrypt/test12.bmp)


NIST and Diehard Tests
----------------------

Output was tested for randmoness using the
[Statistical Test Suite](http://csrc.nist.gov/groups/ST/toolkit/rng/stats_tests.html)
developed by the National Institute for Standards and Technology (NIST),
as well as other *reliable* tests that are incorporated into the
[diehard test suite](https://en.wikipedia.org/wiki/Diehard_tests).

Below are the results of such tests:

```
#=============================================================================#
        test_name   |ntup| tsamples |psamples|  p-value |Assessment
#=============================================================================#
   diehard_birthdays|   0|       100|     100|0.45599727|  PASSED  
      diehard_operm5|   0|   1000000|     100|0.05654986|  PASSED  
  diehard_rank_32x32|   0|     40000|     100|0.40071203|  PASSED  
    diehard_rank_6x8|   0|    100000|     100|0.00000000|  FAILED  
   diehard_bitstream|   0|   2097152|     100|0.00000000|  FAILED
diehard_count_1s_str|   0|    256000|     100|0.00000000|  FAILED
diehard_count_1s_byt|   0|    256000|     100|0.00506266|  PASSED
 diehard_parking_lot|   0|     12000|     100|0.00000000|  FAILED
    diehard_2dsphere|   2|      8000|     100|0.99539586|   WEAK
    diehard_3dsphere|   3|      4000|     100|0.40968538|  PASSED
     diehard_squeeze|   0|    100000|     100|0.00000000|  FAILED
        diehard_runs|   0|    100000|     100|0.14636277|  PASSED
       diehard_craps|   0|    200000|     100|0.00022379|   WEAK
```

