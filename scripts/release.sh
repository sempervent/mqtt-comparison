#!/bin/bash

# Release script for MQTT Multi-Language Benchmarks
set -e

# Get current version
CURRENT_VERSION=$(cat VERSION)
echo "Current version: $CURRENT_VERSION"

# Check if we're on main branch
if [ "$(git branch --show-current)" != "main" ]; then
    echo "Error: Must be on main branch to release"
    exit 1
fi

# Check if working directory is clean
if [ -n "$(git status --porcelain)" ]; then
    echo "Error: Working directory is not clean"
    git status --short
    exit 1
fi

# Create git tag
TAG="v$CURRENT_VERSION"
echo "Creating tag: $TAG"

git tag -a "$TAG" -m "Release $CURRENT_VERSION

- Enhanced documentation with live dashboard integration
- Fixed GitHub Pages URL configuration
- Generated comprehensive benchmark reports
- Added version management and CI improvements"

echo "Tag created: $TAG"
echo "To push the tag: git push origin $TAG"
echo "To push all changes: git push origin main"
