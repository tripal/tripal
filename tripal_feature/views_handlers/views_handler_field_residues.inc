<?php
/**
 * @file
 * Contains views_handler_field_residues field handler
 */

/**
 * A handler to provide proper displays for sequence residues.
 *
 * @ingroup tripal_feature
 * @ingroup views_field_handlers
 */
class views_handler_field_residues extends views_handler_field {

  /**
   * {@inheritdoc}
   */
  function construct() {
    parent::construct();
      $this->additional_fields['residues'] = array('table' => 'feature', 'field' => 'residues');
  }

  /**
   * {@inheritdoc}
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * {@inheritdoc}
   */
  function render($values) {
    // add a <br> every 100 bp
    return ereg_replace("(.{100})", "\\1<br />", $values->feature_residues);

  }
}
