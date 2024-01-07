<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\FormValidator;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal additional type widget.
 *
 * @FieldWidget(
 *   id = "chado_additional_type_widget_default",
 *   label = @Translation("Chado Type Reference Widget"),
 *   description = @Translation("A chado type reference widget"),
 *   field_types = {
 *     "chado_additional_type_type_default"
 *   }
 * )
 */
class ChadoAdditionalTypeWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $chado = \Drupal::service('tripal_chado.database');

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $type_table = $storage_settings['type_table'];
    $fixed_value = $field_settings['fixed_value'];

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $type_id = $item_vals['type_id'] ?? NULL;
    $prop_id = 0;
    $link_id = 0;
    if ($type_table != $base_table) {
      $prop_id = $item_vals['prop_id'] ?? 0;
      $link_id = $item_vals['link_id'] ?? 0;
    }

    // Find the term if one is present.
    $default_autoc = '';
    $term_string = '';
    if ($type_id) {
      $query = $chado->select('1:cvterm', 'cvt');
      $query->leftJoin('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
      $query->leftJoin('1:db', 'db', 'dbx.db_id = db.db_id');
      $query->fields('cvt', ['name']);
      $query->fields('dbx', ['accession']);
      $query->fields('db', ['name']);
      $query->condition('cvt.cvterm_id', $type_id);
      $result = $query->execute()->fetchObject();
      $default_autoc = $result->name  . ' (' . $result->db_name . ':' . $result->accession . ')';
      $term_string = $result->db_name . ':' . $result->accession;
    }

    // If this is a fixed value then get it.
    else if ($fixed_value === TRUE) {

      // If this field is indicated to be a fixed value
      // Then we want to grab the term information from
      // the field settings and use that rather then a
      // user submitted value.
      $idSpace = $field_settings['termIdSpace'];
      $accession = $field_settings['termAccession'];
      $term_string = $idSpace . ':' . $accession;

      // Now we need the cvterm name.
      $query = $chado->select('1:cvterm', 'cvt');
      if ($accession) {
        $query->fields('cvt', ['cvterm_id', 'name']);
        $query->join('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
        $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
        $query->condition('db.name', $idSpace);
        $query->condition('dbx.accession', $accession);
        $cvterm = $query->execute()->fetchObject();
        $default_autoc = $cvterm->name  . ' (' .$idSpace . ':' . $accession . ')';
      }
    }

    // Mark this field as disabled if the value is fixed.
    $disabled = FALSE;
    if ($fixed_value and $default_autoc) {
      $disabled = TRUE;
    }

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['prop_id'] = [
      '#type' => 'value',
      '#default_value' => $prop_id,
    ];
    $elements['link_id'] = [
      '#type' => 'value',
      '#default_value' => $link_id,
    ];
    $elements['type_id'] = [
      '#type' => 'value',
      '#value' => $type_id,
    ];
    $elements['value'] = [
      '#type' => 'value',
      '#default_value' => $term_string,
    ];
    $elements['term_name'] = [
      '#type' => 'value',
      '#default_value' => '',
    ];
    $elements['id_space'] = [
      '#type' => 'value',
      '#default_value' => '',
    ];
    $elements['accession'] = [
      '#type' => 'value',
      '#default_value' => '',
    ];

    // Use the element defaults. They contain the required value, title, etc.
    $elements['term_autoc'] = $element + [
      '#type' => 'textfield',
      '#description' =>  t("Enter a vocabulary term name. A set of matching " .
        "candidates will be provided to choose from. You may find the multiple matching terms " .
        "from different vocabularies. The full accession for each term is provided " .
        "to help choose. Only the top 10 best matches are shown at a time."),
      '#default_value' => $default_autoc,
      '#autocomplete_route_name' => 'tripal.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 10],
      '#disabled' => $disabled,
    ];
    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach ($values as $delta => $item) {
       $matches = [];
       if (preg_match('/(.+?)\(([^\(]+?):(.+?)\)/', $item['term_autoc'], $matches)) {
         $termIdSpace = $matches[2];
         $termAccession = $matches[3];

         /** @var \Drupal\tripal\TripalVocabTerms\TripalTerm $term **/
         $idSpace = $idSpace_manager->loadCollection($termIdSpace);
         $term = $idSpace->getTerm($termAccession);
         $cvterm_id = $term->getInternalId();
         $values[$delta]['type_id'] = $cvterm_id;
         $values[$delta]['value'] = $term->getName();
         $values[$delta]['term_name'] = $term->getName();
         $values[$delta]['id_space'] = $term->getIdSpace();
         $values[$delta]['accession'] = $term->getAccession();
       }
    }
    return $values;
  }
}
