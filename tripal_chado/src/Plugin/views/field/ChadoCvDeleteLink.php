<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Link;
use Drupal\Core\Render\Markup;
use Drupal\Core\Url;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;

/**
 * Field handler to present a link to delete a chado CV.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_cv_delete_link')]
class ChadoCvDeleteLink extends FieldPluginBase {

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
    $cv_id = $values->cv_id;
    $url = Url::fromUserInput('/admin/tripal/storage/chado/cv_delete/' . $cv_id);
    $link = Link::fromTextAndUrl('Delete', $url)->toString()->getGeneratedLink();

    return Markup::create($link);
  }

}
