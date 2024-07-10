<?php

namespace Drupal\tripal_chado\Plugin\DataType;

use Drupal\Core\TypedData\TypedData;
use Drupal\tripal_chado\TypedData\ChadoLinkerDataDefinition;

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