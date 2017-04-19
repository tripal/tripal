<?php

class TripalWebServiceProvider {
  private $services;
  private $possibleStatus;


  public function __construct() {
    $this->services = array();
  }

  public function addService($service) {
    $this->services[] = $service;
  }

  public function getPossibleStatus() {
    return $this->possibleStatus;
  }
}