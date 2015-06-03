#!/bin/bash
NUM_FRAMES=$1
TEMP_DIR=/tmp/sample/
FRAMERATE=24
SIZE=1280x920
mkdir -p ${TEMP_DIR}

for i in $(seq -f %05g 1 $NUM_FRAMES);do

    echo "$i of $NUM_FRAMES"
    convert -size $SIZE xc:White \
      -gravity Center \
      -weight 700  \
      -pointsize 200 \
      -annotate 0 "FRAME \n ${i}" \
      ${TEMP_DIR}/frame_${i}.png > /dev/null 2>&1
done

ffmpeg -f image2 -framerate $FRAMERATE \
    -pattern_type sequence \
        -start_number 1 \
        -r $FRAMERATE -i ${TEMP_DIR}/frame_%05d.png -s $SIZE -f sample.avi
