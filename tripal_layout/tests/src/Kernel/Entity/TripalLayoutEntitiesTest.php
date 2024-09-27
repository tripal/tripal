<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\Core\Entity\FieldableEntityInterface;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;
use \Drupal\Tests\user\Traits\UserCreationTrait;

/**
 * Tests the TripalLayoutDefaultView and TripalLayoutDefaultForm entities.
 *
 * @group TripalLayoutDisplay
 */
class TripalLayoutEntities extends TripalTestKernelBase {

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
    $this->installEntitySchema('tripal_entity');
    $this->installEntitySchema('tripal_entity_type');

  }

  /**
   * Provides senarios to test the the TripalLayoutDefaultView and
   * TripalLayoutDefaultForm entities.
   *
   * @return array
   *   An array of senarios to test.
   */
  public function provideLayoutDisplayEntitySenarios() {
    $senarios = [];

    $entity_defns = [
      'view' => [
        'class' => 'TripalLayoutDefaultView',
        'id' => 'tripal_layout_default_view'
      ],
      'form' => [
        'class' => 'TripalLayoutDefaultForm',
        'id' => 'tripal_layout_default_form'
      ],
    ];

    $bundle_defns = [
      'organism' => [
        'id' => 'organism',
      ],
    ];

    $senarios['organism_view'] = [
      'display_context' => 'view',
      'entity_defn' => $entity_defns['view'],
      'bundle_defn' => $bundle_defns['organism'],
    ];

    return $senarios;
  }

  /**
   * Tests loading a TripalLayoutEntity.
   *
   * @dataProvider provideLayoutDisplayEntitySenarios
   *
   * @param string $display_context
   *   The type of display entity we are testing. One of 'view' or 'form'.
   * @param array $entity_defn
   *   Details about the TripalLayoutEntity we are testing.
   *   Expected keys include: class and id.
   * @param array $bundle_defn
   *   Details about the TripalEntityType whose display we want to test.
   *   Expected keys include: id
   * @return void
   */
  public function testTripalLayoutEntityLoad(string $display_context, array $entity_defn, array $bundle_defn) {
    $this->markTestIncomplete('Just starting out');
  }
}
