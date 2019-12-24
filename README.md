![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

# Tripal 4 Drupal 8

## UNDER ACTIVE DEVELOPMENT

This project acts as the home of Tripal 4 development. Once Tripal 4 is stable, it will be merged back into the [Core Tripal Repository](https://github.com/tripal/tripal). 

## Currently working on [Group 1](https://github.com/tripal/t4d8/issues/1)

We currently have working entities for the following: Tripal vocabularies, Tripal Terms, Tripal Content Types, Tripal Content! However, nothing is connected to Chado at this point.

### How to Contribute

Tripal 4 development has been planned in the issue queue of this repository with the entire code-based of Tripal 3 being catagorized into groups which should be completed in order. For a summary of the tasks assigned to a given group, go to the issue labelled with the `roadmap` and group tag for a specific group. For example, for Group 1, the task list is in #1 which has both the `Roadmap` and `Group 1` tags.

To aid in the development of Tripal 4, 
1. Choose a task from the current group
2. Create an issue stating your intention
3. Tag this issue with the `Task` and `Group 1` tags.
4. Keep track of your progress and design in this issue
5. Once the task is complete, create a PR referencing this issue.
6. Once the PR is merged, check the task checkbox in the original `Roadmap` issue.

# Required Dependencies
* Drupal:
  * Drupal 8.x
  * Drupal core modules: Search, Path, View, Entity, and PHP modules.
  * Drupal contributed modules:
* PostgreSQL
* PHP 7.1+
* UNIX/Linux

# Installation

1. Install [Drupal 8.x](https://www.drupal.org/docs/develop/using-composer/using-composer-to-install-drupal-and-manage-dependencies).
2. Clone this repository in your `web/modules` directory.
3. Enable Tripal in your site using the Administration Toolbar > Extend
4. Use drush to rebuild the cache (`drush cache-rebuild`) so Tripal menu items appear correctly.


# Development Testing

See the [Drupal "Running PHPUnit tests" guide](https://www.drupal.org/node/2116263) for instructions on running tests on your local environment. In order to ensure our Tripal functional testing is fully bootstrapped, tests should be run from Drupal core. Specifically, in your Drupal root run the following command from your drupal root to run all Tripal tests.

```
./vendor/bin/phpunit modules/t4d8/tripal/tests/
```
