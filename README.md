# LD6002 family for ESPHome

ESPHome configurations and components for the Hi-Link HLK-LD6002 family of
60 GHz radars.

| Module | ESPHome configuration | Reports | Mini or CeilSense example |
| --- | --- | --- | --- |
| HLK-LD6002 | `ld6002:` with `model: ld6002` | Breathing rate and heart rate | [LD6002](examples/ceilsense-mini-ld6002.yaml) |
| HLK-LD6002B | Built-in `ld6002b:` in ESPHome 2026.8.0 or newer | Presence, up to three XYZ targets and detection areas | [LD6002B](examples/ceilsense-ld6002b.yaml) |
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

Use the [built-in ESPHome LD6002B component](https://esphome.io/components/sensor/ld6002b/)
with ESPHome 2026.8.0 or newer. The example pins below match the ESP32-S3
example. Use the radar UART pins for your board.

```yaml
uart:
  id: uart_ld6002b
  tx_pin: GPIO16
  rx_pin: GPIO15
  baud_rate: 115200

ld6002b:
  id: ld6002b_radar
  uart_id: uart_ld6002b
```

When migrating an existing configuration, remove `ld6002b` from
`external_components` and remove `throttle` and `max_data_len` from
`ld6002b:`. Use per-sensor filters for reporting intervals and rename any
`dop_idx` sensor keys to `doppler_index`. If your board has no `wakeup_pin`
configured, omit `auto_wake` and `wakeup_pulse_ms`. Keep `ld6002` in
`external_components` if you also use an LD6002 or LD6002C.

The [complete B example](examples/ceilsense-ld6002b.yaml) shows the entities,
module boot handling and target reporting. The vendored B driver remains
available at `v0.1.0-beta.1` for firmware built with older ESPHome versions.

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

The legacy LD6002B implementation is vendored from
[ESPHome PR #13532](https://github.com/esphome/esphome/pull/13532) by
[@hepter](https://github.com/hepter) at commit `52e0219`. The ESPHome license
applies (C++ GPLv3 and Python MIT). See [LICENSE](LICENSE).
