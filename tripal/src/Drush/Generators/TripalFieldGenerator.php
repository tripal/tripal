<?php declare(strict_types = 1);

namespace Drupal\tripal\Drush\Generators;

use DrupalCodeGenerator\Asset\AssetCollection as Assets;
use DrupalCodeGenerator\Attribute\Generator;
use DrupalCodeGenerator\Command\BaseGenerator;
use DrupalCodeGenerator\GeneratorType;
use DrupalCodeGenerator\Validator\RegExp;
use DrupalCodeGenerator\Validator\Required;
use DrupalCodeGenerator\Utils;

#[Generator(
  name: 'tripal:field',
  description: 'Generates a Tripal Field Type, Widget and Formatter for fields not interacting with Chado.',
  templatePath: __DIR__ . '/../../../templates/generator',
  type: GeneratorType::MODULE_COMPONENT,
)]
final class TripalFieldGenerator extends BaseGenerator {

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

    // Validators
    $id_validator = new RegExp('/^[a-z][a-z0-9_]*[a-z0-9]$/', 'The value must consist of only lower case alphanumeric characters and underscores. It should start with a letter and not end with an underscore.');
    $label_validator = new RegExp('/^[a-zA-Z][a-zA-Z0-9- ]*[a-zA-Z0-9]$/', 'The value must be alphanumeric. We suggest focusing on a title-case human-readable name for your field.');
    $term_validator = new RegExp('/:/', 'The value must be an ID Space and Accession defining the term with a : separating them (e.g. rdfs:type).');

    // Field Type
    $vars['field_id'] = $prompt->ask('Field Type | ID', '{machine_name}_example', $id_validator);
    $vars['field_label'] = $prompt->ask('Field Type | Label', Utils::machine2human($vars['field_id'], TRUE) . ' Field Type', $label_validator);
    $vars['field_description'] = $prompt->ask('Field Type | Description');
    $vars['field_term'] = $prompt->ask('Field Type | Term (IdSpace:Accession)', 'rdfs:type', $term_validator);
    $vars['field_class'] = $prompt->askClass('Field Type | Class', '{field_id|camelize}TypeItem');

    // Field Widget
    $vars['widget_id'] = $prompt->ask('Field Widget | ID', '{field_id}_widget', $id_validator);
    $vars['widget_label'] = $prompt->ask('Field Widget | Label', Utils::machine2human($vars['widget_id'], TRUE), $label_validator);
    $vars['widget_description'] = $prompt->ask('Field Widget | Description');
    $vars['widget_class'] = $prompt->askClass('Field Widget | Class', '{widget_id|camelize}');

    // Field Formatter
    $vars['formatter_id'] = $prompt->ask('Field Formatter | ID', '{field_id}_formatter', $id_validator);
    $vars['formatter_label'] = $prompt->ask('Field Formatter | Label', Utils::machine2human($vars['formatter_id'], TRUE), $label_validator);
    $vars['formatter_description'] = $prompt->ask('Field Formatter | Description');
    $vars['formatter_class'] = $prompt->askClass('Field Formatter | Class', '{formatter_id|camelize}');

    $assets->addFile('src/Plugin/Field/FieldType/{field_class}.php', 'tripal-field-type.twig');
    $assets->addFile('src/Plugin/Field/FieldWidget/{widget_class}.php', 'tripal-field-widget.twig');

    $assets->addFile('src/Plugin/Field/FieldFormatter/{formatter_class}.php', 'tripal-field-formatter.twig');
  }

}
