<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\FieldItemBase;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\tripal\TripalStorage\BoolStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\TypedData\DataDefinition;
use \RuntimeException;

/**
 * Defines the Tripal field item base class.
 */
abstract class TripalFieldItemBase extends FieldItemBase implements TripalFieldItemInterface {

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [
      'termIdSpace' => '',
      'termAccession' => '',
      # 'max_delta' => 100,
      // A simple flag to indicate that we should enable debugging information
      // for this field type.
      // This will be used by ChadoStorage to tell the ChadoFieldDebugger service
      // to display debugging information. All you need to do as a developer is
      // set this variable to TRUE in your field and debuggin information will be
      // displayed on the screen and in the drupal logs when you create, edit,
      // and load content that has your field attached.
      'debug' => FALSE,
    ];
    return $settings + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    // We copy over the field settings for the CV term to the storage
    // settings from whatever child class is calling this function.
    $child_class = static::class;
    $settings = [
      'termIdSpace' => ($child_class::defaultFieldSettings()['termIdSpace'] ?? ''),
      'termAccession' => ($child_class::defaultFieldSettings()['termAccession'] ?? ''),
      'storage_plugin_id' => '',
      'storage_plugin_settings' => [],
    ];
    return $settings + parent::defaultStorageSettings();
  }


  /**
   * A helper function for the fieldSettingsForm.
   *
   * Builds the table the describes the term assigned to the field.
   *
   * @param array $elements
   * @param \Drupal\tripal\TripalVocabTerms\TripalTerm $term
   * @param \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase $idSpace
   * @param \Drupal\tripal\TripalVocabTerms\TripalVocabularyBase $vocabulary
   */
  protected function buildVocabularyTermTable(array &$elements,
      \Drupal\tripal\TripalVocabTerms\TripalTerm $term,
      \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase $idSpace,
      \Drupal\tripal\TripalVocabTerms\TripalVocabularyBase $vocabulary) {

    // Construct a table for the vocabulary information.
    $headers = ['Term Property', 'Value'];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Vocabulary Name',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Vocabulary Description',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getLabel(),
    ];
    $rows[] = [
      [
        'data' => 'Term ID Space',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Term ID Space Description',
        'header' => TRUE,
        'width' => '20%',
      ],
      $idSpace->getDescription(),
    ];
    $rows[] = [
      [
        'data' => 'Term Accession',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getAccession(),
    ];
    $rows[] = [
      [
        'data' => 'Term ID',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getIdSpace() . ':' . $term->getAccession(),
    ];
    $rows[] = [
      [
        'data' => 'Term Name',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Term Definition',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getDefinition(),
    ];
    $elements['field_term_fs']['table_label'] = [
      '#type' => 'item',
      '#title' => $this->t('The Current Term'),
      '#description' => $this->t("Terms belong to a vocabulary (e.g. Sequence "  .
          "Ontology) and are identified with a unique accession which is often  " .
          "numeric but may not be (e.g. gene accession is 0000704 in the Sequence " .
          "Ontology). Term IDs are prefixed with an ID Space (e.g. SO). The " .
          "ID Space and the accession will uniquely identify a term (e.g. SO:0000704).")
    ];

    $elements['field_term_fs']['field_term'] = [
      '#type' => 'table',
      '#header'=> $headers,
      '#rows' => $rows,
      '#empty' => $this->t('There is no term associated with this field.'),
      '#sticky' => False
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];

    $is_open = FALSE;
    $term = NULL;
    $idSpace = NULL;
    $vocabulary = NULL;
    $termIdSpace = $this->getSetting('termIdSpace');
    $termAccession = $this->getSetting('termAccession');
    $debug = $this->getSetting('debug');

    $elements['debug'] = [
      '#type' => 'checkbox',
      '#title' => 'Enable Debugging',
      '#description' => 'Enabling debugging on the field will print out a number of debugging messages both on screen and in the logs to help developers diagnose any problems which may be occuring.',
      '#default_value' => $debug,
    ];

    $default_vocabulary_term = '';
    // For Drupal ~10.2 our values are now in the subform
    $vocabulary_term = $form_state->getValue(['field_storage', 'subform', 'settings', 'field_term_fs', 'vocabulary_term'])
        ?? $form_state->getValue(['settings', 'field_term_fs', 'vocabulary_term']);
    if ($vocabulary_term) {
      $default_vocabulary_term = $vocabulary_term;
    }
    $first_pass = $form_state->getUserInput(['settings', 'field_term_fs', 'vocabulary_term'])?FALSE:TRUE;

    if (!$termIdSpace or !$termAccession) {
      if (!$default_vocabulary_term) {
        // Only display this message once
        if ($first_pass) {
          \Drupal::messenger()->addWarning(t("The field is missing an assigned controlled vocabulary term. Please set one"));
        }
      }
      $is_open = TRUE;
    }
    if ($termIdSpace) {
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($termIdSpace);
      if (!$idSpace) {
        \Drupal::messenger()->addWarning(t("The ID Space assigned to this field (@idSpace) cannot be found.",
            ['@idSpace' => $termIdSpace]));
        $is_open = TRUE;
      }
      if ($idSpace) {
        $term = $idSpace->getTerm($termAccession);
        if (!$term) {
          \Drupal::messenger()->addWarning(t("The term accession assigned to this field (@term) cannot be found.",
              ['@term' => $termIdSpace . ':' . $termAccession]));
          $is_open = TRUE;
        }
        else {
          $vocabulary = $term->getVocabularyObject();
          if (!$vocabulary) {
            \Drupal::messenger()->addWarning(t("The term assigned to this field (@term) does not specify a vocabulary.",
                ['@term' => $termIdSpace . ':' . $termAccession]));
            $is_open = TRUE;
          }
          $default_vocabulary_term = !$default_vocabulary_term ? ($term->getName() . ' (' . $term->getIdSpace() . ':' . $term->getAccession() . ')') : $default_vocabulary_term;
        }
      }
    }

    $elements['field_term_fs'] = [
      '#type' => 'details',
      '#title' => $this->t("Controlled Vocabulary Term"),
      '#description' => $this->t("All fields attached to a Tripal-based content " .
          "type must be associated with a controlled vocabulary term. " .
          "Use caution when changing the term. It should accurately represent " .
          "the type of data stored in this field.  Using terms that are developed ".
          "by the community (e.g. Sequence Ontology, etc.) ensures that the ".
          "data on your site is discoverable and interoperable."),
      '#open' => $is_open,
    ];

    $element_title = "Set the Term";
    if ($term and $idSpace and $vocabulary) {
      $this->buildVocabularyTermTable($elements, $term, $idSpace, $vocabulary);
      $element_title = "Change the Term";
    }

    $elements['field_term_fs']["vocabulary_term"] = [
      "#type" => "textfield",
      "#title" => $this->t($element_title),
      "#required" => TRUE,
      "#description" => $this->t("Enter a vocabulary term name. A set of matching " .
        "candidates will be provided to choose from. You may find the multiple matching terms " .
        "from different vocabularies. The full accession for each term is provided " .
        "to help choose. Only the top 10 best matches are shown at a time."),
      '#default_value' => $default_vocabulary_term,
      '#autocomplete_route_name' => 'tripal.cvterm_autocomplete',
      '#autocomplete_route_parameters' => array('count' => 10),
      '#element_validate' => [[static::class, 'fieldSettingsFormValidate']],
    ];

    return $elements + parent::fieldSettingsForm($form, $form_state);
  }

  /**
   * Form element validation handler
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   */
  public static function fieldSettingsFormValidate(array $form, FormStateInterface $form_state) {
    $settings = $form_state->getValue('settings');
    $term_str = $settings['field_term_fs']['vocabulary_term'];
    $matches = [];
    if (preg_match('/(.+?)\((.+?):(.+?)\)/', $term_str, $matches)) {
      $idSpace_name = $matches[2];
      $accession = $matches[3];
      $form_state->setValue(['settings', 'termIdSpace'], $idSpace_name);
      $form_state->setValue(['settings', 'termAccession'], $accession);
    }
    else {
      $form_state->setErrorByName('field_term_fs][vocabulary_term',
          'Please provide a valid term. It must have the ID space and accession in parentheses.');
    }
  }

  /**
   * Returns a placeholder properties array for fields where the
   * base table has not yet been set when manually adding a field.
   *
   * @param object $field_definition
   *   The field configuration object. This can be an instance of:
   *   \Drupal\field\Entity\FieldStorageConfig or
   *   \Drupal\field\Entity\FieldConfig
   */
  private static function placeholderProperties($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $record_id_term = 'SIO:000729';
    return([
      new IntStoragePropertyType($entity_type_id, 'placeholder', 'record_id', $record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
      ])
    ]);
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    $properties = [];
    $prop_types = get_called_class()::tripalTypes($field_definition) ?? self::placeholderProperties($field_definition);
    foreach ($prop_types as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $properties[$type->getKey()] = DataDefinition::create("integer");
      }
      else if ($type instanceof VarCharStoragePropertyType) {
        $properties[$type->getKey()] = DataDefinition::create("string");
      }
      else if ($type instanceof TextStoragePropertyType) {
        $properties[$type->getKey()] = DataDefinition::create("string");
      }
      else if ($type instanceof BoolStoragePropertyType) {
        $properties[$type->getKey()] = DataDefinition::create("boolean");
      }
      else {
        throw new RuntimeException('Unknown Tripal Property Type class "' . get_class($type) . '"');
      }
    }

    if (empty($properties)) {
      throw new RuntimeException("Cannot return empty array.");
    }

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    $schema = [];
    $prop_types = get_called_class()::tripalTypes($field_definition) ?? self::placeholderProperties($field_definition);
    foreach ($prop_types as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $column = [
          "type" => "int"
        ];
        $schema["columns"][$type->getKey()] = $column;
      }
      else if ($type instanceof VarCharStoragePropertyType) {
        $column = [
          "type" => "varchar",
          "length" => $type->getMaxCharacterSize()
        ];
        $schema["columns"][$type->getKey()] = $column;
      }
      else if ($type instanceof TextStoragePropertyType) {
        $column = [
          "type" => "text",
        ];
        $schema["columns"][$type->getKey()] = $column;
      }
      else if ($type instanceof BoolStoragePropertyType) {
        $column = [
          "type" => "int",
          "size" => "tiny",
          "pgsql_type" => "boolean",
        ];
        $schema["columns"][$type->getKey()] = $column;
      }
      else {
        throw new RuntimeException('Unknown Tripal Property Type class "' . get_class($type) . '"');
      }
    }

    if (empty($schema)) {
      throw new RuntimeException("Cannot return empty array.");
    }
    return $schema;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];
    $settings = $this->getSetting('storage_plugin_settings');

    // turn into selection
    $elements["storage_plugin_id"] = [
      "#type" => "textfield",
      "#title" => $this->t("Tripal Storage Plugin ID."),
      '#default_value' => $this->getSetting('storage_plugin_id'),
      "#required" => TRUE,
      "#description" => $this->t("The plugin ID of the storage backend."),
      "#disabled" => TRUE
    ];

    // Make a fieldset for each property setting.
    if (array_key_exists('property_settings', $settings)) {
      $property_settings = $settings['property_settings'];
      $property_elements = [];
      foreach ($property_settings as $key => $propset_values) {

        $prop_rows = [];
        foreach ($propset_values as $propkey => $propval) {
          if ($propval === FALSE) {
            $propval = "False";
          }
          if ($propval === TRUE) {
            $propval = "True";
          }
          $prop_rows[] = [
            [
              'data' => $propkey,
              'header' => TRUE,
              'width' => '20%'
            ],
            $propval
          ];
        }
        $prop_element = [
          '#type' => 'details',
          '#title' => $key,
          '#open' => False,
          'prop_settings_table' => [
            '#type' => 'table',
            '#header'=> [],
            '#rows' => $prop_rows,
            '#empty' => $this->t('There are no settings.'),
            '#sticky' => False
          ],
        ];
        $property_elements[$key] = $prop_element;
      }
      $renderer = \Drupal::service('renderer');
      $settings['property_settings'] = $renderer->render($property_elements);;
    }

    // Construct a table for the vocabulary information.
    $headers = ['Storage Property', 'Value'];
    $rows = [];
    foreach ($settings as $setting_name => $setting_value) {
      $rows[] = [
        [
          'data' => $setting_name,
          'header' => TRUE,
          'width' => '20%',
        ],
        $setting_value,
      ];
    }
    $elements['settings_fs'] = [
      '#type' => 'details',
      '#title' => $this->t("Storage Settings"),
      '#description' => $this->t("The following storage settings apply for this field."),
      '#open' => False,
    ];
    $elements['settings_fs']['table_label'] = [
      '#type' => 'item',
      '#title' => $this->t('Current Settings'),
    ];
    $elements['settings_fs']['settings_table'] = [
      '#type' => 'table',
      '#header'=> $headers,
      '#rows' => $rows,
      '#empty' => $this->t('There are no settings.'),
      '#sticky' => False
    ];
    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  /**
   * {@inheritdoc}
   */
  public function tripalStorageId() {
    return $this->getSetting("storage_plugin_id");
  }

  /**
   * {@inheritdoc}
   */
  public function tripalSave($field_item, $field_name, $prop_types, $prop_values, $entity) {
    $delta = $field_item->getName();
    foreach ($prop_values as $property) {
      $prop_key = $property->getKey();
      $value = $entity->get($field_name)->get($delta)->get($prop_key)->getValue();
      $property->setValue($value);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalLoad($field_item, $field_name, $prop_types, $prop_values, $entity) {
    $delta = $field_item->getName();
    foreach ($prop_values as $property) {
      $prop_key = $property->getKey();
      $entity->get($field_name)->get($delta)->get($prop_key)->setValue($property->getValue(), False);
    }
  }


  /**
   * {@inheritdoc}
   */
  public function tripalClear($field_item, $field_name, $prop_types, $prop_values, $entity) {
    $delta = $field_item->getName();

    foreach ($prop_values as $prop_value) {
      $prop_key = $prop_value->getKey();

      // Get the settings from the property type whose key matches this value.
      $settings = ['drupal_store' => FALSE];
      foreach ($prop_types as $prop_type) {
        if ($prop_type->getKey() == $prop_key) {
          $settings = $prop_type->getStorageSettings();
        }
      }

      // Keep properties that have caching enabled.
      if (array_key_exists('drupal_store', $settings) and $settings['drupal_store'] == TRUE) {
        continue;
      }
      // Clear all other properties.
      $entity->get($field_name)->get($delta)->get($prop_key)->setValue('', False);
    }
  }

  /**
   * Santizies a property key.
   *
   * Property keys are often controlled vocabulary IDs, which is the IdSpace
   * and accession separated by a colon. The colon is not supported by the
   * storage backend and must be converted to an underscore. This
   * function performs that task
   *
   * @param string $key
   *
   * @return string
   *   A santizied string.
   */
  public function sanitizeKey($key) {
    return preg_replace('/[^\w]/', '_', $key);
  }

  /**
   * Returns the settings from the form state
   *
   * Under Drupal ~10.2 the settings array is located in a subform.
   * This function will figure out where it is, and return it.
   *
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return array
   *   The settings array
   */
  public static function getFormStateSettings(FormStateInterface $form_state) {
    $settings = [];
    // First test Drupal ~10.2 location
    $settings = $form_state->getValue(['field_storage', 'subform', 'settings']);
    // Otherwise if Drupal <= 10.1
    if (!$settings) {
      $settings = $form_state->getValue('settings');
    }
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate($field_definition, $default_value = NULL) {

    // If we have a parent, then the field is attached to an entity. If it's just
    // an instance without a parent then the entity_id should stay null.
    $entity_id = NULL;
    $entity_type_id = 'tripal_entity';
    if ($this->getParent()) {
      $entity = $this->getEntity();
      $entity_type_id = $entity->getEntityTypeId();
      $entity_id = $entity->id();
    }

    $value_key = $this->mainPropertyName();

    // Get the list of property types defind by this field and then
    // return a corresponding array of property value objects.
    $field_class = get_class($this);
    $prop_types = $field_class::tripalTypes($field_definition);
    $prop_values = [];
    foreach ($prop_types as $prop_type) {
      $key = $prop_type->getKey();
      $prop_value = new StoragePropertyValue($entity_type_id, $field_class::$id,
          $key, $prop_type->getTerm()->getTermId(), $entity_id);

      if ($key == $value_key and $default_value) {
        $prop_value->setValue($default_value);
      }
      $prop_values[] = $prop_value;
    }
    return $prop_values;
  }

}
