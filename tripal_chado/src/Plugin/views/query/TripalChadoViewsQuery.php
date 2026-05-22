<?php

namespace Drupal\tripal_chado\Plugin\views\query;

use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\views\Attribute\ViewsQuery;
use Drupal\views\Plugin\views\query\Sql;

/**
 * Use a chado connection for drupal views to allow use of chado tables.
 *
 * @ViewsQuery(
 *   id = "tripal_chado_views_connection",
 *   title = @Translation("Tripal Chado SQL Connection for Views"),
 *   help = @Translation("Connection to chado for a Drupal view.")
 * )
 */
#[ViewsQuery(
  id: 'tripal_chado_views_connection',
  title: new TranslatableMarkup('Tripal Chado SQL Connection for Views'),
  help: new TranslatableMarkup('Connection to chado for a Drupal view.')
)]
class TripalChadoViewsQuery extends Sql {

  /**
   * {@inheritdoc}
   */
  public function getConnection() {
    $chado_connection = \Drupal::service('tripal_chado.database');
    return $chado_connection;
  }

}
