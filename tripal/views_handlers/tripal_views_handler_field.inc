<?php
/**
 * @file
 *   Views field handler for basic TripalFields fields.
 */

/**
 * Views field handler for basic TripalFields fields.
 */
class tripal_views_handler_field extends views_handler_field {
  function query() {
    parent::query();
    // We need to add an alias to our TripalFields so that the
    // views can find the results.  With SQL it sets the alias for each
    // field and expects to find that alias in the results array.  Without
    // setting this alias Views can't find our results from our
    // tripal_views_query plugin.
    $this->field_alias = $this->real_field;
  }
}