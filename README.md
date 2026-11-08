# LD6002 family for ESPHome

External components for the Hi-Link HLK-LD6002 family of 60 GHz radars.

| Module | ESPHome configuration | Reports | Mini or CeilSense example |
| --- | --- | --- | --- |
| HLK-LD6002 | `ld6002:` with `model: ld6002` | Breathing rate and heart rate | [LD6002](examples/ceilsense-mini-ld6002.yaml) |
| HLK-LD6002B | `ld6002b:` | Presence, up to three XYZ targets and detection areas | [LD6002B](examples/ceilsense-ld6002b.yaml) |
| HLK-LD6002C | `ld6002:` with `model: ld6002c` | Presence, fall status and depth below the radar | [LD6002C](examples/ceilsense-mini-ld6002c.yaml) |

The LD6002 and LD6002C profiles are experimental and read-only. The stock
radars were exercised on a CeilSense Mini using separate USB test firmware.
See the [bench notes](docs/bench-notes.md) for what was observed on hardware.
The repository compiles all three example configurations in CI.

## LD6002 and LD6002C

Add the component source to your ESPHome configuration. On the CeilSense Mini
v3, J5 pin 7 goes to ESP32-C6 GPIO1. Check the wiring and power requirements in
the [bench notes](docs/bench-notes.md#wiring) before connecting a module.

```yaml
external_components:
  - source: github://smarthomeshop/ld6002@main
    components: [ld6002]

uart:
  id: radar_uart
  rx_pin: GPIO1
  baud_rate: 1382400
  rx_buffer_size: 2048

ld6002:
  model: ld6002
  uart_id: radar_uart
  breathing_rate:
    name: "Breathing Rate"
  heart_rate:
    name: "Heart Rate"
```

The example uses the UART rate verified on our LD6002. Some firmware versions
use 115200 baud. Use the rate that matches the connected module.

For the LD6002C, use **115200 baud** and replace the `ld6002:` section with:

```yaml
ld6002:
  model: ld6002c
  uart_id: radar_uart
  presence:
    name: "Presence"
  fall_detected:
    name: "Fall Detected"
  target_depth:
    name: "Target Depth"
```

The C's mounting height, threshold and detection limits are stored on the
radar. Configure them separately with the Hi-Link tool or UART commands.
Target depth is the reported distance below the radar in metres. The C's fall
status comes from its own firmware. The bench trials saw both false alarms
and missed detections at different thresholds.

The complete examples use Wi-Fi and the native ESPHome API. To compile an
example from a local clone, create `examples/secrets.yaml` with `wifi_ssid`
and `wifi_password`, then run `esphome compile examples/<example>.yaml`.

## LD6002B

The existing `ld6002b:` platform, ceiling package and `v0.1.0-beta.1` release
are preserved. The old repository URL can still be used by existing Git-based
ESPHome configurations. Use the new URL for new configurations.

```yaml
external_components:
  - source: github://smarthomeshop/ld6002@v0.1.0-beta.1
    components: [ld6002b]

uart:
  id: uart_ld6002b
  tx_pin: GPIO16
  rx_pin: GPIO15
  baud_rate: 115200

ld6002b:
  id: ld6002b_radar
  uart_id: uart_ld6002b
  throttle: 2s
```

See the [complete B example](examples/ceilsense-ld6002b.yaml) for entity
configuration, module boot handling and target reporting. Match the example
pins to your CeilSense board.

### Ceiling profile

```yaml
packages:
  mounting: github://smarthomeshop/radar-mounting/packages/mounting/ceiling.yaml@v1.0.0
  radar_profile: github://smarthomeshop/radar-mounting/packages/radars/ld6002b.yaml@v1.0.0
  radar_ceiling_mode: github://smarthomeshop/ld6002/packages/ld6002b-ceiling-mode.yaml@v0.1.0-beta.1
```

The final package sets the B's installation mode to `top` after startup.
The generic mounting package describes floor projection and unit conversion
for Room Designer.

### Upstream and license

The LD6002B implementation is vendored from [ESPHome PR #13532](https://github.com/esphome/esphome/pull/13532)
by [@hepter](https://github.com/hepter) at commit `52e0219`. The ESPHome
license applies (C++ GPLv3 and Python MIT). See [LICENSE](LICENSE).
