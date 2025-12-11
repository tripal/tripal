![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

# Tripal 4 (Drupal 11)

## Compatibility

![Target Drupal Version 11.2.x-dev](https://img.shields.io/badge/Target%20Drupal%20Version-11.2.x-informational)

Tested on ![PostgreSQL 14](https://img.shields.io/badge/PostreSQL-14-success) - ![PostgreSQL 18](https://img.shields.io/badge/PostreSQL-18-success)

The following table proves the compatibility for the current development version of Tripal.

<<<<<<< HEAD
| Drupal      | 10.4.x              | 10.5.x              | 11.1.x              | 11.2.x              |
|-------------|---------------------|---------------------|---------------------|---------------------|
| **PHP 8.2** | ![Grid82-104-Badge] | ![Grid82-105-Badge] |                     |                     |
| **PHP 8.3** | ![Grid83-104-Badge] | ![Grid83-105-Badge] | ![Grid83-111-Badge] | ![Grid83-112-Badge] |
| **PHP 8.4** |                     |                     | ![Grid84-111-Badge] | ![Grid84-112-Badge] |
| **PHP 8.5** |                     |                     | ![Grid85-111-Badge] | ![Grid85-112-Badge] |
=======
| PHP\Drupal | 10.4.x-dev          | 10.5.x-dev          | 11.1.x-dev          | 11.2.x-dev          |
|------------|---------------------|---------------------|---------------------|---------------------|
| **PHP8.1** | ![Grid81-104-Badge] | ![Grid81-105-Badge] |                     |                     |
| **PHP8.2** | ![Grid82-104-Badge] | ![Grid82-105-Badge] |                     |                     |
| **PHP8.3** | ![Grid83-104-Badge] | ![Grid83-105-Badge] | ![Grid83-111-Badge] | ![Grid83-112-Badge] |
| **PHP8.4** |                     |                     | ![Grid84-111-Badge] | ![Grid84-112-Badge] |

>>>>>>> origin/4.x

[Grid81-104-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_4x.yml/badge.svg
[Grid81-105-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_5x.yml/badge.svg

[Grid82-104-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_4x.yml/badge.svg
[Grid82-105-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_5x.yml/badge.svg

[Grid83-104-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_4x.yml/badge.svg
[Grid83-105-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_5x.yml/badge.svg
[Grid83-111-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D11_1x.yml/badge.svg
[Grid83-112-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D11_2x.yml/badge.svg

[Grid84-111-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.4_D11_1x.yml/badge.svg
[Grid84-112-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.4_D11_2x.yml/badge.svg

[Grid85-111-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.5_D11_1x.yml/badge.svg
[Grid85-112-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.5_D11_2x.yml/badge.svg

### Code Coverage

This project uses QLTY Cloud to determine the quality of our codebase and the coverage of our test suite. Compatibility above is based on passing of this test suite.

[![Code Coverage](https://qlty.sh/gh/tripal/projects/tripal/coverage.svg)](https://qlty.sh/gh/tripal/projects/tripal)

[![Maintainability](https://qlty.sh/gh/tripal/projects/tripal/maintainability.svg)](https://qlty.sh/gh/tripal/projects/tripal)

### Compatibility of past Tripal versions

**NOTE: We currently recommend running the most recent development version when developing your production sites.**

| Tripal     | Drupal              | PHP         | PostgreSQL |
|------------|---------------------|-------------|------------|
| 4.0-alpha1 | >=9.2 <=10.0        | >=8.0 <=8.1 | >=13 <=16  |
| 4.0-alpha2 | >=10.0 <= 10.1      | >=8.1 <=8.2 | >=13 <=16  |
| 4.0-alpha3 | >=10.4 <= 11.2      | >=8.1 <=8.3 | >=13 <=17  |
| 4.0-dev    | >=10.4 <= 11.x.dev  | >=8.2 <=8.5 | >=14 <=18  |

## Current Timeline

### Required for Tripal 4.alpha4 Release

- Display suite / page work (partially complete; needs default layouts for existing content types + auto application).
- Search via Views (partially complete; waiting on default views for existing content types)

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
