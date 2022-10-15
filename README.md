![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

# Tripal 4 (Drupal 9 + 10)

                   _   _                 _____                 _                                  _   
         /\       | | (_)               |  __ \               | |                                | |  
        /  \   ___| |_ ___   _____      | |  | | _____   _____| | ___  _ __  _ __ ___   ___ _ __ | |_
       / /\ \ / __| __| \ \ / / _ \     | |  | |/ _ \ \ / / _ \ |/ _ \| '_ \| '_ ` _ \ / _ \ '_ \| __|
      / ____ \ (__| |_| |\ V /  __/     | |__| |  __/\ V /  __/ | (_) | |_) | | | | | |  __/ | | | |_
     /_/    \_\___|\__|_| \_/ \___|     |_____/ \___| \_/ \___|_|\___/| .__/|_| |_| |_|\___|_| |_|\__|
                                                                      | |                             
                                                                      |_|                             


This project acts as the home of Tripal 4 development. Once Tripal 4 is stable, it will be merged back into the [Core Tripal Repository](https://github.com/tripal/tripal).

## Compatibility

![Target Drupal Version 9.4.x-dev](https://img.shields.io/badge/Target%20Drupal%20Version-9.4.x-informational)

![PostgreSQL 13](https://img.shields.io/badge/PostreSQL-13-success)

| Drupal      | 9.2.x                                                                                                                                                                    | 9.3.x                                                                                                                                                                        | 9.4.x                                                                                                                                                                        | 9.5.x                                                                                                                                                                        | 10.0.x                                                                                                                                                                         |
|-------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **PHP 8.0** | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_2x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_2x.yml) | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_3x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_3x.yml)     | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_4x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_4x.yml)     | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_5x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8_D9_5x.yml)     |                                                                                                                                                                                |
| **PHP 8.1** |                                                                                                                                                                          | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8.1_D9_3x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8.1_D9_3x.yml) | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8.1_D9_4x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8.1_D9_4x.yml) | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8.1_D9_5x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/MAIN-phpunit-php8.1_D9_5x.yml) | [![PHPUnit](https://github.com/tripal/t4d8/actions/workflows/phpunit-php8.1-D10-0x.yml/badge.svg)](https://github.com/tripal/t4d8/actions/workflows/phpunit-php8.1-D10-0x.yml) |


## Current Timeline

### Required for Tripal 4.alpha1 Release

- ✅ Testing on 9.5.x + 10.0.x w/ target 9.4.x ([#257](https://github.com/tripal/t4d8/issues/257))
- Make Tripal DBX more stable ([#217](https://github.com/tripal/t4d8/issues/217), [#218](https://github.com/tripal/t4d8/issues/218))
- Ensure Content Type is using Terms fully ([#256](https://github.com/tripal/t4d8/issues/256))
- ✅ Automated creation of fields for Chado ([#258](https://github.com/tripal/t4d8/issues/258))
- ✅ Organism complex field (tests foreign key fields; [#259](https://github.com/tripal/t4d8/issues/259) )
- [IN REVIEW] Properties complex field (tests linking table fields; [#265](https://github.com/tripal/t4d8/issues/265))
- Type/CVterm complex field (needed for many base tables, )
- ✅ GFF3 Importer (needed for basic loading of data to test content types; [#275](https://github.com/tripal/t4d8/pull/275))
- Documentation + Tutorials for upgrading + creating fields
- ✅ Implementing core Chado Storage for fields ([#236](https://github.com/tripal/t4d8/issues/236), [#237](https://github.com/tripal/t4d8/issues/237))
- Merge the t4d8 repository back into the tripal repository

### Required for Tripal 4.alpha2 Release

- Publishing of the pages
- Finish implementing Chado Storage for fields ([#238](https://github.com/tripal/t4d8/issues/238), [#239](https://github.com/tripal/t4d8/issues/239))
- Drush generator/updator for fields
- Update existing core fields
- Finish Chado Vocabs/ID Spaces/Terms ([#242](https://github.com/tripal/t4d8/issues/242), [#243](https://github.com/tripal/t4d8/issues/243))
- Legacy API
- Move data type specific functionality into extension modules ([#281](https://github.com/tripal/t4d8/issues/281))

### Required for Tripal 4.alpha3 Release

- Display suite / page work
- Importers: Publications, Taxonomy, Newick
- Search (ElasticSearch rather then Views)

## How to get involved!

This upgrade to Drupal 9 is a community effort. As such, we NEED YOUR HELP!

  - To get involved, please join [our Tripal Slack](http://tripal.info/join/slack) and comment in the #core-dev channel.
    - Alternatively, feel free to contact Lacey-Anne Sanderson through Slack direct message.
    - We can use help both with programming, documentation, outreach and welcome all individuals from all backgrounds!
  - We prefer [automated testing](https://tripal4.readthedocs.io/en/latest/dev_guide/testing.html) for all Pull Requests (PRs) and are happy to guide you through the process!
    - Just comment on the issue if you need help designing tests
    - If you need help implementing tests see our [documentation](https://tripal4.readthedocs.io/en/latest/dev_guide/testing.html)
  - If more documentation would be helpful, [let us know what is missing here](https://github.com/tripal/t4d8/issues/16) and we'll add it

## Documentation

[Documentation for Tripal 4 has begun on ReadtheDocs](https://tripal4.readthedocs.io/en/latest/dev_guide.html). **Please keep in mind the URL for this documentation will change once Tripal 4 is released.**

  - [Installation](https://tripal4.readthedocs.io/en/latest/install.html)
  - [Automated Testing](https://tripal4.readthedocs.io/en/latest/dev_guide/testing.html)
  - [Developer Guide](https://tripal4.readthedocs.io/en/latest/dev_guide.html)
  - [Contribution Guide](https://tripal4.readthedocs.io/en/latest/contributing.html)

## Tripal Docker

Tripal Docker is currently focused on Development and Unit Testing. There will be a production focused Tripal Docker soon and if you're interested in helping or providing tips -please join us on our Slack channel!

For more information about how to use our fully functional development docker, see [our documentation on ReadtheDocs!](https://tripal4.readthedocs.io/en/latest/install/docker.html)
