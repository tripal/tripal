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
  protected $Mocklabel = '';

  /**
   * @var array
   */
  protected $Mocksettings = '';

  /**
   * Sets the mocked details to use for the test.
   *
   * @param array $details
   *   - label (string)
   *   - settings (array):
   *     - storage_plugin_id (string; e.g. chado_storage)
   *     - storage_plugin_settings (array):
   *       - base_table (string; e.g. feature)
   *       - property_settings (array)
   *         - value (array)
   *           - action (string; one of store, join, replace, function)
   *           - chado_table (string; must match above)
   *           - chado_column (string; name)
   */
  public function setMock($details) {
    $this->Mocklabel = $details['label'];
    $this->Mocksettings = $details['settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function getLabel() {
    return $this->Mocklabel;
  }

  /**
   * {@inheritdoc}
   */
  public function getSettings() {
    return $this->Mocksettings;
  }

  public function __construct(array $values, $entity_type = 'field_config') {
    // DO NOTHING.
    // Don't call the parent because we don't need an operational field.
  }
}
