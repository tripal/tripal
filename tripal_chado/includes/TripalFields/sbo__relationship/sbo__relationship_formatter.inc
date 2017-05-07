<?php

class sbo__relationship_formatter extends ChadoFieldFormatter {

  // The default lable for this field.
  public static $default_label = 'Relationship';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__relationship');

  public static $default_settings = array(
    'title' => 'Relationship',
    'empty' => 'There are no relationships',
  );

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];
    $element = array();
    $element['title'] = array(
      '#type' => 'textfield',
      '#title' => 'Table Header',
      '#default_value' => array_key_exists('title', $settings) ? $settings['title'] : 'Relationship',
    );
    $element['empty'] = array(
      '#type' => 'textfield',
      '#title' => 'Empty text',
      '#default_value' => array_key_exists('empty', $settings) ? $settings['empty'] : 'There are no relationships',
    );

    return $element;
  }

  /**
   * @see TripalFieldFormatter::settingsSummary()
   */
  public function settingsSummary($view_mode) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    $summary = t('Title: @title<br>Empty: @empty', array('@title' => $settings['title'], '@empty' => $settings['empty']));

    return $summary;
  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];
    $rows = array();
    $headers = array($settings['title']);

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $subject_name = $item['value']['local:relationship_subject']['schema:name'];
      $subject_type = $item['value']['local:relationship_subject']['rdfs:type'];
      $object_name = $item['value']['local:relationship_object']['schema:name'];
      $object_type = $item['value']['local:relationship_object']['rdfs:type'];
      $phrase = $item['value']['SIO:000493'];

      // Handle some special cases.
      // For mRNA objects we don't want to show the CDS, exons, 5' UTR, etc.
      // we want to show the parent gene and the protein.
      if ($object_type == 'mRNA' and (in_array($subject_type, array('CDS', 'exon', 'five_prime_UTR', 'three_prime_UTR')))) {
        continue;
      }

      $phrase = preg_replace("/$subject_type/", "<b>$subject_type</b>", $phrase);
      $phrase = preg_replace("/$object_type/", "<b>$object_type</b>", $phrase);

      if (array_key_exists('entity', $item['value']['local:relationship_object'])) {
        list($entity_type, $object_entity_id) = explode(':', $item['value']['local:relationship_object']['entity']);
        if ($object_entity_id != $entity->id) {
          $link = l($object_name, 'bio_data/' . $object_entity_id);
          $phrase = preg_replace("/$object_name/", $link, $phrase);
        }
      }
      if (array_key_exists('entity', $item['value']['local:relationship_subject'])) {
        list($entity_type, $subject_entity_id) = explode(':', $item['value']['local:relationship_subject']['entity']);
        if ($subject_entity_id != $entity->id) {
          $link = l($subject_name, 'bio_data/' . $subject_entity_id);
          $phrase = preg_replace("/$subject_name/", $link, $phrase);
        }
      }

      $rows[][] = array('data' => $phrase, 'class' => array('tripal-entity-unattached field-items'));
    }

    //$pager = $this->generate_pager(count($rows), $per_page);

    $per_page = 10;
    // Initialize the pager
    $current_page = pager_default_initialize(count($rows), $per_page);
    // Split your list into page sized chunks
    $chunks = array_chunk($rows, $per_page, TRUE);
    //format pager
    $pager = theme('pager', array('quantity', count($rows)));


    global $base_path;
    $tmp_base_path = preg_replace('/\//', '\/', $base_path);
    $field_id = 'tripal-entity-' . $entity->id . '--' . $this->field['field_name'];
    $pager = preg_replace('/href="' . $tmp_base_path . 'bio_data\/ajax\/field_attach\/' . $field_id . '\?page=(.+?)"/', 'href="javascript:void(0)" onclick="tripal_navigate_field_pager(\'' . $field_id . '\', $1)"', $pager);
    $pager = preg_replace('/href="' . $tmp_base_path . 'bio_data\/ajax\/field_attach\/' . $field_id . '"/', 'href="javascript:void(0)" onclick="tripal_navigate_field_pager(\'' . $field_id . '\', 0)"', $pager);

    //$pager = preg_replace("/href=\"" . $tmp_base_path . "gensas\/load_job_view_panel\/" . $job_id . "\/\d\"/", 'href="javascript:void(0)" onclick="gensas.show_job_view_panel(\'' . $job_id . '\', \'' . $job->getName() . '\')"', $pager);
    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7

    $table = array(
      'header' => $headers,
      'rows' => $current_page ? $chunks[$current_page] : array(),
      'attributes' => array(
        'id' => 'sbo--relationship-table',
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => $settings['empty'],
    );

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
        '#suffix' => '<img src=\'/sites/all/modules/tripal-7.x-3.x/tripal/theme/images/ajax-loader.gif\' id=\'spinner\'/>' . $pager,
      );
    }
  }
}
