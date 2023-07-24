<?php

namespace Drupal\tripal\TripalTreeParser\PluginManagers;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a tripal importer plugin manager.
 */
class TripalTreeParserManager extends DefaultPluginManager {

  /**
   * Constructs a new tree parser manager.
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
        "Plugin/TripalTreeParser"
        ,$namespaces
        ,$module_handler
        ,'Drupal\tripal\TripalTreeParser\Interfaces\TripalTreeParserInterface'
        ,'Drupal\tripal\TripalTreeParser\Annotation\TripalTreeParser'
    );
    $this->alterInfo("tripal_tree_parser_info");
    $this->setCacheBackend($cache_backend, "tripal_tree_parser_plugins");
  }

  // Plugins can add form elements specific to their parser.
  // Elements common to all parser plugins are defined here.
  // All elements need to be under the 'pub_parser' array index since
  // a placeholder exists for this to be updated by the Ajax callback.
  public function form($form, $form_state) {

    $tree_name = $form_state_values['tree_name'] ?? '';
    $leaf_type = $form_state_values['leaf_type'] ?? '';
    $comment = $form_state_values['description'] ?? '';
    $dbxref = $form_state_values['dbxref'] ?? '';
    $load_later = FALSE;  // Default is to combine tree import with current job

    $form['tree_parser']['tree_name'] = [
      '#type' => 'textfield',
      '#title' => t('Tree Name'),
      '#required' => TRUE,
      '#default_value' => $tree_name,
      '#description' => t('Enter the name used to refer to this phylogenetic tree.'),
      '#maxlength' => 255,
    ];

    $form['tree_parser']['leaf_type'] = [
      '#title' => t('Tree Type (optional)'),
      '#type' => 'textfield',
      '#description' => t("Choose the tree type. The type is
        a valid Sequence Ontology (SO) term. For example, trees derived
        from protein sequences should use the SO term 'polypeptide'.
        When left blank, the tree is assumed to represent a taxonomic tree."),
      '#required' => FALSE,
      '#default_value' => $leaf_type,
      '#autocomplete_route_name' => 'tripal_chado.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 5],
// To-Do: Change line above to this when pull #1585 is merged
//      '#autocomplete_route_parameters' => ['count' => 5, 'cv_id' => $cv_id],
    ];

    $form['tree_parser']['dbxref'] = [
      '#title' => t('Database Cross-Reference'),
      '#type' => 'textfield',
      '#description' => t("Enter a database cross-reference of the form
        [DB name]:[accession]. The database name must already exist in the
        database. If the accession does not exist it is automatically added."),
      '#required' => FALSE,
      '#default_value' => $dbxref,
    ];

    $form['tree_parser']['description'] = [
      '#type' => 'textarea',
      '#title' => t('Description'),
      '#required' => TRUE,
      '#default_value' => $comment,
      '#description' => t('Enter a description for this tree.'),
    ];

    $form['tree_parser']['load_later'] = [
      '#title' => t('Run Tree Import as a Separate Job'),
      '#type' => 'checkbox',
      '#description' => t('Check if tree loading should be performed as a separate job. ' .
        'If not checked, tree loading will be combined with this job.'),
      '#default_value' => $load_later,
    ];

    return $form;
  }

  /**
   * This function is used to rebuild the form if an ajax call is made via a
   * button. The button causes the form to be submitted. We don't want this so we
   * override the validate and submit routines on the form button. Therefore,
   * this function only needs to tell Drupal to rebuild the form
   *
   * @ingroup tripal_tree
   */
  public function tripal_tree_setup_form_ajax_button_validate($form, &$form_state) {
$trigger = $form_state->getTriggeringElement()['#name'];
dpm($trigger, "tripal_pub_setup_form_ajax_button_validate() called, not yet implemented");
    $form_state->setRebuild(TRUE);
  }

  /**
   * This function is just a dummy to override the default form submit on ajax
   * calls for buttons
   *
   * @ingroup tripal_tree
   */
  public function tripal_tree_setup_form_ajax_button_submit($form, &$form_state) {
$trigger = $form_state->getTriggeringElement()['#name'];
dpm($trigger, "tripal_pub_setup_form_ajax_button_submit() called, not yet implemented");
    // do nothing
  }

}
