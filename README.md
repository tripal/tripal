![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

# Tripal 4 Drupal 8

## UNDER ACTIVE DEVELOPMENT

This project acts as the home of Tripal 4 development. Once Tripal 4 is stable, it will be merged back into the [Core Tripal Repository](https://github.com/tripal/tripal). 

## Currently working on [Group 1](https://github.com/tripal/t4d8/issues/1)

We currently have working entities for the following: Tripal vocabularies, Tripal Terms, Tripal Content Types, Tripal Content! However, nothing is connected to Chado at this point (to ensure it is chado-agnostic).

### How to get involved!

This upgrade to Drupal 8 is a community effort. As such, we NEED YOUR HELP! In order to make it less overwhelming for you to jump in and help, the PMC (project-management-committee) has created issues tagged `good first issue`. To take one on, just comment with your intent! We're also in the process of adding documentation through RTD to help orient new developers. Please comment on the issue [How can we help you?](https://github.com/tripal/t4d8/issues/16) with any ideas for documentation you would find useful and any tips which helped you get started!

### Our upgrade Process (in detail)

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
* PostgreSQL
* PHP 7.1+
* UNIX/Linux

# Installation

1. Install [Drupal 8.x](https://www.drupal.org/docs/develop/using-composer/using-composer-to-install-drupal-and-manage-dependencies).
2. Clone this repository in your `web/modules` directory.
3. Enable Tripal in your site using the Administration Toolbar > Extend
4. Use drush to rebuild the cache (`drush cache-rebuild`) so Tripal menu items appear correctly.

## Quick Start (development only)

If you do not yet have a Drupal 8 Tripal 4 development site, these are my steps to set one up. This assumes you already have a local Apache-PostgreSQL-PHP server set-up with `~/Sites` being your web-accessible directory.

```
composer create-project drupal-composer/drupal-project:8.x-dev tripal4 --stability dev --no-interaction
psql --command="CREATE USER tripaladmin WITH PASSWORD 'tripal4developmentonlylocal'"
psql --command="CREATE DATABASE tripal4_dev WITH OWNER tripaladmin"
cd tripal4
drush site-install standard \
  --db-url=pgsql://tripaladmin:tripal4developmentonlylocal@localhost/tripal4_dev \
  --account-mail="tripaladmin@localhost" \
  --account-name=tripaladmin \
  --account-pass=some_admin_password \
  --site-mail="tripaladmin@localhost" \
  --site-name="Tripal 4 Development"
cd web/modules
git clone https://github.com/tripal/t4d8.git
drush en tripal
```
You now have a fully installed Tripal 4 site!

## Docker

The Tripal docker included in this repository is still a work in progress and we would greatly welcome help completing it! If you want to get started fast, I recommend the [official Drupal 8 Docker from DockerHub](https://hub.docker.com/_/drupal). To install Tripal on it, simply clone this repository into the `web/modules` directory and enable.

# Development Testing

See the [Drupal "Running PHPUnit tests" guide](https://www.drupal.org/node/2116263) for instructions on running tests on your local environment. In order to ensure our Tripal functional testing is fully bootstrapped, tests should be run from Drupal core.

The following are the exact steps to run tests assuming you set things up as specified in the quick start above.

```
cd ~/Sites/tripal4/web
export SIMPLETEST_BASE_URL=http://localhost/tripal4/web
export SIMPLETEST_DB=pgsql://tripaladmin:tripal4developmentonlylocal@localhost/tripal4_dev
export BROWSER_OUTPUT_DIRECTORY=~/Sites/tripal4/web/sites/default/simpletest
../vendor/bin/phpunit --configuration core modules/t4d8/tripal/tests/
```
