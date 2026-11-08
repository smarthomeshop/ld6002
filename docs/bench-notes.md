# CeilSense Mini radar bench notes

## Wiring

The Mini PCB v3 J5 connector carries a 3.3 V UART. Use one radar per bus. With
power disconnected, identify pin 1 by its marking. For a separately powered
radar, wire the connections below without plugging the module into J5.

| Radar pin | Connection |
| --- | --- |
| 1, 3V3 | Separate regulated 3.3 V supply with at least 1 A capacity. Leave J5 pin 1 isolated. |
| 2, GND | Supply ground and J5 pin 2 |
| 3, P19/BOOT1 | J5 pin 3. The Mini pulls this low at boot through R35. |
| 7, TX0 | J5 pin 7, ESP32-C6 RX on GPIO1 |
| 8, RX0 | J5 pin 8, ESP32-C6 TX on GPIO6, for radar commands only |

The Mini's LM3671 can supply 600 mA to the entire 3.3 V rail. Hi-Link specifies
at least 1 A of supply capacity for all three modules. It lists typical currents
of 135 mA for B and 120 mA for C, which explains why a bench setup may work with
a lower-rated supply. Keep the two supply outputs isolated and share ground
for UART. The module's UART uses 3.3 V logic.
[Hi-Link LD6002](https://www.hlktech.com/en/Goods-337.html) ·
[LD6002B](https://www.hlktech.com/Goods-338.html) ·
[LD6002C](https://www.hlktech.com/Goods-339.html) ·
[TI LM3671 specifications](https://www.ti.com/product/LM3671)

## LD6002B — ceiling tracking

On 8 November 2026 the Mini was mounted at 2.38 m with the radar in Top mode.
The module sent XYZ coordinates for individual targets in report `0x0A04`.
In this installation, the viewer used `2.38 m − radar Z` for the estimated
height of the tracked point. At the desk the median raw Z was about 1.52 m
sitting and 0.66 m standing. Those correspond to estimated point heights of
0.86 m and 1.72 m. A separate check directly beneath the radar confirmed the
direction of the Z axis. Posture labels used the two local reference
measurements. A repeatable tracking or seated-presence radius was outside
these trials.

## LD6002C — fall and presence

The C reports fall status (`0x0E02`) and presence (`0x0F09`) as one-byte
booleans. On the tested module, `0x0E0E` was a little-endian float in metres
below the radar. Subtract it from the installation height to estimate the
height of the tracked point. User-log mode can stream a debug point cloud
(`0x0A08`). It is a collection of reflections without the B's stable
per-target XYZ reports.

- With the default 60 cm threshold, walking out of the room triggered false
  alarms in this installation.
- With the installation height set to 2.38 m and the threshold to 25 cm, a
  repeat exit produced no alarm. Lying on one side also produced no alarm.
  The median reported height during that floor trial was approximately 28 cm.
- At 40 cm, the same floor trial triggered the module's fall status and
  activated the Mini firmware's red strobe after about 15 seconds of low
  readings. Reported point height during the sustained low period was
  approximately 15–35 cm. An equivalent exit trial at 40 cm is still needed.

The 40 cm threshold is a test setting for this room. The radar acknowledged
2.38 m installation height, sensitivity 10 and four horizontal limits of
1.5 m. Read settings with `0x0E06`. Set installation height with `0x0E04`
and the fall threshold with `0x0E08`. The
[ESPHome monitor example](../examples/ceilsense-mini-ld6002c.yaml) receives
reports at 115200 baud and leaves radar settings alone. The Mini's red ring
and USB live view were part of a separate bench firmware.
[Hi-Link LD6002C downloads](https://www.hlktech.com/Goods-339.html)

## LD6002 — breathing and heart rate

The module without a suffix uses `0x0A14` for breaths per minute and
`0x0A15` for beats per minute, both little-endian floats. The
[ESPHome bench example](../examples/ceilsense-mini-ld6002.yaml) starts at
1382400 baud, 8N1. Hi-Link has also documented versions using 115200 baud.
Use the setting that yields checksum-valid reports from the module on the bench.

On 8 November 2026 the ordinary LD6002 sent checksum-valid reports through
the Mini's J5 at 1382400 baud, with the module powered by J5 for a short test.
We received rate, phase (`0x0A13`) and target-distance (`0x0A16`) reports.
The previous 115200-baud capture contained bytes but no valid frames. The
UART result establishes communication on this unit. Supply voltage, ripple
and current were outside the scope of the test.

At a mounting height of 2.38 m, a one-minute desk trial to the side of the
radar yielded no usable measurements. In a second one-minute trial nearly
under the radar, it reported a target at approximately 1.00–1.05 m and
intermittent heart and breathing estimates. The reported rates were outside
the scope of any comparison against a reference instrument.

For the first trial, aim at the chest of one seated, still person about
0.4–1.5 m away. Allow roughly one minute for the first estimate. Record
actual module readings against a reference before evaluating accuracy or
ceiling placement.
[Hi-Link protocol v1.3](https://h.hlktech.com/download/HLK-LD6002/1/HLK-LD6002-%E5%91%BC%E5%90%B8%E5%BF%83%E7%8E%87%E6%A3%80%E6%B5%8B%E9%9B%B7%E8%BE%BE%E6%A8%A1%E7%BB%84%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AEV1.3.pdf) ·
[Hi-Link operating conditions](https://www.hlktech.com/en/Goods-337.html)
