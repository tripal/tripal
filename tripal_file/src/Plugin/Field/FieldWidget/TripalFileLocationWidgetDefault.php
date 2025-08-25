<?php

namespace Drupal\tripal_file\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\File\FileUrlGenerator;
use Drupal\Core\Form\FormStateInterface;
use Drupal\file\Entity\File;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

#[FieldWidget(
  id: 'tripal_file_location_widget_default',
  label: new TranslatableMarkup('Tripal File Location Widget'),
  description: new TranslatableMarkup('The default file location widget.'),
  field_types: [
    'tripal_file_location_type_default',
  ],
)]
class TripalFileLocationWidgetDefault extends ChadoWidgetBase {

  /**
   * Service to convert a uri to the local path.
   *
   * We need to access this service directly if the file
   * is not registered with Drupal. We will only load it
   * when it is necessary.
   */
  protected static ?FileUrlGenerator $file_url_generator = NULL;

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
#    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
#    $linker_fkey_column = $storage_settings['linker_fkey_column']
#      ?? $storage_settings['base_column'] ?? 'file_id';
#    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $fileloc_id = $item_vals['fileloc_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $uri = $item_vals['fileloc_uri'] ?? '';
    $rank = $item_vals['fileloc_rank'] ?? $delta;
    $md5checksum = $item_vals['fileloc_md5checksum'] ?? '';
    $size = $item_vals['fileloc_size'] ?? '';
    $filename = $item_vals['fileloc_filename'] ?? '';

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    // pass the field machine name through the form for massageFormValues()
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];

    $elements['fileloc_id'] = [
      '#type' => 'value',
      '#default_value' => $fileloc_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['fileloc_uri'] = [
      '#title' => $this->t('URI'),
      '#type' => 'textfield',
      '#default_value' => $uri,
      '#description' => $this->t('Enter a web URL or a local URI to a file on this site. This value is required.'),
      // It is required, but check this in validation.
      '#required' => FALSE,
      '#element_validate' => [[static::class, 'validateFilelocUri']],
    ];
    $elements['fileloc_filename'] = [
      '#title' => $this->t('File Name'),
      '#type' => 'textfield',
      '#default_value' => $filename,
      '#description' => $this->t('Enter an optional alternative name to display if there is no file name included in the URL.'),
      '#required' => FALSE,
    ];
    $elements['fileloc_md5checksum'] = [
      '#title' => $this->t('MD5 Checksum'),
      '#type' => 'textfield',
      '#default_value' => trim($md5checksum),
      '#description' => $this->t('If the file is local, this will be determined automatically.'),
      '#required' => FALSE,
      '#element_validate' => [[static::class, 'validateMd5checksum']],
    ];
    $elements['fileloc_size'] = [
      '#title' => $this->t('File Size'),
      '#type' => 'number',
      '#default_value' => $size,
      '#min' => 0,
      '#description' => $this->t('If the file is local, this will be determined automatically.'),
      '#required' => FALSE,
    ];
    // Mirror the delta as the rank value.
    $elements['fileloc_rank'] = [
      '#type' => 'value',
      '#default_value' => $delta,
    ];

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // lookup md5 checksum and size for local files.
    foreach ($values as $delta => $properties) {
      $uri = $properties['fileloc_uri'];
      if ($uri) {
        $scheme = parse_url($uri, PHP_URL_SCHEME);
        // Only local files are handled here.
        if ($scheme == 'public') {
          $file_path = self::GetLocalPath($uri);
          if ($file_path) {
            $file_size = filesize($file_path);
            $file_md5_checksum = md5_file($file_path);
            $values[$delta]['fileloc_size'] = $file_size;
            $values[$delta]['fileloc_md5checksum'] = $file_md5_checksum;
          }
        }
      }

    }
    return $values;
  }

  /**
   * Get the local filesystem full path for a public:// uri.
   *
   * @param string $uri
   *   The uri to look up, e.g. public://dir/filename.txt.
   * @return string
   *   If the uri is for a local filesystem file, returns the local
   *   filesystem path, or an empty string if the file does not exist.
   *   If the uri is external, the passed uri value is returned unchanged.
   */
  protected static function GetLocalPath(string $uri): string {
    $file_path = $uri;
    $scheme = parse_url($uri, PHP_URL_SCHEME);
    // Only evaluate for a local file.
    if ($scheme == 'public') {
      if (!self::$file_url_generator) {
        self::$file_url_generator = \Drupal::service('file_url_generator');
      }
      $file_path = \Drupal::root() . self::$file_url_generator->generateString($uri);
      if (!file_exists($file_path)) {
        $file_path = '';
      }
    }
    return $file_path;
  }

  /**
   * Form element validation handler for the uri field.
   *
   * This field is required in the database table, but we do not set
   * it as required in the form because doing so affects empty records.
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateFilelocUri($element, FormStateInterface $form_state) {
    // element_parents e.g. 0 => "project_relationship", 1 => 0, 2 => "related_record".
    $element_parents = $element['#parents'];
    $delta = $element_parents[1];
    $element_value = $element['#value'];
    $form_state_values = $form_state->getValues();
    $other_values = $form_state_values['file_location'][$delta];
    if ($element_value == '') {
      // If all other fields of the same delta are empty, then we allow
      // the empty value, as it is probably the last delta.
      $other_values_empty = (!$other_values['fileloc_filename']
                          && !$other_values['fileloc_md5checksum']
                          && !$other_values['fileloc_size']);
      if (!$other_values_empty) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('The URI field is a required value.'));
      }
    }
    else {
      if (!self::GetLocalPath($element_value)) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('The specified file does not exist in the local filesystem.'));
      }
    }
  }

  /**
   * Form element validation handler for the MD5 Checksum field.
   *
   * An MD5 checksum must have a length of exactly 32 and be valid
   * hexadecimal characters. An empty value is also allowed.
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateMd5checksum($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    // element_parents e.g. 0 => "project_relationship", 1 => 0, 2 => "related_record"
    $element_value = $element['#value'];
    if ($element_value != '' && $element_value != '                                ') {
      if (!preg_match('/^[0-9A-Fa-f]{32}$/', $element_value)) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('An MD5 checksum must be exactly 32 characters long and only contain valid hexadecimal characters, or be left blank.'));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return self::defaultSelectSettings() + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    return parent::settingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    return parent::settingsSummary();
  }
}
//@todo: edit and save not working
//@todo: empty deltas getting saved
