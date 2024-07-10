<?php

namespace Drupal\tripal\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Block\BlockPluginInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a 'Content Type Bar Chart' Block.
 *
 * @Block(
 *   id = "content_type_bar_chart",
 *   admin_label = @Translation("Content Type Bar Chart"),
 *   category = @Translation("Tripal"),
 * )
 */
class ContentBarChartBlock extends BlockBase implements BlockPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function build() {
    $db = \Drupal::database();
    $output = "";

    $query = $db->select('tripal_entity', 'te');
    $query->addField('te', 'type');
    $query->addExpression('COUNT(te.type)', 'count');
    $query->groupBy('te.type');
    $entity_types = $query->execute();

    $entity_count_listing = [];
    while (($entity_type = $entity_types->fetchObject())) {
      $entity_count_listing[] = [
        'name' => \Drupal::config('tripal.content_type.' . $entity_type->type)->get('label'),
        'count' => $entity_type->count,
      ];
    }

    // Initialize JS and CSS arrays.
    $settings = [];
    $libraries = [];

    $libraries[] = 'tripal/tripal';
    $libraries[] = 'tripal/d3';
    $libraries[] = 'tripal/tripal-dashboard';
    $libraries[] = 'system/drupal.collapse';
    $settings['tripal']['dashboard']['entityCountListing'] = $entity_count_listing;
    $settings['tripal']['dashboard']['barChartBuilt'] = FALSE;

    $output .= "<div id=\"block-tripal-content-type-barchart\"><div id=\"tripal-entity-types\" class=\"tripal-entity-types-pane\">
        <p>A list of the published Tripal content types and the number of each.</p>
        <div id=\"tripal-entity-type-chart\"></div>
      </div></div>";

    return [
      '#markup' => $output,
      '#attached' => [
        'library' => $libraries,
        'drupalSettings' => $settings,
      ],
    ];
  }

}