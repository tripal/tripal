<?php

namespace Drupal\tripal\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Block\BlockPluginInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a 'Dashboard Notifications' Block.
 *
 * @Block(
 *   id = "notifications",
 *   admin_label = @Translation("Notifications"),
 *   category = @Translation("Tripal"),
 * )
 */
class NotificationsBlock extends BlockBase implements BlockPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function build() {
    $db = \Drupal::database();
    $output = "hello";

    // Prepare table header
    $header = [
      'title' => [
        'data' => t('Title'),
      ],
      'details' => [
        'data' => t('Details'),
      ],
      'type' => [
        'data' => t('Type'),
        'field' => 'tan.type',
      ],
      'actions' => [
        'data' => t('Actions'),
      ],
    ];

    $query = $db->select('tripal_admin_notfications', 'tan')
      ->extend('\Drupal\Core\Database\Query\TableSortExtender');

    $results = $query->fields('tan')
      ->condition('enabled', 1, '=')
      ->orderByHeader($header)
      ->execute();
    $rows = [];

    // TODO

    /*
      foreach($results as $result){
        $data['operation'] = ' | ';
        $data['operation'] .= l(t('Dismiss Notification'), 'admin/disable/notification/' . $result->note_id);

        $actions = unserialize($result->actions);
        foreach($actions as $action){
          $label = key($actions);
          $link = $action;
        }

        $rows[] = array(
          'Title' => $result->title,
          'Details' => $result->details,
          'Type' => $result->type,
          'Actions' => l(t($label), $link) . $data['operation'],
        );
      }
      if(!empty($rows)) {
        //Number of records shown in per page
        $per_page = 10;
        $current_page = pager_default_initialize(count($rows), $per_page);
        $chunks = array_chunk($rows, $per_page, TRUE);

        // Output of table with the paging
        $table = theme('table',
          array(
            "header" => $header,
            "rows" => $chunks[ $current_page ],
            "attributes" => array(),
            "sticky" => TRUE,
            "caption" => "",
            "colgroups" => array(),
            "empty" => t("No notifications.")
          )
        );
        $table .= theme('pager', array('quantity', count($rows)));

        $fieldset_table = array(
          '#title' => t('Notifications'),
          '#collapsed' => FALSE,
          '#collapsible' => TRUE,
          '#attributes' => array('class' => array('collapsible')),
          '#children' => $table,
        );

        //return pager with limited number of records.
        $block['content'] = theme('fieldset', array('element' => $fieldset_table));
      }
      else {
        $block['content'] = 'There are no notifications at this time.';
      }
      $block['title'] = 'Tripal Administrative Notifications';
      break;
    */

    return [
      '#markup' => $output,
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $form = parent::blockForm($form, $form_state);

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function blockSubmit($form, FormStateInterface $form_state) {
    parent::blockSubmit($form, $form_state);
  }

}

