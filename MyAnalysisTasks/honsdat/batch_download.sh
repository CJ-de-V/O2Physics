#!/bin/bash

mkdir -p ./data  # Ensure output directory exists

MAX_PARALLEL=5   # Number of parallel downloads allowed
JOBS=0

# Download the first 5 AO2D files as stated in da for loopdiloop

for ((i=21; i<=30; i++)); do
  PADDED=$(printf "%03d" $i)
  GRID_PATH="/alice/sim/2025/LHC25b4b2/0/559408/AOD/${PADDED}/AO2D.root"
  LOCAL_FILE="./data/AO2D_${PADDED}.root"
  #Add a line that prints the filenames to a file in the format data/AO2D_numberhere.root so that we don't have to copypaste it here

  (
    echo "Copying $GRID_PATH -> $LOCAL_FILE"
    alien_cp  "alien:${GRID_PATH}" "file:${LOCAL_FILE}"
  ) &  # Run in background, VERBOSE & TRY 3x in case of failure: -v -retry 3 removed because of malarkey

  ((JOBS++))
  
  # Wait if too many jobs are running
  if [[ $JOBS -ge $MAX_PARALLEL ]]; then
    wait -n  # Wait for any job to finish
    ((JOBS--))
  fi
done

wait  # Wait for all remaining jobs to finish

