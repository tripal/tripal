<?php
/**
 * @file
 * Contains tripal_views_handler_area_action_links Area Handler
 */

/**
 * Provides an area for adding action links to a view.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_area_action_links extends views_handler_area {

  /**
   * {@inheritdoc}
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['link-1']['label-1'] = array('default' => '');
    $options['link-1']['path-1'] = array('default' => '');

    $options['link-2']['label-2'] = array('default' => '');
    $options['link-2']['path-2'] = array('default' => '');

    $options['link-3']['label-3'] = array('default' => '');
    $options['link-3']['path-3'] = array('default' => '');

    $options['link-4']['label-4'] = array('default' => '');
    $options['link-4']['path-4'] = array('default' => '');

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['label']['#default_value'] = 'Action Links';

    $form['link-1'] = array(
      '#type' => 'fieldset',
      '#title' => t('Link #1')
    );

    $form['link-1']['label-1'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#description' => t('The text that will be displayed as the link'),
      '#default_value' => $this->options['link-1']['label-1'],
    );

    $form['link-1']['path-1'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('The path that the link will link to'),
      '#default_value' => $this->options['link-1']['path-1']
    );

    $form['link-2'] = array(
      '#type' => 'fieldset',
      '#title' => t('Link #2')
    );

    $form['link-2']['label-2'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#description' => t('The text that will be displayed as the link'),
      '#default_value' => $this->options['link-2']['label-2'],
    );

    $form['link-2']['path-2'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('The path that the link will link to'),
      '#default_value' => $this->options['link-2']['path-2']
    );

    $form['link-3'] = array(
      '#type' => 'fieldset',
      '#title' => t('Link #3')
    );

    $form['link-3']['label-3'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#description' => t('The text that will be displayed as the link'),
      '#default_value' => $this->options['link-3']['label-3'],
    );

    $form['link-3']['path-3'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('The path that the link will link to'),
      '#default_value' => $this->options['link-3']['path-3']
    );

    $form['link-4'] = array(
      '#type' => 'fieldset',
      '#title' => t('Link #4')
    );

    $form['link-4']['label-4'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#description' => t('The text that will be displayed as the link'),
      '#default_value' => $this->options['link-4']['label-4'],
    );

    $form['link-4']['path-4'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('The path that the link will link to'),
      '#default_value' => $this->options['link-4']['path-4']
    );

  }

  /**
   * {@inheritdoc}
   */
  function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);
    ddl($form_state, 'form state in submit');
    $this->options['link-1']['label-1'] = $form_state['values']['options']['link-1']['label-1'];
    $this->options['link-1']['path-1'] = $form_state['values']['options']['link-1']['path-1'];

    $this->options['link-2']['label-2'] = $form_state['values']['options']['link-2']['label-2'];
    $this->options['link-2']['path-2'] = $form_state['values']['options']['link-2']['path-2'];

    $this->options['link-3']['label-3'] = $form_state['values']['options']['link-3']['label-3'];
    $this->options['link-3']['path-3'] = $form_state['values']['options']['link-3']['path-3'];

    $this->options['link-4']['label-4'] = $form_state['values']['options']['link-4']['label-4'];
    $this->options['link-4']['path-4'] = $form_state['values']['options']['link-4']['path-4'];
  }

  /**
   * {@inheritdoc}
   */
  function render($empty = FALSE) {
    if (!$empty || !empty($this->options['empty'])) {
      $output = '<ul class="action-links">';

      // First link
      if (!empty($this->options['link-1']['label-1']) AND !empty($this->options['link-1']['path-1'])) {
        $output .= '<li>' . l($this->options['link-1']['label-1'], $this->options['link-1']['path-1']) . '</li>';
      }

      // Second link
      if (!empty($this->options['link-2']['label-2']) AND !empty($this->options['link-2']['path-2'])) {
        $output .= '<li>' . l($this->options['link-2']['label-2'], $this->options['link-2']['path-2']) . '</li>';
      }

      // Third link
      if (!empty($this->options['link-3']['label-3']) AND !empty($this->options['link-3']['path-3'])) {
        $output .= '<li>' . l($this->options['link-3']['label-3'], $this->options['link-3']['path-3']) . '</li>';
      }

      // Fourth link
      if (!empty($this->options['link-4']['label-4']) AND !empty($this->options['link-4']['path-4'])) {
        $output .= '<li>' . l($this->options['link-4']['label-4'], $this->options['link-4']['path-4']) . '</li>';
      }
      $output .= '</ul>';
      return $output;
    }
    return '';
  }

}