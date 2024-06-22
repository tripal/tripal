<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalJob;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal\Kernel\Services\TripalJob\FakeClasses\callableClassForTripalJobs;

/**
 * Focused on testing callables as the callback.
 *
 * @group Tripal
 * @group TripalJobs
 */
class CallablesJobTest extends TripalTestKernelBase {
  use UserCreationTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'tripal'];

  /**
   * The current logged in test user.
   *
   * @var object
   */
  protected object $user;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // ... users need access to system.action config.
    $this->installConfig('system');
    // ... tripal jobs are associated with a user.
    $this->installEntitySchema('user');
    // ... now the tripal job tables themselves.
    $this->installSchema('tripal', ['tripal_jobs']);

    // Create and log-in a user.
    $this->user = $this->setUpCurrentUser();
  }

  /**
   * Data Provider: Provides valid jobs to create.
   *
   * @return array
   *   Each element is a test set to pass to testTripalJob_create_valid()
   *   and is expected to have the following keys:
   *    - details: An associative array of the job details.
   *        The following keys are allowed:
   *        - job_name: The human readable name for the job.
   *        - modulename: The name of the module adding the job.
   *        - callback: The name of a function to be called when the job is executed.
   *        - arguments:  An array of arguments to be passed on to the callback.
   *        - uid: The uid of the user adding the job
   *        - priority: The priority at which to run the job where the highest
   *          priority is 10 and the lowest priority is 1. The default
   *          priority is 10.
   *        - includes: An array of paths to files that should be included in order
   *          to execute the job. Use the module_load_include function to get a path
   *          for a given file.
   *        - ignore_duplicate: (Optional). Set to TRUE to ignore a job if it has
   *          the same name as another job which has not yet run. If TRUE and a job
   *          already exists then this object will reference the job already in the
   *          queue rather than a new submission.  The default is TRUE.
   */
  public function provideValidJobs() {
    $sets = [];

    $sets[] = [
      'details' => [
        'job_name' => 'Basic testing job with a simple callable.',
        'modulename' => 'tripal',
        'callback' => '\Drupal\Tests\tripal\Kernel\Services\TripalJob\FakeClasses\callableClassForTripalJobs::myCallbackMethod',
        'arguments' => [],
      ],
    ];

    return $sets;
  }

  /**
   * Tests the TripalJob::create() method can successfully create valid jobs.
   *
   * @dataProvider provideValidJobs
   */
  public function testTripalJob_create_valid($details) {

    // Add user to the job details since we can't do it in the data provider.
    $details['uid'] = $this->user->id();

    // Now try to create the job.
    $job_id = \Drupal::service('tripal.job')->create($details);
    $this->assertIsNumeric($job_id,
      "Creating a TripalJob was not successful as we did not have a job_id returned.");
  }
}
