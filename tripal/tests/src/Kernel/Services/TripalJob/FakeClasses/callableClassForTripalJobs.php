<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalJob\FakeClasses;

class callableClassForTripalJobs {

  public static function myCallbackMethod($job) {
    return $job;
  }
}
