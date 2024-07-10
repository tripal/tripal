<?php

namespace Drupal\tripal_chado\Plugin\views\field;

use Drupal\Core\Url;
use Drupal\Component\Utility\Xss;
use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Render\Markup;

/**
 * Field handler to present a link to delete a chado custom table.
 *
 * @ingroup views_field_handlers
 *
 * @ViewsField("chado_mviews_delete_link")
 */
class ChadoMviewsDeleteLink extends FieldPluginBase {

  /**
   * {@inheritdoc}
   */
  public function usesGroupBy() {
    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function query() {
    // do nothing -- to override the parent query.
  }

  /**
   * {@inheritdoc}
   */
  protected function defineOptions() {
    $options = parent::defineOptions();

    // Override the alter text option to always alter the text.
    $options['alter']['contains']['alter_text'] = ['default' => TRUE];
    $options['hide_alter_empty'] = ['default' => FALSE];
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);

    // Remove the checkbox
    unset($form['alter']['alter_text']);
    unset($form['alter']['text']['#states']);
    unset($form['alter']['help']['#states']);
    $form['#pre_render'][] = [$this, 'preRenderCustomForm'];
  }

  /**
   * {@inheritdoc}
   */
  public static function trustedCallbacks() {
    $callbacks = parent::trustedCallbacks();
    $callbacks[] = 'preRenderCustomForm';
    return $callbacks;
  }

  /**
   * {@inheritdoc}
   */
  public function render(ResultRow $values) {
    // Return the text, so the code never thinks the value is empty.
    $mview_id = $values->mview_id;
    $str = "wth";
    $url = Url::fromUserInput('/admin/tripal/storage/chado/mview_delete/' . $mview_id)->toString();
    $html = '<a href="' .  $url . '">Delete</a>';
    $markup = new Markup();

    return $markup->create($html);
  }

  /**
   * Prerender function to move the textarea to the top of a form.
   *
   * @param array $form
   *   The form build array.
   *
   * @return array
   *   The modified form build array.
   */
  public function preRenderCustomForm($form) {
    $form['text'] = $form['alter']['text'];
    $form['help'] = $form['alter']['help'];
    unset($form['alter']['text']);
    unset($form['alter']['help']);

    return $form;
  }

}
