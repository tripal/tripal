<?php

class schema__alternate_name_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Synonyms';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('schema__alternate_name');

  // The list of default settings for this formatter.
  public static $default_settings = array();
  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $chado_table = $this->field['settings']['chado_table'];
    foreach ($items as $delta => $item) {
      if (array_key_exists('chado-' . $chado_table . '__synonym_id', $item) and
          $item['chado-' . $chado_table . '__synonym_id']) {
            $synonym = chado_generate_var('synonym', array('synonym_id' => $item['chado-' . $chado_table . '__synonym_id']));
            $name = $synonym->name;
            if ($synonym->type_id->name != 'exact') {
              $name .= ' (<i>' . $synonym->type_id->name . '</i>)';
            }
            $element[$delta] = array(
              '#type' => 'markup',
              '#markup' => $name,
            );
          }
    }
  }
}
