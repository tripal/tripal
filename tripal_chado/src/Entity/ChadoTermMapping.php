<?php

namespace Drupal\tripal_chado\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\tripal_chado\Entity\ChadoTermMappingInterface;

/**
 * Defines the Chado Term Mapping Configuration entity.
 *
 * @ConfigEntityType(
 *   id = "chado_term_mapping",
 *   label = @Translation("Chado Term Mapping"),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal_chado\ListBuilders\ChadoTermMappingListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal_chado\Form\ChadoTermMappingForm",
 *       "edit" = "Drupal\tripal_chado\Form\ChadoTermMappingForm",
 *       "delete" = "Drupal\tripal_chado\Form\ChadoTermMappingDeleteForm",
 *     }
 *   },
 *   config_prefix = "chado_term_mapping",
 *   admin_permission = "administer tripal",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   config_export = {
 *     "id",
 *     "label",
 *     "description",
 *     "tables"
 *   },
 *   links = {
 *     "edit-form" = "/admin/tripal/storage/chado/terms/{chado_term_mapping}",
 *     "delete-form" = "/admin/tripal/storage/chado/terms/{chado_term_mapping}/delete",
 *   }
 * )
 */
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
}
