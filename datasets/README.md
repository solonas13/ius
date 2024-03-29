Uncertain strings description
===

The `text` directory contains 3 real uncertain strings:

* The full chromosome 22 of the Homo sapiens genome (v. GRCh37) was combined with a set of SNPs taken from the final phase of the 1000 Genomes Project (phase 3) representing 2504 samples on GRCh37 [https://doi.org/10.1093/nar/gkz836]. The length of the sequence (chr22.in), after removing the multiple 'N's at the beginning and ending, is 35,194,566.
* The full chromosome of Enterococcus faecium Aus0004 strain (CP003351) combined with a set of SNPs taken from 1432 samples [https://doi.org/10.1038/s42003-022-03194-2]. The length of the sequence (efm.in) is 2,955,294.
* The full genome of SARS-CoV-2 (isolate Wuhan-Hu-1) combined with a set of SNPs taken from 1181 samples [https://doi.org/10.7554/eLife.66857]. The length of the sequence (sars.in) is 29,903.

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

Generating patterns with a probability threshold 1/z
===

We show this for the chr22 dataset. First, unzip the `chr22.in.gz` file
```console
cd text
gzip -dk chr22.in.gz
cd ..
```
Second, compile using `make` and execute the following to generate patterns
of length in [64, 1024] for different z values:

```console

mkdir patterns

./index -t ./text/chr22.in -z 2 -o ./patterns/chr22.z2
./index -t ./text/chr22.in -z 4 -o ./patterns/chr22.z4
./index -t ./text/chr22.in -z 8 -o ./patterns/chr22.z8
./index -t ./text/chr22.in -z 16 -o ./patterns/chr22.z16
./index -t ./text/chr22.in -z 32 -o ./patterns/chr22.z32

gzip *
```
The patterns are saved in compressed form in `patterns` directory.
