<?php

namespace Drupal\tripal_chado\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Config\FileStorage;
use Drupal\Core\Entity\Attribute\ConfigEntityType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\Entity\ChadoTermMappingInterface;
use Drupal\tripal_chado\Form\ChadoTermMappingForm;
use Drupal\tripal_chado\Form\ChadoTermMappingDeleteForm;
use Drupal\tripal_chado\ListBuilders\ChadoTermMappingListBuilder;

/**
 * Defines the Chado Term Mapping Configuration entity.
 */
#[ConfigEntityType(
  id: 'chado_term_mapping',
  label: new TranslatableMarkup('Chado Term Mapping'),
  handlers: [
    'list_builder' => ChadoTermMappingListBuilder::class,
    'form' => [
      'add' => ChadoTermMappingForm::class,
      'edit' => ChadoTermMappingForm::class,
      'delete' => ChadoTermMappingDeleteForm::class,
    ],
  ],
  config_prefix: 'chado_term_mapping',
  admin_permission: 'administer tripal',
  bundle_of: 'tripal_entity',
  entity_keys: [
    'id' => 'id',
    'label' => 'label',
  ],
  links: [
    'edit-form' => '/admin/tripal/storage/chado/terms/{chado_term_mapping}',
    'delete-form' => '/admin/tripal/storage/chado/terms/{chado_term_mapping}/delete',
  ],
  config_export: [
    'id',
    'label',
    'description',
    'tables',
  ],
)]
class ChadoTermMapping extends ConfigEntityBase implements ChadoTermMappingInterface {

  /**
   * The ChadoTermMapping ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The ChadoTermMapping label.
   *
   * @var string
   */
  protected $label;

  /**
   * The ChadoTermMapping description.
   *
   * @var string
   */
  protected $description;

  /**
   * The list of tables containing the term mapping
   *
   * @var array
   */
  protected $tables;


  /**
   * Retrieves the current description for the term mapping setup.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }

  /**
   * Retrieves a list of table names, sorted alphabetically.
   *
   * @return array
   */
  public function getTableNames() : array {
    $tables = [];
    foreach ($this->tables as $table_def) {
      $tables[] = $table_def['name'];
    }
    sort($tables);
    return $tables;
  }

  /**
   * Retrieves the Term Id for a given Chado table and column
   *
   * @param string $table
   *   The Chado table name
   * @param string $column
   *   The Chado column name
   *
   * @return string
   *   The term ID for the column.
   */
  public function getColumnTermId($table, $column) : string {
    foreach ($this->tables as $table_def) {
      if ($table_def['name'] == $table) {
        foreach ($table_def['columns'] as $column_def) {
          if ($column_def['name'] == $column) {
            return $column_def['term_id'];
          }
        }
      }
    }
    return '';
  }

  /**
   * Refresh a mapping from its yaml file.
   * Intended for update hooks to call when a yaml file is updated.
   *
   * @param string $config_path
   *   The yaml file path that follows the format:
   *     path / module . storage_id . mapping_id
   *   Examples:
   *     config/install/tripal.tripal_content_terms.chado_content_terms
   *     config/install/tripal_chado.chado_term_mapping.core_mapping
   */
  public static function refreshMapping($config_path) {
    $parts = preg_split('/[\/\.]/', $config_path);
    $mapping_id = array_pop($parts);
    $storage_id = array_pop($parts);
    $module = 'tripal_chado';

    $storage = \Drupal::entityTypeManager()->getStorage($storage_id);
    $path = \Drupal::service('extension.list.module')->getPath($module);
    $fileStorage = new FileStorage($path);
    $config = $fileStorage->read($config_path);
    if (!is_array($config)) {
      throw new \Exception("refreshMapping configuration path not found: $config_path");
    }
    $mapping = $storage->load($mapping_id);
    if ($mapping) {
      $storage->delete([$mapping]);
    }
    $mapping = $storage->create($config);
    $mapping->save();
  }
}
