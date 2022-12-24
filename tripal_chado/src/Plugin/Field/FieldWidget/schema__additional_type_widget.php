<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\FormValidator;

/**
 * Plugin implementation of default Tripal string type widget.
 *
 * @FieldWidget(
 *   id = "schema__additional_type_widget",
 *   label = @Translation("Chado Type Reference Widget"),
 *   description = @Translation("A chado type reference widget"),
 *   field_types = {
 *     "schema__additional_type"
 *   }
 * )
 */
class schema__additional_type_widget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $chado = \Drupal::service('tripal_chado.database');

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();
    $storage_settings = $field_settings['storage_plugin_settings'];

    $cvterm_id = $items[$delta]->value ?? NULL;
    $default_value = '';
    if ($cvterm_id) {
      $query = $chado->select('1:cvterm', 'cvt');
      $query->leftJoin('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
      $query->leftJoin('1:db', 'db', 'dbx.db_id = db.db_id');
      $query->fields('cvt', ['name']);
      $query->fields('dbx', ['accession']);
      $query->fields('db', ['name']);
      $query->condition('cvt.cvterm_id', $cvterm_id);
      $result = $query->execute()->fetchObject();
      $default_value = $result->name  . ' (' . $result->db_name . ':' . $result->accession . ')';
    }

    $fixed_value = NULL;
    if (array_key_exists('fixed_value', $storage_settings) and !empty($storage_settings['fixed_value'])) {
      $fixed_value = $storage_settings['fixed_value'];
      list($idSpace, $accession) = explode(':', $fixed_value);
      $query = $chado->select('1:cvterm', 'cvt');
      if ($accession) {
        $query->fields('cvt', ['cvterm_id', 'name']);
        $query->join('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
        $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
        $query->condition('db.name', $idSpace);
        $query->condition('dbx.accession', $accession);
        $cvterm = $query->execute()->fetchObject();
        $default_value = $cvterm->name  . ' (' .$idSpace . ':' . $accession . ')';
      }
    }

    // Use the element defaults. They contain the required value, title, etc.
    $element['value_autoc'] = $element;

    // Cusotmize the widget element.
    $element['value_autoc']['#type'] = 'textfield';
    $element['value_autoc']['#description'] =  t("Enter a vocabulary term name. A set of matching " .
      "candidates will be provided to choose from. You may find the multiple matching terms " .
      "from different vocabularies. The full accession for each term is provided " .
      "to help choose. Only the top 10 best matches are shown at a time.");
    $element['value_autoc']['#default_value'] = $default_value;
    $element['value_autoc']['#autocomplete_route_name'] = 'tripal.cvterm_autocomplete';
    $element['value_autoc']['#autocomplete_route_parameters'] = ['count' => 10];
    if ($fixed_value and $default_value) {
      $element['#disabled'] = TRUE;
    }

    // Store the numeric value in a hidden value.
    $element['value'] = [
      '#type' => 'value',
      '#value' => $cvterm_id,
    ];

    return $element + parent::formElement($items, $delta, $element, $form, $form_state);
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach ($values as $delta => $item) {
       $matches = [];
       if (preg_match('/(.+?)\((.+?):(.+?)\)/', $item['value_autoc'], $matches)) {
         $termIdSpace = $matches[2];
         $termAccession = $matches[3];

         $idSpace = $idSpace_manager->loadCollection($termIdSpace);
         $term = $idSpace->getTerm($termAccession);
         $cvterm_id = $term->getInternalId();
         $values[$delta]['value'] = $cvterm_id;
       }
    }

    return $values;
  }
}