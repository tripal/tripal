<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;
use Drupal\tripal_chado\Controller\ChadoDbxrefAutocompleteController;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Plugin implementation of default Chado dbxref widget.
 *
 * @FieldWidget(
 *   id = "chado_dbxref_widget_default",
 *   label = @Translation("Chado Dbxref Widget"),
 *   description = @Translation("The default dbxref widget."),
 *   field_types = {
 *     "chado_dbxref_type_default"
 *   }
 * )
 */
class ChadoDbxrefWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $chado = \Drupal::service('tripal_chado.database');

    // Get the list of databases
    $databases = [];
    $query = $chado->select('db', 'd');
    $query->fields('d', ['db_id', 'name']);
    $query->orderBy('name');
    $results = $query->execute();
    while ($db = $results->fetchObject()) {
      $databases[$db->db_id] = $db->name;
    }

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'biomaterial_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();

    // Retrieve a value we need to get from the form state after an ajax callback
    $field_name = $items->getFieldDefinition()->get('field_name');
    $db_id = $form_state->getValue([$field_name, $delta, 'dbxref', 'db_id']);
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    if (!$db_id) {
      $db_id = $item_vals['dbxref_db_id'] ?? 0;
    }
    $dbxref_id = $item_vals['dbxref_id'] ?? 0;
    $accession = $item_vals['dbxref_accession'] ?? '';
    $machine_name = $items->getName();

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['link'] = [
      '#type' => 'value',
      '#default_value' => $link,
    ];

    // The next two fields are inserted into the passed $element so they
    // will be grouped, and we need to indicate that this is a fieldset.
    $element['#type'] = 'fieldset';

    $element['db_id'] = [
      '#type' => 'select',
      '#title' => t('Database Name'),
      '#required' => FALSE,
      '#weight' => 1,
      '#options' => $databases,
      '#empty_option' => t('-- Select --'),
      '#default_value' => $db_id,
      '#ajax' => [
        'callback' =>  [$this, 'widgetAjaxCallback'],
        'event' => 'change',
        'progress' => [
          'type' => 'throbber',
          'message' => $this->t('Preparing Accession field...'),
        ],
        'wrapper' => 'edit-' . $machine_name . '-accession-' . $delta,
      ],
    ];
    $element['dbxref_accession'] = [
      '#type' => 'textfield',
      '#title' => 'Database Accession',
      '#prefix' => '<div id="edit-' . $machine_name . '-accession-' . $delta . '">',
      '#suffix' => '</div>',
      '#weight' => 2,
      '#default_value' => $accession,
      '#disabled' => $db_id?FALSE:TRUE,
      '#autocomplete_route_name' => 'tripal_chado.dbxref_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 5, 'db_id' => $db_id],
    ];
    $elements['dbxref'] = $element;

    // If there are any additional columns present in the linker table,
    // use a default of 1 which will work for type_id or rank.
    // or pub_id. Any existing value will pass through as the default.
    foreach ($property_definitions as $property => $definition) {
      if (($property != 'linker_id') and preg_match('/^linker_/', $property)) {
        $default_value = $item_vals[$property] ?? 1;
        $elements[$property] = [
          '#type' => 'value',
          '#default_value' => $default_value,
        ];
      }
    }
    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Handle any empty values.
    foreach ($values as $val_key => $value) {
      $db_id = $value['dbxref']['db_id'];
      $accession = $value['dbxref']['dbxref_accession'];
      if ($accession == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no dbxref_id, this means
          // we need to pass in this record to chado storage to
          // have the linker record be deleted there.
        }
        else {
          unset($values[$val_key]);
        }
      }
      else {
        // See if we can convert the returned string to its dbxref_id value
        $dbxref_autocomplete = new ChadoDbxrefAutocompleteController();
        $dbxref_id = $dbxref_autocomplete->getDbxrefId($accession, $db_id);

        // This is a new dbxref, we need to insert it and retrieve the dbxref_id.
        if (!$dbxref_id) {
          $chado = \Drupal::service('tripal_chado.database');

          $db_name = '';
          if (preg_match('/([^:]+):(.+)$/', $accession, $matches)) {
            $db_name = $matches[1];
            $accession = $matches[2];
          }

          // Get database db_id from db_name
          if ($db_name) {
            $query = $chado->select('1:db', 'db');
            $query->fields('db', ['db_id']);
            $query->condition('db.name', $db_name, '=');
            $db_id = $query->execute()->fetchField();
          }

          if ($db_id) {
            $insert = $chado->insert('1:dbxref');
              $insert->fields([
                'accession' => $accession,
                'db_id' => $db_id,
              ]);
            $dbxref_id = $insert->execute();
          }
        }
        $values[$val_key]['dbxref_id'] = $dbxref_id;
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
   * Ajax callback to update the db_id for the accession autocomplete.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function widgetAjaxCallback($form, &$form_state) {

    // Extract the field's machine name and delta from the triggering element,
    // e.g. "field_study_dbxref[0][dbxref][db_id]".
    $triggering_element = $form_state->getTriggeringElement()['#name'];
    preg_match('/^([^\[]+)\[(\d+)\]/', $triggering_element, $matches);
    $machine_name = $matches[1];
    $delta = $matches[2];

    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-' . $machine_name . '-accession-' . $delta,
        $form[$machine_name]['widget'][$delta]['dbxref']['dbxref_accession']));
    return $response;
  }
}
