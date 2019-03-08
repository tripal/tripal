<?php

/**
 * @file
 * Definition of views_handler_filter_string.
 */

/**
 * Basic textfield filter to handle string filtering commands
 * including equality, like, not like, etc.
 *
 * @ingroup views_filter_handlers
 */
class tripal_views_handler_filter_entity_string extends views_handler_filter {

  // exposed filter options
  var $always_multiple = TRUE;

  function option_definition() {
    $options = parent::option_definition();

    $options['expose']['contains']['required'] = [
      'default' => FALSE,
      'bool' => TRUE,
    ];

    return $options;
  }

  /**
   * This kind of construct makes it relatively easy for a child class
   * to add or remove functionality by overriding this function and
   * adding/removing items from this array.
   */
  function operators() {
    // The operators keys must match the ones used in the TripalFieldQuery
    $operators = [
      '=' => [
        'title' => t('Is equal to'),
        'short' => t('='),
        'method' => 'op_equal',
        'values' => 1,
      ],
      '<>' => [
        'title' => t('Is not equal to'),
        'short' => t('!='),
        'method' => 'op_equal',
        'values' => 1,
      ],
      'CONTAINS' => [
        'title' => t('Contains'),
        'short' => t('contains'),
        'method' => 'op_contains',
        'values' => 1,
      ],
      'STARTS WITH' => [
        'title' => t('Starts with'),
        'short' => t('begins'),
        'method' => 'op_starts',
        'values' => 1,
      ],
      'NOT STARTS' => [
        'title' => t('Does not start with'),
        'short' => t('not_begins'),
        'method' => 'op_not_starts',
        'values' => 1,
      ],
      'ENDS WITH' => [
        'title' => t('Ends with'),
        'short' => t('ends'),
        'method' => 'op_ends',
        'values' => 1,
      ],
      'NOT ENDS' => [
        'title' => t('Does not end with'),
        'short' => t('not_ends'),
        'method' => 'op_not_ends',
        'values' => 1,
      ],
      'NOT' => [
        'title' => t('Does not contain'),
        'short' => t('!has'),
        'method' => 'op_not',
        'values' => 1,
      ],
    ];
    // if the definition allows for the empty operator, add it.
    if (!empty($this->definition['allow empty'])) {
      $operators += [
        'empty' => [
          'title' => t('Is empty (NULL)'),
          'method' => 'op_empty',
          'short' => t('empty'),
          'values' => 0,
        ],
        'not empty' => [
          'title' => t('Is not empty (NOT NULL)'),
          'method' => 'op_empty',
          'short' => t('not empty'),
          'values' => 0,
        ],
      ];
    }
    // Add regexp support for MySQL.
    if (Database::getConnection()->databaseType() == 'mysql') {
      $operators += [
        'regular_expression' => [
          'title' => t('Regular expression'),
          'short' => t('regex'),
          'method' => 'op_regex',
          'values' => 1,
        ],
      ];
    }

    return $operators;
  }

  /**
   * Build strings from the operators() for 'select' options
   */
  function operator_options($which = 'title') {
    $options = [];
    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }

    return $options;
  }

  function admin_summary() {
    if ($this->is_a_group()) {
      return t('grouped');
    }
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    $options = $this->operator_options('short');
    $output = '';
    if (!empty($options[$this->operator])) {
      $output = check_plain($options[$this->operator]);
    }
    if (in_array($this->operator, $this->operator_values(1))) {
      $output .= ' ' . check_plain($this->value);
    }
    return $output;
  }

  function operator_values($values = 1) {
    $options = [];
    foreach ($this->operators() as $id => $info) {
      if (isset($info['values']) && $info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }

  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
    // We have to make some choices when creating this as an exposed
    // filter form. For example, if the operator is locked and thus
    // not rendered, we can't render dependencies; instead we only
    // render the form items we need.
    $which = 'all';
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];

      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator_id'])) {
        // exposed and locked.
        $which = in_array($this->operator, $this->operator_values(1)) ? 'value' : 'none';
      }
      else {
        $source = 'edit-' . drupal_html_id($this->options['expose']['operator_id']);
      }
    }

    if ($which == 'all' || $which == 'value') {
      $form['value'] = [
        '#type' => 'textfield',
        '#title' => t('Value'),
        '#size' => 30,
        '#default_value' => $this->value,
      ];
      if (!empty($form_state['exposed']) && !isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }

      if ($which == 'all') {
        $form['value'] += [
          '#dependency' => [$source => $this->operator_values(1)],
        ];
      }
    }

    if (!isset($form['value'])) {
      // Ensure there is something in the 'value'.
      $form['value'] = [
        '#type' => 'value',
        '#value' => NULL,
      ];
    }
  }

  function operator() {
    return $this->operator == '=' ? 'LIKE' : 'NOT LIKE';
  }

  /**
   * Add this filter to the query.
   *
   * Due to the nature of fapi, the value and the operator have an unintended
   * level of indirection. You will find them in $this->operator
   * and $this->value respectively.
   */
  function query() {
  }

  function op_equal($field) {
  }

  function op_contains($field) {
  }

  function op_starts($field) {
  }

  function op_not_starts($field) {
  }

  function op_ends($field) {
  }

  function op_not_ends($field) {
  }

  function op_not($field) {
  }

  function op_regex($field) {
  }

  function op_empty($field) {
  }
}
