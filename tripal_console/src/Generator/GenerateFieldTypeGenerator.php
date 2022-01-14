<?php

namespace Drupal\tripal_console\Generator;

use Drupal\Console\Core\Generator\Generator;
use Drupal\Console\Core\Generator\GeneratorInterface;
use Drupal\Console\Extension\Manager;

/**
 * Class GenerateFieldTypeGenerator.
 *
 * @package Drupal\Console\Generator
 */
class GenerateFieldTypeGenerator extends Generator implements GeneratorInterface {

  /**
   * @var Manager
   */
  protected $extensionManager;

  /**
   * GenerateFieldTypeGenerator constructor.
   *
   * @param Manager $extensionManager
   */
  public function __construct(Manager $extensionManager) {
      $this->extensionManager = $extensionManager;
  }

  /**
   * {@inheritdoc}
   */
  public function generate(array $parameters) {

    // Twig template path.
    $tripal_console_path = \Drupal::service('extension.list.module')->getPath('tripal_console');
    $twig_file = 'fieldtype.php.twig';
    // Add the template folder to the places to look for TWIG templates.
    $this->renderer->addSkeletonDir(DRUPAL_ROOT . '/' . $tripal_console_path . '/templates');

    // Determine the filename of the file we want to create.
    $module = $parameters['module'];
    $module_path_plugins = $this->extensionManager->getPluginPath(
      $module, 'Field/FieldType');
    $class_name = $parameters['type_class'];
    $output_file = $module_path_plugins . '/' . $class_name . '.php';

    // Now finally, render the output file based on the twig template
    // and paramters passed in from the command.
    $this->renderFile($twig_file, $output_file, $parameters);
  }
}
