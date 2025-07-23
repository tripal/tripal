<?php

namespace Drupal\tripal_chado\Plugin\DataType;

use Drupal\Core\TypedData\TypedData;
use Drupal\Core\TypedData\Attribute\DataType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TypedData\ChadoLinkerDataDefinition;

#[DataType(
  id: 'chado_linker',
  label: new TranslatableMarkup('Chado Linker'),
  definition_class: ChadoLinkerDataDefinition::class,
)]
/**
 * Plugin implementation of the ChadoLinker data type
 *
 * @DataType(
 *   id = "chado_linker",
 *   label = @Translation("Chado Linker"),
 *   definition_class = "\Drupal\tripal_chado\TypedData\ChadoLinkerDataDefinition"
 * )
 */
class ChadoLinker extends TypedData  {


}
