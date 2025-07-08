#!/bin/bash

# Script to create a GitHub repository for the MentraOS Display Connector

echo "Creating GitHub repository for MentraOS Display Connector..."

# Initialize git if not already done
if [ ! -d .git ]; then
  git init
  echo "Git repository initialized."
fi

# Add all files
git add .

# Initial commit
git commit -m "Initial commit: MentraOS Display Connector"

# Create a new repository on GitHub (requires GitHub CLI)
if command -v gh &> /dev/null; then
  echo "Creating GitHub repository using GitHub CLI..."
  gh repo create mentraos-display-connector --public --source=. --push
else
  echo "GitHub CLI not found. Please install it or create the repository manually:"
  echo "1. Go to https://github.com/new"
  echo "2. Create a new repository named 'mentraos-display-connector'"
  echo "3. Run the following commands:"
  echo "   git remote add origin git@github.com:yourusername/mentraos-display-connector.git"
  echo "   git branch -M main"
  echo "   git push -u origin main"
fi

echo "Done!"
