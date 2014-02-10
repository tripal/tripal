<?php
/**
 * @file
 * Contains tripal_views_handler_filter_select_cvterm
 */

/**
 * This Handler provides a select list for the type field
 *
 *  NOTE: This handler only works when applied to the type_id field in the base_table of
 *  this view.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_select_cvterm extends tripal_views_handler_filter_select_string {

  /**
   * Provide the options used in the select list.
   * Override this function in extended handlers to easily change option list.
   *
   * @return
   *   An array of options where the key is the value of this field in the database
   */
  function get_select_options() {

    if (isset($this->options['show_all'])) {
      $cv_id = variable_get('chado_' . $this->view->base_table . '_cv', NULL);
      if ($cv_id) {
        $results = chado_select_record('cvterm', array('cvterm_id', 'name'), array('cv_id' => $cv_id));
        if (empty($results)) {
          $results = array();
        }
        foreach ($results as $c) {
          $cvterms[$c->cvterm_id] = $c->name;
        }
      }
      else {
        //get a list of cvs currently used
        if ($this->view->base_table == 'cvterm') {
          $sql = 'SELECT distinct(cv.cv_id) FROM chado.' . $this->view->base_table
            .' LEFT JOIN chado.cv cv ON cv.cv_id=cvterm.cv_id';
        }
        else {
          $sql = 'SELECT distinct(cv.cv_id) FROM chado.' . $this->view->base_table
            .' LEFT JOIN chado.cvterm cvterm ON cvterm.cvterm_id=' . $this->view->base_table . '.type_id '
            .'LEFT JOIN chado.cv cv ON cv.cv_id=cvterm.cv_id';
        }
        // D7 TODO: Check DBTNG changes work
        $resource = chado_query($sql);
        $cvterms = array();
        foreach ($resource as $r) {
          $results = chado_select_record('cvterm', array('cvterm_id', 'name'), array('cv_id' => $r->cv_id));
          if (empty($results)) {
            $results = array();
          }
          foreach ($results as $c) {
            $cvterms[$c->cvterm_id] = $c->name;
          }
        }
      }// end of if variable not defined

    }
    else {
      // @coder-ignore: non-drupal schema therefore table prefixing does not apply
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT cvterm_id, name FROM {cvterm} WHERE cvterm_id IN (SELECT distinct(" . $this->field . ") FROM {" . $this->table . "})";
      $resource = chado_query($sql);
      $cvterms = array();
      foreach ($resource as $r) {
        $cvterms[$r->cvterm_id] = $r->name;
      }
    }
    //sort cvterms by name (case insensitive)
    natcasesort($cvterms);

    if ($this->options['expose']['select_optional']) {
      $options['All'] = '--Any--';
    }

    return $options + $cvterms;

  }

}
