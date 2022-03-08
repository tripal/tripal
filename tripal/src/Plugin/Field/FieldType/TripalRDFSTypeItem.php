<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;

/**
 * Plugin implementation of Tripal RDFS content type.
 *
 * @FieldType(
 *   id = "tripal_rdfs_type",
 *   label = @Translation("Content Type"),
 *   description = @Translation("The resource content type."),
 * )
 */
class TripalRDFSTypeItem extends TripalFieldItemBase {

  public static $id = "tripal_rdfs_type";

  /**
   * {@inheritdoc}
   */
  public function tripalTypes() {
    //TODO: need VarCharStoragePropertyType
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate() {
    return [StoragePropertyValue($this->getEntity()->getEntityTypeId(),$this->id,"type")];
  }

  /**
   * {@inheritdoc}
   */
  public function tripalLoad($properties,$entity) {
    foreach ($properties as $property) {
      if ($property->getFieldKey() == "type") {
        $entity->tripalRDFSType = $property->value();
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalSave($properties,$entity) {
    foreach ($properties as $property) {
      if ($property->getFieldKey() == "type") {
        $property->setValue($entity->tripalRDFSType);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalClear($entity) {
    $entity->tripalRDFSType = "";
  }
}
