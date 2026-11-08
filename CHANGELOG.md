# Changelog

## Unreleased

- Added experimental ESPHome UART decoding for LD6002C fall and presence
  reports and for LD6002 breathing and heart rate reports.
- Documented CeilSense Mini wiring and the LD6002B and LD6002C ceiling tests.

## [v0.1.0-beta.1] - 2026-10-12

- Published the first immutable experimental package release for the LD6002B.
- Pinned the vendored ESPHome component to upstream commit `52e0219`.
- Added the shared ceiling-mounting metadata and top-installation package used
  by Room Designer.
- Validated the example configuration and a complete ESP32-S3 firmware build
  with ESPHome 2026.7.3.
- Kept the release in beta until tracking and command behavior have been
  validated repeatedly on physical LD6002B hardware.
