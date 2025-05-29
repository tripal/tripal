<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\tripal_chado\Controller\ChadoGenericAutocompleteController;


/**
 * Defines the Chado field widget base class.
 *
 * For linking multi-cardinality fields, this class includes the following
 * helper methods to provide support for the "remove" button added by Drupal.
 * To enable this support, the following need to be implemented in your widget:
 *  1. You have the following elements in formElement():
 *     - a hidden value element with the key of 'record_id' that contains the
 *       base table primary key. This is used to determine if an element should
 *       be removed due to being set to empty.
 *     - a hidden value element with the key of 'field_name' and the value is
 *       the name of the field (see code example below).
 *       @code
 *         $field_name = $items->getFieldDefinition()->get('field_name');
 *         $elements['field_name'] = [
 *           '#type' => 'value',
 *           '#default_value' => $field_name,
 *         ];
 *       @endcode
 *  2. You call saveInitialValues() at the bottom of your formElement() and
 *     pass in information about the linking record.
 *  3. You call massageLinkingFormValues() or massagePropertyFormValues() in
 *     your massageFormValues() and indicate the requested elements. See the doc
 *     block headers for these methods for specifics.
 */
abstract class ChadoWidgetBase extends TripalWidgetBase {

  /**
   * Saves some values from the initial form state when an entity
   * is first edited for multi-cardinality linking fields.
   * These values are needed to support the "Remove" button.
   *
   * @param int $delta
   *   The numeric index of the item.
   * @param string $field_name
   *   The machine name of the field used for linking the info we're saving in
   *   form_state with the values submitted by the form.
   * @param int $linker_id
   *   The primary key value of the record in the linking table.
   * @param FormStateInterface &$form_state
   *   The current form state.
   */
  protected function saveInitialValues(int $delta, string $field_name, int $linker_id, FormStateInterface &$form_state) {
    $storage = $form_state->getStorage();
    // We want the initial values, so never update them once saved.
    if (!($storage['initial_values'][$field_name][$delta] ?? FALSE)) {
      $storage['initial_values'][$field_name][$delta] = [
        'linker_id' => $linker_id,
      ];
      $form_state->setStorage($storage);
    }
  }

  /**
   * Gets the select_limit value.
   *
   * This finds the value by using, in order:
   *   1. An explicit value passed to this function
   *   2. If not defined, then the value the field's settings
   *   3. If not defined, then the value from the global setting
   *   4. If not defined, then use the default value of 50
   *
   * @param int ?$select_limit
   *   If not NULL or an empty string, use this value
   *
   * @return int
   *   A non-negative integer. Zero is used to indicate to always use autocomplete.
   */
  protected function getSelectLimit(?int $select_limit = NULL) {
    if (is_null($select_limit) or (trim($select_limit) === '')) {
      $select_limit = $this->getSetting('widget_select_limit');
      if (is_null($select_limit) or (trim($select_limit) === '')) {
        $select_limit = \Drupal::config('tripal.settings')->get('tripal_entity_type.widget_global_select_limit');
        if (is_null($select_limit) or (trim($select_limit) === '')) {
          $select_limit = 50;
        }
      }
    }
    return $select_limit;
  }

  /**
   * Generic select form element generator. For a small number of values
   * this creates a select, for many values this creates an autocomplete.
   *
   * @param string $pkey_column
   *   The name of the primary key column
   * @param int|null $default_id
   *   The pkey_id value of the default, if one exists
   * @param array $options
   *   'base_table' - Chado table name
   *   'column_name' - Column name in the base table
   *   'type_column' - Column in base table specifying type,
   *       or single character placeholder if none exists.
   *   'property_table' - same as base table if type is stored there
   *   'type_id' - cvterm_id to limit by, 0 for no limiting.
   *   'match_operator' - Either "CONTAINS" or "STARTS_WITH"
   *   'match_limit' -Number of records that the autoselect will present
   *   'size' - Size of the autocomplete form field
   *   'placeholder' - Placeholder before autocomplete is filled
   *   'select_limit' - The maximum number of records for a select. If more,
   *       then use autocomplete. Use zero if autocomplete always wanted.
   *       If NULL or empty string, then the global setting will be used.
   *
   * @return array
   *   The appropriate form element
   */
  protected function genericSelectElement(string $pkey_column, ?int $default_id,
      array $options): array {

    // Set some defaults to keep each of the fields simpler
    $options['type_id'] ??= 0;
    $options['select_limit'] = $this->getSelectLimit($options['select_limit'] ?? NULL);
    $options['match_operator'] ??= $this->getSetting('match_operator') ?? 'CONTAINS';
    $options['match_limit'] ??= $this->getSetting('match_limit') ?? 10;
    $options['size'] ??= $this->getSetting('size');
    $options['placeholder'] ??= $this->getSetting('placeholder');

    // Validation for developers
    $required = ['base_table', 'column_name', 'type_column', 'property_table'];
    foreach ($required as $key) {
      if (!array_key_exists($key, $options)) {
        throw new \Exception(t('genericSelectElement options is missing required key "@key"', ['@key' => $key]));
      }
    }

    $element = [];

    // Construct a query
    // A single wildcard indicates that all records are to be returned
    $string = '%';
    // Add one to select limit so we know it is exceeded
    $count_options = $options;
    $count_options['match_limit'] = $options['select_limit'] + 1;
    $query = ChadoGenericAutocompleteController::getQuery($string, $count_options);

    // Get a count of the number of possible values, unless forcing always autocomplete
    $count = 1;
    if ($options['select_limit'] > 0) {
      $count = $query->countQuery()->execute()->fetchField();
    }

    // For a large number of options, or if limit is zero, use an autocomplete
    if ($count > $options['select_limit']) {
      // Look up the default value if one was specified
      $default_value = '';
      if ($default_id) {
        // We can reuse the existing query since only one change is needed
        $query->condition($pkey_column, $default_id, '=');
        $result = $query->execute()->fetchObject();
        if ($result) {
          // Strip HTML tags if present, e.g. in Pub title
          $default_value = strip_tags($result->value ?? '');
          if (property_exists($result, 'type') and $result->type) {
            $default_value .= ' [' . $result->type . ']';
          }
          // Append the chado pkey id value
          $default_value .= ' (' . $default_id . ')';
        }
      }
      $element = [
        '#type' => 'textfield',
        '#default_value' => $default_value,
        '#autocomplete_route_name' => 'tripal_chado.generic_autocomplete',
        '#size' => $options['size'],
        '#maxlength' => 1000,
        '#placeholder' => $options['placeholder'],
      ];
      unset($options['size']);
      unset($options['placeholder']);
      $element['#autocomplete_route_parameters'] = $options;
    }

    // For a small number of options, use a select
    else {
      $select_query = ChadoGenericAutocompleteController::getQuery($string, $options);
      $results = $select_query->execute();
      $select_options = [];
      while ($record = $results->fetchObject()) {
        // Strip HTML tags if present, e.g. in Pub title
        $value = strip_tags($record->value ?? '');
        if (property_exists($record, 'type') and $record->type) {
          $value .= ' [' . $record->type . ']';
        }
        $select_options[$record->pkey] = $value;
      }
      natcasesort($select_options);
      $element = [
        '#type' => 'select',
        '#options' => $select_options,
        '#default_value' => $default_id,
        '#empty_option' => $this->t('- Select -'),
      ];
    }
    $element['#element_validate'] = [[static::class, 'validateAutocomplete']];
    return $element;
  }

  /**
   * Default settings associated with the generic select element
   *
   * @return array
   *   The default settings.
   */
  public static function defaultSelectSettings(): array {
    return [
      'widget_select_limit' => '',  // Global setting applies when not set
      'match_operator' => 'CONTAINS',
      'match_limit' => 10,
      'size' => 60,
      'placeholder' => '',
    ];
  }

  /**
   * Elements for the widget settings form for the generic select element
   *
   */
  public function selectSettingsForm(array $form, FormStateInterface $form_state) {
    $element = [];
    $element['widget_select_limit'] = [
      '#type' => 'number',
      '#min' => 0,
      '#step' => 1,
      '#title' => t('Maximum records for a select'),
      '#description' => $this->t('The value here controls whether a widget select element uses'
                        . ' a dropdown select list, or an autocomplete.'
                        . ' A dropdown can be difficult to use and is a performance problem'
                        . ' if the number of records is large.'
                        . ' When the number of records is larger than the value entered here,'
                        . ' then use an autocomplete.'
                        . ' If the value is left blank, then the global setting will be used.'
                        . ' Enter <em>0</em> to indicate that an autocomplete should always be used.'),
      '#required' => FALSE,
      '#default_value' => $this->getSetting('widget_select_limit') ?? '',
    ];

    // These elements are copied directly from Drupal
    $element['match_operator'] = [
      '#type' => 'radios',
      '#title' => $this->t('Autocomplete matching'),
      '#default_value' => $this->getSetting('match_operator'),
      '#options' => $this->getMatchOperatorOptions(),
      '#description' => $this->t('Select the method used to collect autocomplete suggestions. Note that <em>Contains</em> can cause performance issues on sites with thousands of entities.'),
    ];
    $element['match_limit'] = [
      '#type' => 'number',
      '#title' => $this->t('Number of results'),
      '#default_value' => $this->getSetting('match_limit'),
      '#min' => 0,
      '#description' => $this->t('The number of suggestions that will be listed. Use <em>0</em> to remove the limit.'),
    ];
    $element['size'] = [
      '#type' => 'number',
      '#title' => $this->t('Size of textfield'),
      '#default_value' => $this->getSetting('size') ?: 60,
      '#min' => 1,
      '#required' => TRUE,
    ];
    $element['placeholder'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Placeholder'),
      '#default_value' => $this->getSetting('placeholder'),
      '#description' => $this->t('Text that will be shown inside the field until a value is entered. This hint is usually a sample value or a brief description of the expected format.'),
    ];

    return $element;
  }

  /**
   * Summary of the settings for the generic select element
   */
  public function selectSettingsSummary() {
    $summary = [];
    $select_limit = $this->getSetting('widget_select_limit');
    if (is_null($select_limit) or ($select_limit === '')) {
      $global_select_limit = \Drupal::config('tripal.settings')
        ->get('tripal_entity_type.widget_global_select_limit') ?? 50;
      $select_limit = $this->t('@global_select_limit (global setting)',
                               ['@global_select_limit' => $global_select_limit]);
    }

    $summary[] = $this->t('Maximum records for a select: @select_limit',
                          ['@select_limit' => $select_limit]);
    $operators = $this->getMatchOperatorOptions();
    $summary[] = $this->t('Autocomplete matching: @match_operator',
                          ['@match_operator' => $operators[$this->getSetting('match_operator')]]);
    $size = $this->getSetting('match_limit') ?: $this->t('unlimited');
    $summary[] = $this->t('Autocomplete suggestion list size: @size',
                          ['@size' => $size]);
    $summary[] = $this->t('Textfield size: @size',
                          ['@size' => $this->getSetting('size')]);
    $placeholder = $this->getSetting('placeholder');
    if (is_null($placeholder) or ($placeholder === '')) {
      $summary[] = $this->t('No placeholder');
    }
    else {
      $summary[] = $this->t('Placeholder: @placeholder',
                            ['@placeholder' => $placeholder]);
    }
    return $summary;
  }

  /**
   * Assists the massageFormValues() function for linking fields, that
   * is, double-hop fields where an intermediate linking table is used.
   * This includes properly handling deletion of the record in the
   * linking table in chado.
   *
   * @param string $fkey
   *   The foreign key column name in the linking table.
   *   Needed because it is not guaranteed to be in $values array,
   *   e.g. for dbxref.
   * @param array $values
   *   The submitted form values produced by the widget.
   *   - If the widget does not manage multiple values itself, the array holds
   *     the values generated by the multiple copies of the $element generated
   *     by the formElement() method, keyed by delta.
   *   - If the widget manages multiple values, the array holds the values
   *     of the form element generated by the formElement() method.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state.
   * @param string $linker_key
   *   The key used for the linking table primary key.
   *   For most fields this is "linker_id" and can be omitted, but
   *   see the synonym field for an exception.
   *
   * @return array
   *   An array of field values, keyed by delta.
   */
  protected function massageLinkingFormValues(string $fkey, array $values,
      FormStateInterface $form_state, string $linker_key = 'linker_id') {
    if (!$values) {
      return $values;
    }

    // In some cases the foreign key is not the same name as in the
    // base table, e.g. manufacturer_id as a fkey for contact_id.
    // n.b. this has no effect for the property field.
    $first_delta = array_key_first($values);
    $fkey = $values[$first_delta]['linker_fkey_column'] ?? $fkey;

    // The machine name for the field. Sometimes there are multiple
    // copies of one field, e.g. properties, so this distinguishes them.
    $field_name = $values[$first_delta]['field_name'];

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is changed to "- Select -"
    $retained_records = [];
    foreach ($values as $val_key => $value) {
      if ($value[$linker_key]) {
        $retained_records[$val_key] = $value[$linker_key];
      }
      if ($value[$fkey] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no linked record id, this
          // means we need to pass in this record to chado storage
          // to have the linker record be deleted there. To do
          // this, we need to have the correct primitive type for
          // this field, so change from empty string to zero.
          $values[$val_key][$fkey] = 0;
        }
        else {
          // If there is no record_id, then it is the empty
          // field at the end of the list, and should be removed
          // so that drupal does not try to save it.
          unset($values[$val_key]);
        }
      }
    }

    // If there were any values in the initial values that are not
    // present in the current form state, then an existing record
    // was deleted by clicking the "Remove" button. Similarly to
    // the code above, we need to include these in the values array
    // so that chado storage is informed to delete the linking record.
    $next_delta = $values ? array_key_last($values) + 1 : 0;
    $storage_values = $form_state->getStorage();
    $initial_values = (array_key_exists($field_name, $storage_values['initial_values'])) ? $storage_values['initial_values'][$field_name] : [];
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id', regardless of $linker_key value.
      $linker_id = $initial_value['linker_id'] ?? 0;
      if ($linker_id and !in_array($linker_id, $retained_records)) {
        // This item was removed from the form. Add back a value
        // so that chado storage knows to remove the chado record.
        $values[$next_delta][$linker_key] = $linker_id;
        $values[$next_delta][$fkey] = 0;
        $next_delta++;
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      $values[$val_key]['_weight'] = $i;
      $i++;
    }
    return $values;
  }

  /**
   * A helper for massageFormValues() where the generic autocomplete is used.
   *
   * The genericSelectElement() will return an integer in the $values array
   * when a select is used, but returns a string with an embedded id value
   * in parentheses when the autocomplete is used. In this latter case, we
   * need to return just the integer value.
   * Note that if you somehow pass a text string without an embedded value,
   * then no changes are made, however, this should be prevented by validation.
   *
   * @param string $pkey_id
   *   The name of the value to be massaged, e.g. "analysis_id"
   * @param array $values
   *   The submitted form values produced by the widget.
   * @return array
   *   The massaged values
   */
  protected function genericSelectMassageFormValues(string $pkey_id, array $values): array {
    foreach ($values as $key => $info) {
      if (array_key_exists($pkey_id, $info)) {
        // If already an integer, then do nothing
        if (!preg_match('/^\d+$/', $info[$pkey_id])) {
          if (preg_match('/\((\d+)\)$/', $info[$pkey_id], $matches)) {
            $values[$key][$pkey_id] = $matches[1];
          }
        }
      }
    }
    return $values;
  }

  /**
   * Assists the massageFormValues() function for property fields, that
   * is, single-hop fields where the linked table contains a value.
   * This includes properly handling deletion of the record in the
   * linked table in chado.
   *
   * @param string $val
   *   The name that the value is stored under, i.e. 'value'
   * @param array $values
   *   The submitted form values produced by the widget.
   *   - If the widget does not manage multiple values itself, the array holds
   *     the values generated by the multiple copies of the $element generated
   *     by the formElement() method, keyed by delta.
   *   - If the widget manages multiple values, the array holds the values
   *     of the form element generated by the formElement() method.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state.
   * @param string $rank_term
   *   If present, used to reset rank in values.
   * @param string $linker_key
   *   The key used for the linking table primary key.
   *   For property fields this is "prop_id" and can be omitted.
   *
   * @return array
   *   An array of field values, keyed by delta.
   */
  protected function massagePropertyFormValues(string $val, array $values,
      FormStateInterface $form_state, string $rank_term = NULL, string $linker_key = 'prop_id') {

    if (!$values) {
      return $values;
    }

    // The field name for the field. There are usually multiple
    // copies of a property field, so this distinguishes them.
    $first_delta = array_key_first($values);
    if (array_key_exists('field_name', $values[$first_delta])) {
      $field_name = $values[$first_delta]['field_name'];
    }
    else {
      $class_name = get_class($this);
      throw new \Exception("The field name is not set for this property field (class: $class_name). It is needed when massaging the values in order to keep them unique.");
    }

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is changed to "- Select -"
    $retained_records = [];
    foreach ($values as $val_key => $value) {
      if ($value[$linker_key]) {
        $retained_records[$val_key] = $value[$linker_key];
      }
      if (array_key_exists($val, $value) and ($value[$val] == '')) {
        if ($value['record_id']) {
          // If there is a record_id, but no value, this
          // means we need to pass in this record to chado storage
          // to have the linker record be deleted there. Here,
          // the empty string is the correct primitive type,
          // so nothing to change.
        }
        else {
          // If there is no record_id, then it is the empty
          // field at the end of the list, and can be ignored.
          unset($values[$val_key]);
        }
      }
    }

    // If there were any values in the initial values that are not
    // present in the current form state, then an existing record
    // was deleted by clicking the "Remove" button. Similarly to
    // the code above, we need to include these in the values array
    // so that chado storage is informed to delete the linking record.
    $next_delta = $values ? array_key_last($values) + 1 : 0;
    $storage_values = $form_state->getStorage();
    $initial_values = $storage_values['initial_values'][$field_name];
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id', regardless of $linker_key value.
      $linker_id = $initial_value['linker_id'];
      if ($linker_id and !in_array($linker_id, $retained_records)) {
        // This item was removed from the form. Add back a value
        // so that chado storage knows to remove the chado record.
        $values[$next_delta][$linker_key] = $linker_id;
        $values[$next_delta][$val] = '';
        $next_delta++;
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      if (array_key_exists($val, $values[$val_key]) and ($values[$val_key][$val])) {
        $values[$val_key]['_weight'] = $i;
        if ($rank_term) {
          $values[$val_key][$rank_term] = $i;
        }
        $i++;
      }
    }
    return $values;
  }

  /**
   * Form element validation handler for an autocomplete field
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateAutocomplete($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];

    // The value must either be an integer, or a string with an integer
    // value in parentheses at the end.
    $valid = TRUE;
    if ($element_value) {
      $valid = FALSE;
      if (preg_match('/^\d+$/', $element_value)) {
        $valid = TRUE;
      }
      elseif (preg_match('/\(\d+\)$/', $element_value)) {
        $valid = TRUE;
      }
    }
    if (!$valid) {
      $form_state->setErrorByName(implode('][', $element_parents),
          'The specified record must include its chado record number in parentheses at the end');
    }
  }

  /**
   * Returns the options for the match operator.
   *
   * @return array
   *   List of options.
   */
  protected function getMatchOperatorOptions() {
    return [
      'STARTS_WITH' => $this->t('Starts with'),
      'CONTAINS' => $this->t('Contains'),
    ];
  }

}
