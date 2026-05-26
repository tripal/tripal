<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Url;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Render\Markup;

/**
 * Field handler to present a link to edit a chado CV.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_cv_edit_link')]
class ChadoCvEditLink extends FieldPluginBase {

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
    $cv_id = $values->{'1cv_cv_id'};
    $url = Url::fromUserInput('/admin/tripal/storage/chado/cv/' . $cv_id)->toString();
    $html = '<a href="' . $url . '">Edit</a>';
    $markup = new Markup();

    return $markup->create($html);
  }

}
