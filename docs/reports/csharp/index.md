# CSHARP

| enc | variant | qos | count | bytes(avg) | pub_ms | recv_ms | tps |
|---|---|---:|---:|---:|---|---|---|
| cbor | small | 1 | 5 | 98 | n=5 min=1.80 p50=2.10 p95=2.30 max=2.40 avg=2.12 | n=5 min=0.90 p50=1.10 p95=1.20 max=1.30 avg=1.10 | n=5 min=410.00 p50=470.00 p95=500.00 max=550.00 avg=472.00 |
| json | small | 1 | 5 | 128 | n=5 min=1.60 p50=1.90 p95=2.10 max=2.20 avg=1.92 | n=5 min=0.80 p50=1.00 p95=1.10 max=1.20 avg=1.00 | n=5 min=450.00 p50=520.00 p95=550.00 max=620.00 avg=522.00 |
| msgpack | small | 1 | 5 | 95 | n=5 min=1.30 p50=1.60 p95=1.70 max=1.80 avg=1.58 | n=5 min=0.60 p50=0.70 p95=0.80 max=0.90 avg=0.74 | n=5 min=550.00 p50=620.00 p95=650.00 max=750.00 avg=630.00 |
| protobuf | small | 1 | 5 | 89 | n=5 min=1.10 p50=1.30 p95=1.40 max=1.50 avg=1.30 | n=5 min=0.40 p50=0.50 p95=0.60 max=0.70 avg=0.54 | n=5 min=650.00 p50=800.00 p95=800.00 max=900.00 avg=770.00 |