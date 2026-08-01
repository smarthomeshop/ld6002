# LD6002B for ESPHome

ESPHome external component for the Hi-Link HLK-LD6002B 60GHz mmWave radar
(presence, multi-target 3D tracking and detection areas), used in SmartHomeShop
products.

ESPHome has no built-in LD6002B support yet. This component is vendored from
the pending upstream pull request
[esphome/esphome#13532](https://github.com/esphome/esphome/pull/13532) by
[@hepter](https://github.com/hepter), pinned at commit `52e0219`, so our
firmware builds do not depend on an open PR that can change or be rebased at
any time. All credit for the component goes to the original author. Licensed
under the ESPHome License (C++ under GPLv3, Python under MIT), see LICENSE.

## Usage

```yaml
external_components:
  - source: github://smarthomeshop/ld6002b@main
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

See `examples/` for a complete configuration with sensors, tracking targets
and the module boot pin handling. For internal use in SmartHomeShop firmware;
no support is provided for use outside SmartHomeShop products.

## Universal ceiling profile

LD6002B products can use the same mounting contract as LD2450 and LD2460:

```yaml
packages:
  mounting: github://smarthomeshop/radar-mounting/packages/mounting/ceiling.yaml@main
  radar_profile: github://smarthomeshop/radar-mounting/packages/radars/ld6002b.yaml@main
  radar_ceiling_mode: github://smarthomeshop/ld6002b/packages/ld6002b-ceiling-mode.yaml@main
```

This describes floor projection, metre-to-mm conversion, three-dimensional
coordinates and three targets. The final package creates the LD6002B
`installation_mode` select and sets it to `top` after startup; the generic
mounting package itself never sends radar-specific commands.

## Updating the vendored component

Pull the latest from the upstream PR branch and re-pin:

```bash
git clone --depth 1 --branch ld6002b https://github.com/hepter/esphome /tmp/hepter-esphome
rm -rf components/ld6002b && cp -r /tmp/hepter-esphome/esphome/components/ld6002b components/
```

Then compile-test against the ESPHome release our products use before pushing.
If the upstream PR merges into ESPHome, switch products to the built-in
component and archive this repository.
