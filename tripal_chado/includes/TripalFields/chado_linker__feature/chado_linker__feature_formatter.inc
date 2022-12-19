<?php

class chado_linker__feature_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Features';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__feature'];


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
    // Get the settings
    $settings = $display['settings'];

    $type_term = chado_get_semweb_term('feature', 'type_id');
    $name_term = chado_get_semweb_term('feature', 'name');
    $uniquename_term = chado_get_semweb_term('feature', 'uniquename');
    $summary_term = 'NCIT:C25463'; // ncit:Count

    $headers = ['Name', 'Uniquename', 'Feature Type'];
    $rows = [];

    // When there are too many records to display, switch to
    // a summary view.
    if (array_key_exists($summary_term, $items[0]['value'])) {
      $content = $items[0]['value'][$summary_term];
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }

    // Normal output is a table of features
    else {
      // This configurable cell styling implements a maximum height
      // for the content, if taller there is a scrollbar.
      $max_height = variable_get('chado_linker_max_height', 200);
      $cell_begin = '<div class="chado-linker--cell" style="max-height:' . $max_height . 'px">';
      $cell_end = '</div>';

      foreach ($items as $delta => $item) {
        $feature = $item['value'];
        if (!$feature) {
          continue;
        }

        // Get the field values
        $feature_name = $feature[$name_term];
        $uniquename = $feature[$uniquename_term];
        $type = $feature[$type_term];

        // Add a link if there is an entity.
        if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
          list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
          $feature_name = l($feature_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
        }

        // This cell styling implements a maximum height for the
        // content, if taller there is a scrollbar.
        $rows[] = [
          $cell_begin . $feature_name . $cell_end,
          $cell_begin . $uniquename . $cell_end,
          $cell_begin . $type . $cell_end,
        ];
      }

      // Build the pager
      $items_per_page = array_key_exists('items_per_page', $this->instance['settings']) ? $this->instance['settings']['items_per_page'] : 10;
      $total_records = count($rows);
      $total_pages = (int) ($total_records / $items_per_page) + 1;
      $pelement = 0;
      $current_page = pager_default_initialize($total_records, $items_per_page, $pelement);
      $pager = theme('pager', [
        'tags' => [],
        'element' => $pelement,
        'parameters' => [],
        'quantity' => $total_pages,
      ]);
      $pager = $this->ajaxifyPager($pager, $entity);
      $page_items = array_chunk($rows, $items_per_page);
      $caption = '';
      $content = theme_table([
        'header' => $headers,
        'rows' => count($rows) > 0 ? $page_items[$current_page] : [],
        'attributes' => [
          'class' => 'tripal-data-table',
        ],
        'sticky' => FALSE,
        'caption' => $caption,
        'colgroups' => [],
        'empty' => $settings['empty'] ?? 'There is no linked information',
      ]);

      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content . $pager,
      ];
    }
  }
}
