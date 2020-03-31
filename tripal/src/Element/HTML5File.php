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
   *
   */
  public static function processHTML5File(&$element, FormStateInterface $form_state, &$form) {
    $module = $element['#usage_module'] ?? 'tripal';
    $type = $element['#usage_id'] . '-' . $element['#usage_type'] . '-' . $module;
    $type_var_name = 'uploader_' . $element['#usage_id'] . '_' . $element['#usage_type'] . '_' . $module;
    $name = $element['#name'];
    $name = preg_replace('/[^\w]/', '_', $name);
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

    $element['html5_file_table_key'] = [
      '#type' => 'hidden',
      '#value' => $type,
      '#attributes' => [
        'class' => [
          'tripal-html5-file-upload-table-key',
        ],
      ],
    ];
    $element['html5_file_table'] = [
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

    $element['html5_file_submit'] = [
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
   *
   */
  public static function validateHTML5File(&$element, FormStateInterface $form_state, $form) {
    // TODO.
    /*
      $is_required = $element['#required'];
      $name = $element['#name'];
      $name = preg_replace('/[^\w]/', '_', $name);
      $fid = NULL;
      if (is_array($element['#value']) and array_key_exists($name, $element['#value'])) {
        $fid = $element['#value'][$name];
      }

      // TODO: the fid should just be the $element['#value'] why isn't this
      // working given the tripal_html5_file_value function below.

      if ($is_required and !$fid) {
        form_error($element, t('A file must be provided.'));
      }
    */
  }

  /**
   * {@inheritdoc}
   */
  public static function valueCallback(&$element, $input = FALSE, FormStateInterface $form_state) {
    if ($input) {
      if (is_array($input)) {
        $name = $element['#name'];
        $name = preg_replace('/[^\w]/', '_', $name);
        return $input['#name'];
      }
      return $input;
    }
  }

  /**
   *
   */
  public static function preRenderHTML5File($element) {
    // TODO.
    $element['#attributes']['type'] = 'html5file';
    Element::setAttributes($element, array(
      'id',
      'name',
      'size',
    ));
    static::setAttributes($element, array(
      'js-form-file',
      'form-file',
    ));
    return $element;
  }

}
