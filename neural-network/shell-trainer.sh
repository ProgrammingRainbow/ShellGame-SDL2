#!/usr/bin/env bash

# Defaults
FILENAME="bash-network"
LAYER_COUNT=1
LAYERS=(15)
POPULATION=48
SELECTION=$(( POPULATION * 2 / 3 ))
TRIALS=2
GENERATIONS=400
MUT_RATE=5
MUT_RANGE=5
X_RANGE=60
Y_RANGE=240

readonly MAX_LAYER_COUNT=5
readonly MAX_LAYER_SIZE=64
readonly MAX_POPULATION=500
readonly MAX_TRIALS=50
readonly MAX_GENERATIONS=100000
readonly MAX_RANGE=500

while [[ $# -gt 1 ]]; do
    case "$1" in
        --filename)
            shift
            FILENAME="$1"
            shift
            ;;
        --layers)
            shift
            LAYERS=()
            LAYER_COUNT=0
            for (( l = 0; l < MAX_LAYER_COUNT; l++)); do
                if (( $1 > 0 && $1 <= MAX_LAYER_SIZE )); then
                    LAYERS+=($1)
                    (( LAYER_COUNT++ ))
                    shift
                else
                    break
                fi
            done
            ;;
        --population)
            shift
            if (( $1 > 0 && $1 <= MAX_POPULATION )); then
                POPULATION=$1
            else
                echo "Error: --population must be between 1 and $MAX_POPULATION"
                exit 1
            fi
            shift
            ;;
        --selection)
            shift
            if (( $1 > 0 && $1 < MAX_POPULATION )); then
                SELECTION=$1
            else
                echo "Error: --selection must be between 1 and $MAX_POPULATION"
                exit 1
            fi
            shift
            ;;
        --trials)
            shift
            if (( $1 > 0 && $1 <= MAX_TRIALS )); then
                TRIALS=$1
            else
                echo "Error: --trials must be between 1 and $MAX_TRIALS"
                exit 1
            fi
            shift
            ;;
        --generations)
            shift
            if (( $1 > 0 && $1 <= MAX_GENERATIONS )); then
                GENERATIONS=$1
            else
                echo "Error: --generations must be between 1 and $MAX_GENERATIONS"
                exit 1
            fi
            shift
            ;;
        --mut_rate)
            shift
            if (( $1 > 0 && $1 <= MAX_RANGE )); then
                MUT_RATE=$1
            else
                echo "Error: --mut_rate must be between 1 and $MAX_RANGE"
                exit 1
            fi
            shift
            ;;
        --mut_range)
            shift
            if (( $1 > 0 && $1 <= MAX_RANGE )); then
                MUT_RANGE=$1
            else
                echo "Error: --mut_range must be between 1 and $MAX_RANGE"
                exit 1
            fi
            shift
            ;;
        --x_range)
            shift
            if (( $1 > 0 && $1 <= MAX_RANGE )); then
                X_RANGE=$1
            else
                echo "Error: --x_range must be between 1 and $MAX_RANGE"
                exit 1
            fi
            shift
            ;;
        --y_range)
            shift
            if (( $1 > 0 && $1 <= MAX_RANGE )); then
                Y_RANGE=$1
            else
                echo "Error: --y_range must be between 1 and $MAX_RANGE"
                exit 1
            fi
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if (( SELECTION > POPULATION )); then
    echo "Error: selection $SELECTION must be less than $POPULATION."
    exit 1
fi

if (( LAYER_COUNT == 0 )); then
    echo "Error: layer_count $LAYER_COUNT should be between 1 and $MAX_LAYER_COUNT."
    exit 1
fi

# Global constants.
readonly WIDTH=800
readonly HALF_WIDTH=$(( WIDTH / 2 ))
readonly HEIGHT=600
readonly PLAYER_OFFSET=48
readonly PLAYER_TOP=377
readonly PLAYER_Y=456
readonly GROUND=535
readonly VELOCITY=5
readonly FLAKES=15

readonly FILENAME
readonly LAYER_COUNT
readonly LAYERS
readonly POPULATION
readonly SELECTION
readonly TRIALS
readonly GENERATIONS
readonly MUT_RATE
readonly MUT_RANGE
readonly X_RANGE
readonly Y_RANGE

readonly INPUTS=15
readonly OUTPUTS=2
readonly DISPLAY=$(( 20 < SELECTION ? 20 : SELECTION ))
readonly INDEX_NETWORK=12
readonly SCALE=1000

NETWORK_SIZE=$(( ( INPUTS + 1 ) * LAYERS[0] ))
for (( i = 1; i < LAYER_COUNT; i++ )); do
    (( NETWORK_SIZE += ( LAYERS[i - 1] + 1 ) * LAYERS[i] ))
done
(( NETWORK_SIZE += ( LAYERS[LAYER_COUNT - 1] + 1 ) * OUTPUTS + INDEX_NETWORK ))
readonly NETWORK_SIZE

echo
echo "filename:  $FILENAME"
echo "layer_count: $LAYER_COUNT"
printf "%s" "layers:"
for (( i = 0; i < LAYER_COUNT; i++ )); do
    (( LAYERS[i] )) && printf " %s" "${LAYERS[$i]}"
done
echo
echo "population $POPULATION"
echo "selection $SELECTION"
echo "trials: $TRIALS"
echo "generations: $GENERATIONS"
echo "mut_rate: $MUT_RATE"
echo "mut_range: $MUT_RANGE"
echo "x_range: $X_RANGE"
echo "y_range: $Y_RANGE"
echo

sleep 5

run_network() {
    local -i player_x=$HALF_WIDTH
    local -i score=0
    local -i playing=1
    local -i rel_x=0
    local -i rel_y=0
    local -i input_count=$INPUTS
    local -i index=$INDEX_NETWORK
    local -i l=0
    local -i n=0
    local -i i=0
    local -i f=0
    local -i t=0

    local -a flakes_x
    local -a flakes_y
    local -a inputs
    local -a outputs
    local -a network

    if [ -e "/tmp/network_$1" ]; then
        read -a network < "/tmp/network_$1"
        rm -f "/tmp/network_$1"
    else
        for (( n = INDEX_NETWORK; n < NETWORK_SIZE; n++ )); do
            (( network[n] = ( RANDOM % ( MUT_RANGE * 2 + 1 ) ) - MUT_RANGE ))
        done
        (( network[0] = 0 )) # Score
        (( network[1] = TRIALS ))
        (( network[2] = 0 )) # Generations
        (( network[3] = 0 )) # Mutations
        (( network[4] = LAYER_COUNT ))
        (( network[5] = LAYERS[0] ))
        (( network[6] = LAYERS[1] ))
        (( network[7] = LAYERS[2] ))
        (( network[8] = LAYERS[3] ))
        (( network[9] = LAYERS[4] ))
        (( network[10] = X_RANGE ))
        (( network[11] = Y_RANGE ))
    fi

    for (( f = 0; f < FLAKES; f++ )); do
        (( flakes_x[f] = RANDOM % WIDTH ))
        (( flakes_y[f] = -( RANDOM % ( HEIGHT * 2 ) ) ))
    done

    for (( t = 0; t < TRIALS; t++ )); do
        while (( playing )); do
            for (( f = 0; f < FLAKES; f++ )); do
                (( rel_x = ( flakes_x[f] - player_x ) ))
                if (( rel_x > HALF_WIDTH )); then
                    (( rel_x -= WIDTH ))
                elif (( rel_x < -HALF_WIDTH )); then
                    (( rel_x += WIDTH ))
                fi
                if (( rel_x > 0 )); then
                    if (( rel_x < X_RANGE )); then
                        (( rel_x = X_RANGE - rel_x ))
                    else
                        (( rel_x = 0 ))
                    fi
                else
                    if (( rel_x > -X_RANGE )); then
                        (( rel_x = -X_RANGE + rel_x ))
                    else
                        (( rel_x = 0 ))
                    fi
                fi
                (( rel_y = ( PLAYER_Y - flakes_y[f] ) ))
                (( rel_y < 0)) && (( rel_y = -rel_y ))
                if (( rel_y < Y_RANGE )); then
                    (( rel_y = Y_RANGE - rel_y ))
                else
                    (( rel_y = 0 ))
                fi
                (( inputs[f] = rel_y * rel_x * 200 ))
            done

            # Hidden layers
            input_count=$INPUTS
            index=$INDEX_NETWORK
            for (( l = 0; l < LAYER_COUNT; l++ )); do
                for (( n = 0; n < LAYERS[l]; n++ )); do
                    (( outputs[n] = 0 ))
                    for (( i = 0; i < input_count; i++ )); do
                        (( outputs[n] += inputs[i] * network[index] ))
                        (( index++ ))
                    done
                    (( outputs[n] /= SCALE ))
                    (( outputs[n] += network[index] ))
                    (( index++ ))
                    (( outputs[n] = (( outputs[n] > 0 ) ? outputs[n] : 0 ) ))
                done
                (( input_count = LAYERS[l] ))
                inputs=("${outputs[@]}")
            done

            # Output layer
            for (( n = 0; n < OUTPUTS; n++ )); do
                (( outputs[n] = 0 ))
                for (( i = 0; i < input_count; i++ )); do
                    (( outputs[n] += inputs[i] * network[index] ))
                    (( index++ ))
                done
                (( outputs[n] /= SCALE ))
                (( outputs[n] += network[index] ))
                (( index++ ))
                (( outputs[n] = (( outputs[n] > 0 ) ? outputs[n] : 0 ) ))
            done

            if (( outputs[0] > outputs[1] )); then
                (( player_x -= VELOCITY ))
                (( player_x < 0 )) && (( player_x += WIDTH ))
            fi
            if (( outputs[1] > outputs[0] )); then
                (( player_x += VELOCITY ))
                (( player_x > WIDTH )) && (( player_x -= WIDTH ))
            fi

            for (( f = 0; f < FLAKES; f++ )); do
                (( flakes_y[f] += VELOCITY ))
                if (( flakes_y[f] > PLAYER_TOP )); then
                    if (( flakes_y[f] > GROUND )); then
                        (( flakes_x[f] = RANDOM % WIDTH ))
                        (( flakes_y[f] = -( RANDOM % HEIGHT ) ))
                        continue
                    elif (( flakes_x[f] > player_x - PLAYER_OFFSET )); then
                        if (( flakes_x[f] < player_x + PLAYER_OFFSET )); then
                            if (( f < 5)); then
                                playing=0
                            else
                                (( score++ ))
                                (( flakes_x[f] = RANDOM % WIDTH ))
                                (( flakes_y[f] = -( RANDOM % HEIGHT ) ))
                            fi
                        fi
                    fi
                fi
            done
        done
        playing=1
        for (( f = 0; f < FLAKES; f++ )); do
            (( flakes_x[f] = RANDOM % WIDTH ))
            (( flakes_y[f] = -( RANDOM % ( HEIGHT * 2 ) ) ))
        done
    done
    (( network[0] = score ))
    printf "%s " "${network[@]}" > "/tmp/network_$1"
}

for (( p = 0; p < $POPULATION; p++ )); do
    rm -f "/tmp/network_$p"
done

for (( s = 0; s < $SELECTION; s++ )); do
    rm -f "/tmp/top_$s"
done

for (( gen = 0; gen < $GENERATIONS; gen++ )); do
    pids=()
    for (( p = 0; p < $POPULATION; p++ )); do
        run_network $p &
        pids+=($!)
    done

    indexed=()
    for (( p = 0; p < $POPULATION; p++ )); do
        wait "${pids[$p]}"
        read -a network < "/tmp/network_$p"
        indexed+=("${network[0]}:$p")
    done 

    # Sort and store in a new array
    IFS=$'\n' sorted=($(printf "%s\n" "${indexed[@]}" | sort -nr))
    IFS=' '

    for (( s = 0; s < $SELECTION; s++ )); do
        val="${sorted[s]%%:*}"
        idx="${sorted[s]##*:}"
        rm -f "/tmp/top_$s"
        cp "/tmp/network_$idx" "/tmp/top_$s"
    done

    for (( p = 0; p < $POPULATION; p++ )); do
        rm -f "/tmp/network_$p"
        read -a network < "/tmp/top_$(( p % SELECTION ))"

        (( network[2] += 1 ))

        if (( p >= SELECTION )); then
            (( network[3] += 1 ))
            for (( i = INDEX_NETWORK; i < NETWORK_SIZE; i++ )); do
                if (( (RANDOM % SCALE) < MUT_RATE )); then
                    (( network[i] += ( RANDOM % ( MUT_RANGE * 2 + 1) ) - MUT_RANGE ))
                fi
            done
        fi

        printf "%s " "${network[@]}" > "/tmp/network_$p"
    done

    if (( p < DISPLAY )); then
        printf "%s" "Rank: $p, Fitness: $(( network[0] / network[1] )), "
        printf "%s\n" "Generation: ${network[2]}, Mutations: ${network[3]}"
    fi

    echo "generation: $(( gen + 1 ))"
done

cp "/tmp/network_0" "$FILENAME"

for (( p = 0; p < $POPULATION; p++ )); do
    rm -f "/tmp/network_$p"
done

for (( s = 0; s < $SELECTION; s++ )); do
    rm -f "/tmp/top_$s"
done

