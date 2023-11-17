valgrind --tool=massif --time-unit=ms --massif-out-file=massif_res/massif.out.wst.z2 ./index -t ../chr22.in -z 2 -p ../patterns/z2
valgrind --tool=massif --time-unit=ms --massif-out-file=massif_res/massif.out.wst.z4 ./index -t ../chr22.in -z 4 -p ../patterns/z4
valgrind --tool=massif --time-unit=ms --massif-out-file=massif_res/massif.out.wst.z8 ./index -t ../chr22.in -z 8 -p ../patterns/z8
valgrind --tool=massif --time-unit=ms --massif-out-file=massif_res/massif.out.wst.z16 ./index -t ../chr22.in -z 16 -p ../patterns/z16
valgrind --tool=massif --time-unit=ms --massif-out-file=massif_res/massif.out.wst.z32 ./index -t ../chr22.in -z 32 -p ../patterns/z32
