<?php


use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalJobsApiTest extends TripalTestCase {

  use DBTransaction;

  /**
   * Tests the ability to create a tripal job.
   *
   * @test
   */
  public function should_create_a_tripal_job() {
    $job_id = tripal_add_job('Test adding jobs', 'tripal', 'tripal_tripal_cron_notification', [], 1);
    $this->assertTrue(is_numeric($job_id));
  }
}
