<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Url;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Render\Markup;

/**
 * Field handler to present a link to show dbxrefs in one DB.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_db_items_link')]
class ChadoDbItemsLink extends FieldPluginBase {

  /**
   * {@inheritdoc}
   */
  public function query() {
    // Do nothing -- to override the parent query.
  }

  /**
   * {@inheritdoc}
   */
  public function render(ResultRow $values) {
    // Return the text, so the code never thinks the value is empty.
    $db_name = $values->db_name;
    $url = Url::fromUserInput('/admin/tripal/terms/chado_dbxref?name=' . $db_name)->toString();
    $html = '<a href="' . $url . '">Items</a>';
    $markup = new Markup();

    return $markup->create($html);
  }

}
