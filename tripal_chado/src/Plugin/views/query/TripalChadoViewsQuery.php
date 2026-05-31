<?php

namespace Drupal\tripal_chado\Plugin\views\query;

use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\views\Attribute\ViewsQuery;
use Drupal\views\Plugin\views\join\JoinPluginBase;
use Drupal\views\Plugin\views\query\Sql;
use Drupal\views\ViewExecutable;

/**
 * Use a chado connection for drupal views to allow use of chado tables.
 *
 * @ViewsQuery(
 *   id = "tripal_chado_views_query",
 *   title = @Translation("Tripal Chado Views Query"),
 *   help = @Translation("Provides a connection to chado for a Drupal view.")
 * )
 */
#[ViewsQuery(
  id: 'tripal_chado_views_query',
  title: new TranslatableMarkup('Tripal Chado Views Query'),
  help: new TranslatableMarkup('Provides a connection to chado for a Drupal view.')
)]
class TripalChadoViewsQuery extends Sql {

  /**
   * {@inheritdoc}
   */
  public function getConnection() {
    $chado_connection = \Drupal::service('tripal_chado.database');
    return $chado_connection;
  }

  /**
   * Executes the views query.
   *
   * Extended to allow referencing chado tables without 1: prefix.
   *
   * {@inheritdoc}
   */
  public function execute(ViewExecutable $view) {
    // Instruct TripalDbx to also prefix tables without a 1: prefix,
    // e.g. "{db}" will be prefixed as "chado.db", since this class is
    // intended strictly for chado table views, and drupal does play
    // well with our 1: prefixing notation.
    $chado_connection = $this->getConnection();
    $chado_connection->useTripalDbxSchemaFor('Drupal\Core\Database\Query\Query');

    parent::execute($view);

    // Undo the override.
    $chado_connection->useDrupalSchemaFor('Drupal\Core\Database\Query\Query');
  }

}
