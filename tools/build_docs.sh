#!/usr/bin/env bash
set -euo pipefail
python3 tools/render_reports.py
mkdocs build --strict
