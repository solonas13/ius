ius: indexing uncertain strings
===============================

This repository hosts implementations of several algorithms for indexing uncertain (weighted) strings.

Requirements
===
* A modern, C++11 ready compiler such as g++ version 4.9 or higher
* Boost libraries ready on your system. On Ubuntu, e.g., via:

```
sudo apt install libboost-all-dev
```

Datasets
===

To look at the the datasets, go to the `datasets' directory and follow the instructions there.


How to run
===

To run, for example, the MWST-SE index, you can execute the following:

```
cd MWST-SE
make
./index -t ../datasets/text/sars.in -z 1024 -p ../datasets/patterns/sars.z64p64.txt -l 64 -o pm_time/sars_z64p64.txt
```

Citation
===
```
E. Gabory, C. Liu, G. Loukides, S. P. Pissis, and W. Zuba
Space-Efficient Indexes for Uncertain Strings
(submitted manuscript)
```

<b>License</b>: GNU GPLv3 License; Copyright (C) 2023 E. Gabory, C. Liu, G. Loukides, S. P. Pissis, and W. Zuba.
