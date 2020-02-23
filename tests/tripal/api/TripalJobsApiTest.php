<?php

namespace Tests\tripal\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalJobsApiTest extends TripalTestCase {

  use DBTransaction;

  /**
   * Tests the ability to create a tripal job.
   *
   * @test
   */
  public function testCreatingAJobWorks() {
    $job_id = tripal_add_job('Test adding jobs', 'tripal',
      'tripal_tripal_cron_notification', [], 1);
    $this->assertTrue(is_numeric($job_id));
  }

  /** @test */
  public function testRetrievingAJob() {
    $job = factory('tripal_jobs')->create();

    $job2 = tripal_get_job($job->job_id);

    $this->assertNotEmpty($job2);
    $this->assertObjectHasAttribute('job_id', $job2);
    $this->assertEquals($job2->job_id, $job->job_id);
  }

  /** @test */
  public function testRetrievingActiveJobs() {
    factory('tripal_jobs')->create();
    $jobs = tripal_get_active_jobs();

    $this->assertNotEmpty($jobs);
  }

  /** @test */
  public function testRetrievingActiveJobsWithAGivenModule() {
    factory('tripal_jobs')->create([
      'modulename' => 'tripal_test_suite',
      'status' => 'Running',
    ]);
    $jobs = tripal_get_active_jobs('tripal_test_suite');

    $this->assertNotEmpty($jobs);
  }

  /** @test */
  public function testRetrievingCompletedJobsDoesNotHappen() {
    factory('tripal_jobs')->create([
      'modulename' => 'tripal_test_suite',
      'status' => 'Completed',
    ]);
    $jobs = tripal_get_active_jobs('tripal_test_suite');

    $this->assertEmpty($jobs);
  }
}
