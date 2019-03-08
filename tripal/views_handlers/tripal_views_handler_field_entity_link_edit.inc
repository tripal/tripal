<?php


class tripal_views_handler_field_entity_link_edit extends tripal_views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = ['default' => '', 'translatable' => TRUE];
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = [
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    ];
    parent::options_form($form, $form_state);

    // The path is set by render_link function so don't allow to set it.
    $form['alter']['path'] = ['#access' => FALSE];
    $form['alter']['external'] = ['#access' => FALSE];
  }

  function render($values) {
    if ($entity = $this->get_value($values)) {
      return $this->render_link($entity, $values);
    }
  }

  function render_link($entity, $values) {

    if (entity_access('edit', 'TripalEntity', $entity)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'bio_data/' . $entity->id . '/edit';
      $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
      return $text;
    }
  }
}
