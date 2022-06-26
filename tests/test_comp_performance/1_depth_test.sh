PRINT_HEADERS=1
BATCH_SIZE=4096
HELIB_M=$(( BATCH_SIZE * 4 ))
SEAL_N=$(( BATCH_SIZE * 2 ))
HELIB_BITS=119
# HELIB_BITS=100 # Test to see if fewer bits affect accuracy at each scale
HELIB_C=2
SEAL_MAX_LOG_Q=202 # For 128-bit security
SEAL_BIG_PRIMES_BITS=60
# SEAL_BIG_PRIMES_BITS=50
SCALES=(5 10 15 20 25 30 35 40)
MAX_NR=1
for HELIB_SCALE in "${SCALES[@]}"
do
    for TIMES_TO_SQUARE in {1..10}
    do
        ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
        # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
        PRINT_HEADERS=0
    done
done
for SEAL_SCALE in "${SCALES[@]}"
do
    for TIMES_TO_SQUARE in {1..10}
    do
        SEAL_NUM_SMALL_PRIMES=$(( ( $SEAL_MAX_LOG_Q-(2*$SEAL_BIG_PRIMES_BITS) ) / $SEAL_SCALE ))
        SEAL_NUM_SMALL_PRIMES=$(( $SEAL_NUM_SMALL_PRIMES < $TIMES_TO_SQUARE ? $SEAL_NUM_SMALL_PRIMES : $TIMES_TO_SQUARE ))
        ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
        # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
    done
done

BATCH_SIZE=8192
HELIB_M=$(( BATCH_SIZE * 4 ))
SEAL_N=$(( BATCH_SIZE * 2 ))
SEAL_MAX_LOG_Q=411 # For 128-bit security
for HELIB_SCALE in "${SCALES[@]}"
do
    for HELIB_C in 6 3 2
    do
        if [[ "$HELIB_C" -eq 6 ]]; then HELIB_BITS=358; fi
        if [[ "$HELIB_C" -eq 3 ]]; then HELIB_BITS=299; fi
        if [[ "$HELIB_C" -eq 2 ]]; then HELIB_BITS=239; fi
        for TIMES_TO_SQUARE in {1..10}
        do
            ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
            # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
            PRINT_HEADERS=0
        done
    done
done
for SEAL_SCALE in "${SCALES[@]}"
    for TIMES_TO_SQUARE in {1..10}
    do
        SEAL_NUM_SMALL_PRIMES=$(( ( $SEAL_MAX_LOG_Q-(2*$SEAL_BIG_PRIMES_BITS) ) / $SEAL_SCALE ))
        SEAL_NUM_SMALL_PRIMES=$(( $SEAL_NUM_SMALL_PRIMES < $TIMES_TO_SQUARE ? $SEAL_NUM_SMALL_PRIMES : $TIMES_TO_SQUARE ))
        ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
        # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
    done
done

BATCH_SIZE=16384
HELIB_M=$(( BATCH_SIZE * 4 ))
SEAL_N=$(( BATCH_SIZE * 2 ))
SEAL_MAX_LOG_Q=827 # For 128-bit security
MAX_NR=1
for HELIB_C in 8 6 4 3 2
do
    if [[ "$HELIB_C" -eq 8 ]]; then HELIB_BITS=725; fi
    if [[ "$HELIB_C" -eq 6 ]]; then HELIB_BITS=717; fi
    if [[ "$HELIB_C" -eq 4 ]]; then HELIB_BITS=669; fi
    if [[ "$HELIB_C" -eq 3 ]]; then HELIB_BITS=613; fi
    if [[ "$HELIB_C" -eq 2 ]]; then HELIB_BITS=558; fi
    for HELIB_SCALE in "${SCALES[@]}"
    do
        for TIMES_TO_SQUARE in {1..20}
        do
            ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
            # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR helib $HELIB_M $HELIB_BITS $HELIB_SCALE $HELIB_C
            PRINT_HEADERS=0
        done
    done
done
for SEAL_SCALE in "${SCALES[@]}"
do
    for TIMES_TO_SQUARE in {1..20}
    do
        SEAL_NUM_SMALL_PRIMES=$(( ( $SEAL_MAX_LOG_Q - ( 2 * $SEAL_BIG_PRIMES_BITS ) ) / $SEAL_SCALE ))
        SEAL_NUM_SMALL_PRIMES=$(( $SEAL_NUM_SMALL_PRIMES < $TIMES_TO_SQUARE ? $SEAL_NUM_SMALL_PRIMES : $TIMES_TO_SQUARE ))
        ./1_power_raising $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
        # ./2_multiply_plain $TIMES_TO_SQUARE $PRINT_HEADERS $MAX_NR seal $SEAL_N $SEAL_BIG_PRIMES_BITS $SEAL_NUM_SMALL_PRIMES $SEAL_SCALE $SEAL_SCALE
    done
done