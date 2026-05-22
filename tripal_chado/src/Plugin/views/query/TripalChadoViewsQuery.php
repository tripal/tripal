<?php

namespace Drupal\tripal_chado\Plugin\views\query;

use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\views\Attribute\ViewsQuery;
use Drupal\views\Plugin\views\query\Sql;

use Drupal\Core\Database\Database;
use Drupal\tripal\TripalDBX\TripalDbxConnection;

/**
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
   * @inheritdoc
   */
  public function getConnection() {
$caller1 = debug_backtrace()[1]['function'];//;;;
$caller2 = debug_backtrace()[2]['function'];//;;;
$caller3 = debug_backtrace()[3]['function'];//;;;
    print "CP1001 TripalChadoViewsQuery getConnection() called from $caller1 $caller2 $caller3\n";
#    // Set the replica target if the replica option is set for the view.
#    $target = empty($this->options['replica']) ? 'default' : 'replica';
#    // Use an external database when the view configured to.
#    $key = $this->view->base_database ?? 'default';
#print "CP1002 Database::getConnection( target \"$target\", key \"$key\")\n";//;;;
    $chado_connection = \Drupal::service('tripal_chado.database');
$p = $chado_connection->getPrefix();
print "CP1002 called getConnection, prefix \"$p\"\n";
    return $chado_connection;
    #return Database::getConnection($target, $key);
  }

}
