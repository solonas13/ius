ius: indexing uncertain strings
===============================

This repository maintains the implementation of several new algorithms for <b>indexing uncertain (weighted) strings</b>.
An uncertain string of length n over an alphabet Σ is a sequence of n distributions of Σ. Importantly, our indexing schemes 
work for any ASCII alphabet Σ and are not negatively affected when the alphabet size |Σ| grows; the only overhead in this
case, would be to read the input file in memory.

Requirements
===
* A GNU/Linux system
* A modern C++11 ready compiler such as g++ version 4.9 or higher
* Boost libraries ready on your system. On Ubuntu, e.g., via: `sudo apt install libboost-all-dev`

Datasets
===

Here is the supported format of uncertain strings:

```
29903
ACGT
1 0 0 0
0.1 0.3 0.3 0.3
0 0 0.5 0.5
...
```

The first line is the number n of distributions; the second line is the alphabet Σ; the subsequent n lines are 
the distributions of Σ. 

To access our datasets, go to the `datasets` directory and follow the instructions there. We provide three real
datasets and a tool to simulate patterns from a given dataset (uncertain string). 


How to run
===

To index, for example, the SARS‑CoV‑2 dataset, and perform pattern matching you can execute the following after creating 
the patterns in compressed form (.gz):

```
cd MWST-SE
make
./index -t ../datasets/text/sars.in -z 64 -p ../datasets/patterns/sars.z64p64.txt.gz -l 64 -o pm_time/sars_z64p64.txt
```

For each index, a README file is included in the corresponding directory with instructions on how to run.

Citation
===
```
E. Gabory, C. Liu, G. Loukides, S. P. Pissis, and W. Zuba
Space-Efficient Indexes for Uncertain Strings
ICDE 2024
```

<b>License</b>: GNU GPLv3 License; Copyright (C) 2023 E. Gabory, C. Liu, G. Loukides, S. P. Pissis, and W. Zuba.
