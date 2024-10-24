<?php

namespace Drupal\Tests\tripal_layout\Kernel\Controller;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal_layout\Traits\TripalLayoutTestTrait;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultView;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultForm;

/**
 * Tests applying Details field groups to both form and view displays.
 *
 * @group TripalLayoutDisplay
 * @group TripalLayoutDisplayController
 */
class TripalLayoutControllerDetailsTest extends TripalTestKernelBase {

  use TripalLayoutTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'tripal', 'tripal_chado', 'tripal_layout'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_layout_default_form');
    $this->installEntitySchema('tripal_layout_default_view');
  }

  public function testApplyDetailsFieldGroup() {
    $this->markTestIncomplete('Just starting this');
  }
}
