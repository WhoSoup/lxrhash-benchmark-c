# lxrhash-benchmark-c

A quick comparison between go and c, using the default hash settings with a 25 bit bytemap. Both the Go and the C apps time the creation of the table and then run 100,000 hashes, successively hashing the result of the previous hash, starting with "00000000000000000000000000000000".

Written for a 64-bit machine, no instructions on usage.

## Results

Run on an i3-7100U with 8GB DDR4 ram.

**C**
```
    generate() took 17.577942 seconds
    hash() took 15.396397 seconds
```
    
**Go**
```
    generate() took 17.397934533s
    hash() took 15.319676967s
```
