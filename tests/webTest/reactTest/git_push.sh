#!/bin/bash

# Check if a commit message is provided
if [ -z "$1" ]; then
  echo "Usage: ./git-push.sh \"Your commit message\""
  exit 1
fi

# Stage all changes
git add .

# Commit with the message provided as the first argument
git commit -m "$1"

# Push to the current branch
git push

