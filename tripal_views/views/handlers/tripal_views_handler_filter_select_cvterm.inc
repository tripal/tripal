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

      $where = '';
      if ($this->options['exposed']) {
        // build a where clause that will filter the list in the drop box
        // using fields that are not exposed and that are for the table
        // from whcih the values in the drop box will be slected and
        // we only want to use non-exposed fields because these are not
        // available to the user to edit--they're fixed.
        $filters = (is_array($this->view->filter)) ? $this->view->filter : array();
        foreach ($filters as $filter_name => $details) {
           // we only want to inclue non-exposed filters
           if ($details->options['exposed'] == FALSE) {
              $value = $details->value;
              if (is_array($details->value) AND isset($details->value['value'])) {
                $value = $details->value['value'];
              }

              $field = $details->field;
              if (($this->table == $this->view->base_table) AND ($details->field == 'type_id')) {
                $field = 'cvterm_id';
              }

              if (is_array($value)) {
                // we only want to filter on the table we're getting the list from
                if (strcmp($details->table, $this->table)==0 AND !empty($value)) {
                  $where .= "$field IN (" . implode(', ', $value) . ')';
                  $where .= ' AND ';
                }
              }
              else {
                // we only want to filter on the table we're getting the list from
                if (strcmp($details->table, 'cvterm')==0 AND !empty($value)) {
                  $where .= "$field $details->operator " . $value;
                  $where .= ' AND ';
                }
              }
           }
        }
        if ($where) {
          $where = ' AND ' . $where;
          $where = substr($where, 0, -5); # remove the final ' AND '
        }
      }

      // @coder-ignore: non-drupal schema therefore table prefixing does not apply
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT cvterm_id, name FROM {cvterm} WHERE cvterm_id IN (SELECT distinct(" . $this->field . ") FROM {" . $this->table . "}) " . $where . ' ORDER BY cvterm.name ASC';
      $resource = chado_query($sql);
      $cvterms = array();

      if ($this->options['select_optional']) {
        $cvterms['All'] = '- Any -';
      }

      foreach ($resource as $r) {
        $cvterms[$r->cvterm_id] = $r->name;
      }
    }
    //sort cvterms by name (case insensitive)
    natcasesort($cvterms);

    return $cvterms;

  }

  /**
   * {@inheritdoc}
   */
  function options_definition() {
    return parent::option_definition();
  }

  /**
   * {@inheritdoc}
   */
  function expose_form(&$form, &$form_state) {
    parent::expose_form($form, $form_state);
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  function expose_submit($form, &$form_state) {
    parent::expose_submit($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  function expose_options() {
    parent::expose_options();
  }

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
  }

}
