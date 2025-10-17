<?php
//https://www.drupal.org/docs/creating-custom-modules/creating-custom-field-types-widgets-and-formatters/create-a-custom-field-widget

namespace Drupal\tripal_file\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\File\FileUrlGenerator;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\file\Entity\File;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of the default Chado file location widget.
 */
#[FieldWidget(
  id: 'chado_file_location_widget_default',
  label: new TranslatableMarkup('Chado File Location Widget'),
  description: new TranslatableMarkup('The default file location widget.'),
  field_types: [
    'chado_file_location_type_default',
  ],
)]
class ChadoFileLocationWidgetDefault extends ChadoWidgetBase {

  /**
   * Service to convert a uri to its corresponding local path.
   *
   * We need to access this service directly for any files that are
   * not registered with Drupal. We will only load this service when
   * it is necessary.
   *
   * @var Drupal\Core\File\FileUrlGenerator|null
   */
  protected static ?FileUrlGenerator $file_url_generator = NULL;

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $fileloc_id = $item_vals['fileloc_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $uri = $item_vals['fileloc_uri'] ?? '';
    $md5checksum = $item_vals['fileloc_md5checksum'] ?? '';
    $size = $item_vals['fileloc_size'] ?? '';
    $filename = $item_vals['fileloc_filename'] ?? '';

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    // Pass the field machine name through the form for massageFormValues().
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
// @todo issue with layout https://www.drupal.org/project/drupal/issues/3519949
    $elements['fileloc_uri'] = [
      '#title' => $this->t('URI'),
      '#type' => 'textfield',
      '#default_value' => $uri,
      '#description' => $this->t('Enter a web URL or a local URI to a file on this site.'),
      '#required' => FALSE,
      '#element_validate' => [[static::class, 'validateFilelocUri']],
    ];
    $elements['fileloc_upload'] = [
      '#type' => 'managed_file',
      '#title' => $this->t('Upload file'),
      '#description' => $this->t('An uploaded file is stored locally at the path specified in the URI field. Valid extensions are: ')
        . $valid_extensions,
      '#upload_location' => $this->getSetting('upload_location'),
      '#multiple' => FALSE,
      '#required' => FALSE,
      '#upload_validators' => [
        'FileExtension' => [
           'extensions' => $this->getSetting('valid_extensions'),
        ],
      ],
    ];
    $elements['fileloc_filename'] = [
      '#title' => $this->t('File Name'),
      '#type' => 'textfield',
      '#default_value' => $filename,
      '#description' => $this->t('Enter an optional alternative name to display if there is no file name included in the URL.'),
      '#required' => FALSE,
    ];
    $elements['fileloc_size'] = [
      '#title' => $this->t('File Size'),
      '#type' => 'number',
      '#default_value' => $size,
      '#min' => 0,
      '#description' => $this->t('If the file is local, this will be determined automatically.'),
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
    // Mirror the delta as the rank value.
    $elements['fileloc_rank'] = [
      '#type' => 'value',
      '#default_value' => $delta,
    ];

    // Save some initial values to allow later handling of the "Remove" button.
    $this->saveInitialValues($delta, $field_name, $fileloc_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    foreach (array_keys($values) as $delta) {
      // Handle uploaded file element.
      $this->massageFile($values, $delta, $form, $form_state);

      // Use the Drupal delta value as the chado rank.
      if ($values[$delta]['fileloc_uri'] ?? '') {
        $values[$delta]['fileloc_rank'] = $delta;
      }

      // Look up md5 checksum and size for local files.
      $uri = $values[$delta]['fileloc_uri'] ?? '';
      if ($uri) {
        // We can only lookup local files, ignore external files.
        $scheme = parse_url($uri, PHP_URL_SCHEME);
        if ($scheme == 'public') {
          $file_path = self::getLocalPath($uri);
          if ($file_path) {
            $file_size = filesize($file_path);
            $file_md5_checksum = md5_file($file_path);
            $values[$delta]['fileloc_size'] = $file_size;
            $values[$delta]['fileloc_md5checksum'] = $file_md5_checksum;
          }
        }

        // Extract a filename from the URI if one was not supplied.
        if (!$values[$delta]['fileloc_filename']) {
          // To allow parsing of a drupal uri like public://, add a fake host.
          $tmp_uri = preg_replace('/^public:\/\//', 'public://host/', $uri);
          $path = parse_url($tmp_uri, PHP_URL_PATH);
          if ($path) {
            $values[$delta]['fileloc_filename'] = basename($path);
          }
        }

      }
    }

    // Now we can do the standard massaging.
    $values = $this->genericSelectMassageFormValues('fileloc_id', $values);
    $values = $this->massagePropertyFormValues('fileloc_uri', $values, $form_state, NULL, 'fileloc_id');

    return $values;
  }

  /**
   * @todo
   *
   * This massage also gets called when a file is selected.
   */
  protected function massageFile(array &$values, int $delta, array $form, FormStateInterface $form_state) {
    $triggering_element = $form_state->getTriggeringElement()['#name'];
    if ($triggering_element == 'op' && $form_state->isValidationComplete()) {
      $managed_files = $form_state->getValue('file_location')[$delta]['fileloc_upload'] ?? [];
      $fileloc_uri = $values[$delta]['fileloc_uri'] ?? '';
      if (!empty($managed_files)) {
        // This is a loop, but we only support one file here.
        foreach ($managed_files as $file_id) {
          $file = File::load($file_id);
          $managed_file_uri = $file->getFileUri();
          if ($fileloc_uri && $fileloc_uri != $managed_file_uri) {
// @todo move file?
// \Drupal::service('file.repository')->move(?, ?);
// move(FileInterface $source, $destination, $replace): Moves a managed file to a new location and updates its database entry.
          }
          $values[$delta]['fileloc_uri'] = $managed_file_uri;

        }
      }
    }
  }

  /**
   * Get the local filesystem absolute path for a public:// uri.
   *
   * @param string $uri
   *   The uri to look up, e.g. 'public://dir/filename.txt'.
   *
   * @return string
   *   If the uri is for a local filesystem file, returns the local
   *   filesystem absolute path, or an empty string if the file does not exist.
   *   If the uri is external, the passed uri value is returned unchanged.
   */
  protected static function getLocalPath(string $uri): string {
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
   * Widget form element validation handler for the uri field.
   *
   * This field is required in the database table, but we do not set
   * the form field as required because doing so affects empty records.
   *
   * @param array $element
   *   The form element being validated.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return void
   *   No return value.
   */
  public static function validateFilelocUri(array $element, FormStateInterface $form_state): void {
    // Element_parents e.g. 0 => "file_location",
    // 1 => 0 (delta), 2 => "fileloc_uri".
    $element_parents = $element['#parents'];
    $delta = $element_parents[1];
    $element_value = $element['#value'];
    $form_state_values = $form_state->getValues();
    $other_values = $form_state_values['file_location'][$delta];
    // Need uri, but a managed file can generate the uri later.
    if ($element_value == '' && empty($other_values['fileloc_upload']['fids'])) {
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
    elseif ($element_value) {
      // Check that this is a well-formed uri using the same code
      // as the formatter.
      $scheme = parse_url($element_value, PHP_URL_SCHEME);
      if (!$scheme) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('The specified URI is not valid.'));
      }
      // Validates that public:// files exist.
      elseif (!self::getLocalPath($element_value)) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('The specified file does not exist in the local filesystem.'));
      }
    }
  }

  /**
   * Widget form element validation handler for the MD5 Checksum field.
   *
   * An MD5 checksum must have a length of exactly 32 and be valid
   * hexadecimal characters. An empty value is also allowed.
   *
   * @param array $element
   *   The form element being validated.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return void
   *   No return value.
   */
  public static function validateMd5checksum(array $element, FormStateInterface $form_state): void {
    $element_parents = $element['#parents'];
    // Element_parents e.g. 0 => "file_location",
    // 1 => 0, 2 => "fileloc_md5checksum".
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
    return [
      'valid_extensions' => 'gif jpg png bz gz zip txt csv tsv xls xlsx doc docx odf fna fa fasta faa gff gtf gff3 vcf bcf bam bai',
      'upload_location' => 'public://',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];
    $elements['valid_extensions'] = [
      '#title' => $this->t('Valid Extensions'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('valid_extensions'),
      '#description' => $this->t('Enter a list of file extensions that are allowed to be uploaded to this site.'),
      '#required' => FALSE,
    ];
    $elements['upload_location'] = [
      '#title' => $this->t('Upload Location'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('upload_location'),
      '#description' => $this->t('Enter a local path where files will be uploaded. It must start with "public://".'),
      '#required' => TRUE,
      '#element_validate' => [[static::class, 'validateSettingsUploadLocation']],
    ];
    return $elements + parent::settingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];

    $valid_extensions = $this->getSetting('valid_extensions');
    $upload_location = $this->getSetting('upload_location');

    $summary[] = $this->t("Valid Extensions: @valid_extensions", ['@valid_extensions' => $valid_extensions]);
    $summary[] = $this->t("Upload Location: @upload_location", ['@upload_location' => $upload_location]);

    return $summary + parent::settingsSummary();
  }

  /**
   * Settings form element validation handler for the upload location.
   *
   * @param array $element
   *   The form element being validated.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object of the settings form.
   *
   * @return void
   *   No return value.
   */
  public static function validateSettingsUploadLocation(array $element, FormStateInterface $form_state): void {
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];
    if (!preg_match('/^public:\/\//', $element_value)) {
      $form_state->setErrorByName(implode('][', $element_parents),
        t('The upload location must start with "public://"'));
    }
    if (!self::getLocalPath($element_value)) {
      $form_state->setErrorByName(implode('][', $element_parents),
        t('The specified path does not exist in the local filesystem, you need to create it first.'));
    }
  }

}

