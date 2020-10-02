<?php

namespace Drupal\tripal_console\Command;

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

/**
 * Class GenerateFieldTypeCommand.
 *
 * Drupal\Console\Annotations\DrupalCommand (
 *     extension="tripal_console",
 *     extensionType="module"
 * )
 */
class GenerateFieldTypeCommand extends ContainerAwareCommand {

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
    parent::__construct();
  }

  /**
   * {@inheritdoc}
   */
  protected function configure() {
    $this
      ->setName('tripal:generate:fieldType')
      ->setDescription('Generate Tripal 4 Field Type file following coding standards.')
      ->addOption(
          'module',
          null,
          InputOption::VALUE_REQUIRED,
          $this->trans('commands.common.options.module')
      )
      ->addOption(
          'type-class',
          null,
          InputOption::VALUE_REQUIRED,
          $this->trans('commands.generate.plugin.field.options.type-class')
      )
      ->addOption(
          'type-label',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.type-label')
      )
      ->addOption(
          'type-plugin-id',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.type-plugin-id')
      )
      ->addOption(
          'type-description',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.type-description')
      )
      ->addOption(
          'formatter-class',
          null,
          InputOption::VALUE_REQUIRED,
          $this->trans('commands.generate.plugin.field.options.formatter-class')
      )
      ->addOption(
          'formatter-label',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.formatter-label')
      )
      ->addOption(
          'formatter-plugin-id',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.formatter-plugin-id')
      )
      ->addOption(
          'widget-class',
          null,
          InputOption::VALUE_REQUIRED,
          $this->trans('commands.generate.plugin.field.options.formatter-class')
      )
      ->addOption(
          'widget-label',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.widget-label')
      )
      ->addOption(
          'widget-plugin-id',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.widget-plugin-id')
      )
      ->addOption(
          'field-type',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.field-type')
      )
      ->addOption(
          'default-widget',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.default-widget')
      )
      ->addOption(
          'default-formatter',
          null,
          InputOption::VALUE_OPTIONAL,
          $this->trans('commands.generate.plugin.field.options.default-formatter')
      )
      ->setAliases(['gpf']);
  }

  /**
   * {@inheritdoc}
   */
  protected function interact(InputInterface $input, OutputInterface $output) {

    // --module option
    // $this->getModuleOption();

    // --type-class option
    $typeClass = $input->getOption('type-class');
    if (!$typeClass) {
        $typeClass = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.type-class'),
            'ExampleFieldType'
        );
        $input->setOption('type-class', $typeClass);
    }

    // --type-label option
    $label = $input->getOption('type-label');
    if (!$label) {
        $label = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.type-label'),
            'Example Field Type'
        );
        $input->setOption('type-label', $label);
    }

    // --type-plugin-id option
    $plugin_id = $input->getOption('type-plugin-id');
    if (!$plugin_id) {
        $plugin_id = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.type-plugin-id'),
            'example_field_type'
        );
        $input->setOption('type-plugin-id', $plugin_id);
    }

    // --type-description option
    $description = $input->getOption('type-description');
    if (!$description) {
        $description = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.type-description'),
            $this->trans('commands.generate.plugin.field.suggestions.my-field-type')
        );
        $input->setOption('type-description', $description);
    }

    // --widget-class option
    $widgetClass = $input->getOption('widget-class');
    if (!$widgetClass) {
        $widgetClass = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.widget-class'),
            'ExampleWidgetType',
        );
        $input->setOption('widget-class', $widgetClass);
    }

    // --widget-label option
    $widgetLabel = $input->getOption('widget-label');
    if (!$widgetLabel) {
        $widgetLabel = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.widget-label'),
            'ExampleWidgetType'
        );
        $input->setOption('widget-label', $widgetLabel);
    }

    // --widget-plugin-id option
    $widget_plugin_id = $input->getOption('widget-plugin-id');
    if (!$widget_plugin_id) {
        $widget_plugin_id = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.widget-plugin-id'),
            'ExampleWidgetType'
        );
        $input->setOption('widget-plugin-id', $widget_plugin_id);
    }

    // --formatter-class option
    $formatterClass = $input->getOption('formatter-class');
    if (!$formatterClass) {
        $formatterClass = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.formatter-class'),
            'ExampleFormatterType',
        );
        $input->setOption('formatter-class', $formatterClass);
    }

    // --formatter-label option
    $formatterLabel = $input->getOption('formatter-label');
    if (!$formatterLabel) {
        $formatterLabel = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.formatter-label'),
            'ExampleFormatterType'
        );
        $input->setOption('formatter-label', $formatterLabel);
    }

    // --formatter-plugin-id option
    $formatter_plugin_id = $input->getOption('formatter-plugin-id');
    if (!$formatter_plugin_id) {
        $formatter_plugin_id = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.formatter-plugin-id'),
            'ExampleFormatterType'
        );
        $input->setOption('formatter-plugin-id', $formatter_plugin_id);
    }

    // --field-type option
    $field_type = $input->getOption('field-type');
    if (!$field_type) {
        $field_type = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.field-type'),
            $plugin_id
        );
        $input->setOption('field-type', $field_type);
    }

    // --default-widget option
    $default_widget = $input->getOption('default-widget');
    if (!$default_widget) {
        $default_widget = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.default-widget'),
            $widget_plugin_id
        );
        $input->setOption('default-widget', $default_widget);
    }

    // --default-formatter option
    $default_formatter = $input->getOption('default-formatter');
    if (!$default_formatter) {
        $default_formatter = $this->getIo()->ask(
            $this->trans('commands.generate.plugin.field.questions.default-formatter'),
            $formatter_plugin_id
        );
        $input->setOption('default-formatter', $default_formatter);
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function execute(InputInterface $input, OutputInterface $output) {
    //$this->getIo()->info('execute');
    $this->getIo()->warning('Not Yet Implemented.');
    $this->generator->generate([]);
  }

}
