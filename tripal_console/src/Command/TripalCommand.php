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
use \Drupal\Console\Core\Utils\TranslatorManager;

/**
 * Class GenerateFieldTypeCommand.
 *
 * Drupal\Console\Annotations\DrupalCommand (
 *     extension="tripal_console",
 *     extensionType="module"
 * )
 */
class TripalCommand extends ContainerAwareCommand {

  /**
   * Contains the YAML parsed output from the command-specific YAML file.
   * @see src/UserText/[command name].yml
   */
  protected $user_text;

  /**
   * Contains the YAML parsed output from the common YAML file.
   * @see src/UserText/commonText.yml
   */
  protected $common_text;

  /**
   * Constructs a new GenerateFieldTypeCommand object.
   */
  public function __construct() {
    $module_path = \Drupal::service('extension.list.module')->getPath('tripal_console');

    $this->common_text = Yaml::parse(file_get_contents("$module_path/src/UserText/commonText.yml"));

    parent::__construct();
  }

  /**
   * Used in interact method to prompt the user for a given option
   * and save the value they provide.
   *
   * @param object $input
   *   The input object passes to the interact method.
   * @param string $option
   *   The name of the option you want to prompt for as set in the configure
   *   method. There should be a value in $user_text[option-help][optionname]
   *   that provides the text for the prompt. @see src/UserText/commandname.yml
   */
  protected function promptOption($input, $option) {
    $optionValue = $input->getOption($option);
    if (!$optionValue) {
        $question = $this->user_text['option-help'][$option];
        if (!$question) {
          $question = $this->common_text['option-help'][$option]; }
        $optionValue = $this->getIo()->ask($question);
        $input->setOption($option, $optionValue);
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function configure() {}

  /**
   * {@inheritdoc}
   */
  protected function interact(InputInterface $input, OutputInterface $output) {}

  /**
   * {@inheritdoc}
   */
  protected function execute(InputInterface $input, OutputInterface $output) {}

}
