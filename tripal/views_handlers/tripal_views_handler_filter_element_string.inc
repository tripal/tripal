<?php

class tripal_views_handler_filter_element_string extends tripal_views_handler_filter_string {

  function init(&$view, &$options) {
    parent::init($view, $options);

    // Fix identifier names that don't match what will be in the form state.
    $this->options['expose']['identifier'] = preg_replace('/\./', '_', $this->options['expose']['identifier']);
    $this->options['expose']['operator'] = preg_replace('/\./', '_', $this->options['expose']['operator']);
    $this->options['expose']['operator_id'] = preg_replace('/\./', '_', $this->options['expose']['operator_id']);
  }
}