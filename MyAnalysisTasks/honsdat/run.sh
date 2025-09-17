### Simple run script
# Tell stdout to be quiet
export O2_LOGLEVEL=error
export FAIRMQ_LOG_SEVERITY=error
export ROOT_LOG_LEVEL=Error
export FAIRMQ_NO_PROGRESS=true
# --severity error reiinclude this in optionS below
OPTION="-b --configuration json://myConfig_multi_file.json --pipeline track-propagation:8" # -b : No graphical display
o2-analysis-hons-data ${OPTION} #| o2-analysis-track-propagation ${OPTION} | o2-analysis-timestamp ${OPTION} | o2-analysis-tracks-extra-v002-converter ${OPTION} | o2-analysis-mccollision-converter ${OPTION}

### This one is for converting between run2&run3 data, makes a mess of amany a thing 

# o2-analysis-tracks-extra-v002-converter added based on comments in the mattermost channel, it's because of the type of data we're using ( run 2 ) and our O2 being primed for run 3

# Also found here: https://aliceo2group.github.io/analysis-framework/docs/troubleshooting/
