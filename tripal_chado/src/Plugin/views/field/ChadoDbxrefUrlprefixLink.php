<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Url;
use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Attribute\ViewsField;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Render\Markup;

/**
 * Field handler to present a link to edit a chado CV term.
 *
 * @ingroup views_field_handlers
 */
#[ViewsField('chado_dbxref_urlprefix_link')]
class ChadoDbxrefUrlprefixLink extends FieldPluginBase {

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
    // This requires that these three values have been defined in the view
    // in columns preceeding the column using this.
    $urlprefix = $values->db_urlprefix;
    $db = $values->db_name;
    $accession = $values->dbxref_accession;
    if ($urlprefix && $accession) {
      $urlprefix = preg_replace('/{db}/', $db, $urlprefix);
      $urlprefix = preg_replace('/{accession}/', $accession, $urlprefix);
      $html = '<a href="' . $urlprefix . '">🔗' . $db . ':' . $accession . '</a>';
    }
    else {
      $html = $this->t('not available');
    }
    $markup = new Markup();

    return $markup->create($html);
  }

}
