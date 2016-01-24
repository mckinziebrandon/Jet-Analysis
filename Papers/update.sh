#!/bin/bash
# Update this directory to have same contents as ~/Dropbox/Jet\ Radius\ Analysis/Papers/.

DROPBOXFILES=~/Dropbox/JetRadiusAnalysis/Papers/*
printf "Updating files . . . \n"

# Loop over all papers in Dropbox.
for filename in ${DROPBOXFILES}; do
    # Remove path prefix from filename.
    FILE=${filename##*/}

    # If file not found in this directory, copy it here.
    if [ ! -f "$FILE" ] ; then
        echo "Copying $FILE into directory . . . "
        cp "$filename" .
    fi 
done;

printf "Update Complete!\n"


