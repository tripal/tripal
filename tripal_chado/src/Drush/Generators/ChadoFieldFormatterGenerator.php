<?php declare(strict_types = 1);

namespace Drupal\tripal_chado\Drush\Generators;

use DrupalCodeGenerator\Asset\AssetCollection as Assets;
use DrupalCodeGenerator\Attribute\Generator;
use DrupalCodeGenerator\Command\BaseGenerator;
use DrupalCodeGenerator\GeneratorType;
use DrupalCodeGenerator\Validator\RegExp;
use DrupalCodeGenerator\Validator\Required;
use DrupalCodeGenerator\Utils;

#[Generator(
  name: 'tripal-chado:field-formatter',
  description: 'Generates a Chado Formatter to be used with an existing Chado Field.',
  templatePath: __DIR__ . '/../../../templates/generator',
  type: GeneratorType::MODULE_COMPONENT,
)]
final class ChadoFieldFormatterGenerator extends BaseGenerator {

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

    // Field Formatter
    $vars['formatter_id'] = $prompt->ask('Field Formatter ID', 'chado_example_formatter', $id_validator);
    $vars['formatter_label'] = $prompt->ask('Field Formatter Label', Utils::machine2human($vars['formatter_id'], TRUE), $label_validator);
    $vars['formatter_description'] = $prompt->ask('Field Formatter Description');
    $vars['field_id'] = $prompt->ask('Default Field Type id', Utils::removeSuffix($vars['formatter_id'], '_formatter'), $id_validator);
    $vars['formatter_class'] = $prompt->askClass(default: '{formatter_id|camelize}');

    $assets->addFile('src/Plugin/Field/FieldFormatter/{formatter_class}.php', 'chado-field-formatter.twig');
  }

}
