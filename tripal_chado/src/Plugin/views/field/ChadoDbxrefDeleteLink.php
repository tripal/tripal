<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Link;
use Drupal\Core\Render\Markup;
use Drupal\Core\Url;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;

/**
 * Field handler to present a link to delete a chado dbxref.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_dbxref_delete_link')]
class ChadoDbxrefDeleteLink extends FieldPluginBase {

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
    $dbxref_id = $values->dbxref_id;
    $url = Url::fromUserInput('/admin/tripal/storage/chado/dbxref_delete/' . $dbxref_id)->toString();
    $link = Link::fromTextAndUrl('Delete', $url)->toString()->getGeneratedLink();

    return Markup::create($link);
  }

}
