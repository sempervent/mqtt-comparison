#!/usr/bin/env python3
import os, json, csv, statistics as stats, glob, datetime, pathlib
from collections import defaultdict

DOCS = pathlib.Path("docs")
ASSETS = DOCS / "assets" / "charts"
REPORTS = DOCS / "reports"

def rows_from_file(p):
    try:
        if p.suffix.lower()==".json":
            payload = json.loads(p.read_text())
            if isinstance(payload, dict): yield payload
            elif isinstance(payload, list):
                for row in payload: yield row
        elif p.suffix.lower()==".csv":
            with p.open() as f:
                for row in csv.DictReader(f):
                    yield row
        else:
            # allow .jsonl (one json per line)
            with p.open() as f:
                for line in f:
                    line=line.strip()
                    if not line: continue
                    try: yield json.loads(line)
                    except: pass
    except Exception as e:
        yield {"_error": f"Failed to parse {p}: {e}"}

def normalize(r):
    if "_error" in r: return r
    def get(k, d=None): return r.get(k, d)
    return {
        "lang": get("lang") or "unknown",
        "role": get("role") or "pub",
        "enc":  get("enc")  or "json",
        "variant": str(get("variant") or "small"),
        "qos": int(get("qos") or 1),
        "bytes": int(get("bytes") or get("payload_bytes") or 0),
        "pub_ms": float(get("pub_ms") or 0),
        "recv_ms": float(get("recv_ms") or 0),
        "tps": float(get("tps") or 0),
        "ts": get("ts") or datetime.datetime.utcnow().isoformat()
    }

def load_all():
    records=[]
    for p in glob.glob("results/**/*", recursive=True):
        p=pathlib.Path(p)
        if not p.is_file(): continue
        for r in rows_from_file(p):
            records.append(normalize(r))
    return [r for r in records if "_error" not in r]

def agg(records):
    by_lang = defaultdict(list)
    for r in records:
        by_lang[r["lang"]].append(r)
    return by_lang

def mk(parent, name):
    parent.mkdir(parents=True, exist_ok=True)
    return parent / name

def stat_block(rows, key):
    xs=[float(r.get(key,0)) for r in rows if r.get(key) not in (None,"")]
    if not xs: return "n=0"
    def p(q): 
        i=int(q*(len(xs)-1))
        return f"{sorted(xs)[i]:.2f}"
    return f"n={len(xs)} min={min(xs):.2f} p50={p(.5)} p95={p(.95)} max={max(xs):.2f} avg={sum(xs)/len(xs):.2f}"

def write_index(by_lang):
    idx = mk(REPORTS, "index.md")
    lines = ["# Benchmark Reports",
             "",
             "This section is generated from `results/`. Missing pages mean no data yet.",
             ""]
    # summary table
    lines += ["| Language | Encodings | Latest Samples |",
              "|---|---|---|"]
    for lang, rows in sorted(by_lang.items()):
        encs = sorted({r["enc"] for r in rows})
        latest = max(rows, key=lambda r: r["ts"]) if rows else None
        lines += [f"| **{lang}** | {', '.join(encs)} | {latest['ts'] if latest else '—'} |"]
    idx.write_text("\n".join(lines))

def write_lang_pages(lang, rows):
    LDIR = mk(REPORTS, lang)
    # language index
    lines=["# " + lang.upper(), "",
           "| enc | variant | qos | count | bytes(avg) | pub_ms | recv_ms | tps |",
           "|---|---|---:|---:|---:|---|---|---|"]
    from collections import Counter, defaultdict
    key=lambda r:(r["enc"], r["variant"], r["qos"])
    buckets=defaultdict(list)
    for r in rows: buckets[key(r)].append(r)
    for (enc,variant,qos), xs in sorted(buckets.items()):
        avg_bytes = sum(x["bytes"] for x in xs)/len(xs)
        lines.append(f"| {enc} | {variant} | {qos} | {len(xs)} | {avg_bytes:.0f} | {stat_block(xs,'pub_ms')} | {stat_block(xs,'recv_ms')} | {stat_block(xs,'tps')} |")
    mk(LDIR, "index.md").write_text("\n".join(lines))
    # per-encoding pages
    for enc in sorted({r["enc"] for r in rows}):
        enc_rows=[r for r in rows if r["enc"]==enc]
        page=["# " + f"{lang.upper()} — {enc.upper()}",
              "",
              f"_Samples: {len(enc_rows)}_",
              "",
              "## Distribution (pub_ms)",
              "",
              "```text",
              stat_block(enc_rows,"pub_ms"),
              "```",
              "",
              "## Distribution (recv_ms)",
              "",
              "```text",
              stat_block(enc_rows,"recv_ms"),
              "```"]
        mk(LDIR, f"{enc}.md").write_text("\n".join(page))

def main():
    records = load_all()
    if not records:
        mk(REPORTS,"index.md").write_text("# Benchmark Reports\n\n_No results yet. Run the harness to populate `results/`._\n")
        return
    by_lang = agg(records)
    write_index(by_lang)
    for lang, rows in by_lang.items():
        write_lang_pages(lang, rows)

if __name__ == "__main__":
    main()
