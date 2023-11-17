Uncertain strings description
=============================

The text directory contains 3 real uncertain strings:

* The full chromosome 22 of the Homo sapiens genome (v. GRCh37) was combined with a set of SNPs taken from the final phase of the 1000 Genomes Project (phase 3) representing 2504 samples on GRCh37. The length of the sequence (chr22.in), after removing the multiple 'N' at the beginning and ending, is 35194566.
* The full chromosome of Enterococcus faecium Aus0004 strain (CP003351) combined with a set of SNPs [4] taken from 1432 samples. The length of the sequence (efm.in) is 2955294.
* The full genome of SARS-CoV-2 (isolate Wuhan-Hu-1) combined with a set of single nucleotide polymorphisms (SNPs) [2] taken from 1181 samples. The length of the sequence (sars.in) is 29903.

Generating patterns with a certain z
=============================

First, unzip the chr22.in.gz file
```console
cd text
gzip -dk chr22.in.gz
cd ..
```
Second, compile using `make`

```console
./index -t ./text/chr22.in -z 32 -o ./patterns/z32
./index -t ./text/chr22.in -z 64 -o ./patterns/z64
./index -t ./text/chr22.in -z 128 -o ./patterns/z128
./index -t ./text/chr22.in -z 256 -o ./patterns/z256
./index -t ./text/chr22.in -z 512 -o ./patterns/z512
./index -t ./text/chr22.in -z 1024 -o ./patterns/z1024
./index -t ./text/chr22.in -z 2048 -o ./patterns/z2048
```
The patterns are saved in `patterns` folder.
