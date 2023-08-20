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
  name: 'tripal:field-widget',
  description: 'Generates a Tripal Field Widget for developing Tripal fields with no interactiion with Chado.',
  templatePath: __DIR__ . '/../../../templates/generator',
  type: GeneratorType::MODULE_COMPONENT,
)]
final class TripalFieldWidgetGenerator extends BaseGenerator {

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
    $vars['widget_id'] = $prompt->ask('Field Widget ID', '{machine_name}_example_widget', $id_validator);
    $vars['field_label'] = $prompt->ask('Field Widget Label', Utils::machine2human($vars['widget_id'], TRUE), $label_validator);
    $vars['description'] = $prompt->ask('Field Widget Description');
    $vars['field_id'] = $prompt->ask('Default Field Type id', Utils::removeSuffix($vars['widget_id'], '_widget'), $id_validator);
    $vars['class'] = $prompt->askClass(default: '{widget_id|camelize}');

    $assets->addFile('src/Plugin/Field/FieldWidget/{class}.php', 'tripal-field-widget.twig');
  }

}
