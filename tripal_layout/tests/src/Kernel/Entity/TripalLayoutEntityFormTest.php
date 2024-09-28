<?php

namespace Drupal\Tests\tripal_layout\Kernel\Entity;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal_layout\Traits\TripalLayoutTestTrait;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultView;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultForm;

/**
 * Tests the TripalLayoutDefaultView and TripalLayoutDefaultForm entities.
 *
 * @group TripalLayoutDisplay
 */
class TripalLayoutEntityFormTest extends TripalTestKernelBase {

  use TripalLayoutTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'tripal', 'tripal_layout'];

  /**
   * A simple form layout config entity for testing.
   *
   * @var TripalLayoutDefaultForm
   */
  protected TripalLayoutDefaultForm $form_entity;

  /**
   * A simple view layout config entity for testing.
   *
   * @var TripalLayoutDefaultView
   */
  protected TripalLayoutDefaultView $view_entity;

  /**
   * Provides the yaml file information for the test entities to be
   * created in the setup.
   *
   * @var array
   *   They key is the id of a tripal layout config entity and the value is
   *   the full path to a YAML file defining a specific instance.
   */
  protected array $test_entity_yaml_files = [
    'tripal_layout_default_view' => __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_view.test_view.yml',
    'tripal_layout_default_form' => __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_form.test_form.yml',
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_layout_default_form');
    $this->installEntitySchema('tripal_layout_default_view');

    // Create layout entities to be used in testing.
    $this->view_entity = $this->createLayoutEntityFromConfig(
      'tripal_layout_default_view',
      $this->test_entity_yaml_files['tripal_layout_default_view']
    );
    $this->form_entity = $this->createLayoutEntityFromConfig(
      'tripal_layout_default_form',
      $this->test_entity_yaml_files['tripal_layout_default_form']
    );
  }

  /**
   * Tests the delete form for the TripalLayoutDefaultView and
   * TripalLayoutDefaultForm entities.
   *
   * @return void
   */
  public function testDeleteForm() {

  }
}
