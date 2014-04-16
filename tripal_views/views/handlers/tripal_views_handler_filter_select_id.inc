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
class tripal_views_handler_filter_select_id extends tripal_views_handler_filter_select_string {

  /**
   * {@inheritdoc}
   */
  function init(&$view, &$options) {
    parent::init($view, $options);

    if (preg_match('/(\w+)_id/',$this->field,$matches)) {
      $this->parent_table = $matches[1];
    }
  }

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

      $where_clauses = $this->get_select_option_where();
      $where = '';
      if (!empty($where_clauses)) {
        $where = ' AND ' . implode(' AND ', $where_clauses);
      }

      $sql = "SELECT PARENT.%field as id, PARENT.name as name
              FROM {%parent_table} PARENT
                LEFT JOIN {%table} CHILD ON CHILD.%field = PARENT.%field
              WHERE CHILD.%field IS NOT NULL $where
              GROUP BY PARENT.%field
              ORDER BY name";
      $sql = str_replace(array('%field', '%table','%parent_table'),array($this->field, $this->table, $this->parent_table), $sql);
      $resource = chado_query($sql);
      $options = array();

      if ($this->options['select_optional']) {
        $options['All'] = '- Any -';
      }

      foreach ($resource as $r) {
        $options[$r->id] = $r->name;
      }
    }
    //sort options by name (case insensitive)
    natcasesort($options);

    return $options;

  }

  /**
   * For the SQL generating the options, determine the WHERE clauses
   *
   * @return
   *   An array of full qualified where clauses (ie: table.myfield = 'fred')
   */
  function get_select_option_where() {
    $where = parent::get_select_option_where();

    return $where;
  }

  /**
   * {@inheritdoc}
   */
  function option_definition() {
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
