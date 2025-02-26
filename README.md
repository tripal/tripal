![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

# Tripal 4 (Drupal 11)

## Compatibility

![Target Drupal Version 11.1.x-dev](https://img.shields.io/badge/Target%20Drupal%20Version-11.1.x-informational)

Tested on ![PostgreSQL 13](https://img.shields.io/badge/PostreSQL-13-success) - ![PostgreSQL 17](https://img.shields.io/badge/PostreSQL-17-success)

The following table proves the compatibility for the current development version of Tripal.

| Drupal      | 10.3.x              | 10.4.x              | 10.5.x              | 11.0.x              | 11.1.x              |
|-------------|---------------------|---------------------|---------------------|---------------------|---------------------|
| **PHP 8.1** | ![Grid81-103-Badge] | ![Grid81-104-Badge] | ![Grid81-105-Badge] |                     |                     |
| **PHP 8.2** | ![Grid82-103-Badge] | ![Grid82-104-Badge] | ![Grid82-105-Badge] |                     |                     |
| **PHP 8.3** | ![Grid83-103-Badge] | ![Grid83-104-Badge] | ![Grid83-105-Badge] | ![Grid83-110-Badge] | ![Grid83-111-Badge] |

[Grid81-103-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_3x.yml/badge.svg
[Grid81-104-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_4x.yml/badge.svg
[Grid81-105-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_5x.yml/badge.svg

[Grid82-103-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_3x.yml/badge.svg
[Grid82-104-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_4x.yml/badge.svg
[Grid82-105-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_5x.yml/badge.svg

[Grid83-103-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_3x.yml/badge.svg
[Grid83-104-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_4x.yml/badge.svg
[Grid83-105-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_5x.yml/badge.svg

[Grid83-110-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D11_0x.yml/badge.svg
[Grid83-111-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D11_1x.yml/badge.svg

### Code Coverage

This project uses Code Climate to determine the quality of our codebase and the coverage of our test suite. Compatibility above is based on passing of this test suite.

[![Test Coverage](https://api.codeclimate.com/v1/badges/994fcd39a0eef9cff742/test_coverage)](https://codeclimate.com/github/tripal/tripal/test_coverage)

[![Maintainability](https://api.codeclimate.com/v1/badges/994fcd39a0eef9cff742/maintainability)](https://codeclimate.com/github/tripal/tripal/maintainability)

### Compatibility of past Tripal versions

| Tripal     | Drupal              | PHP         | PostgreSQL |
|------------|---------------------|-------------|------------|
| 4.x-dev    | >=10.3 <11.0        | >=8.1 <=8.3 | >=13 <=17  |
| 4.x-dev    | >=11.0 <=11.1.x-dev | 8.3         | >=16 <=17  |
| 4.0-alpha1 | >=9.2 <=10.0        | >=8.0 <=8.1 | 13         |
| 4.0-alpha2 | >=10.0 <= 10.1      | >=8.1 <=8.2 | 13         |

## Current Timeline

### Required for Tripal 4.alpha3 Release

- Display suite / page work (partially complete)
- ✅ Importers: Publications
- Search (ElasticSearch + Views)

## How to get involved!

This upgrade to Drupal 11 is a community effort. As such, we NEED YOUR HELP!

  - To get involved, please join [our Tripal Slack](http://tripal.info/join/slack) and comment in the #core-dev channel.
    - Alternatively, feel free to contact Lacey-Anne Sanderson through Slack direct message.
    - We can use help both with programming, documentation, outreach and welcome all individuals from all backgrounds!
  - We prefer [automated testing](https://tripaldoc.readthedocs.io/en/latest/dev_guide/testing.html) for all Pull Requests (PRs) and are happy to guide you through the process!
    - Just comment on the issue if you need help designing tests
    - If you need help implementing tests see our [documentation](https://tripaldoc.readthedocs.io/en/latest/dev_guide/testing.html)
  - If more documentation would be helpful, [please create an issue here](https://github.com/tripal/tripal_doc/issues) and we'll add it.

## Documentation

[Documentation for Tripal 4 has begun on ReadtheDocs](https://tripaldoc.readthedocs.io/en/latest/dev_guide.html).

  - [Installation](https://tripaldoc.readthedocs.io/en/latest/install.html)
  - [Automated Testing](https://tripaldoc.readthedocs.io/en/latest/dev_guide/testing.html)
  - [Developer Guide](https://tripaldoc.readthedocs.io/en/latest/dev_guide.html)
  - [Contribution Guide](https://tripaldoc.readthedocs.io/en/latest/contributing.html)

## Tripal Docker

Tripal Docker is currently focused on Development and Automated Testing. There will be a production focused Tripal Docker soon and if you're interested in helping or providing tips -please join us on our Slack channel!

For more information about how to use our fully functional development docker, see [our documentation on ReadtheDocs!](https://tripaldoc.readthedocs.io/en/latest/install/docker.html)
