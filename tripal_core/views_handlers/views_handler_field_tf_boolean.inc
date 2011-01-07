<?php
// $Id: views_handler_field_boolean.inc,v 1.3.2.1 2010/03/25 01:14:22 merlinofchaos Exp $

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
 */
class views_handler_field_tf extends views_handler_field {

  function render($values) {
    $value = $values->{$this->field_alias};
    
    if (preg_match('/^t$/', $value)) {
      return 'Yes';
    } else {
      return 'No';
    }
  }
}
