<?php

namespace Drupal\tripal\Field;

use Drupal\Core\Field\FieldItemBase;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\TypedData\DataDefinitionInterface;
use Drupal\Core\TypedData\Plugin\DataType\Map;
use Drupal\Core\TypedData\TypedDataInterface;
use Drupal\Core\Messenger\MessengerTrait;


/**
 * A Tripal-based entity field item.
 *
 * Entity field items making use of this base class have to implement
 * the static method propertyDefinitions().
 *
 */

abstract class TripalFieldItemBase extends FieldItemBase {
  use MessengerTrait;

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [
      // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
      'term_vocabulary' => 'schema',
      // The name of the term.
      'term_name' => 'Thing',
      // The unique ID (i.e. accession) of the term.
      'term_accession' => 'Thing',
      // Set to TRUE if the site admin is not allowed to change the term
      // type, otherwise the admin can change the term mapped to a field.
      'term_fixed' => FALSE,
      // Set to TRUE if the field should be automatically attached to an entity
      // when it is loaded. Otherwise, the callee must attach the field
      // manually.  This is useful to prevent really large fields from slowing
      // down page loads.  However, if the content type display is set to
      // "Hide empty fields" then this has no effect as all fields must be
      // attached to determine which are empty.  It should always work with
      // web services.
      'auto_attach' => TRUE,
    ];
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $element = [];
    $settings = $this->getSettings();

    // Get the term for this instance.
    $vocabulary = $settings['term_vocabulary'];
    $accession = $settings['term_accession'];
    $term_name = $settings['term_name'];
    $term_fixed = $settings['term_fixed'];
    $term = tripal_get_term_details($vocabulary, $accession);

    if (is_object($term)) {
      $vocab_name = $term['vocabulary']['name'];
      $vocab_description = $term['vocabulary']['description'];
      $term_name = $term['name'];
      $term_def = $term['definition'];
    }
    else {
      $vocab_name = 'UKNOWN';
      $vocab_description = 'UKNOWN';
      $term_name = 'UKNOWN';
      $term_def = 'UKNOWN';

      $this->messenger()->addWarning($this->t(
        'The term, %accession, does not yet exist for this field.', [
        '%accession' => $vocabulary . ':' . $accession,
      ]));
    }
    // Construct a table for the vocabulary information.
    $headers = [];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Vocabulary',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocab_name . ' (' . $vocabulary . ') ' . $vocab_description,
    ];
    $rows[] = [
      [
        'data' => 'Term',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary . ':' . $accession,
    ];
    $rows[] = [
      [
        'data' => 'Name',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term_name,
    ];
    $rows[] = [
      [
        'data' => 'Definition',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term_def,
    ];

    $element['term_vocabulary'] = [
      '#type' => 'value',
      '#value' => $vocabulary,
    ];
    $element['term_name'] = [
      '#type' => 'value',
      '#value' => $term_name,
    ];
    $element['term_accession'] = [
      '#type' => 'value',
      '#value' => $accession,
    ];
    $description = t('All fields attached to a Tripal-based content type must ' .
        'be associated with a controlled vocabulary term.  Please use caution ' .
        'when changing the term for this field as other sites may expect this term ' .
        'when querying web services.');
    if ($term_fixed) {
      $description = t('All fields attached to a Tripal-based content type must ' .
          'be associated with a controlled vocabulary term. This field mapping is ' .
          'required and cannot be changed');
    }
    $element['field_term'] = [
      '#type' => 'fieldset',
      '#title' => 'Controlled Vocabulary Term',
      '#description' => $description,
      '#description_display' => 'before',
      '#prefix' => '<div id = "tripal-field-term-fieldset">',
      '#suffix' => '</div>',
      '#weight' => 1000,
    ];
    $element['field_term']['details'] = [
      '#type' => 'table',
      '#title' => 'Current Term',
      '#header' => $headers,
      '#rows' => $rows
    ];

    // If this field mapping is fixed then don't let the user change it.
    if ($term_fixed != TRUE) {
      $element['field_term']['new_name'] = [
        '#type' => 'textfield',
        '#title' => 'Change the term',
        // TODO: This autocomplete path should not use Chado.
        //'#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
      ];
      $element['field_term']['select_button'] = [
        '#type' => 'button',
        '#value' => t('Lookup Term'),
        '#name' => 'select_cvterm',
        '#ajax' => [
          'callback' => "tripal_fields_select_term_form_ajax_callback",
          'wrapper' => "tripal-field-term-fieldset",
          'effect' => 'fade',
          'method' => 'replace',
        ],
      ];
    }
    // @TODO: We don't yet have the term lookup working.
/*
    // If a new term name has been specified by the user then give some extra
    // fields to clarify the term.
    $term_name = '';
    if (array_key_exists('values', $form_state) and array_key_exists('new_name', $form_state['values'])) {
      $term_name = array_key_exists('values', $form_state) ? $form_state['values']['new_name'] : '';
    }
    if (array_key_exists('input', $form_state) and array_key_exists('new_name', $form_state['input'])) {
      $term_name = array_key_exists('input', $form_state) ? $form_state['input']['new_name'] : '';
    }
    if ($term_name) {
      $element['field_term']['instructions'] = [
        '#type' => 'item',
        '#title' => 'Matching terms',
        '#markup' => t('Please select the term the best matches the ' .
          'content type you want to associate with this field. If the same term exists in ' .
          'multiple vocabularies you will see more than one option below.'),
      ];
      $match = [
        'name' => $term_name,
      ];
      $terms = chado_generate_var('cvterm', $match, ['return_array' => TRUE]);
      $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
      $num_terms = 0;
      foreach ($terms as $term) {
        // Save the user a click, by setting the default value as 1 if there's
        // only one matching term.
        $default = FALSE;
        $attrs = [];
        if ($num_terms == 0 and count($terms) == 1) {
          $default = TRUE;
          $attrs = ['checked' => 'checked'];
        }
        $element['field_term']['term-' . $term->cvterm_id] = [
          '#type' => 'checkbox',
          '#title' => $term->name,
          '#default_value' => $default,
          '#attributes' => $attrs,
          '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
          '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
          '<br><b>Definition:</b>  ' . $term->definition,
        ];
        $num_terms++;
      }
      if ($num_terms == 0) {
        $element['field_term']['none'] = [
          '#type' => 'item',
          '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>',
        ];
      }
    }
    */
    $element['#element_validate'][] = 'tripal_field_instance_settings_form_alter_validate';
    return $element;
  }
}
