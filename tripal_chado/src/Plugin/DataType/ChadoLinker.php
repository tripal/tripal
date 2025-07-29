<?php

namespace Drupal\tripal_chado\Plugin\DataType;

use Drupal\Core\TypedData\TypedData;
use Drupal\Core\TypedData\Attribute\DataType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TypedData\ChadoLinkerDataDefinition;

/**
 * Plugin implementation of the ChadoLinker data type
 */
#[DataType(
  id: 'chado_linker',
  label: new TranslatableMarkup('Chado Linker'),
  definition_class: ChadoLinkerDataDefinition::class,
)]
class ChadoLinker extends TypedData  {


}
