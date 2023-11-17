 (/usr/bin/time -v ./index -t ../efm.in -z 8 -p ../patterns/efm_z8 -o pm_time/efm_z8time.txt) &> peak_ram/efm_z2peak.txt
 (/usr/bin/time -v ./index -t ../efm.in -z 16 -p ../patterns/efm_z16 -o pm_time/efm_z16time.txt) &> peak_ram/efm_z4peak.txt
 (/usr/bin/time -v ./index -t ../efm.in -z 32 -p ../patterns/efm_z32 -o pm_time/efm_z32time.txt) &> peak_ram/efm_z8peak.txt
 (/usr/bin/time -v ./index -t ../efm.in -z 64 -p ../patterns/efm_z64 -o pm_time/efm_z64time.txt) &> peak_ram/efm_z16peak.txt
 (/usr/bin/time -v ./index -t ../efm.in -z 128 -p ../patterns/efm_z128 -o pm_time/efm_z128time.txt) &> peak_ram/efm_z32peak.txt
 
 (/usr/bin/time -v ./index -t ../sars.in -z 64 -p ../patterns/z64 -o pm_time/sars_z64time.txt) &> peak_ram/sars_z64peak.txt
 (/usr/bin/time -v ./index -t ../sars.in -z 128 -p ../patterns/z128 -o pm_time/sars_z128time.txt) &> peak_ram/sars_z4peak.txt
 (/usr/bin/time -v ./index -t ../sars.in -z 256 -p ../patterns/z256 -o pm_time/sars_z256time.txt) &> peak_ram/sars_z8peak.txt
 (/usr/bin/time -v ./index -t ../sars.in -z 512 -p ../patterns/z512 -o pm_time/sars_z512time.txt) &> peak_ram/sars_z16peak.txt
 (/usr/bin/time -v ./index -t ../sars.in -z 1024 -p ../patterns/z1024 -o pm_time/sars_z1024time.txt) &> peak_ram/sars_z32peak.txt