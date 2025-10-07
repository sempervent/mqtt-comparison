# CPP

| enc | variant | qos | count | bytes(avg) | pub_ms | recv_ms | tps |
|---|---|---:|---:|---:|---|---|---|
| cbor | small | 1 | 5 | 98 | n=5 min=0.68 p50=0.71 p95=0.72 max=0.75 avg=0.71 | n=5 min=0.32 p50=0.34 p95=0.35 max=0.38 avg=0.34 | n=5 min=1300.00 p50=1400.00 p95=1450.00 max=1450.00 avg=1400.00 |
| json | small | 1 | 5 | 128 | n=5 min=0.61 p50=0.68 p95=0.72 max=0.75 avg=0.68 | n=5 min=0.28 p50=0.32 p95=0.35 max=0.38 avg=0.33 | n=5 min=1300.00 p50=1450.00 p95=1500.00 max=1600.00 avg=1450.00 |
| msgpack | small | 1 | 5 | 95 | n=5 min=0.55 p50=0.59 p95=0.62 max=0.64 avg=0.60 | n=5 min=0.22 p50=0.26 p95=0.28 max=0.30 avg=0.26 | n=5 min=1550.00 p50=1650.00 p95=1700.00 max=1800.00 avg=1660.00 |
| protobuf | small | 1 | 5 | 89 | n=5 min=0.52 p50=0.56 p95=0.58 max=0.61 avg=0.56 | n=5 min=0.22 p50=0.25 p95=0.26 max=0.28 avg=0.25 | n=5 min=1600.00 p50=1750.00 p95=1800.00 max=1900.00 avg=1750.00 |