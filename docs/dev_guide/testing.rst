
Automated Testing
===================

Tripal 4 is being developed with automated testing as it is upgraded. This greatly improves the stability of our software and our ability to fix any bugs. We highly recommend developing automated testing alongside any extension modules you create! This guide is intended to explain how automated testing is working for Tripal 4 and help you develop similar tests for your extensions.

Additional Resources:
 - `Official Drupal 8: Testing Documentation <https://www.drupal.org/docs/testing>`_
 - `Official Drupal 8: PHPUnit file structure, namespace, and required metadata <https://www.drupal.org/docs/testing/phpunit-in-drupal/phpunit-file-structure-namespace-and-required-metadata>`_
 - `Official Drupal 8: Running PHPUnit Tests <https://www.drupal.org/docs/testing/phpunit-in-drupal/running-phpunit-tests>`_
 - `Official Drupal 8: PHPUnit Browser test tutorial <https://www.drupal.org/docs/testing/phpunit-in-drupal/phpunit-browser-test-tutorial>`_
 - `Drupal 8: Writing Your First Unit Test With PHPUnit <https://www.axelerant.com/resources/team-blog/drupal-8-writing-your-first-unit-test-with-phpunit>`_
 - `Writing Simple (PHPUnit) Tests for Your D8 module <https://www.mediacurrent.com/blog/writing-simple-phpunit-tests-your-d8-module/>`_

How run automated tests locally
---------------------------------

See the `Drupal "Running PHPUnit tests" guide <https://www.drupal.org/node/2116263>`_ for instructions on running tests on your local environment. In order to ensure our Tripal functional testing is fully bootstrapped, tests should be run from Drupal core.

If you are using the docker distributed with this module, then you can run tests using:

.. code:: bash

  docker exec --workdir=/var/www/drupal9/web/modules/contrib/tripal t4d8 phpunit
