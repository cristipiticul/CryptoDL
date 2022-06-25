# Batch Size: 4096
for TIMES_TO_SQUARE in 1 2
do
    for MAX_NR in 1 8 64
    do
        for helib_scale in {1..40}
        do
            # ./1_power_raising.out $TIMES_TO_SQUARE 0 $MAX_NR helib 16384 119 $helib_scale 2
            ./2_multiply_plain.out $TIMES_TO_SQUARE 0 $MAX_NR helib 16384 119 $helib_scale 2
        done
        for seal_scale in {1..40}
        do
            # ./1_power_raising.out $TIMES_TO_SQUARE 0 $MAX_NR seal 8192 60 2 $seal_scale $seal_scale
            ./2_multiply_plain.out $TIMES_TO_SQUARE 0 $MAX_NR seal 8192 60 2 $seal_scale $seal_scale
        done
    done
done

# Batch Size: 8192
for TIMES_TO_SQUARE in 1 2
do
    for MAX_NR in 1 8 64
    do
        for helib_scale in 1 5 10 15 20 25 30 35 40
        do
            # ./1_power_raising.out $TIMES_TO_SQUARE 0 $MAX_NR helib 32768 358 $helib_scale 6
            # ./1_power_raising.out $TIMES_TO_SQUARE 0 $MAX_NR helib 32768 299 $helib_scale 3
            # ./1_power_raising.out $TIMES_TO_SQUARE 0 $MAX_NR helib 32768 239 $helib_scale 2
            # ./2_multiply_plain.out $TIMES_TO_SQUARE 0 $MAX_NR helib 16384 119 $helib_scale 2
        done
        for seal_scale in 1 5 10 15 20 25 30 35 40
        do
            # ./1_power_raising.out $TIMES_TO_SQUARE 0 $MAX_NR seal 16384 60 7 $seal_scale $seal_scale
            # ./2_multiply_plain.out $TIMES_TO_SQUARE 0 $MAX_NR seal 8192 60 2 $seal_scale $seal_scale
        done
    done
done