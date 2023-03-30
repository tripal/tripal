<?php

namespace Drupal\tripal\TripalPubParser\PluginManagers;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a tripal importer plugin manager.
 */
class TripalPubParserManager extends DefaultPluginManager {

  /**
   * Constructs a new publication parser manager.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param string $plugin_interface
   *   The interface each plugin should implement.
   * @param string $plugin_definition_annotation_name
   *   The name of the annotation that contains the plugin definition.
   */
  public function __construct(
      \Traversable $namespaces
      ,CacheBackendInterface $cache_backend
      ,ModuleHandlerInterface $module_handler
  ) {
    parent::__construct(
        "Plugin/TripalPubParser"
        ,$namespaces
        ,$module_handler
        ,'Drupal\tripal\TripalPubParser\Interfaces\TripalPubParserInterface'
        ,'Drupal\tripal\TripalPubParser\Annotation\TripalPubParser'
    );
    $this->alterInfo("tripal_pub_parser_info");
    $this->setCacheBackend($cache_backend, "tripal_pub_parser_plugins");
  }

  // Plugins will add form elements specific to their parser.
  // Elements common to all parser plugins are defined here.
  public function form($form, $form_state) {
//to-do get these values
    $disabled = '';
    $do_contact = '';

    $form['pub_parser']['loader_name'] = [
      '#title' => t('Loader Name'),
      '#type' => 'textfield',
      '#description' => t("Please provide a name for this loader setup"),
      '#required' => TRUE,
    ];
    $form['pub_parser']['disabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Disabled'),
      '#description' => t('Check to disable this importer.'),
      '#default_value' => $disabled,
    ];
    $form['pub_parser']['do_contact'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Contact'),
      '#description' => t('Check to create an entry in the contact table for each author of'
         . ' a matching publication during import. This allows storage of additional information'
         . ' such as affilation, etc. Otherwise, only authors\' names are retrieved'),
      '#default_value' => $do_contact,
    ];

    // Add the form for the criteria
//to-do    tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $num_criteria, $criteria);

    // Add the submit buttons
    $form['save'] = [
      '#type' => 'submit',
      '#value' => t('Save Importer'),
    ];
    $form['test'] = [
      '#type' => 'submit',
      '#value' => t('Test Importer'),
    ];
    $form['delete'] = [
      '#type' => 'submit',
      '#value' => t('Delete Importer'),
      '#attributes' => ['style' => 'float: right;'],
    ];

    // add in the section where the test results will appear
    $form['results'] = [
      '#markup' => '<div id="tripal-pub-importer-test-section"></div>',
    ];

    return $form;
  }

}
