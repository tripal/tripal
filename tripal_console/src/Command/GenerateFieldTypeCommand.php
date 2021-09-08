<?php

namespace Drupal\tripal_console\Command;

use Symfony\Component\Yaml\Yaml;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;
use Drupal\Console\Core\Command\ContainerAwareCommand;
use Drupal\Console\Core\Generator\GeneratorInterface;
use Drupal\Console\Utils\Validator;
use Drupal\Console\Command\Shared\ModuleTrait;
use Drupal\Console\Command\Shared\ConfirmationTrait;
use Drupal\Console\Core\Command\Command;
use Drupal\Console\Extension\Manager;
use Drupal\Console\Core\Utils\StringConverter;
use Drupal\Console\Core\Utils\ChainQueue;
use Drupal\Core\Field\FieldTypePluginManager;
use Drupal\Console\Core\Utils\TranslatorManager;
use Drupal\tripal_console\Command\TripalCommand;

/**
 * Class GenerateFieldTypeCommand.
 *
 * Drupal\Console\Annotations\DrupalCommand (
 *     extension="tripal_console",
 *     extensionType="module"
 * )
 */
class GenerateFieldTypeCommand extends TripalCommand {

  /**
   * Drupal\Console\Core\Generator\GeneratorInterface definition.
   *
   * @var \Drupal\Console\Core\Generator\GeneratorInterface
   */
  protected $generator;

  /**
   * Constructs a new GenerateFieldTypeCommand object.
   */
  public function __construct(GeneratorInterface $generator) {
    $this->generator = $generator;

    $module_path = \Drupal::service('extension.list.module')->getPath('tripal_console');
    $this->user_text = Yaml::parse(file_get_contents("$module_path/src/UserText/tripal.generate.fieldType.yml"));

    parent::__construct();
  }

  /**
   * {@inheritdoc}
   */
  protected function configure() {
    $this
      ->setName('tripal:generate:fieldType')
      ->setDescription($this->user_text['description'])
      ->addOption(
          'module',                                    // full-length option.
          null,                                        // option alias.
          InputOption::VALUE_REQUIRED,                 // Constraints.
          $this->common_text['option-help']['module']  // Help Text.
      )
      ->addOption(
          'default-widget',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['default-widget']
      )
      ->addOption(
          'default-formatter',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['default-formatter']
      )
      ->addOption(
          'vocab-short',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['vocab-short']
      )
      ->addOption(
          'vocab-name',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['vocab-name']
      )
      ->addOption(
          'vocab-description',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['vocab-description']
      )
      ->addOption(
          'term-name',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['term-name']
      )
      ->addOption(
          'term-accession',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['term-accession']
      )
      ->addOption(
          'term-definition',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['term-definition']
      )
      ->addOption(
          'chado-table',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['chado-table']
      )
      ->addOption(
          'chado-column',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['chado-column']
      )
      ->addOption(
          'chado-base',
          null,
          InputOption::VALUE_REQUIRED,
          $this->user_text['option-help']['chado-base']
      )
      ->addOption(
          'type-class',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->user_text['option-help']['type-class']
      )
      ->addOption(
          'type-label',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->user_text['option-help']['type-label']
      )
      ->addOption(
          'type-plugin-id',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->user_text['option-help']['type-plugin-id']
      )
      ->addOption(
          'type-description',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->user_text['option-help']['type-description']
      )
      ->setAliases(['trpgen-fieldType']);
  }

  /**
   * {@inheritdoc}
   */
  protected function interact(InputInterface $input, OutputInterface $output) {

    // Prompt the user for these values.
    $this->promptOption($input, 'module');
    $this->promptOption($input, 'vocab-short');
    $this->promptOption($input, 'vocab-name');
    $this->promptOption($input, 'vocab-description');
    $this->promptOption($input, 'term-name');
    $this->promptOption($input, 'term-accession');
    $this->promptOption($input, 'term-definition');
    $this->promptOption($input, 'chado-table');
    $this->promptOption($input, 'chado-column');
    $this->promptOption($input, 'chado-base');
    $this->promptOption($input, 'default-widget');
    $this->promptOption($input, 'default-formatter');

    // The following options we don't want to prompt for
    // but instead are composites of the above required options.
    $option = 'type-class';
    $optionValue = $input->getOption($option);
    if (!$optionValue) {
      $optionValue = $input->getOption('vocab-short')
        . ucfirst(str_replace(' ', '', $input->getOption('term-name')))
        . 'Item';
      $input->setOption($option, $optionValue);
    }

    $option = 'type-label';
    $optionValue = $input->getOption($option);
    if (!$optionValue) {
      $optionValue = $input->getOption('term-name');
      $input->setOption($option, $optionValue);
    }

    $option = 'type-plugin-id';
    $optionValue = $input->getOption($option);
    if (!$optionValue) {
      $optionValue = $input->getOption('vocab-short')
        . '__'
        . ucfirst(str_replace(' ', '', $input->getOption('term-name')));
      $optionValue = strtolower($optionValue);
      $input->setOption($option, $optionValue);
    }

    $option = 'type-description';
    $optionValue = $input->getOption($option);
    if (!$optionValue) {
      $optionValue = $input->getOption('term-definition');
      $input->setOption($option, $optionValue);
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function execute(InputInterface $input, OutputInterface $output) {

    $summary = 'Generating files in %module for FIELD TYPE: %label (%class)...';
    $summary = strtr($summary, [
      '%module' => $input->getOption('module'),
      '%label' => $input->getOption('type-label'),
      '%class' => $input->getOption('type-class'),
    ]);
    $this->getIo()->info($summary);

    // Now do what we said we would ;-)
    // -- Compile our paramters:
    $parameters = [];
    foreach (['module', 'vocab-short', 'vocab-name', 'vocab-description',
    'term-name', 'term-accession', 'term-definition', 'chado-table',
    'chado-column', 'chado-base', 'type-class', 'type-label',
    'type-plugin-id', 'type-description','default-widget', 'default-formatter'] as $option) {
      $twig_var = str_replace('-','_', $option);
      $parameters[$twig_var] = $input->getOption($option);
    }
    $this->generator->generate($parameters);
  }

}
