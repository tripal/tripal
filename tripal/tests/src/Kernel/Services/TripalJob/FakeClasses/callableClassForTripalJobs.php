<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalJob\FakeClasses;

class callableClassForTripalJobs {

  public static function myCallbackMethod($job) {

    $job->log('We were able to successfully run the job.');
  }
}
