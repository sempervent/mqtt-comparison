# JULIA

| enc | variant | qos | count | bytes(avg) | pub_ms | recv_ms | tps |
|---|---|---:|---:|---:|---|---|---|
| json | small | 1 | 5 | 128 | n=5 min=1.10 p50=1.30 p95=1.40 max=1.50 avg=1.30 | n=5 min=0.50 p50=0.60 p95=0.70 max=0.80 avg=0.64 | n=5 min=650.00 p50=800.00 p95=800.00 max=900.00 avg=770.00 |
| msgpack | small | 1 | 5 | 95 | n=5 min=0.90 p50=1.10 p95=1.20 max=1.30 avg=1.10 | n=5 min=0.30 p50=0.40 p95=0.50 max=0.60 avg=0.44 | n=5 min=750.00 p50=900.00 p95=1000.00 max=1100.00 avg=910.00 |