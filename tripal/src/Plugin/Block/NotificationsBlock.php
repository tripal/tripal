<?php

namespace Drupal\tripal\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Block\BlockPluginInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Pager\Pager;

/**
 * Provides a 'Dashboard Notifications' Block.
 *
 * @Block(
 *   id = "notifications",
 *   admin_label = @Translation("Tripal Administrative Notifications"),
 *   category = @Translation("Tripal"),
 * )
 */
class NotificationsBlock extends BlockBase implements BlockPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function build() {
    $db = \Drupal::database();

    // Prepare table header
    $header = [
      [
        'data' => t('Title'),
        'field' => 'tan.title',
      ],
      [
        'data' => t('Details'),
        'field' => 'tan.details',
      ],
      [
        'data' => t('Type'),
        'field' => 'tan.type',
      ],
      [
        'data' => t('Actions'),
      ],
    ];

    $table_name = 'tripal_admin_notifications';
    $query = $db->select($table_name, 'tan')
      ->extend('\Drupal\Core\Database\Query\TableSortExtender')
      ->fields('tan')
      ->condition('enabled', 1, '=');

    $results = $query->orderByHeader($header)
      ->execute();
    $rows = [];

    while (($result = $results->fetchObject())) {
      $action_links = [];
      $actions = unserialize($result->actions);
      foreach ($actions as $label => $route) {
        $url = Url::fromRoute($route);
        $action_links[] = Link::fromTextAndUrl(t($label), $url)->toString();
      }
      $action_links[] = Link::fromTextAndUrl(t('Dismiss Notification'), Url::fromRoute('tripal.dashboard_disable_notification', ['id' => $result->note_id]))->toString();

      $rows[] = [
        $result->title,
        $result->details,
        $result->type,
        [
          'data' => [
            '#markup' => implode(' | ', $action_links),
          ],
        ],
      ];
    }

    if (!empty($rows)) {
      $per_page = 10;
      $pager = new Pager(count($rows), $per_page);
      $chunks = array_chunk($rows, $per_page, TRUE);

      return [
        '#type' => 'table',
        '#header' => $header,
        '#rows' => $rows,
      ];
    }
    return [
      '#markup' => t('There are no notifications at this time.'),
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
