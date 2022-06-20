echo Test: 1_power_raising >> test.csv
echo Start time: `date +"%Y-%m-%d %T"` >> test.csv
./1_power_raising.out 0 1 helib 16384 119 40 2
./1_power_raising.out 1 0 helib 16384 119 40 2
./1_power_raising.out 2 0 helib 16384 119 21 2
./1_power_raising.out 3 0 helib 16384 119 8 2
./1_power_raising.out 4 0 helib 16384 119 4 2
./1_power_raising.out 0 0 seal 8192 60 40 40 60 40
./1_power_raising.out 1 0 seal 8192 60 40 40 60 40
./1_power_raising.out 2 0 seal 8192 60 40 40 60 40
./1_power_raising.out 3 0 seal 8192 55 30 30 30 55 30
./1_power_raising.out 3 0 seal 8192 50 33 33 33 50 33
./1_power_raising.out 3 0 seal 8192 45 37 37 37 45 37
./1_power_raising.out 4 0 seal 8192 41 30 30 30 30 41 30
./1_power_raising.out 4 0 seal 8192 39 31 31 31 31 39 31