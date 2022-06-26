PRINT_HEADERS=1
BATCH_SIZE=4096
HELIB_M=$(( BATCH_SIZE * 4 ))
SEAL_N=$(( BATCH_SIZE * 2 ))
HELIB_BITS=119
# HELIB_BITS=100 # Test to see if fewer bits affect accuracy at each scale
HELIB_C=2
SEAL_BIG_PRIMES_BITS=60
# SEAL_BIG_PRIMES_BITS=50
SEAL_NUM_SMALL_PRIMES=2
for TIMES_TO_SQUARE in 1 2
do
    for MAX_NR in 1 8 64
    do
        for HELIB_SCALE in {1..40}
        do
            ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
            # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
            PRINT_HEADERS=0
        done
        for SEAL_SCALE in {1..40}
        do
            ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
            # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
        done
    done
done

BATCH_SIZE=8192
HELIB_M=$(( BATCH_SIZE * 4 ))
SEAL_N=$(( BATCH_SIZE * 2 ))
SEAL_NUM_SMALL_PRIMES=7
for TIMES_TO_SQUARE in 1 2
do
    for MAX_NR in 1 8 64
    do
        for HELIB_C in 6 3 2
        do
            if [[ "$HELIB_C" -eq 6 ]]; then HELIB_BITS=358; fi
            if [[ "$HELIB_C" -eq 3 ]]; then HELIB_BITS=299; fi
            if [[ "$HELIB_C" -eq 2 ]]; then HELIB_BITS=239; fi
            for HELIB_SCALE in {1..40}
            do
                ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
                # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
                PRINT_HEADERS=0
            done
        done
        for SEAL_SCALE in {1..40}
        do
            ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
            # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
        done
    done
done

BATCH_SIZE=16384
HELIB_M=$(( BATCH_SIZE * 4 ))
SEAL_N=$(( BATCH_SIZE * 2 ))
SEAL_NUM_SMALL_PRIMES=17
TIMES_TO_SQUARE=1
MAX_NR=1
HELIB_C=8 # for scale, C doesn't matter so much
if [[ "$HELIB_C" -eq 8 ]]; then HELIB_BITS=725; fi
if [[ "$HELIB_C" -eq 6 ]]; then HELIB_BITS=717; fi
if [[ "$HELIB_C" -eq 4 ]]; then HELIB_BITS=669; fi
if [[ "$HELIB_C" -eq 3 ]]; then HELIB_BITS=613; fi
if [[ "$HELIB_C" -eq 2 ]]; then HELIB_BITS=558; fi
for HELIB_SCALE in {1..40}
do
    ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
    # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
    PRINT_HEADERS=0
done
for SEAL_SCALE in {1..40}
do
    ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
    # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
done