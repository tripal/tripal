.. _tests:

Unit Tests for Tripal
=======================

This guide is for developers looking to contribute code to the core Tripal project.  It introduces the testing philosophy and guidelines for Tripal core.  Tripal uses Tripal Test Suite, which brings bootstraps your Tripal site for PHPUnit.  It also provides conveniences like name spacing, seeders, transactions, and data factories.

Tripal Test Suite
-------------------

For a basic introduction of Tripal Testing, please see the `Test Suite documentation <https://tripaltestsuite.readthedocs.io/en/latest/>`_.

Installation
~~~~~~~~~~~~~~

After cloning the `Tripal Github repo <https://github.com/tripal/tripal>`_, you will need to install the developer dependencies required to run tests locally.  To do this, you'll need to `install Composer <https://getcomposer.org/doc/00-intro.md>`_, and then execute ``composer install`` in your project root.

Remember to run ``composer update`` to update Tripal TestSuite before writing and running new tests. This is especially important when running pull requests that contribute unit tests. If tests are passing on the Travis environment but not on your machine, running composer update might resolve the problem.

Testing criteria
-----------------

For facilitate accepting your pull requests, your code should include tests.  The tests should meet the following guidelines:

* All tests pass
* Tests pass in all environments (Travis)
* Tests don't modify the database (use transactions or clean up after yourself)
* Tests are properly organized (see organization section below)
* Tests run quietly

Test organization
------------------

Tests should be placed in ``tests/``.  This root directory contains the following files:
 - ``bootstrap.php``: Test directory configuration.  Don't modify this.
 - ``DatabasSeeders/``: `Database seeders <https://github.com/statonlab/TripalTestSuite#database-seeders>`_, for filling Chado with permanent test data.
 - ``DataFactory.php``: `Data factories <https://github.com/statonlab/TripalTestSuite#factories>`_, for providing test-by-test Chado data.
 - ``example.env``: An example environment file.  Configure this to match your development site and save as ``.env``.  Read more here: https://tripaltestsuite.readthedocs.io/en/latest/environment.html

Test files must end with ``Test.php`` to be recognized by PHPUnit.  The tests themselves should be organized by submodule, and category.

Submodules
~~~~~~~~~~~

* tripal
* tripal_bulk_loader
* tripal_chado
* tripal_chado_views
* tripal_daemon
* tripal_ds
* tripal_ws

Categories
~~~~~~~~~~

* API
* theme
* views
* drush
* fields
* entities
* admin
* loaders

So for example, tests for the file ``tripal/api/tripal.jobs.api.inc`` should go in ``tests/tripal/api/TripalJobsAPITest.php``. tests that don't fit in any of these categories should be placed in ``tests/[submodule]/``.

In order for tests to run locally, you'll need an environmental file ``tests/.env`` with the project root, base url, and locale.  See ``tests/example.env`` for an example.

Writing tests
--------------

Tagging tests
~~~~~~~~~~~~~~~~

You should tag your test with relevant groups.  For example, our Tripal Chado API tests should be tagged with ``@group api``.  We don't need to tag it with ``@group chado`` because it is in the *testsuite* (the submodule folder) Chado.

If your test is related to a specific issue on the Tripal repo, thats great! You can use the ``@ticket`` tag to link it: ie, ``@ticket 742`` for issue number 742.

Defining the test class
~~~~~~~~~~~~~~~~~~~~~~~~~~

The test class file should extend ``StatonLab\TripalTestSuite\TripalTestCase`` instead of ``TestCase`` to take advantage of the Tripal Test Suite tools.  Tests should use a database transaction to ensure the database state is the same at the start and end of the test case.  Your test class name should match the file.


.. code-block:: php

  use StatonLab\TripalTestSuite\DBTransaction;
  use StatonLab\TripalTestSuite\TripalTestCase;

  class TripalChadoOrganismAPITest extends TripalTestCase {
  	use DBTransaction;
  }


Defining individual tests
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An ideal test operates *independently* of other tests: by default, unit tests run in random order.  How, then, do we provide our test with relevant data?  We use **Factories**, which you can read about on in the `Tripal Test Suite documentation <https://tripaltestsuite.readthedocs.io/en/latest/factories.html>`_.  In the below example, we create an organism with known information, and assert that we can retrieve it with the Chado API functions.


.. code-block:: php


  namespace Tests\tripal_chado\api;

  use StatonLab\TripalTestSuite\DBTransaction;
  use StatonLab\TripalTestSuite\TripalTestCase;

  class TripalChadoOrganismAPITest extends TripalTestCase {

    use DBTransaction;

    /**
     * Test tripal_get_organism.
     *
     * @group api
     */
    public function test_tripal_get_organism() {

      $genus_string = 'a_genius_genus';
      $species_string = 'fake_species';

      $organism = factory('chado.organism')->create([
        'genus' => $genus_string,
        'species' => $species_string,
      ]);

      $results = [];

      $results[] = tripal_get_organism(['organism_id' => $organism->organism_id]);
      $results[] = tripal_get_organism([
        'genus' => $genus_string,
        'species' => $species_string,
      ]);

      foreach ($results as $result) {
        $this->assertNotFalse($result);
        $this->assertNotNull($result);
        $this->assertObjectHasAttribute('genus', $result);
        $this->assertEquals($genus_string, $result->genus);
      }
    }

    public function test_tripal_get_organism_fails_gracefully() {
      $result = tripal_get_organism([
        'genus' => uniqid(),
        'species' => uniqid(),
      ]);

      $this->assertNull($result);
    }
  }


.. note::

  You typically will want at least one test per public method in your file or class. Tests should start with ``test_``, otherwise it wont run by default in PHPUnit (you can also declare that it is a test in the method documentation using ``@test``.

Testing quietly
~~~~~~~~~~~~~~~~

Tests should run quietly.  If the output goes to standard out, you can use ``ob_start()`` and ``ob_end_clean()``.


.. code-block:: php


    ob_start();//dont display the job message
    $bool = tripal_chado_publish_records($values);
    ob_end_clean();


If the message comes from the Tripal error reporter, you must use ``"TRIPAL_SUPPRESS_ERRORS=TRUE"`` to suppress the Tripal error reporter message.

.. code-block:: php


  /**
   * Test chado_publish_records returns false given bad bundle.
   *
   * @group api
   */
  public function test_tripal_chado_publish_records_false_with_bad_bundle() {
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");//this will fail, so we suppress the tripal error reporter
    $bool = tripal_chado_publish_records(['bundle_name' => 'never_in_a_million_years']);
    $this->assertFalse($bool);
    putenv("TRIPAL_SUPPRESS_ERRORS");//unset
  }
