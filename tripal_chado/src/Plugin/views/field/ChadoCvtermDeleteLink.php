<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Url;
use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Render\Markup;

/**
 * Field handler to present a link to delete a chado CV term.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_cvterm_delete_link')]
class ChadoCvtermDeleteLink extends FieldPluginBase {

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
    $cvterm_id = $values->cvterm_id;
    $url = Url::fromUserInput('/admin/tripal/storage/chado/cvterm_delete/' . $cvterm_id)->toString();
    $html = '<a href="' . $url . '">Delete</a>';
    $markup = new Markup();

    return $markup->create($html);
  }

}
