<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Sequence widget.
 *
 * @FieldWidget(
 *   id = "chado_synonym_widget_default",
 *   label = @Translation("Chado Alias Widget"),
 *   description = @Translation("The default chado synonym widget."),
 *   field_types = {
 *     "chado_synonym_type_default"
 *   }
 * )
 */
class ChadoSynonymWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $chado = \Drupal::service('tripal_chado.database');

    $schema = $chado->schema();
    $synonym_table_def = $schema->getTableDef('synonym', ['format' => 'Drupal']);
    $syn_name_len = $synonym_table_def['fields']['name']['size'];

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_pkey_id = $item_vals['linker_pkey_id'] ?? 0;
    $linker_base_fkey_id = $item_vals['linker_base_fkey_id'] ?? 0;
    $linker_synonym_fkey_id = $item_vals['linker_synonym_fkey_id'] ?? 0;
    $linker_pub_id = $item_vals['linker_pub_id'] ?? 0;
    $is_current = $item_vals['is_current'] ?? TRUE;
    $is_internal = $item_vals['is_internal'] ?? FALSE;
    $name = $item_vals['name'] ?? '';
    $synonym_type = $item_vals['synonym_type'] ?? 'exact';
    $synonym_sgml = $item_vals['synonym_sgml'] ?? '';

    // Get the `exact` synonym type.  There are other types
    // that are installed by Tripal such as BROAD, NARROW, and RELATED
    // but these are meant for terms. Anyone using this field should
    // simply be adding alternate names so this will always be
    // an "exact" synonym.
    $query = $chado->select('1:cvterm', 'cvt');
    $query->leftJoin('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
    $query->fields('cvt', ['cvterm_id', 'name', 'definition']);
    $query->condition('cv.name', 'synonym_type');
    $results = $query->execute();
    $default_syn_type_id = NULL;
    while ($result = $results->fetchObject()) {
      if ($result->name == 'exact') {
        $default_syn_type_id = $result->cvterm_id;
      }
    }

    // Get the default null publication for now until we have a
    // decent pub lookup field.
    // @todo: update this for a pub selector field later.
    $query = $chado->select('1:pub', 'p');
    $query->fields('p', ['pub_id']);
    $query->condition('p.uniquename', 'null');
    $linker_pub_id = $query->execute()->fetchField();

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    // primary key column of linking table, e.g. feature_synonym_id
    $elements['linker_pkey_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_pkey_id,
    ];
    // linker_base_fkey_id corresponds to base table id, e.g. feature_id
    $elements['linker_base_fkey_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_base_fkey_id,
    ];
    // linker_synonym_fkey_id corresponds to synonym_id
    $elements['linker_synonym_fkey_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_synonym_fkey_id,
    ];
    $elements['linker_pub_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_pub_id,
    ];
    $elements['name'] = $element + [
      '#type' => 'textfield',
      '#title' => 'Synonym',
      '#default_value' => $name,
      '#maxlength' => $syn_name_len,
      '#description' => 'An alias or alternate name for this entity.',
      '#weight' => 10,
    ];
    $elements['synonym_type_id'] = [
      '#type' => 'value',
      '#default_value' => $default_syn_type_id,
    ];
    $elements['synonym_type'] = [
      '#type' => 'value',
      '#default_value' => $synonym_type,
    ];
    $elements['synonym_sgml'] = [
      '#type' => 'value',
      '#default_value' => $synonym_sgml,
    ];
    $elements['is_current'] = [
      '#type' => 'checkbox',
      '#title' => t('Is this synonym current?'),
      '#description' => t('If a different synonym has replaced this one, uncheck this box. '.
          'However, more than one synonym can be current.'),
      '#default_value' => $is_current,
      '#weight' => 12,
    ];
    $elements['is_internal'] = [
      '#type' => 'checkbox',
      '#title' => 'Is this synonym internal?',
      '#default_value' => $is_internal,
      '#description' => t('Typically a synonym exists so that somebody querying the database with an ' .
          'obsolete name can find the object they\'re looking for.  If the synonym has been used publicly '.
          'and deliberately (e.g. in a paper), it may also be listed in reports as a synonym. If the ' .
          'synonym was not used deliberately (e.g. there was a typo which went public), then the ' .
          'synonym is internal.'),
      '#weight' => 14,
    ];

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $linker_pkey_id, 'linker_synonym_fkey_id', $form_state, 'linker_pkey_id');

    return $elements;
  }


  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $chado = \Drupal::service('tripal_chado.database');

    // Remove any empty values that don't have a name
    foreach ($values as $delta => $item) {
      if ($item['name'] == '') {
        unset($values[$delta]);
      }
    }

    // Iterate through the synonyms and try to get the synonym_id.
    // If the synonym is not already present in the synonym table,
    // then add it.
    foreach ($values as $delta => $item) {
      $name = $item['name'];
      $synonym_type_id = $item['synonym_type_id'];

      $query = $chado->select('1:synonym', 's');
      $query->fields('s', ['name', 'synonym_id']);
      $query->condition('s.name', $item['name']);
      $synonym = $query->execute()->fetchObject();
      if (!$synonym) {
        $insert = $chado->insert('1:synonym');
        $insert->fields([
          'name' => $name,
          'type_id' => $synonym_type_id,
          'synonym_sgml' => '',
        ]);
        $insert->execute();
        $synonym = $query->execute()->fetchObject();
      }
      $values[$delta]['linker_synonym_fkey_id'] = $synonym->synonym_id;
    }

    return $this->massageLinkingFormValues('linker_synonym_fkey_id', $values, $form_state, 'linker_pkey_id');
  }

}
