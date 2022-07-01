set -x
CONFIGURATIONS=(
    "helib 16384 119 5 2"
    "seal 8192 60 4 20 20"
    "helib 16384 119 25 2"
    "seal 8192 60 2 40 40"

    "helib 32768 358 9 6"
    "seal 16384 60 10 25 25"
    "helib 32768 358 24 6"
    "seal 16384 60 7 40 40"

    "helib 65536 717 8 6"
    "seal 32768 60 10 25 25"
    "helib 65536 717 23 6"
    "seal 32768 60 17 40 40"
)
NUM_TESTS=1
PRINT_HEADERS=0
for NUM_FILTERS in {1..4}
do
    (
        echo "Unloading weights for NUM_FILTERS=$NUM_FILTERS";
        cd chest/; 
        rm -r weights; 
        python3 chestmnist.py chestmnist_K$NUM_FILTERS.h5
    )
    for CFG in "${CONFIGURATIONS[@]}"
    do
    # CFG="${CONFIGURATIONS[11]}"
        ./chest/chestmnist $NUM_FILTERS $NUM_TESTS $PRINT_HEADERS $CFG
        PRINT_HEADERS=0
    done
done
