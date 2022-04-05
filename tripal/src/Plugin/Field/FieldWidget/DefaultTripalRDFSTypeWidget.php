<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal RDFS content type widget.
 *
 * @FieldWidget(
 *   id = "default_tripal_rdfs_type_widget",
 *   label = @Translation("Default Content Type Widget"),
 *   description = @Translation("The default resource content type widget."),
 *   field_types = {
 *     "tripal_rdfs_type"
 *   }
 * )
 */
class DefaultTripalRDFSTypeWidget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element["details"]["type"] = [
      "#type" => "string",
      "#title" => $this->t("Content Type"),
      "#required" => TRUE,
      "#description" => $this->t("The resource content type.")
    ];

    return $elements;
  }
}
