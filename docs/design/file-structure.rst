Module + File Structure
=========================

Tripal is a package of multiple Drupal modules (currently 4) with common documentation meant to be used together. We separate our code into multiple modules to allow site developers to choose the functionality they want and to improve maintainability. All modules require the base ``Tripal`` module but all others should be independent of each other completely and the base Tripal module should be able to be used alone.

- ``tripal``: contains all generic Tripal functionality with a focus on APIs + Vocabularies, and Entities.
- ``tripal_chado``: implements APIs in the ``tripal`` folder with specifics for supporting Chado. Additionally, this includes many data importers and eventually fields, as well as, Chado-specific APIs.
- ``tripal_console``: Tripal implementations of `Drupal Console commands <https://drupalconsole.com/docs/ro/commands/>`_. This is focused on making the development of Tripal easier and is meant to include commands for generating Tripal/Chado plugin files and re-writing Tripal 3 field classes in the new Tripal 4 way. Note: `Drush commands <https://www.drush.org/latest/>`_ are still used for the administration of Tripal and should go in the appropriate submodule.
- ``tripaldocker``: Provides a `Docker <https://www.docker.com/>`_ image currently focused on Tripal development. There is a plan to make this a production-ready Docker image in the future.
- ``docs``: contains our `official Tripal 4 ReadtheDocs documentation <https://tripal4.readthedocs.io/en/latest/>`_.
- ``.github``: contains GitHub-specific files such as our `testing workflow/actions <https://github.com/tripal/t4d8/actions>`_.
- ``.gitignore``: includes patterns for files that should not be committed to our repository using git.
- ``composer.json``: described our PHP package to `Packagist <https://packagist.org/packages/tripal/tripal>`_ using `Composer <https://getcomposer.org/>`_.
- ``composer.lock``: formed when you install Tripal using composer and keeps track of any dependencies. We commit it so you can see versions of dependencies tests were last run using.
- ``phpunit.xml``: our `PHPUnit <https://phpunit.readthedocs.io>`_ test configuration.
- ``README.md``: our face to the developer community and the best place to start.
