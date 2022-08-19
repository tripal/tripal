<?php

namespace Drupal\Tests\tripal_chado\Functional\MockClass;

use Drupal\field\Entity\FieldConfig;
use \Drupal\Core\Field\FieldConfigInterface;

/**
 * A Mock FieldConfig class to be used in the ChadoStorage testing.
 */
class FieldConfigMock extends FieldConfig implements FieldConfigInterface {

  /**
   * @var string
   */
  protected $mock_chado_table = '';

  /**
   * @var string
   */
  protected $mock_chado_column = '';

  public function setMockChadoMapping($chado_table, $chado_column) {
    $this->mock_chado_table = $chado_table;
    $this->mock_chado_column = $chado_column;
  }

  /**
   * {@inheritdoc}
   */
  public function getSettings() {
    return [
      'storage_plugin_settings' => [
        'chado_table' => $this->mock_chado_table,
        'chado_column' => $this->mock_chado_column,
      ],
    ];
  }

  public function __construct(array $values, $entity_type = 'field_config') {
    // DO NOTHING.
    // Don't call the parent because we don't need an operational field.
  }
}
