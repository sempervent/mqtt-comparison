# Build & Preview Docs

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
python tools/render_reports.py
mkdocs serve
```

To publish:

1. Push to `main` and let GitHub Actions deploy to Pages.
2. In **Settings → Pages**, choose **GitHub Actions** as the source (one-time).
