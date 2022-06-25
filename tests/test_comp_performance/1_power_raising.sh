echo Test: 1_power_raising >> 1_test.csv
echo Start time: `date +"%Y-%m-%d %T"` >> 1_test.csv
for MAX_NR in 1 8 64
do
    ./1_power_raising.out 0 1 $MAX_NR helib 16384 119 40 2
    ./1_power_raising.out 0 0 $MAX_NR seal 8192 60 2 40 40
    ./1_power_raising.out 1 0 $MAX_NR helib 16384 119 40 2
    ./1_power_raising.out 1 0 $MAX_NR seal 8192 60 2 40 40
    ./1_power_raising.out 2 0 $MAX_NR helib 16384 119 21 2
    ./1_power_raising.out 2 0 $MAX_NR seal 8192 60 2 40 40
    ./1_power_raising.out 3 0 $MAX_NR seal 8192 55 3 30 30
    ./1_power_raising.out 3 0 $MAX_NR seal 8192 50 3 33 33
    ./1_power_raising.out 3 0 $MAX_NR seal 8192 45 3 37 37
    ./1_power_raising.out 3 0 $MAX_NR helib 16384 119 8 2
    ./1_power_raising.out 4 0 $MAX_NR helib 16384 119 4 2
    ./1_power_raising.out 4 0 $MAX_NR seal 8192 41 4 30 30
    ./1_power_raising.out 4 0 $MAX_NR seal 8192 39 4 31 31

    ./1_power_raising.out 0 0 $MAX_NR helib 32768 358 40 6
    ./1_power_raising.out 0 0 $MAX_NR helib 32768 299 40 3
    ./1_power_raising.out 0 0 $MAX_NR helib 32768 239 40 2
    ./1_power_raising.out 0 0 $MAX_NR seal 16384 60 7 40 40
    ./1_power_raising.out 1 0 $MAX_NR helib 32768 358 40 6
    ./1_power_raising.out 1 0 $MAX_NR helib 32768 299 40 3
    ./1_power_raising.out 1 0 $MAX_NR helib 32768 239 40 2
    ./1_power_raising.out 1 0 $MAX_NR seal 16384 60 7 40 40
    ./1_power_raising.out 2 0 $MAX_NR helib 32768 358 40 6
    ./1_power_raising.out 2 0 $MAX_NR helib 32768 299 40 3
    ./1_power_raising.out 2 0 $MAX_NR helib 32768 239 40 2
    ./1_power_raising.out 2 0 $MAX_NR seal 16384 60 7 40 40
    ./1_power_raising.out 3 0 $MAX_NR helib 32768 358 40 6
    ./1_power_raising.out 3 0 $MAX_NR helib 32768 299 40 3
    ./1_power_raising.out 3 0 $MAX_NR helib 32768 239 40 2
    ./1_power_raising.out 3 0 $MAX_NR seal 16384 60 7 40 40
    ./1_power_raising.out 4 0 $MAX_NR helib 32768 358 40 6
    ./1_power_raising.out 4 0 $MAX_NR helib 32768 299 40 3
    ./1_power_raising.out 4 0 $MAX_NR helib 32768 239 40 2
    ./1_power_raising.out 4 0 $MAX_NR seal 16384 60 7 40 40
    ./1_power_raising.out 5 0 $MAX_NR helib 32768 358 40 6
    ./1_power_raising.out 5 0 $MAX_NR helib 32768 299 40 3
    ./1_power_raising.out 5 0 $MAX_NR helib 32768 239 40 2
    ./1_power_raising.out 5 0 $MAX_NR seal 16384 60 7 40 40
done