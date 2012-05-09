<?php

/**
 * A handler to provide proper displays for booleans.
 *
 * Allows for display of true/false, yes/no, on/off.
 *
 * Definition terms:
 *   - output formats: An array where the first entry is displayed on boolean false
 *      and the second is displayed on boolean true. An example for sticky is:
 *      @code
 *      'output formats' => array(
 *        'sticky' => array('', t('Sticky')),
 *      ),
 *      @endcode
 *
 * @ingroup views_field_handlers
 * @ingroup tripal_core
 */
class views_handler_field_chado_tf_boolean extends views_handler_field_boolean {

  // Changes the rendered value: t='Yes' & f='No'
  // Rendered value depends on type of value chosen in options
  function render($values) {
    $value = $values->{$this->field_alias};
    if (!empty($this->options['not'])) {
      $value = !$value;
    }

    if (isset($this->formats[$this->options['type']])) {
      return preg_match('/t/',$value) ? $this->formats[$this->options['type']][0] : $this->formats[$this->options['type']][1];
    }
    else {
      return preg_match('/t/', $value) ? $this->formats['yes-no'][0] : $this->formats['yes-no'][1];
    }
  }
  
}
