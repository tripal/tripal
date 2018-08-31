
This guide is for developers looking to contribute code to the core Tripal project.  It introduces the testing philosophy and guidelines for Tripal core.  Tripal uses Tripal Test Suite, which brings bootstraps your Tripal site for PHPUnit.  It also provides conveniences like namespacing, seeders, transactions, and data factories.


## Tripal Test Suite

For a basic introduction of Tripal Testing, please see the [Test Suite repo](https://github.com/statonlab/TripalTestSuite).


### Installation

After cloning the [Tripal github repo](https://github.com/tripal/tripal), you will need to install the developer dependencies required to run tests locally.  To do this, you'll need to [install Composer](https://getcomposer.org/doc/00-intro.md), and then execute `composer install` in your project root.

Remember to run `composer update` to update TripalTestSuite before writing and running new tests. This is especially important when running pull requests that contribute unit tests. If tests are passing on the Travis environment but not on your machine, running composer update might resolve the problem.

## Testing criteria
For facilitate accepting your pull requests, your code should include tests.  The tests should meet the following guidelines:

* All tests pass
* Tests pass in all environments (Travis)
* Tests don't modify the database (use transactions or clean up after yourself)
* Tests are properly organized (see organization section below)
* Tests run quietly

## Test organization

Tests should be placed in `tests/`.  This root directory contains the following files:
* `bootstrap.php` - Test directory configuration.  Don't modify this.
* `DatabasSeeders/` - [Database seeders](https://github.com/statonlab/TripalTestSuite#database-seeders), for filling Chado with permanent test data
* `DataFactory.php` - [Data factories](https://github.com/statonlab/TripalTestSuite#factories), for providing test-by-test Chado data.
* `example.env` - An example environment file.  Configure this to match your development site and save as `.env`.

Tests must end with `Test.php` to be recognized by PHPUnit.  The tests themselves should be organized by submodule, and category.  

##### Submodules

* tripal
* tripal_bulk_loader
* tripal_chado
* tripal_chado_views
* tripal_daemon
* tripal_ds
* tripal_ws

##### Categories
* API
* theme
* views
* drush
* fields
* entities
* admin
* loaders

So for example, tests for the file `tripal/api/tripal.jobs.api.inc` should go in `tests/tripal/api/TripalJobsAPITest.php`. tests that don't fit in any of these categories should be placed in `tests/[submodule]/`.

In order for tests to run locally, you'll need an environmental file `tests/.env` with the project root, base url, and locale.  See `tests/example.env` for an example.

## Writing tests

When doing test driven development, you might be running tests over and over.  To speed you along, you can assign your tests a unique `@group` tag, ie `@group failing`.  Then specify your novel group when you run phpunit, ie `phpunit --group failing`.

You should also tag your test with relevant groups.  For example, our Tripal Chado API tests should be tagged with `@group api`.  We don't tag it with `@group chado` because it is in the *testsuite* (the submodule folder) Chado.


## Defining the test class


Once you've identified where your test will go, we can start writing our test.


Tripal Test suite provides a convenient way to start writing a test class: `tripaltest make:test TestName`.  From the project root, our example  `./vendor/bin/tripaltest make:test tripal_chado/api/TripalChadoOrganismAPITest`.  This will generate a test stub file with namespacing.


The test class file should extend `StatonLab\TripalTestSuite\TripalTestCase` instead of `TestCase` to take advantage of the Tripal Test Suite tools.  For example, to wrap our tests in a database transaction (so we can indescriminately insert and modify without having to revert consider how to clean up the database after), we use `StatonLab\TripalTestSuite\DBTransaction;`.  Your test class name should match the file.


```php
use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoOrganismAPITest extends TripalTestCase {
	use DBTransaction;

```


You typically will want at least one test per public method in your file or class. In the below test class, I define a single test: `test_tripal_get_organism()`.  The test should start with `test_`, otherwise it wont run by default in PHPUnit (you can also declare that it is a test in the method documentation using `@test`.   

An ideal test operates *independently* of other tests: by default, unit tests run in random order.  How, then, do we provide our test with relevant data?  We use **Factories**, which you can read about on the [Tripal Test Suite repo](https://github.com/statonlab/TripalTestSuite#factories).  In the below example, we create an organism with known information, and assert that we can retrieve it with the Chado API functions.



```php

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

```
### Testing quietly

Code may output errors when failing intentionally, or as part of job progress.  This can clutter the test environment, so you should wrap the offending methods.  If the output goes to standard out, you can use `ob_start()` and `ob_end_clean()`.


```php

    ob_start();//dont display the job message
    $bool = tripal_chado_publish_records($values);
    ob_end_clean();

```

If the message comes from the Tripal error reporter, you must use `"TRIPAL_SUPPRESS_ERRORS=TRUE"` to suppress the tripal error reporter message.

```php

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

 ```
