![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

# Tripal 4 (Drupal 10)

                   _   _                 _____                 _                                  _
         /\       | | (_)               |  __ \               | |                                | |
        /  \   ___| |_ ___   _____      | |  | | _____   _____| | ___  _ __  _ __ ___   ___ _ __ | |_
       / /\ \ / __| __| \ \ / / _ \     | |  | |/ _ \ \ / / _ \ |/ _ \| '_ \| '_ ` _ \ / _ \ '_ \| __|
      / ____ \ (__| |_| |\ V /  __/     | |__| |  __/\ V /  __/ | (_) | |_) | | | | | |  __/ | | | |_
     /_/    \_\___|\__|_| \_/ \___|     |_____/ \___| \_/ \___|_|\___/| .__/|_| |_| |_|\___|_| |_|\__|
                                                                      | |
                                                                      |_|


## Compatibility

![Target Drupal Version 10.2.x-dev](https://img.shields.io/badge/Target%20Drupal%20Version-10.2.x-informational)

Tested on ![PostgreSQL 13](https://img.shields.io/badge/PostreSQL-13-success) + ![PostgreSQL 16](https://img.shields.io/badge/PostreSQL-16-success)

| Drupal      | 10.2.x              | 10.3.x              | 11.0.x              |
|-------------|---------------------|---------------------|---------------------|
| **PHP 8.1** | ![Grid81-102-Badge] | ![Grid81-103-Badge] |                     |
| **PHP 8.2** | ![Grid82-102-Badge] | ![Grid82-103-Badge] |                     |
| **PHP 8.3** | ![Grid83-102-Badge] | ![Grid83-103-Badge] | ![Grid83-110-Badge] |

[Grid81-102-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_2x.yml/badge.svg
[Grid81-103-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.1_D10_3x.yml/badge.svg

[Grid82-102-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_2x.yml/badge.svg
[Grid82-103-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.2_D10_3x.yml/badge.svg

[Grid83-102-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_2x.yml/badge.svg
[Grid83-103-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D10_3x.yml/badge.svg

[Grid83-110-Badge]: https://github.com/tripal/tripal/actions/workflows/MAIN-phpunit-php8.3_D11_0x.yml/badge.svg

### Code Coverage

This project uses Code Climate to determine the quality of our codebase and the coverage of our test suite. Compatibility above is based on passing of this test suite.

[![Test Coverage](https://api.codeclimate.com/v1/badges/994fcd39a0eef9cff742/test_coverage)](https://codeclimate.com/github/tripal/tripal/test_coverage)

[![Maintainability](https://api.codeclimate.com/v1/badges/994fcd39a0eef9cff742/maintainability)](https://codeclimate.com/github/tripal/tripal/maintainability)

## Current Timeline

### Required for Tripal 4.alpha3 Release

- Display suite / page work
- Importers: Publications
- Search (ElasticSearch + Views)

## How to get involved!

This upgrade to Drupal 10 is a community effort. As such, we NEED YOUR HELP!

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
