<?php
/**
 * @file
 *   Views field handler for basic TripalFields fields.
 */

/**
 * Views field handler for basic TripalFields fields.
 */
class tripal_views_handler_field_image extends tripal_views_handler_field {

  /**
   * Overrides option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['image_width'] = ['default' => ''];
    $options['image_height'] = ['default' => ''];

    return $options;
  }

  /**
   * Overrides options_form().
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['image_width'] = [
      '#type' => 'textfield',
      '#title' => t('Image Width'),
      '#description' => t('Please provide the width for display of images. Leave blank to accept the width of the image.'),
      '#size' => '10',
      '#default_value' => $this->options['image_width'],
    ];
    $form['image_height'] = [
      '#type' => 'textfield',
      '#title' => t('Image Height'),
      '#description' => t('Please provide the height for display of images. Leave blank to accept the width of the image.'),
      '#size' => '10',
      '#default_value' => $this->options['image_height'],
    ];
  }

  /**
   * Render the field.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  function render($values) {
    $value = $this->get_value($values);
    // Handle single value fields:
    if (count($value == 1)) {
      $fid = $value[0]['fid'];
      if ($fid) {
        $file = file_load($fid);
        $url = file_create_url($file->uri);
        $width = '';
        $height = '';
        if ($this->options['image_width']) {
          $width = ' width = "' . $this->options['image_width'] . '"';
        }
        if ($this->options['image_height']) {
          $height = ' height = "' . $this->options['image_height'] . '"';
        }
        return '<img src="' . $url . '"' . $width . $height . ' border="0">';

      }
    }
    return '';
  }
}