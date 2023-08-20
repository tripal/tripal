<?php declare(strict_types = 1);

namespace Drupal\tripal\Drush\Generators;

use DrupalCodeGenerator\Asset\AssetCollection as Assets;
use DrupalCodeGenerator\Attribute\Generator;
use DrupalCodeGenerator\Command\BaseGenerator;
use DrupalCodeGenerator\GeneratorType;
use DrupalCodeGenerator\Utils;
use DrupalCodeGenerator\Validator\RegExp;
use DrupalCodeGenerator\Validator\Required;

#[Generator(
  name: 'tripal:field-type',
  description: 'Generates a Tripal Field Type for developing Tripal fields with no interactiion with Chado.',
  templatePath: __DIR__ . '/../../../templates/generator',
  type: GeneratorType::MODULE_COMPONENT,
)]
final class TripalFieldTypeGenerator extends BaseGenerator {

  /**
   * {@inheritdoc}
   */
  protected function generate(array &$vars, Assets $assets): void {
    $prompt = $this->createInterviewer($vars);

    // Module Machine Name
    $vars['machine_name'] = $prompt->askMachineName();

    // Validators
    $id_validator = new RegExp('/^[a-z][a-z0-9_]*[a-z0-9]$/', 'The value must consist of only lower case alphanumeric characters and underscores. It should start with a letter and not end with an underscore.');
    $label_validator = new RegExp('/^[a-zA-Z][a-zA-Z0-9- ]*[a-zA-Z0-9]$/', 'The value must be alphanumeric. We suggest focusing on a title-case human-readable name for your field.');
    $term_validator = new RegExp('/:/', 'The value must be an ID Space and Accession defining the term with a : separating them (e.g. rdfs:type).');

    // Field Type
    $vars['field_id'] = $prompt->ask('Field Type ID', '{machine_name}_example', $id_validator);

    $vars['field_label'] = $prompt->ask('Field Type Label', Utils::machine2human($vars['field_id'], TRUE) . ' Field Type', $label_validator);
    $vars['field_description'] = $prompt->ask('Field Type Description');
    $vars['field_term'] = $prompt->ask('Field Term (IdSpace:Accession)', 'rdfs:type', $term_validator);
    $vars['widget_id'] = $prompt->ask('Default Field Widget ID', '{field_id}_widget', $id_validator);
    $vars['formatter_id'] = $prompt->ask('Default Field Formatter ID', '{field_id}_formatter', $id_validator);
    $vars['field_class'] = $prompt->askClass(default: '{field_id|camelize}TypeItem');

    $assets->addFile('src/Plugin/Field/FieldType/{field_class}.php', 'tripal-field-type.twig');
  }

}
