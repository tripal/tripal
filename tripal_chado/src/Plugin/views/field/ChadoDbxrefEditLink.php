<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Url;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Render\Markup;

/**
 * Field handler to present a link to edit a chado dbxref.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_dbxref_edit_link')]
class ChadoDbxrefEditLink extends FieldPluginBase {

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
    $url = Url::fromUserInput('/admin/tripal/storage/chado/dbxref/' . $dbxref_id)->toString();
    $html = '<a href="' . $url . '">Edit</a>';
    $markup = new Markup();

    return $markup->create($html);
  }

}
