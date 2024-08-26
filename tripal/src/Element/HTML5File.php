<?php

namespace Drupal\tripal\Element;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element\FormElement;
use Drupal\Core\Render\Element;

/**
 * Provides an HTML5 file upload form element.
 *
 * @FormElement("html5_file")
 */
class HTML5File extends FormElement {

  /**
   * {@inheritdoc}
   */
  public function getInfo() {
    $class = get_class($this);
$caller = debug_backtrace()[1]['function']; dpm($class, "CP02 getInfo called by $caller, class"); //@@@
    return [
      '#input' => TRUE,
      '#size' => 60,
      '#process' => [
        [$class, 'processHTML5File'],
      ],
      '#element_validate' => [
        [$class, 'validateHTML5File'],
      ],
      '#pre_render' => [
        [$class, 'preRenderHTML5File'],
      ],
    ];
  }

  /**
   * Process a form element before rendering.
   *
   * @param array $element
   *   The element being processed.
   * @param FormStateInterface $form_state
   *   The state of the form being processed.
   * @param array $form
   *   The form being processed.
   *
   * @return array
   *   The processed element.
   */
  public static function processHTML5File(array &$element, FormStateInterface $form_state, array &$form) {
    $module = $element['#usage_module'] ?? 'tripal';
    $type = $element['#usage_id'] . '-' . $element['#usage_type'] . '-' . $module;
    $type_var_name = 'uploader_' . $element['#usage_id'] . '_' . $element['#usage_type'] . '_' . $module;
    $name = HTML5File::getBaseKey($element);
dpm($name, "CP01 name"); //@@@
    $allowed_types = $element['#allowed_types'] ?? [];
    $cardinality = $element['#cardinality'] ?? 1;
    $paired = $element['#paired'] ?? FALSE;

    $headers = [
      ['data' => 'File'],
      [
        'data' => 'Size',
        'width' => '10%',
      ],
      [
        'data' => 'Upload Progress',
        'width' => '20%',
      ],
      [
        'data' => 'Action',
        'width' => '10%',
      ],
    ];
    if ($paired) {
      $headers = [
        ['data' => 'File #1'],
        [
          'data' => 'Size',
          'width' => '10%',
        ],
        [
          'data' => 'Upload Progress',
          'width' => '20%',
        ],
        [
          'data' => 'Action',
          'width' => '10%',
        ],
        ['data' => 'File #2'],
        [
          'data' => 'Size',
          'width' => '10%',
        ],
        [
          'data' => 'Upload Progress',
          'width' => '20%',
        ],
        [
          'data' => 'Action',
          'width' => '10%',
        ],
      ];
    }

    $rows = [];

    $element[$name . '_table_key'] = [
      '#type' => 'hidden',
      '#value' => $type,
      '#attributes' => [
        'class' => [
          'tripal-html5-file-upload-table-key',
        ],
      ],
    ];
    $element[$name . '_table'] = [
      '#type' => 'table',
      '#header' => $headers,
      '#rows' => $rows,
      '#title' => $element['#title'] ?? 'File Upload',
      '#description' => $element['#description'],
      '#attributes'  => [
        'class' => [
          'tripal-html5-file-upload-table',
        ],
        'id' => 'tripal-html5-file-upload-table-' . $type,
      ],
      '#empty' => t('There are currently no files.'),
      '#sticky' => TRUE,
    ];

    $element[$name] = [
      '#type' => 'hidden',
      '#attributes' => [
        'id' => 'tripal-html5-upload-fid-' . $type,
      ],
      '#default_value' => $element['#value'],
    ];

    $element[$name . '_submit'] = [
      '#type' => 'submit',
      '#value' => 'Upload File',
      '#name' => 'tripal_html5_file_upload_submit-' . $type,
      '#attributes' => [
        'id' => 'tripal-html5-file-upload-submit-' . $type,
        'onclick' => 'return (false)',
      ],
    ];

    $categories = [$element['#usage_id'] . '-' . $element['#usage_type']];
    if ($paired) {
      $categories = [
        $element['#usage_id'] . '-' . $element['#usage_type'] . '-f',
        $element['#usage_id'] . '-' . $element['#usage_type'] . '-r',
      ];
    }

    $uploader_settings = [
      'table_id' => '#tripal-html5-file-upload-table-' . $type,
      'submit_id' => '#tripal-html5-file-upload-submit-' . $type,
      'category' => $categories,
      'cardinality' => $cardinality,
      'target_id' => 'tripal-html5-upload-fid-' . $type,
      'module' => $module,
      'allowed_types' => $allowed_types,
    ];

    $form['#attached']['drupalSettings']['tripal'][$type_var_name] = $uploader_settings;
    $form['#attached']['library'][] = 'tripal/uploader';
    $form['#attached']['library'][] = 'tripal/upload-file';
    $form['#attached']['library'][] = 'tripal/tripal-file';

    return $element;
  }

  /**
   * Ensures that the input to the element is valid.
   *
   * @param array $element
   *   The element being validated.
   * @param FormStateInterface $form_state
   *   The state of the form being validated.
   * @param array $form
   *   The form being validated.
   */
  public static function validateHTML5File(array &$element, FormStateInterface &$form_state, array $form) {
    $is_required = $element['#required'];
dpm($is_required, "CP11 is_required"); //@@@
    $fid = $element['#value'] ?? NULL;

    if ($is_required and !$fid) {
      $form_state->setError($element, t('A file must be provided'));
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function valueCallback(&$element, $input, FormStateInterface $form_state) {
dpm($input, "CP21 valueCallback"); //@@@
    if ($input) {
      if (is_array($input)) {
        $name = HTML5File::getBaseKey($element);
        return $input[$name];
      }
      return $input;
    }
  }

  /**
   * Alter the element immediately before rendering.
   *
   * @param array $element
   *   The element being altered.
   *
   * @return array
   *   The altered element.
   */
  public static function preRenderHTML5File(array $element) {
dpm("CP22 preRenderHTML5File");//@@@
    $element['#attributes']['type'] = 'html5file';
    return $element;
  }

  /**
   * Returns the base key to be used by process and valueCallback functions.
   *
   * @param array $element
   *   The element we want the base key of.
   *
   * @return string
   *   The base key.
   */
  protected static function getBaseKey(array $element) {
dpm(end($element['#array_parents']), "CP23 getBaseKey"); //@@@
    return end($element['#array_parents']);
  }

}
