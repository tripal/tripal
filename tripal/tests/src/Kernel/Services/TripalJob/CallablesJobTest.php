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
        'job_name' => 'Basic testing job with a string-based static callable.',
        'modulename' => 'tripal',
        'callback' => '\Drupal\Tests\tripal\Kernel\Services\TripalJob\FakeClasses\callableClassForTripalJobs::myCallbackMethod',
        'arguments' => [],
      ],
    ];

    $sets[] = [
      'details' => [
        'job_name' => 'Basic testing job with a array-based static callable.',
        'modulename' => 'tripal',
        'callback' => [callableClassForTripalJobs::class, 'myCallbackMethod'],
        'arguments' => [],
      ],
    ];

    $sets[] = [
      'details' => [
        'job_name' => 'Basic testing job with function and includes.',
        'modulename' => 'tripal',
        'callback' => 'myFunctionCallback',
        'includes' => ['tests/src/Kernel/Services/TripalJob/FakeClasses/single_include.php'],
        'arguments' => [],
      ],
    ];

    return $sets;
  }

  /**
   * Tests the various valid callables in the job processes.
   *
   * @dataProvider provideValidJobs
   */
  public function testTripalJob_validjobs($details) {

    // Add user to the job details since we can't do it in the data provider.
    $details['uid'] = $this->user->id();

    // If files need to be included then add the current module path.
    if (array_key_exists('includes', $details)) {
      $tripal_path = \Drupal::service('extension.list.module')->getPath('tripal');
      foreach ($details['includes'] as $k => $path) {
        $details['includes'][$k] = $tripal_path . '/' . $path;
      }
    }

    // Now try to create the job.
    $job_id = \Drupal::service('tripal.job')->create($details);
    $this->assertIsNumeric($job_id,
      "Creating a TripalJob was not successful as we did not have a job_id returned.");

    // Try to load it.
    $job = new \Drupal\tripal\Services\TripalJob();
    $job->load($job_id);
    $this->assertIsObject($job, "Unableto retrieve the job we just created.");

    // Use the getters to get all the details to check that it was loaded properly.
    $this->assertEquals($job_id, $job->getJobID(),
      "Unable to retrieve the job ID of the job we just created.");
    $this->assertEquals($details['uid'], $job->getUID(),
      "Unable to retrieve the user of the job we just created.");
    $this->assertEquals($details['job_name'], $job->getJobName(),
      "Unable to retrieve the name of the job we just created.");
    $this->assertEquals($details['modulename'], $job->getModuleName(),
      "Unable to retrieve the module name of the job we just created.");
    $this->assertEquals($details['callback'], $job->getCallback(),
      "Unable to retrieve the callback of the job we just created.");
    $this->assertEquals($details['arguments'], $job->getArguments(),
      "Unable to retrieve the arguments of the job we just created.");
    $this->assertIsObject($job->getJob(),
      "Unable to return a job object from the job we just loaded.");

    // Now we try to run it!
    // If something goes wrong then an exception will be thrown.
    // We use output buffering so we can condirm the log message within the
    // callback is printed.
    ob_start();
    $job->run();
    $output = ob_get_clean();
    $this->assertStringContainsString('We were able to successfully run the job.', $output,
      "We did not recieve the expected output when the job was run.");
  }
}
