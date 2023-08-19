<?php declare(strict_types = 1);

namespace Drupal\tripal\Drush\Generators;

use DrupalCodeGenerator\Asset\AssetCollection as Assets;
use DrupalCodeGenerator\Attribute\Generator;
use DrupalCodeGenerator\Command\BaseGenerator;
use DrupalCodeGenerator\GeneratorType;
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

    // Field Type
    $vars['field_id'] = $prompt->ask('FieldType id', '{machine_name}_example', $id_validator);
    $vars['field_label'] = $prompt->ask('FieldType label', '{machine_name|camelize} Example Field Type', $label_validator);
    $vars['description'] = $prompt->ask('FieldType description');
    $vars['widget_id'] = $prompt->ask('Default Field Widget id', '{machine_name}_example_widget', $id_validator);
    $vars['formatter_id'] = $prompt->ask('Default Field Formatter id', '{machine_name}_example_formatter', $id_validator);
    $vars['class'] = $prompt->askClass(default: '{field_id|camelize}TypeItem');

    $assets->addFile('src/Plugin/Field/FieldType/{class}.php', 'tripal-field-type.twig');
  }

}
